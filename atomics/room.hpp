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
#include <map>
#include <chrono>

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

struct PersonRelationInfo{
	string relationshipType;
	int startTime;
	int endTime;
	int totalTime;
};

template<typename TIME> class RoomModel{
  public:
    
    string roomID;
	int ACHVentilation;
	int socialDistanceThreshold;
	int maxOccupancy;
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
		vector<PersonInfo> peopleWaitingOutsideRoom;
        float CO2Concentration;
		vector<ProbGetSick> peopleLeaving;
		map<string,map<string,PersonRelationInfo>> relationshipsInRoom;
		
    }; state_type state;

    //default constructor
    RoomModel () noexcept{
		
    }
	
    RoomModel (string i_roomID, int i_ACHVentilation, int i_socialDistanceThreshold, int i_maxOccupancy, int i_respIncPerMin, float i_volumeOfRoom,
					int i_sickPeopleCO2Factor, int i_highCO2FactorThresholds[4], int i_highCO2Factors[4], int i_vaccinatedFactor, 
					int i_wearsMaskFactor, int i_socialDistanceFactor) noexcept{
        //initializing model parameters
		state.numberPeople = 0;
		
        state.numberPeopleWearingMasksCorrectly = 0;
        state.socialDistancing = 0;
        state.numberSickPeople = 0;
		state.peopleInRoom.clear();
		state.peopleWaitingOutsideRoom.clear();
		state.CO2Concentration = 400;
		state.peopleLeaving.clear();
		state.relationshipsInRoom.clear();
		
		roomID = i_roomID;
		ACHVentilation = i_ACHVentilation;
		socialDistanceThreshold = i_socialDistanceThreshold;
		maxOccupancy = i_maxOccupancy;
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
			
			if (state.numberPeople < maxOccupancy){
				state.peopleInRoom.push_back(msgInToRoom);
			} else {
				//If the room is at or above maxOccupancy, then the person uses their relationships with those in the room to decide if they want to enter
				
				//First they count if each person is a friend, stranger, or family member
				unsigned seed1 = chrono::system_clock::now().time_since_epoch().count();
				default_random_engine generator(seed1);
				int strangers = 0;
				int acquaintances = 0;
				int friends = 0;
				for (int j = 0; j < state.numberPeople; j++){
					string ID = state.peopleInRoom[j].personID;
					string relationshipType = "";
					bool found = false;
					for (int l = 0; l < msgInToRoom.relationships.size(); l++){
						
						if (msgInToRoom.relationships[l].PersonID.compare(ID) == 0){
							found = true;
							relationshipType = msgInToRoom.relationships[l].Relationship_type;
							break;
						}
					}
					assert(found == true);
					
					if (relationshipType.compare("stranger") == 0){
						strangers++;
					} else if (relationshipType.compare("acquaintance") == 0){
						acquaintances++;
					} else {
						friends++;
					}
				}
				
				assert(strangers+friends+acquaintances == state.numberPeople);
				
				//The percentages of acquaintances to friends to strangers is calculated
				float strangersPercent = strangers/state.numberPeople;
				float friendsPercent = friends/state.numberPeople;
				float acquaintancesPercent = acquaintances/state.numberPeople;
				
				//the probability of them entering the room given the probability of each category is calculated by multiplying the category percentage by the appropriate behaviour factor
				int strangersProb = floor(msgInToRoom.relationshipBehaviour.at("stranger").EnterMaxOccRoomProbability*strangersPercent);
				int friendsProb = floor(msgInToRoom.relationshipBehaviour.at("friends").EnterMaxOccRoomProbability*friendsPercent);
				int acquaintancesProb = floor(msgInToRoom.relationshipBehaviour.at("acquaintance").EnterMaxOccRoomProbability*acquaintancesPercent);
				
				
				uniform_int_distribution<int> probEnterDistribution(0,100);
				
				//If r is then less than any of the probabilities of each category, then they will enter the room, otherwise they will wait outside
				int r = probEnterDistribution(generator);
				if ((r <= strangersProb)||(r <= friendsProb)||(r <= acquaintancesProb)){
					state.peopleInRoom.push_back(msgInToRoom);
				} else {
					state.peopleWaitingOutsideRoom.push_back(msgInToRoom);
				}
				
			}
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
					
					
					
					ProbGetSick personLeaving;
					personLeaving.personID = msgOutFromRoom.personID;

					
					
					int wearsMaskFactorUsed = 0;
					int socialDistanceFactorUsed = 0;
					int vaccinatedFactorUsed = 0;
					int highCO2FactorUsed = 0;
					int sickPeopleCO2FactorUsed = 0;
					
					//If the person is vaccinated the factor that determines its impact on infection is used
					if (msgOutFromRoom.vaccinated){
						vaccinatedFactorUsed = vaccinatedFactor;
					}
					
					vector<PersonRelationInfo> strangers;
					vector<PersonRelationInfo> acquaintances;
					vector<PersonRelationInfo> friends;
					
					//The people that the person leaving had interacted with in this room are tallied and divided into the friends, strangers, or acquaintances categories
					map <string, PersonRelationInfo> currRelationships = state.relationshipsInRoom.at(personLeaving.personID);
					int count = 0;
					map<string, PersonRelationInfo>::iterator iter;
					for (iter = currRelationships.begin(); iter != currRelationships.end(); iter++){
						PersonRelationInfo relationInfo = iter->second;
						if (relationInfo.relationshipType.compare("acquaintances") == 0){
							acquaintances.push_back(relationInfo);
						} else if (relationInfo.relationshipType.compare("friends") == 0){
							friends.push_back(relationInfo);
						} else {
							strangers.push_back(relationInfo);
						}
						count++;
					}
					assert(count == (acquaintances.size()+friends.size()+strangers.size()));
					
					//The start time, end time and total time spent in the room by the person leaving are retrieved
					float mainStartTime = state.peopleInRoom[k].timeEntering;

					float mainEndTime = state.peopleInRoom[k].timeLeaving;

					float mainTotalTime = state.peopleInRoom[k].minsUntilLeaving;

					
					//The person is erased from the peopleInRoom list
					state.peopleInRoom.erase(state.peopleInRoom.begin()+k);

					//The behaviour factors for mask wearing and social distancing for each relationship type are retrieved
					float strangersMaskBehaviour = msgOutFromRoom.relationshipBehaviour.at("stranger").MaskWearingProbability;
					float friendsMaskBehaviour = msgOutFromRoom.relationshipBehaviour.at("friends").MaskWearingProbability;
					float acquaintancesMaskBehaviour = msgOutFromRoom.relationshipBehaviour.at("acquaintance").MaskWearingProbability;
					
					float strangersSocialDistanceBehaviour = msgOutFromRoom.relationshipBehaviour.at("stranger").SafeDistanceProbability;
					float friendsSocialDistanceBehaviour = msgOutFromRoom.relationshipBehaviour.at("friends").SafeDistanceProbability;
					float acquaintancesSocialDistanceBehaviour = msgOutFromRoom.relationshipBehaviour.at("acquaintance").SafeDistanceProbability;
				
					int averageMaskProb = 0;
					int averageSocialDistanceProb = 0;
					
					//The time spent in the room with the person leaving is calculated for each person in the acquaintances category
					for (int i = 0; i < acquaintances.size(); i++){
						int startTime;
						int endTime;
						int totalTime;
						
						
						if (acquaintances[i].startTime <= mainStartTime){
							//The person entered before the person leaving
							startTime = mainStartTime;
						} else {
							//The person entered after the person leaving
							startTime = acquaintances[i].startTime;
						}
						
						if (acquaintances[i].endTime < mainEndTime){
							//The person left before the person leaving
							endTime = acquaintances[i].endTime;
						} else {
							//The person left after the person leaving
							endTime = mainEndTime;
						}

						//This is the time they spent in the room together
						totalTime = endTime - startTime;
						
						//This is the percentage of the person leaving's time spent in the room with that person
						float acquaintancesTimePercent = totalTime/mainTotalTime;
						
						//The probability is the percentage multiplied by the appropriate behaviour factor
						averageMaskProb += floor(acquaintancesTimePercent*acquaintancesMaskBehaviour);
						
						averageSocialDistanceProb += floor(acquaintancesTimePercent*acquaintancesSocialDistanceBehaviour);
					}
					
					//The same as the above only for the friends category
					for (int i = 0; i < friends.size(); i++){
						int startTime;
						int endTime;
						int totalTime;
						
						if (friends[i].startTime <= mainStartTime){
							startTime = mainStartTime;
						} else {
							startTime = friends[i].startTime;
						}
						
						if (friends[i].endTime < mainEndTime){
							endTime = friends[i].endTime;
						} else {
							endTime = mainEndTime;
						}

						totalTime = endTime - startTime;

						
						float friendsTimePercent = totalTime/mainTotalTime;
						
						averageMaskProb += floor(friendsTimePercent*friendsMaskBehaviour);

						
						averageSocialDistanceProb += floor(friendsTimePercent*friendsSocialDistanceBehaviour);

					}
					
					//The same as the above only for the strangers category
					for (int i = 0; i < strangers.size(); i++){
						int startTime;
						int endTime;
						int totalTime;
						
						if (strangers[i].startTime <= mainStartTime){
							startTime = mainStartTime;
						} else {
							startTime = strangers[i].startTime;
						}
						
						if (strangers[i].endTime < mainEndTime){
							endTime = strangers[i].endTime;
						} else {
							endTime = mainEndTime;
						}

						totalTime = endTime - startTime;

						
						float strangersTimePercent = totalTime/mainTotalTime;

						
						averageMaskProb += floor(strangersTimePercent*strangersMaskBehaviour);

						
						averageSocialDistanceProb += floor(strangersTimePercent*strangersSocialDistanceBehaviour);

					}
					
					//If the person leaving did share the room with other people, then the average probability is calculated for mask wearing and social distancing
					if ((strangers.size()+friends.size()+acquaintances.size()) > 0){
						averageMaskProb = floor(averageMaskProb/(strangers.size()+friends.size()+acquaintances.size()));
						averageSocialDistanceProb = floor(averageSocialDistanceProb/(strangers.size()+friends.size()+acquaintances.size()));
					}

					
					assert(averageMaskProb <= 100);
					assert(averageSocialDistanceProb <= 100);
					assert(averageMaskProb >= 0);
					assert(averageSocialDistanceProb >= 0);
						
					unsigned seed1 = chrono::system_clock::now().time_since_epoch().count();
					default_random_engine generator(seed1);
					uniform_int_distribution<int> probDistribution(0,100);
					
					int r = probDistribution(generator);
					
					if (averageMaskProb == 0){
						//If the person leaving shared the room with no one, then their own mask wearing tendency is used to determine if the mask wearing factor is used
						if (!msgOutFromRoom.wearsMaskCorrectly){
							wearsMaskFactorUsed = wearsMaskFactor;
						} 
					} else if (r > averageMaskProb){
						//if the value of r (0-100) is greater than the mask wearing probability, then the mask wearing factor is used
						wearsMaskFactorUsed = wearsMaskFactor;
					}
					
					r = probDistribution(generator);
					//If the value of r (0-100) is greater than the social distancing probability, 
					//or if the number of people in the room is greater than the social distance threshold, then the social distance factor is used
					if ((r > averageSocialDistanceProb)||(state.numberPeople>=socialDistanceThreshold)){ // Consider checking in the future when the social distance threshold may have been reached
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
					
					//From the factors used the probability Rank is calculated
					int probabilityRank = vaccinatedFactorUsed + wearsMaskFactorUsed + socialDistanceFactorUsed 
											+ sickPeopleCO2FactorUsed + highCO2FactorUsed;
					if (probabilityRank < 1){
						probabilityRank = 1;
					} else if (probabilityRank > 5){
						probabilityRank = 5;
					}
					
					//The rank is then used to calculate the probability of infection
					float probabilityOfInfection = ((probabilityRank - 1)*20) + (rand() % 21);
					
							
					//If the person is already sick then their chance of being sick upon leaving is guaranteed
					if (msgOutFromRoom.isSick){
						personLeaving.probSick = 1;
					} else {
						personLeaving.probSick = probabilityOfInfection;
					}
					
					state.peopleLeaving.push_back(personLeaving);
					
					state.relationshipsInRoom.erase(personLeaving.personID);
					
					//If there are people waiting to enter the room, then the next person is able to enter
					if ((!state.peopleWaitingOutsideRoom.empty())&&(state.numberPeople-s < maxOccupancy)){
						state.peopleInRoom.push_back(state.peopleWaitingOutsideRoom[0]);
						state.peopleWaitingOutsideRoom.erase(state.peopleWaitingOutsideRoom.begin());
					}
					
					break;
				}
			}
			
			//checks if the person is waiting outside the room
			for (int k = 0; k < state.peopleWaitingOutsideRoom.size(); k++){
				if(msgOutFromRoom.personID.compare(state.peopleWaitingOutsideRoom[k].personID) == 0){
					personFound = true;
					
					state.peopleWaitingOutsideRoom.erase(state.peopleWaitingOutsideRoom.begin()+k);
					
					ProbGetSick personLeaving;
					personLeaving.personID = msgOutFromRoom.personID;
					personLeaving.probSick = 0;
					
					state.peopleLeaving.push_back(personLeaving);
					
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
			
			//If the person does not have an entry in the map, they are added to it.
			if (state.relationshipsInRoom.count(person.personID) == 0){
				map<string, PersonRelationInfo> r;
				state.relationshipsInRoom.insert(pair<string,map<string,PersonRelationInfo>>(person.personID, r));
			}
				
			//Updates the person's relationships with the other people they have shared the room with
			map<string, PersonRelationInfo> existingRelations = state.relationshipsInRoom[person.personID];
			if (!existingRelations.empty()){
				//If they have been sharing the room with other people in their timeslot
				for (int j = 0; j < state.numberPeople; j++){
					if (existingRelations.count(state.peopleInRoom[j].personID) == 0){
						//If a person in the room is not recorded they are added to the person's list of relationships
						PersonInfo relation = state.peopleInRoom[j];
						//Information on the other person is recorded as a PersonRelationInfo struct
						PersonRelationInfo pr;
						string ID = state.peopleInRoom[j].personID;
						if (j != i){
							bool found = false;
							for (int l = 0; l < person.relationships.size(); l++){
							
								if (person.relationships[l].PersonID.compare(state.peopleInRoom[j].personID) == 0){
									found = true;
									pr.relationshipType = person.relationships[l].Relationship_type;
									break;
								}
							}
							assert(found == true);
							pr.startTime = relation.timeEntering;
							pr.endTime = pr.startTime + relation.minsUntilLeaving;
							pr.totalTime = relation.minsUntilLeaving;
							
							existingRelations.insert(pair<string,PersonRelationInfo>(ID, pr));
						}
					}
				}
			} else {
				//If they have not shared the room with anyone yet in their timeslot
				for (int j = 0; j < state.numberPeople; j++){
					if (j != i){
						//All other people in the room are added
						PersonInfo relation = state.peopleInRoom[j];
						PersonRelationInfo pr;
						string ID = state.peopleInRoom[j].personID;
						bool found = false;
						for (int l = 0; l < person.relationships.size(); l++){
							
							if (person.relationships[l].PersonID.compare(ID) == 0){
								found = true;
								pr.relationshipType = person.relationships[l].Relationship_type;
								break;
							}
						}
						assert(found == true);
						pr.startTime = relation.timeEntering;
						pr.endTime = pr.startTime + relation.minsUntilLeaving;
						pr.totalTime = relation.minsUntilLeaving;
							
						existingRelations.insert(pair<string,PersonRelationInfo>(ID, pr));
					}
				}
			}
			state.relationshipsInRoom[person.personID] = existingRelations;
			
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
		
		/*string relationships = "";
		for (int j = 0; j < i.peopleInRoom.size(); j++){
			const string ID = i.peopleInRoom[j].personID;
			relationships += "person" + ID + ": ";
			map<string, PersonRelationInfo> r;
			r = i.relationshipsInRoom.at(ID);
			map<string, PersonRelationInfo>::iterator iter;
			for (iter = r.begin(); iter != r.end(); iter++){
				string relationID = iter->first;
				relationships += relationID;
				if (iter != prev(r.end())){
					relationships += ", ";
				}
			}
			relationships += "\n";
		}*/		
        os << "Number of People in room: "<< i.numberPeople <<" number of people wearing masks: " << i.numberPeopleWearingMasksCorrectly << " Number of people social distancing: " << i.socialDistancing << " Number of sick people: " << i.numberSickPeople << " CO2 concentration in the room: " << i.CO2Concentration << " people ID's in room: " << people << " students travelling at same time: " << i.peopleLeaving.size() << "\n ";

        return os;
    }
           
    #endif // room.hpp
};
