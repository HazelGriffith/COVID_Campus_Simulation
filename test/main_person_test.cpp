//Cadmium Simulator headers
#include <cadmium/modeling/ports.hpp>
#include <cadmium/modeling/dynamic_model.hpp>
#include <cadmium/modeling/dynamic_coupled.hpp>
#include <cadmium/modeling/dynamic_model_translator.hpp>
#include <cadmium/engine/pdevs_dynamic_runner.hpp>
#include <cadmium/logger/common_loggers.hpp>

//Time class header
#include <NDTime.hpp>

//Messages structures
#include "../data_structures/PersonInfo.hpp"
#include "../data_structures/ProbGetSick.hpp"
#include "../data_structures/DecisionMakerBehaviour.hpp"

//Atomic model headers
#include "../atomics/person.hpp"
#include <cadmium/basic_model/pdevs/iestream.hpp> //Atomic model for inputs  

//C++ libraries
#include <iostream>
#include <string>

using namespace std;
using namespace cadmium;
using namespace cadmium::basic_models::pdevs;

using TIME = NDTime;

/***** (1) *****/
/***** Define input port for coupled models *****/

/***** Define output ports for coupled model *****/
struct top_out: public out_port<PersonInfo>{};

/***** (2) *****/
/****** Input Reader atomic model declaration *******************/

template<typename T>
class InputReader_ProbGetSick : public iestream_input<ProbGetSick, T> {

    public:
        InputReader_ProbGetSick () = default;
        InputReader_ProbGetSick (const char* file_path) : 
				iestream_input<ProbGetSick,T> (file_path) {}
};

/***** (3) *****/
int main(){

    /****** Input Reader atomic model instantiation *******************/
    const char * i_input_data = "../input_data/probGetSickInputTest.txt";

    shared_ptr<dynamic::modeling::model> input_reader;

    input_reader = dynamic::translate::make_dynamic_atomic_model					   
		<InputReader_ProbGetSick, TIME, const char*>("input_reader", move(i_input_data));

	/***** (4) *****/
    /****** filter_ProbGetSick atomic model instantiation *******************/
    const char * personFileName = "../data/people/TestPerson1.xml";
	
	shared_ptr<dynamic::modeling::model> person;
	
    person = dynamic::translate::make_dynamic_atomic_model<Person, TIME>("person", move(personFileName));
	
	
	/***** (5) *****/
    /*******TOP MODEL********/
    dynamic::modeling::Ports iports_TOP;
    iports_TOP = {};

    dynamic::modeling::Ports oports_TOP;
    oports_TOP = {typeid(top_out)};

    dynamic::modeling::Models submodels_TOP;
    submodels_TOP = {input_reader, person};

    dynamic::modeling::EICs eics_TOP;
    eics_TOP = {};  // _EIC WOULD GO HERE ; NOT NEEDED BECAUSE IT IS EMTPY IN THIS EXAMPLE

    dynamic::modeling::EOCs eocs_TOP;
    eocs_TOP = {
        dynamic::translate::make_EOC<Person_ports::nextDestination,top_out>("person")
    };

    dynamic::modeling::ICs ics_TOP;
    ics_TOP = {
        dynamic::translate::make_IC<iestream_input_defs<ProbGetSick>::out,Person_ports::infectionProb>(
                                           "input_reader","person")
    };

    shared_ptr<dynamic::modeling::coupled<TIME>> TOP;

    TOP = make_shared<dynamic::modeling::coupled<TIME>>(
             "TOP", submodels_TOP, iports_TOP, oports_TOP, eics_TOP, eocs_TOP, ics_TOP 
    );


	/***** (6) *****/
    /*************** Loggers *******************/
    static ofstream out_ProbGetSick("../simulation_results/person_test_output_ProbGetSick.txt");

    struct oss_sink_ProbGetSick{
        static ostream& sink(){          
            return out_ProbGetSick;
        }
    };

    static ofstream out_state("../simulation_results/person_test_output_state.txt");

    struct oss_sink_state{
        static ostream& sink(){          
            return out_state;
        }
    };


    using state = logger::logger<logger::logger_state, dynamic::logger::formatter<TIME>, oss_sink_state>;

    using log_ProbGetSick = logger::logger<logger::logger_messages, dynamic::logger::formatter<TIME>, oss_sink_ProbGetSick>;

    using global_time_PIs = logger::logger<logger::logger_global_time, dynamic::logger::formatter<TIME>, oss_sink_ProbGetSick>;

    using global_time_sta = logger::logger<logger::logger_global_time, dynamic::logger::formatter<TIME>, oss_sink_state>;

    using logger_top = logger::multilogger<state, log_ProbGetSick, global_time_PIs, global_time_sta>;

    /***** (7) *****/
    /************** Runner call ************************/
    dynamic::engine::runner<NDTime, logger_top> r(TOP, {0});
    r.run_until(NDTime("27:00:00:000"));
    return 0;
}
