#ifndef Filter_People_Out_HPP
#define Filter_People_Out_HPP


#include <cadmium/modeling/ports.hpp>
#include <cadmium/modeling/message_bag.hpp>

#include <limits>
#include <assert.h>
#include <string> 
#include <stdlib.h>
#include <random>
#include <iostream>

#include "../data_structures/PersonInfo.hpp"

using namespace cadmium;
using namespace std;


//Port definition
//definition of input/output events 
struct Filter_People_Out_ports{
    struct outFromFilter : public out_port<PersonInfo>{};
    struct inToFilter : public in_port<PersonInfo>{};
};

template <typename TIME> class Filter_People_Out{
    public: 

    string roomID;

    using input_ports=std::tuple<typename Filter_People_Out_ports::inToFilter>;
    using output_ports=std::tuple<typename Filter_People_Out_ports::outFromFilter>;

    //State Definition
    struct state_type {

    	vector<PersonInfo> msgs_passing_filter;   
    };
    state_type state;
   
    //default constructor
    Filter_People_Out () noexcept{}
	
    Filter_People_Out (string inputRoomID) noexcept{
		roomID = inputRoomID;        
		state.msgs_passing_filter.clear();
    }

    //internal transition function
    void internal_transition(){
		//clear message passing through the filter 
		state.msgs_passing_filter.clear();
    }


    //external transition function
    void external_transition(TIME e, typename make_message_bags<input_ports>::type mbs){
        for (int i = 0 ; i < get_messages<typename Filter_People_Out_ports::inToFilter>(mbs).size() ; i++ ){
			
			//Checks if the room ID and the ID of the person entering match. If they do the person is allowed through, and blocked otherwise
            if(roomID.compare(get_messages<typename Filter_People_Out_ports::inToFilter>(mbs)[i].roomIDLeaving) == 0){
                state.msgs_passing_filter.push_back(get_messages<typename Filter_People_Out_ports::inToFilter>(mbs)[i]);
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
 
        for (int i = 0; i < state.msgs_passing_filter.size(); i++){
            get_messages<typename Filter_People_Out_ports::outFromFilter>(bags).push_back(state.msgs_passing_filter[i]);
        }
        return bags;
    }


    TIME time_advance() const{
        TIME next_internal;
        if (state.msgs_passing_filter.empty() == true) {
            next_internal = numeric_limits<TIME>::infinity();
        } else {
            next_internal = TIME(); 
        }

        return next_internal;

    };

    friend std::ostringstream& operator<<(std::ostringstream& os, const typename Filter_People_Out<TIME>::state_type& i) {
        for (int j = 0; j < (i.msgs_passing_filter).size(); j++){
            os << "Person ID: " << i.msgs_passing_filter[j].personID << " Room leaving: " << i.msgs_passing_filter[j].roomIDLeaving << "; "; 
        
        }
		os << "\n";
        return os;
    }

};
#endif
