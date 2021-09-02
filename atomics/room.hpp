#ifndef RoomModelHPP
#define RoomModelHPP

#include <cadmium/modeling/ports.hpp>
#include <cadmium/modeling/message_bag.hpp>

#include <limits>
#include <assert.h>
#include <string>
#include <list>
#include <random>
#include <vector>
#include <iostream>
#include <math.h>

#include "../data_structures/PersonInfo.hpp"
#include "../data_structures/ProbGetSick.hpp"



using namespace cadmium;
using namespace std;

//Port definition
//definition of input/output events
struct RoomModelPorts{
    struct inToRoom : public in_port<PersonInfo>{};
    struct outFromRoom : public in_port<PersonInfo>{};
    struct out : public out_port<ProbGetSick>{};
};

template<typename TIME> class RoomModel{
  public:
    
    string roomID;
	int ACHVentilation;
	int socialDistanceThreshold;
	int wearsMaskCorrectlyFactor;
	int socialDistancingFactor;
	int respIncPerMin; //CO2 exhaled per minute by a person
	float volumeOfRoom; // m^2
	int sickPeopleCO2Factor;
	int *highCO2FactorThresholds;
	int *highCO2Factors;
	int vaccinatedFactor;
	int wearsMaskFactor;
	int socialDistanceFactor;
	

    using input_ports=std::tuple<typename RoomModelPorts::inToRoom, RoomModelPorts::outFromRoom>;
    using output_ports=std::tuple<typename RoomModelPorts::out>;


    //State definition 
    struct state_type{
		
        int numberPeople;
        int numberPeopleWearingMasksCorrectly;
        int socialDistancing;
		int numberSickPeople;
		vector<PersonInfo> peopleInRoom;
        float CO2Concentration;
		vector<ProbGetSick> peopleLeaving;
		int totalNumberPeople;
		vector<float> probsOfInfection;
		
    }; state_type state;

    //default constructor
    RoomModel () noexcept{
		
    }
	
    RoomModel (string i_roomID, int i_ACHVentilation, int i_socialDistanceThreshold, int i_respIncPerMin, float i_volumeOfRoom,
					int i_sickPeopleCO2Factor, int i_highCO2FactorThresholds[4], int i_highCO2Factors[4], int i_vaccinatedFactor, 
					int i_wearsMaskFactor, int i_socialDistanceFactor) noexcept{
        //initializing model parameters
		state.numberPeople = 0;
		
        state.numberPeopleWearingMasksCorrectly = 0;
        state.socialDistancing = 0;
        state.numberSickPeople = 0;
		state.peopleInRoom.clear();
		state.CO2Concentration = 400;
		state.peopleLeaving.clear();
		state.totalNumberPeople = 0;
		state.probsOfInfection.clear();
		
		roomID = i_roomID;
		ACHVentilation = i_ACHVentilation;
		socialDistanceThreshold = i_socialDistanceThreshold;
		respIncPerMin = i_respIncPerMin;
		volumeOfRoom = i_volumeOfRoom;
		sickPeopleCO2Factor = i_sickPeopleCO2Factor;
		highCO2FactorThresholds = i_highCO2FactorThresholds;
		highCO2Factors = i_highCO2Factors;
		vaccinatedFactor = i_vaccinatedFactor;
		wearsMaskFactor = i_wearsMaskFactor;
		socialDistanceFactor = i_socialDistanceFactor;
		
    }

    //clear peopleLeaving
    void internal_transition() {
        state.peopleLeaving.clear();
    }


    //update Room with new occupants and new CO2 concentration
    void external_transition(TIME e, typename make_message_bags<input_ports>::type mbs){
        
        
        float hoursToMin = e.getHours()*60;
        float min = e.getMinutes();
        float secondsToMin = e.getSeconds()/60;
        float elap_min = (hoursToMin + min + secondsToMin); // convert elapsed time to minutes 
		
		float averageCO2 = 400;
		
		float ventilationRatePerMin = ((ACHVentilation*volumeOfRoom)/60)*1000;
		
		float CO2InPerMin = ventilationRatePerMin * averageCO2;
		
		float CO2PeoplePerMin = respIncPerMin * state.numberPeople;
		
		float CO2OutPerMin;
		
		float CO2ConcentrationFlow;
		
		float temp = elap_min;

		int timeStep = 10;
		while (temp >= timeStep){
			CO2OutPerMin = ventilationRatePerMin*state.CO2Concentration;
			CO2ConcentrationFlow = ((CO2InPerMin + CO2PeoplePerMin - CO2OutPerMin) * timeStep)/(volumeOfRoom*1000);
			state.CO2Concentration = state.CO2Concentration + CO2ConcentrationFlow;
			//assert(state.CO2Concentration > 150);
			//assert(state.CO2Concentration < 2500);

			temp = temp - timeStep;

		}
		

		CO2OutPerMin = ventilationRatePerMin*state.CO2Concentration;
		CO2ConcentrationFlow = ((CO2InPerMin + CO2PeoplePerMin - CO2OutPerMin) * temp)/(volumeOfRoom*1000);
		state.CO2Concentration = state.CO2Concentration + CO2ConcentrationFlow;

	//	assert(state.CO2Concentration > 150);
	//	assert(state.CO2Concentration < 2500);
        
		
        vector<PersonInfo> msgBagInToRoom = get_messages<typename RoomModelPorts::inToRoom>(mbs);
        vector<PersonInfo> msgBagOutFromRoom = get_messages<typename RoomModelPorts::outFromRoom>(mbs);
        
        //Entities going in to the room
        for(int i = 0 ; i < msgBagInToRoom.size(); i++ ){
            PersonInfo msgInToRoom = msgBagInToRoom[i];
            
            //verify that a person entering does not match an existing person in the room
            for (int k = 0; k < state.peopleInRoom.size(); k++){
                assert(msgInToRoom.personID.compare(state.peopleInRoom[k].personID) != 0);
			}
			
			state.peopleInRoom.push_back(msgInToRoom);
			state.totalNumberPeople++;
		}
		
        //Entities coming out of the room
        for(int s = 0 ; s < msgBagOutFromRoom.size(); s++){ 
            PersonInfo msgOutFromRoom = msgBagOutFromRoom[s];
            bool personFound = false;
		    //checks if the person is in the room
		    for (int k = 0; k < state.peopleInRoom.size(); k++){
				
				//If the person is found they are removed from the peopleInRoom list, the probability of them getting sick is calculated,
				//and lastly they are added to the list of people who are leaving the room
                if(msgOutFromRoom.personID.compare(state.peopleInRoom[k].personID) == 0){
					personFound = true;
					
					state.peopleInRoom.erase(state.peopleInRoom.begin()+k);
					
					ProbGetSick personLeaving;
					personLeaving.personID = msgOutFromRoom.personID;
					
					
					
					int wearsMaskFactorUsed = 0;
					int socialDistanceFactorUsed = 0;
					int vaccinatedFactorUsed = 0;
					int highCO2FactorUsed = 0;
					int sickPeopleCO2FactorUsed = 0;
					
					if (msgOutFromRoom.vaccinated){
						vaccinatedFactorUsed = vaccinatedFactor;
					}
					
					if (!msgOutFromRoom.wearsMaskCorrectly){
						wearsMaskFactorUsed = wearsMaskFactor;
					}
					
					if ((!msgOutFromRoom.socialDistance)||(state.numberPeople>=socialDistanceThreshold)){
						socialDistanceFactorUsed = socialDistanceFactor;
					}
					
					if (state.numberSickPeople > 0){
						// should consider range of values like for highCO2Factors
						sickPeopleCO2FactorUsed = sickPeopleCO2Factor;
					}
					
					if (state.CO2Concentration >= highCO2FactorThresholds[3]){
						highCO2FactorUsed = highCO2Factors[3];
					} else if (state.CO2Concentration >= highCO2FactorThresholds[2]){
						highCO2FactorUsed = highCO2Factors[2];
					} else if (state.CO2Concentration >= highCO2FactorThresholds[1]){
						highCO2FactorUsed = highCO2Factors[1];
					} else if (state.CO2Concentration >= highCO2FactorThresholds[0]){
						highCO2FactorUsed = highCO2Factors[0];
					}
					
					int probabilityRank = vaccinatedFactorUsed + wearsMaskFactorUsed + socialDistanceFactorUsed 
											+ sickPeopleCO2FactorUsed + highCO2FactorUsed;
					if (probabilityRank < 1){
						probabilityRank = 1;
					} else if (probabilityRank > 5){
						probabilityRank = 5;
					}
					
					
					float probabilityOfInfection = ((probabilityRank - 1)*20) + (rand() % 21);
					
							
					//If the person is already sick then their chance of being sick upon leaving is guaranteed
					if (msgOutFromRoom.isSick){
						personLeaving.probSick = 1;
					} else {
						personLeaving.probSick = probabilityOfInfection;
						
					}
					
					state.peopleLeaving.push_back(personLeaving);
					
					break;
				}
			}
			
			//If the person is not in the room, an error is thrown
			assert(personFound == true);
        }
		
		//The state of the Room is updated
		state.numberPeople = state.peopleInRoom.size();
		state.numberPeopleWearingMasksCorrectly = 0;
		state.socialDistancing = 0;
		state.numberSickPeople = 0;
		for (int i = 0; i < state.numberPeople; i++){
			PersonInfo person = state.peopleInRoom[i];
			
			if (person.wearsMaskCorrectly){
				state.numberPeopleWearingMasksCorrectly++;
			} 
			
			if (person.socialDistance){
				state.socialDistancing++;
			}
			
			if (person.isSick){
				state.numberSickPeople++;
			}
		}
    }

    void confluence_transition(TIME e, typename make_message_bags<input_ports>::type mbs){
        internal_transition();
        external_transition(TIME(),std::move(mbs));
    }

    //Output Function
    typename make_message_bags<output_ports>::type output() const{
        //output all ProbGetSick from peopleLeaving
        typename make_message_bags<output_ports>::type bags; 
		
		for (int i = 0; i < state.peopleLeaving.size(); i++){
			get_messages<typename RoomModelPorts::out>(bags).push_back(state.peopleLeaving[i]);
		}
		
        return bags;
    }

    //time advance function
    TIME time_advance() const{
        TIME nextInternal;
        if(state.peopleLeaving.empty()){
            nextInternal = numeric_limits<TIME>::infinity();
        }else{
            nextInternal = TIME();
        }
		return nextInternal;
    };
    
    friend std::ostringstream& operator<<(std::ostringstream& os, const typename RoomModel<TIME>::state_type& i){
		string people = "";
		for (int j = 0; j < i.peopleInRoom.size(); j++){
			people += i.peopleInRoom[j].personID;
			if (j != i.peopleInRoom.size()-1){
				people += ",";
			}
		}
        os << "Number of People in room: "<< i.numberPeople <<" number of people wearing masks: " << i.numberPeopleWearingMasksCorrectly << " Number of people social distancing: " << i.socialDistancing << " Number of sick people: " << i.numberSickPeople << " CO2 concentration in the room: " << i.CO2Concentration << " people ID's in room: " << people << " students travelling at same time: " << i.peopleLeaving.size() <<"\n ";
        
        return os;
    }
           
    #endif // room.hpp
};
