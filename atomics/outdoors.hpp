#ifndef OutdoorsModelHPP
#define OutdoorsModelHPP

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
struct OutdoorsModelPorts{
    struct inToOutdoors : public in_port<PersonInfo>{};
    struct outFromOutdoors : public in_port<PersonInfo>{};
    struct out : public out_port<ProbGetSick>{};
};

template<typename TIME> class OutdoorsModel{
  public:
    
    string outdoorsID;

    using input_ports=std::tuple<typename OutdoorsModelPorts::inToOutdoors, OutdoorsModelPorts::outFromOutdoors>;
    using output_ports=std::tuple<typename OutdoorsModelPorts::out>;


    //State definition 
    struct state_type{
		
        int numberPeople;
        int numberPeopleWearingMasksCorrectly;
        int socialDistancing;
		int numberSickPeople;
		vector<PersonInfo> peopleInOutdoors;
		vector<ProbGetSick> peopleLeaving;
    }; state_type state;

    //default constructor
    OutdoorsModel () noexcept{
		
    }
	
    OutdoorsModel (string i_outdoorsID) noexcept{
        //initializing model parameters
		state.numberPeople = 0;
		
        state.numberPeopleWearingMasksCorrectly = 0;
        state.socialDistancing = 0;
        state.numberSickPeople = 0;
		state.peopleInOutdoors.clear();
		state.peopleLeaving.clear();
		
		outdoorsID = i_outdoorsID;
		
    }

    //clear peopleLeaving
    void internal_transition() {
        state.peopleLeaving.clear();
    }


    //update Outdoors with new occupants and new CO2 concentration
    void external_transition(TIME e, typename make_message_bags<input_ports>::type mbs){
        
		
        vector<PersonInfo> msgBagInToOutdoors = get_messages<typename OutdoorsModelPorts::inToOutdoors>(mbs);
        vector<PersonInfo> msgBagOutFromOutdoors = get_messages<typename OutdoorsModelPorts::outFromOutdoors>(mbs);
        
        //Entities going in to the outdoors
        for(int i = 0 ; i < msgBagInToOutdoors.size(); i++ ){
            PersonInfo msgInToOutdoors = msgBagInToOutdoors[i];
            
            //verify that a person entering does not match an existing person in the outdoors
            for (int k = 0; k < state.peopleInOutdoors.size(); k++){
                assert(msgInToOutdoors.personID.compare(state.peopleInOutdoors[k].personID) != 0);
			}
			
			state.peopleInOutdoors.push_back(msgInToOutdoors);
		}
		
        //Entities coming out of the outdoors
        for(int s = 0 ; s < msgBagOutFromOutdoors.size(); s++){ 
            PersonInfo msgOutFromOutdoors = msgBagOutFromOutdoors[s];
            bool personFound = false;
		    //checks if the person is in the outdoors
		    for (int k = 0; k < state.peopleInOutdoors.size(); k++){
				
				//If the person is found they are removed from the peopleInOutdoors list, the probability of them getting sick is calculated,
				//and lastly they are added to the list of people who are leaving the outdoors
                if(msgOutFromOutdoors.personID.compare(state.peopleInOutdoors[k].personID) == 0){
					personFound = true;
					
					state.peopleInOutdoors.erase(state.peopleInOutdoors.begin()+k);
					
					ProbGetSick personLeaving;
					
					personLeaving.personID = msgOutFromOutdoors.personID;
					
					personLeaving.probSick = 0;
					
					state.peopleLeaving.push_back(personLeaving);
					
					break;
				}
			}
			
			//If the person is not in the outdoors, an error is thrown
			assert(personFound == true);
        }
		
		//The state of the Outdoors is updated
		state.numberPeople = state.peopleInOutdoors.size();
		state.numberPeopleWearingMasksCorrectly = 0;
		state.socialDistancing = 0;
		state.numberSickPeople = 0;
		for (int i = 0; i < state.numberPeople; i++){
			PersonInfo person = state.peopleInOutdoors[i];
			
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
			get_messages<typename OutdoorsModelPorts::out>(bags).push_back(state.peopleLeaving[i]);
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
		cout << "Outdoors timeAdvance return statement" << endl;
		return nextInternal;
    };
    
    friend std::ostringstream& operator<<(std::ostringstream& os, const typename OutdoorsModel<TIME>::state_type& i){
		string people = "";
		for (int j = 0; j < i.peopleInOutdoors.size(); j++){
			people += i.peopleInOutdoors[j].personID;
			if (j != i.peopleInOutdoors.size()-1){
				people += ",";
			}
		}
        os << "Number of People in outdoors: "<< i.numberPeople <<" number of people wearing masks: " << i.numberPeopleWearingMasksCorrectly << " Number of people social distancing: " << i.socialDistancing << " Number of sick people: " << i.numberSickPeople << " people ID's in room: " << people <<"\n ";
        
        return os;
    }
           
    #endif // outdoors.hpp
};
