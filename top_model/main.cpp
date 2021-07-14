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

//Atomic model headers
#include "../atomics/room.hpp"
#include "../atomics/filter_People_In.hpp"
#include "../atomics/filter_People_Out.hpp"
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
struct top_out: public out_port<ProbGetSick>{};

/***** (2) *****/
/****** Input Reader atomic model declaration *******************/

template<typename T>
class InputReader_PersonInfo : public iestream_input<PersonInfo, T> {

    public:
        InputReader_PersonInfo () = default;
        InputReader_PersonInfo (const char* file_path) : 
				iestream_input<PersonInfo,T> (file_path) {}
};

/***** (3) *****/
int main(){

    /****** Input Readers atomic model instantiation *******************/
    const char * i_input_filterIn_data = "../input_data/inToRoom_input_test.txt";

    shared_ptr<dynamic::modeling::model> input_filterIn_reader;

    input_filterIn_reader = dynamic::translate::make_dynamic_atomic_model					   
		<InputReader_PersonInfo, TIME, const char*>("input_filterIn_reader", move(i_input_filterIn_data));
		
	const char * i_input_filterOut_data = "../input_data/outFromRoom_input_test.txt";
	
	shared_ptr<dynamic::modeling::model> input_filterOut_reader;

    input_filterOut_reader = dynamic::translate::make_dynamic_atomic_model					   
		<InputReader_PersonInfo, TIME, const char*>("input_filterOut_reader", move(i_input_filterOut_data));

	/***** (4) *****/
    /****** room atomic model instantiation *******************/
    const char * i_roomID = "Room5";
	
	const int i_ventilationRating = 2;
	
	const int i_socialDistanceThreshold = 3;
	
	const int i_wearsMaskCorrectlyFactor = 4;
	
	const int i_socialDistancingFactor = 3;
	
	const int i_inputRespIncreasePerMin = 2;
	
	shared_ptr<dynamic::modeling::model> room5;
	
    room5 = dynamic::translate::make_dynamic_atomic_model<RoomModel, TIME>("room5", move(i_roomID), 
		i_ventilationRating, i_socialDistanceThreshold, i_wearsMaskCorrectlyFactor, i_socialDistancingFactor, i_inputRespIncreasePerMin);
		
    /****** filter atomic model instantiation *******************/
	
	shared_ptr<dynamic::modeling::model> filterIn;
	
    filterIn = dynamic::translate::make_dynamic_atomic_model<Filter_People_In, TIME>("filterIn", move(i_roomID));
	
	shared_ptr<dynamic::modeling::model> filterOut;
	
    filterOut = dynamic::translate::make_dynamic_atomic_model<Filter_People_Out, TIME>("filterOut", move(i_roomID));
	
	/***** (5) *****/
    /*******TOP MODEL********/
    dynamic::modeling::Ports iports_TOP;
    iports_TOP = {};

    dynamic::modeling::Ports oports_TOP;
    oports_TOP = {typeid(top_out)};

    dynamic::modeling::Models submodels_TOP;
    submodels_TOP = {input_filterIn_reader, input_filterOut_reader, room5, filterIn, filterOut};

    dynamic::modeling::EICs eics_TOP;
    eics_TOP = {};  // _EIC WOULD GO HERE ; NOT NEEDED BECAUSE IT IS EMTPY IN THIS EXAMPLE

    dynamic::modeling::EOCs eocs_TOP;
    eocs_TOP = {
        dynamic::translate::make_EOC<RoomModelPorts::out,top_out>("room5")
    };

    dynamic::modeling::ICs ics_TOP;
    ics_TOP = {
        dynamic::translate::make_IC<iestream_input_defs<PersonInfo>::out,Filter_People_In_ports::inToFilter>("input_filterIn_reader","filterIn"),
		dynamic::translate::make_IC<iestream_input_defs<PersonInfo>::out,Filter_People_Out_ports::inToFilter>("input_filterOut_reader","filterOut"),
		dynamic::translate::make_IC<Filter_People_In_ports::outFromFilter,RoomModelPorts::inToRoom>("filterIn","room5"),
		dynamic::translate::make_IC<Filter_People_Out_ports::outFromFilter,RoomModelPorts::outFromRoom>("filterOut","room5")
    };

    shared_ptr<dynamic::modeling::coupled<TIME>> TOP;

    TOP = make_shared<dynamic::modeling::coupled<TIME>>(
             "TOP", submodels_TOP, iports_TOP, oports_TOP, eics_TOP, eocs_TOP, ics_TOP 
    );


	/***** (6) *****/
    /*************** Loggers *******************/
    static ofstream out_ProbGetSick("../simulation_results/simulation_test_output_ProbGetSick.txt");

    struct oss_sink_ProbGetSick{
        static ostream& sink(){          
            return out_ProbGetSick;
        }
    };

    static ofstream out_state("../simulation_results/simulation_test_output_state.txt");

    struct oss_sink_state{
        static ostream& sink(){          
            return out_state;
        }
    };


    using state = logger::logger<logger::logger_state, dynamic::logger::formatter<TIME>, oss_sink_state>;

    using log_PersonInfo = logger::logger<logger::logger_messages, dynamic::logger::formatter<TIME>, oss_sink_ProbGetSick>;

    using global_time_PIs = logger::logger<logger::logger_global_time, dynamic::logger::formatter<TIME>, oss_sink_ProbGetSick>;

    using global_time_sta = logger::logger<logger::logger_global_time, dynamic::logger::formatter<TIME>, oss_sink_state>;

    using logger_top = logger::multilogger<state, log_PersonInfo, global_time_PIs, global_time_sta>;

    /***** (7) *****/
    /************** Runner call ************************/
    dynamic::engine::runner<NDTime, logger_top> r(TOP, {0});
    r.run_until(NDTime("04:00:00:000"));
    return 0;
}
