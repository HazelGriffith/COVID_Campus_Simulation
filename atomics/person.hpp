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
#include <chrono>

#include "../data_structures/WeatherInfo.hpp"
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
	struct weatherUpdates : public in_port<WeatherInfo>{};
};

template <typename TIME> class Person{
    public: 

	DecisionMakerBehaviour person;

    using input_ports=std::tuple<typename Person_ports::infectionProb, typename Person_ports::weatherUpdates>;
    using output_ports=std::tuple<typename Person_ports::nextDestination>;

    //State Definition
    struct state_type {
		int timeRemaining;
		PersonInfo travelInfo;
		int currentWeather;
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
		//sets current weather (should be immediately overwritten by incoming weather update
		state.currentWeather = 0;
		//The PersonInfo message that initializes the person's first location is created here.
		state.travelInfo = PersonInfo(person.ID, person.isSick, person.exposed, person.vaccinated, person.wearingMask, person.location, "" , person.socialDistance, person.weatherThreshold, (person.timeInFirstLocation + person.currStartTime)%1440);
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
			state.timeRemaining = (person.timeInFirstLocation + person.currStartTime)%1440;
			person.setNextLocation(person.timeInFirstLocation);
		} else {
			state.timeRemaining = person.nextLocation.timeInRoomMin;
			person.setNextLocation(state.timeRemaining);
		}
		person.currStartTime = person.nextLocation.startTime;
		state.travelInfo.roomIDLeaving = person.location;
		state.travelInfo.minsUntilLeaving = person.nextLocation.timeInRoomMin;

		//checks if a person is using the tunnels or outdoors to travel between destinations
		//the person will use outdoors if the current weather is greater or equal to their weatherThreshold value, will use tunnels otherwise
		if (person.nextLocation.roomID == "Outdoors"|| person.nextLocation.roomID == "Tunnels") {
			
			if (state.currentWeather >= state.travelInfo.weatherThreshold) {
				person.nextLocation.roomID = "Outdoors";
			}
			else {
				person.nextLocation.roomID = "Tunnels";
			}
		}

		state.travelInfo.roomIDEntering = person.nextLocation.roomID;
		person.location = person.nextLocation.roomID;
		
    }


    //external transition function
    void external_transition(TIME e, typename make_message_bags<input_ports>::type mbs){

		vector<ProbGetSick> msgBagSick = get_messages<typename Person_ports::infectionProb>(mbs);

		vector<WeatherInfo> msgBagWeather = get_messages<typename Person_ports::weatherUpdates>(mbs);
		
        for (int i = 0 ; i < get_messages<typename Person_ports::infectionProb>(mbs).size() ; i++ ){
			//upon receiving a ProbGetSick object from a Room model, a random number from 0 to 100 is taken
			int r = rand() % 101;
			//if that number is less than the probability of being sick from the ProbGetSick object, then the person is now sick
			if (r <= (msgBagSick[i].probSick)){
				if (!person.isSick){
					person.exposed = true;
					state.travelInfo.exposed = true;
				}
			}
        }

		for (int i = 0; i < get_messages<typename Person_ports::weatherUpdates>(mbs).size(); i++) {

			state.currentWeather = msgBagWeather[i].newState;
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
        os << "Person ID: " << i.travelInfo.personID << " isSick: " << i.travelInfo.isSick << " wearsMaskCorrectly: " << i.travelInfo.wearsMaskCorrectly << " the next room is: " << i.travelInfo.roomIDEntering << " the current room is: " << i.travelInfo.roomIDLeaving << " socialDistance: " << i.travelInfo.socialDistance << " staying in the next room for " << i.travelInfo.minsUntilLeaving <<" minutes Weather: " << i.currentWeather << "; "; 
		os << "\n";
        return os;
    }

};
#endif