#ifndef Filter_ProbGetSick_HPP
#define Filter_ProbGetSick_HPP


#include <cadmium/modeling/ports.hpp>
#include <cadmium/modeling/message_bag.hpp>

#include <limits>
#include <assert.h>
#include <string> 
#include <stdlib.h>
#include <random>
#include <iostream>

#include "../data_structures/ProbGetSick.hpp"

using namespace cadmium;
using namespace std;


//Port definition
//definition of input/output events 
struct Filter_ProbGetSick_ports{
    struct outFromFilter : public out_port<ProbGetSick>{};
    struct inToFilter : public in_port<ProbGetSick>{};
};

template <typename TIME> class Filter_ProbGetSick{
    public: 

    string personID;

    using input_ports=std::tuple<typename Filter_ProbGetSick_ports::inToFilter>;
    using output_ports=std::tuple<typename Filter_ProbGetSick_ports::outFromFilter>;

    //State Definition
    struct state_type {

    	vector<ProbGetSick> msgs_passing_filter;   
    };
    state_type state;
   
    //default constructor
    Filter_ProbGetSick () noexcept{}
	
    Filter_ProbGetSick (string inputPersonID) noexcept{
		personID = inputPersonID;        
		state.msgs_passing_filter.clear();
    }

    //internal transition function
    void internal_transition(){
		//clear message passing through the filter 
		state.msgs_passing_filter.clear();
    }


    //external transition function
    void external_transition(TIME e, typename make_message_bags<input_ports>::type mbs){
        for (int i = 0 ; i < get_messages<typename Filter_ProbGetSick_ports::inToFilter>(mbs).size() ; i++ ){
           
		    //Checks if the personID of the filter and the personID of the ProbGetSick object match. If they do the ProbGetSick is allowed through, and blocked otherwise
            if(personID.compare(get_messages<typename Filter_ProbGetSick_ports::inToFilter>(mbs)[i].personID) == 0){
                state.msgs_passing_filter.push_back(get_messages<typename Filter_ProbGetSick_ports::inToFilter>(mbs)[i]);
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
            get_messages<typename Filter_ProbGetSick_ports::outFromFilter>(bags).push_back(state.msgs_passing_filter[i]);
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

    friend std::ostringstream& operator<<(std::ostringstream& os, const typename Filter_ProbGetSick<TIME>::state_type& i) {
        for (int j = 0; j < (i.msgs_passing_filter).size(); j++){
            os << "Person ID: " << i.msgs_passing_filter[j].personID << " Infection Probability: " << i.msgs_passing_filter[j].probSick << "; "; 
        }
		os << "\n";
        return os;
    }

};
#endif