//Cadmium Simulator headers
#include <cadmium/modeling/ports.hpp>
#include <cadmium/modeling/dynamic_model.hpp>
#include <cadmium/modeling/dynamic_coupled.hpp>
#include <cadmium/modeling/dynamic_model_translator.hpp>
#include <cadmium/engine/pdevs_dynamic_runner.hpp>
#include <cadmium/logger/common_loggers.hpp>

//Time class header
#include <NDTime.hpp>

//XML parser
#include "../data/tinyXML/tinyxml.h"
#include "../data/tinyXML/tinystr.h"

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

struct RoomInfo{
	const char* ID;
	int ventilationRating;
	int socialDistanceThreshold;
	int wearsMaskCorrectlyFactor;
	int socialDistanceFactor;
	int inputRespIncreasePerMin;
};

/***** (3) *****/
int main(){

    /****** Input Readers atomic model instantiation *******************/
    const char * i_input_filterIn_data = "../input_data/inToRoom_input_multiple_test.txt";

    shared_ptr<dynamic::modeling::model> input_filterIn_reader;

    input_filterIn_reader = dynamic::translate::make_dynamic_atomic_model					   
		<InputReader_PersonInfo, TIME, const char*>("input_filterIn_reader", move(i_input_filterIn_data));
		
	const char * i_input_filterOut_data = "../input_data/outFromRoom_input_multiple_test.txt";
	
	shared_ptr<dynamic::modeling::model> input_filterOut_reader;

    input_filterOut_reader = dynamic::translate::make_dynamic_atomic_model					   
		<InputReader_PersonInfo, TIME, const char*>("input_filterOut_reader", move(i_input_filterOut_data));

	/***** (4) *****/
    /****** room atomic model instantiation *******************/
    RoomInfo room1Info;
	RoomInfo room2Info;
	
	
	//loading XML document
	TiXmlDocument _document;
    
	bool result = _document.LoadFile("../data/Room1.xml");
	
	if (!result) {cout << "file not loaded " << _document.ErrorDesc() << endl; return 0;} 
	
	//acquiring root of XML document
	TiXmlHandle hDoc(&_document);
    TiXmlElement* pElem;
    TiXmlHandle hRoot(0);
	
	pElem=hDoc.FirstChildElement().ToElement();
       
    if (!pElem) return 0;    
        
    hRoot=TiXmlHandle(pElem);
	
	pElem=hRoot.FirstChild("ID").ToElement();
    if (!pElem) return 0;
    room1Info.ID = pElem->GetText();
	cout << *room1Info.ID;
	
	pElem = hRoot.FirstChild("ventilationRating").ToElement();
	if (!pElem) return 0;
	const char* pVentilationRating = pElem->GetText();
	room1Info.ventilationRating = strtol(pVentilationRating, NULL, 10);
	cout << room1Info.ventilationRating;
	
	pElem = hRoot.FirstChild("socialDistanceThreshold").ToElement();
	if (!pElem) return 0;
	const char* pSocialDistanceThreshold = pElem->GetText();
	room1Info.socialDistanceThreshold = strtol(pSocialDistanceThreshold, NULL, 10);
	cout << room1Info.socialDistanceThreshold;
	
	pElem = hRoot.FirstChild("wearsMaskCorrectlyFactor").ToElement();
	if (!pElem) return 0;
	const char* pWearsMaskCorrectlyFactor = pElem->GetText();
	room1Info.wearsMaskCorrectlyFactor = strtol(pWearsMaskCorrectlyFactor, NULL, 10);
	cout << room1Info.wearsMaskCorrectlyFactor;
	
	pElem = hRoot.FirstChild("socialDistanceFactor").ToElement();
	if (!pElem) return 0;
	const char* pSocialDistanceFactor = pElem->GetText();
	room1Info.socialDistanceFactor = strtol(pSocialDistanceFactor, NULL, 10);
	cout << room1Info.socialDistanceFactor;
	
	pElem = hRoot.FirstChild("inputRespIncreasePerMin").ToElement();
	if (!pElem) return 0;
	const char* pInputRespIncreasePerMin = pElem->GetText();
	room1Info.inputRespIncreasePerMin = strtol(pInputRespIncreasePerMin, NULL, 10);
	cout << room1Info.inputRespIncreasePerMin;
	
	const char * i_roomID1 = "Room1";
	
	const char * i_roomID2 = "Room2";
	
	const int i_ventilationRating = room1Info.ventilationRating;
	
	const int i_socialDistanceThreshold = 1;
	
	const int i_wearsMaskCorrectlyFactor = 3;
	
	const int i_socialDistanceFactor = 4;
	
	const int i_inputRespIncreasePerMin = 5;
	
	shared_ptr<dynamic::modeling::model> room1;
	
    room1 = dynamic::translate::make_dynamic_atomic_model<RoomModel, TIME>("room1", move(room1Info.ID), 
		room1Info.ventilationRating, room1Info.socialDistanceThreshold, room1Info.wearsMaskCorrectlyFactor, room1Info.socialDistanceFactor, room1Info.inputRespIncreasePerMin);
		
	shared_ptr<dynamic::modeling::model> room2;
	
    room2 = dynamic::translate::make_dynamic_atomic_model<RoomModel, TIME>("room2", move(i_roomID2), 
		i_ventilationRating, i_socialDistanceThreshold, i_wearsMaskCorrectlyFactor, i_socialDistanceFactor, i_inputRespIncreasePerMin);
		
		
    /****** filter atomic model instantiation *******************/
	
	shared_ptr<dynamic::modeling::model> filterIn1;
	
    filterIn1 = dynamic::translate::make_dynamic_atomic_model<Filter_People_In, TIME>("filterIn1", move(i_roomID1));
	
	shared_ptr<dynamic::modeling::model> filterOut1;
	
    filterOut1 = dynamic::translate::make_dynamic_atomic_model<Filter_People_Out, TIME>("filterOut1", move(i_roomID1));
	
	shared_ptr<dynamic::modeling::model> filterIn2;
	
    filterIn2 = dynamic::translate::make_dynamic_atomic_model<Filter_People_In, TIME>("filterIn2", move(i_roomID2));
	
	shared_ptr<dynamic::modeling::model> filterOut2;
	
    filterOut2 = dynamic::translate::make_dynamic_atomic_model<Filter_People_Out, TIME>("filterOut2", move(i_roomID2));
	
	/***** (5) *****/
    /*******TOP MODEL********/
    dynamic::modeling::Ports iports_TOP;
    iports_TOP = {};

    dynamic::modeling::Ports oports_TOP;
    oports_TOP = {typeid(top_out)};

    dynamic::modeling::Models submodels_TOP;
    submodels_TOP = {input_filterIn_reader, input_filterOut_reader, room1, room2, filterIn1, filterOut1, filterIn2, filterOut2};

    dynamic::modeling::EICs eics_TOP;
    eics_TOP = {};  // _EIC WOULD GO HERE ; NOT NEEDED BECAUSE IT IS EMTPY IN THIS EXAMPLE

    dynamic::modeling::EOCs eocs_TOP;
    eocs_TOP = {
        dynamic::translate::make_EOC<RoomModelPorts::out,top_out>("room1"),
		dynamic::translate::make_EOC<RoomModelPorts::out,top_out>("room2")
    };

    dynamic::modeling::ICs ics_TOP;
    ics_TOP = {
        dynamic::translate::make_IC<iestream_input_defs<PersonInfo>::out,Filter_People_In_ports::inToFilter>("input_filterIn_reader","filterIn1"),
		dynamic::translate::make_IC<iestream_input_defs<PersonInfo>::out,Filter_People_Out_ports::inToFilter>("input_filterOut_reader","filterOut1"),
		dynamic::translate::make_IC<iestream_input_defs<PersonInfo>::out,Filter_People_In_ports::inToFilter>("input_filterIn_reader","filterIn2"),
		dynamic::translate::make_IC<iestream_input_defs<PersonInfo>::out,Filter_People_Out_ports::inToFilter>("input_filterOut_reader","filterOut2"),
		dynamic::translate::make_IC<Filter_People_In_ports::outFromFilter,RoomModelPorts::inToRoom>("filterIn1","room1"),
		dynamic::translate::make_IC<Filter_People_Out_ports::outFromFilter,RoomModelPorts::outFromRoom>("filterOut1","room1"),
		dynamic::translate::make_IC<Filter_People_In_ports::outFromFilter,RoomModelPorts::inToRoom>("filterIn2","room2"),
		dynamic::translate::make_IC<Filter_People_Out_ports::outFromFilter,RoomModelPorts::outFromRoom>("filterOut2","room2")
    };

    shared_ptr<dynamic::modeling::coupled<TIME>> TOP;

    TOP = make_shared<dynamic::modeling::coupled<TIME>>(
             "TOP", submodels_TOP, iports_TOP, oports_TOP, eics_TOP, eocs_TOP, ics_TOP 
    );


	/***** (6) *****/
    /*************** Loggers *******************/
    static ofstream out_ProbGetSick("../simulation_results/multiple_room_simulation_test_output_ProbGetSick.txt");

    struct oss_sink_ProbGetSick{
        static ostream& sink(){          
            return out_ProbGetSick;
        }
    };

    static ofstream out_state("../simulation_results/multiple_room_simulation_test_output_state.txt");

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
