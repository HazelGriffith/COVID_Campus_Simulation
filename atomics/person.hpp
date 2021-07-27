#ifndef Person_HPP
#define Person_HPP


#include <cadmium/modeling/ports.hpp>
#include <cadmium/modeling/message_bag.hpp>

#include <limits>
#include <assert.h>
#include <string> 
#include <stdlib.h>
#include <random>
#include <iostream>
#include <math.h>

#include "../data_structures/ProbGetSick.hpp"
#include "../data_structures/PersonInfo.hpp"
#include "../data_structures/DecisionMakerBehaviour.hpp"

#include "../data/tinyXML/tinystr.h"
#include "../data/tinyXML/tinyxml.h"

using namespace cadmium;
using namespace std;
using namespace decision_maker_behaviour_structures;


//Port definition
//definition of input/output events 
struct Person_ports{
    struct nextDestination : public out_port<PersonInfo>{};
    struct infectionProb : public in_port<ProbGetSick>{};
};

template <typename TIME> class Person{
    public: 

	DecisionMakerBehaviour person;

    using input_ports=std::tuple<typename Person_ports::infectionProb>;
    using output_ports=std::tuple<typename Person_ports::nextDestination>;

    //State Definition
    struct state_type {
		int timeRemaining;
		PersonInfo travelInfo;
    };
    state_type state;
   
    //default constructor
    Person () noexcept{}
	
    Person (const char * personFileName) noexcept{
		person.load(personFileName);
		//The person's nextLocation object stopped automatically initializing itself, so that is now done here manually
		LocationPlan lp;
		person.nextLocation = lp;
		//time interval is set to 0 such that the PersonInfo message saying they are entering the first room is sent immediately
		state.timeRemaining = 0;
		//The PersonInfo message that initializes the person's first location is created here.
		state.travelInfo = PersonInfo(person.ID, person.isSick, person.wearingMask, person.location, "" , person.socialDistance, person.timeInFirstLocation);
    }

    //internal transition function
    void internal_transition(){
		//sets up the person to output the correct PersonInfo object at the next time interval
		
		if (person.nextLocation.isEmpty()){
			//person.setNextLocation() uses the current start time and time remaining to find the next location
			//because the initial time remaining is set to 0, it will not correctly find the next location
			//Therefore, if the person is using the initial time of 0, the DecisionMakerBehaviour object person's nextLocation
			//variable will be empty and cannot give the correct time remaining. So the first time remaining value is loaded
			//from the person xml file and initialized here
			state.timeRemaining = person.timeInFirstLocation;
		} else {
			state.timeRemaining = person.nextLocation.timeInRoomMin;
		}
		
		//sets the DecisionMakerBehaviour person's nextLocation object to be equal to the LocationPlan object 
		//with a start time equal to the person's current start time + the state's time remaining
		person.setNextLocation(state.timeRemaining);
		
		person.currStartTime = person.nextLocation.startTime;
		
		//The PersonInfo object is updated for the next time interval output
		state.travelInfo.roomIDEntering = person.nextLocation.roomID;
		state.travelInfo.roomIDLeaving = person.location;
		state.travelInfo.minsUntilLeaving = person.nextLocation.timeInRoomMin;
		
		person.location = person.nextLocation.roomID;
    }


    //external transition function
    void external_transition(TIME e, typename make_message_bags<input_ports>::type mbs){
		
		vector<ProbGetSick> msgBag = get_messages<typename Person_ports::infectionProb>(mbs);
		
        for (int i = 0 ; i < get_messages<typename Person_ports::infectionProb>(mbs).size() ; i++ ){
			//upon receiving a ProbGetSick object from a Room model, a random number from 0 to 100 is taken
			int r = rand() % 101;
			//if that number is less than the probability of being sick from the ProbGetSick object, then the person is now sick
			if (r <= (msgBag[i].probSick*100)){
				person.isSick = true;
				state.travelInfo.isSick = true;
			}
        }
    }

    // confluence transition
    void confluence_transition(TIME e, typename make_message_bags<input_ports>::type mbs) {
        internal_transition();
        external_transition(TIME(), std::move(mbs));
    }

    //output function
    typename make_message_bags<output_ports>::type output() const {
		typename make_message_bags<output_ports>::type bags;
		
        get_messages<typename Person_ports::nextDestination>(bags).push_back(state.travelInfo);
        
		return bags;
    }


    TIME time_advance() const{
        TIME next_internal;

		// time remaining is in minutes, so it is split into hours and minutes to construct an NDTime object
        int hours = floor(state.timeRemaining/60);

        next_internal = TIME({hours, state.timeRemaining - (hours*60)});
        
        return next_internal;

    };

    friend std::ostringstream& operator<<(std::ostringstream& os, const typename Person<TIME>::state_type& i) {
        os << "Person ID: " << i.travelInfo.personID << " isSick: " << i.travelInfo.isSick << " wearsMaskCorrectly: " << i.travelInfo.wearsMaskCorrectly << " the next room is: " << i.travelInfo.roomIDEntering << " the current room is: " << i.travelInfo.roomIDLeaving << " socialDistance: " << i.travelInfo.socialDistance << " staying in the next room for " << i.travelInfo.minsUntilLeaving <<" minutes" << "; "; 
		os << "\n";
        return os;
    }

};
#endif