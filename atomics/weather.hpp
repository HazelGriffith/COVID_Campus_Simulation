#ifndef Weather_HPP
#define Weather_HPP


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

#include "../data/tinyXML/tinystr.h"
#include "../data/tinyXML/tinyxml.h"

using namespace cadmium;
using namespace std;
using namespace decision_maker_behaviour_structures;


//Port definition
//definition of input/output events 
struct Weather_ports{
	struct weatherUpdatesIn : public in_port<WeatherInfo> {};
    struct weatherUpdates : public out_port<WeatherInfo>{};
};

template <typename TIME> class Weather{
    public: 

	using input_ports = std::tuple<typename Weather_ports::weatherUpdatesIn>;
    using output_ports=std::tuple<typename Weather_ports::weatherUpdates>;

    //State Definition
    struct state_type {
		int timeRemaining;
		WeatherInfo weatherValue;
    };
    state_type state;
	
	//default constructor
	Weather() noexcept{
		//time interval is set to 0 such that the WeatherInfo message for the first day is sent immediately
		state.timeRemaining = 0;
		//The WeatherInfo message that initializes the initial weather is created here.
		state.weatherValue = WeatherInfo(0,1);
    }

    //internal transition function
    void internal_transition(){
		//change the new state of the weather to a random number between 0-10
		//the old state becomes the previous state
		state.weatherValue = WeatherInfo(state.weatherValue.newState, rand() % 11);

		//set one day until next weather change
		state.timeRemaining = 1440;
    }

	void external_transition(TIME e, typename make_message_bags<input_ports>::type mbs) {}

	void confluence_transition(TIME e, typename make_message_bags<input_ports>::type mbs) {
		internal_transition();
		external_transition(TIME(), std::move(mbs));
	}

    //output function
    typename make_message_bags<output_ports>::type output() const {

		typename make_message_bags<output_ports>::type bags;
		
        get_messages<typename Weather_ports::weatherUpdates>(bags).push_back(state.weatherValue);
        
		return bags;
    }


    TIME time_advance() const{
        TIME next_internal;

		// time remaining is in minutes, so it is split into hours and minutes to construct an NDTime object
        int hours = floor(state.timeRemaining/60);

        next_internal = TIME({hours, state.timeRemaining - (hours*60)});
        
        return next_internal;

    };

    friend std::ostringstream& operator<<(std::ostringstream& os, const typename Weather<TIME>::state_type& i) {
        os << "Weather Value: " << i.weatherValue.newState << " Prev Value: " << i.weatherValue.prevState;
		os << "\n";
        return os;
    }

};
#endif