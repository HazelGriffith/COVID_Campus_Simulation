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

//XML parser
#include "../data/tinyXML/tinyxml.h"
#include "../data/tinyXML/tinystr.h"

//Atomic model headers
#include "../atomics/room.hpp"
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
    const char * i_input_inToRoom_data = "../input_data/personInfoIntoRoomInputTest.txt";

    shared_ptr<dynamic::modeling::model> input_inToRoom_reader;

    input_inToRoom_reader = dynamic::translate::make_dynamic_atomic_model					   
		<InputReader_PersonInfo, TIME, const char*>("input_inToRoom_reader", move(i_input_inToRoom_data));
		
	const char * i_input_outFromRoom_data = "../input_data/personInfoOutFromRoomInputTest.txt";
	
	shared_ptr<dynamic::modeling::model> input_outFromRoom_reader;

    input_outFromRoom_reader = dynamic::translate::make_dynamic_atomic_model					   
		<InputReader_PersonInfo, TIME, const char*>("input_outFromRoom_reader", move(i_input_outFromRoom_data));

	/***** (4) *****/
    /****** room atomic model instantiation *******************/
	
	string roomID = "TestRoom1";

	
	string roomPathStr = "../data/rooms/" + roomID + ".xml";\
	const char * roomPath = roomPathStr.c_str();
	
	//loading document
	TiXmlDocument _document;
	bool result = _document.LoadFile(roomPath);
	if (!result) {cout << "file not loaded " << _document.ErrorDesc() << endl; return 0;} 
		
		
	//acquiring root of XML document
	TiXmlHandle hDoc(&_document);
	TiXmlElement* pElem;
	TiXmlHandle hRoot(0);
		
	pElem=hDoc.FirstChildElement().ToElement();
	if (!pElem) return 0;    
	hRoot=TiXmlHandle(pElem);
		
	//acquiring ID of the room
	pElem=hRoot.FirstChild("ID").ToElement();
	if (!pElem) return 0;
	string ID = pElem->GetText();
		
	//acquiring ventilation rating of the room
	pElem = hRoot.FirstChild("ventilationRating").ToElement();
	if (!pElem) return 0;
	int ventilationRating = strtol(pElem->GetText(), NULL, 10);

	//acquiring social distance threshold of the room
	pElem = hRoot.FirstChild("socialDistanceThreshold").ToElement();
	if (!pElem) return 0;
	int socialDistanceThreshold = strtol(pElem->GetText(), NULL, 10);
		
	//acquiring maximum occupancy of the room
	pElem = hRoot.FirstChild("maxOccupancy").ToElement();
	if (!pElem) return 0;
	int maxOccupancy = strtol(pElem->GetText(), NULL, 10);
		
	//acquiring wearing mask correctly factor of the room
	pElem = hRoot.FirstChild("wearsMaskFactor").ToElement();
	if (!pElem) return 0;
	int wearsMaskFactor = strtol(pElem->GetText(), NULL, 10);

	//acquiring social distance factor of the room
	pElem = hRoot.FirstChild("socialDistanceFactor").ToElement();
	if (!pElem) return 0;
	int socialDistanceFactor = strtol(pElem->GetText(), NULL, 10);
		
	//acquiring vaccinated factor of the room
	pElem = hRoot.FirstChild("vaccinatedFactor").ToElement();
	if (!pElem) return 0;
	int vaccinatedFactor = strtol(pElem->GetText(), NULL, 10);
		
	//acquiring square metres of the room
	pElem = hRoot.FirstChild("squareMetres").ToElement();
	if (!pElem) return 0;
	int sqrMetres = strtol(pElem->GetText(), NULL, 10);
		
	//acquiring height of the room
	pElem = hRoot.FirstChild("height").ToElement();
	if (!pElem) return 0;
	int height = strtol(pElem->GetText(), NULL, 10);
		
	float volumeOfRoom = sqrMetres*height;
		
	//acquiring sick people CO2 factor of the room
	pElem = hRoot.FirstChild("sickPeopleCO2Factor").ToElement();
	if (!pElem) return 0;
	int sickPeopleCO2Factor = strtol(pElem->GetText(), NULL, 10);
		
	//acquiring high CO2 factor thresholds of the room
	pElem = hRoot.FirstChild("highCO2FactorThresholds").ToElement();
	if (!pElem) return 0;
	string text = pElem->GetText();
	int highCO2FactorThresholds[4];
	for (int j = 0; j < 4; j++){
		size_t seperator = text.find(",");
		string number = text.substr(0,seperator);
		highCO2FactorThresholds[j] = strtol(number.c_str(), NULL, 10);

		text.erase(0,seperator+1);
	}
		
	//acquiring high CO2 factors of the room
	pElem = hRoot.FirstChild("highCO2Factors").ToElement();
	if (!pElem) return 0;
	text = pElem->GetText();
	int highCO2Factors[4];
	for (int j = 0; j < 4; j++){
		size_t seperator = text.find(",");
		string number = text.substr(0,seperator);
		highCO2Factors[j] = strtol(number.c_str(), NULL, 10);

		text.erase(0,seperator+1);
	}
		
	//acquiring respiratory increase per minute of the room
	pElem = hRoot.FirstChild("respIncreasePerMin").ToElement();
	if (!pElem) return 0;
	int respIncreasePerMin = strtol(pElem->GetText(), NULL, 10);
		
		
	//The room model is initialized and added to the list of room models
	shared_ptr<dynamic::modeling::model> room;
		
	room = dynamic::translate::make_dynamic_atomic_model<RoomModel, TIME>("Room"+ID, ID, 
		ventilationRating, socialDistanceThreshold, maxOccupancy, respIncreasePerMin, 
		volumeOfRoom, sickPeopleCO2Factor, move(highCO2FactorThresholds), 
		move(highCO2Factors), vaccinatedFactor, wearsMaskFactor, socialDistanceFactor);
	
	
	/***** (5) *****/
    /*******TOP MODEL********/
    dynamic::modeling::Ports iports_TOP;
    iports_TOP = {};

    dynamic::modeling::Ports oports_TOP;
    oports_TOP = {typeid(top_out)};

    dynamic::modeling::Models submodels_TOP;
    submodels_TOP = {input_inToRoom_reader, input_outFromRoom_reader, room};

    dynamic::modeling::EICs eics_TOP;
    eics_TOP = {};  // _EIC WOULD GO HERE ; NOT NEEDED BECAUSE IT IS EMTPY IN THIS EXAMPLE

    dynamic::modeling::EOCs eocs_TOP;
    eocs_TOP = {
        dynamic::translate::make_EOC<RoomModelPorts::out,top_out>("Room"+ID)
    };

    dynamic::modeling::ICs ics_TOP;
    ics_TOP = {
        dynamic::translate::make_IC<iestream_input_defs<PersonInfo>::out,RoomModelPorts::inToRoom>("input_inToRoom_reader","Room"+ID),
		dynamic::translate::make_IC<iestream_input_defs<PersonInfo>::out,RoomModelPorts::outFromRoom>("input_outFromRoom_reader","Room"+ID)
    };

    shared_ptr<dynamic::modeling::coupled<TIME>> TOP;

    TOP = make_shared<dynamic::modeling::coupled<TIME>>(
             "TOP", submodels_TOP, iports_TOP, oports_TOP, eics_TOP, eocs_TOP, ics_TOP 
    );


	/***** (6) *****/
    /*************** Loggers *******************/
    static ofstream out_ProbGetSick("../simulation_results/roomModel_test_output_ProbGetSick.txt");

    struct oss_sink_ProbGetSick{
        static ostream& sink(){          
            return out_ProbGetSick;
        }
    };

    static ofstream out_state("../simulation_results/roomModel_test_output_state.txt");

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
    r.run_until(NDTime("24:00:00:000"));
    return 0;
}
