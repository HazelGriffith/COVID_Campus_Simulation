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
	int ventilationRating;
	int socialDistancingThreshold;
	int wearsMaskCorrectlyFactor;
	int socialDistancingFactor;
	int respIncreasePerMin; //CO2 exhaled per minute by a person
	

    using input_ports=std::tuple<typename RoomModelPorts::inToRoom, RoomModelPorts::outFromRoom>;
    using output_ports=std::tuple<typename RoomModelPorts::out>;


    //State definition 
    struct state_type{
		
        int numberPeople;
        int numberPeopleWearingMasksCorrectly;
        int socialDistancing;
		int numberSickPeople;
		vector<PersonInfo> peopleInRoom;
        int CO2concentration;
		//ProbGetSick *peopleLeaving;
		vector<ProbGetSick> peopleLeaving;
		
    }; state_type state;

    //default constructor
    RoomModel () noexcept{
        state.numberPeople = 0;
        state.numberPeopleWearingMasksCorrectly = 0;
        state.socialDistancing = 0;
        state.numberSickPeople = 0;
		state.peopleInRoom.clear();
		state.CO2concentration = 0;
		state.peopleLeaving.clear();
    }
	
    RoomModel (string inputRoomID, int inputVentilationRating, int inputSocialDistancingThreshold, int inputWearsMaskCorrectlyFactor, int inputSocialDistancingFactor, int inputRespIncreasePerMin) noexcept{
        //initializing model parameters
		roomID = inputRoomID;
		ventilationRating = inputVentilationRating;
		socialDistancingThreshold = inputSocialDistancingThreshold;
		wearsMaskCorrectlyFactor = inputWearsMaskCorrectlyFactor;
		socialDistancingFactor = inputSocialDistancingFactor;
		respIncreasePerMin = inputRespIncreasePerMin;
		RoomModel();
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
		
		//The CO2 concentration is determined by taking the amount of COVID particles exhaled over the elapsed time, dividing them among the people and devices that would inhale these particles,
		//and then adding what remains to the previous CO2 concentration in the room.
        state.CO2concentration = state.CO2concentration + ((state.numberSickPeople * respIncreasePerMin * elap_min)/(state.numberPeople + ventilationRating));
        
       
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
					
					int wearsMaskCorrectlyFactorUsed = 0;
					int socialDistancingFactorUsed = 0;
					
					if (msgOutFromRoom.wearsMaskCorrectly){
						wearsMaskCorrectlyFactorUsed = wearsMaskCorrectlyFactor;
					}
					
					if ((msgOutFromRoom.socialDistance)&&(state.numberPeople < socialDistancingThreshold)){
						socialDistancingFactorUsed = socialDistancingFactor;
					}
										
					//If the person is already sick then their chance of being sick upon leaving is guaranteed
					if (msgOutFromRoom.isSick){
						personLeaving.probSick = 1;
					} else {
						//A person's probability of getting sick is equal to the CO2 concentration in the room divided by the sum of the factors for the protective measures used
						personLeaving.probSick = (state.CO2concentration/(1 + wearsMaskCorrectlyFactorUsed + socialDistancingFactorUsed));
						
						if (personLeaving.probSick > 1){
							personLeaving.probSick = 1;
						}
					}
					
					state.peopleLeaving.push_back(personLeaving);
					//The person leaving is added to a singly-linked list of people leaving
					/*if (state.peopleLeaving != NULL){
						personLeaving.next = state.peopleLeaving;
					}
						
					state.peopleLeaving = &personLeaving;
					*/
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
        /*ProbGetSick *nextPersonLeaving = state.peopleLeaving;

        while (nextPersonLeaving != NULL){
            get_messages<typename RoomModelPorts::out>(bags).push_back(*nextPersonLeaving);
			nextPersonLeaving = nextPersonLeaving->next;
        }*/
		
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
        os << "Number of People in room: "<< i.numberPeople <<" number of people wearing masks: " << i.numberPeopleWearingMasksCorrectly << " Number of people social distancing: " << i.socialDistancing << " Number of sick people: " << i.numberSickPeople << " CO2 concentration in the room: " << i.CO2concentration <<"\n ";
        
        return os;
    }
           
    #endif // room.hpp
};
