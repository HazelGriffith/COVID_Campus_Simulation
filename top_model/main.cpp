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
#include "../data_structures/ProbGetSick.hpp"

//Atomic model headers
#include "../atomics/room.hpp"
#include "../atomics/filter_People_In.hpp"
#include "../atomics/filter_People_Out.hpp"
#include "../atomics/filter_ProbGetSick.hpp"
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
struct top_out: public out_port<ProbGetSick>{};

template<typename T>
class InputReader_PersonInfo : public iestream_input<PersonInfo, T> {

    public:
        InputReader_PersonInfo () = default;
        InputReader_PersonInfo (const char* file_path) : 
				iestream_input<PersonInfo,T> (file_path) {}
};

struct RoomInfo{
	string ID;
	int ventilationRating;
	int socialDistanceThreshold;
	int wearsMaskCorrectlyFactor;
	int socialDistanceFactor;
	int respIncreasePerMin;
};

/***** (3) *****/
int main(){

	/***** (4) *****/	
    /****** room atomic model instantiation *******************/
	
	//initialize lists of RoomInfo structs, filename paths, room models, inToRoom filter models, and outFromRoom filter models
	vector<RoomInfo> roomsInfo;
	
	vector<const char*> roomXMLFiles;
	const char * homePath = "../data/home.xml";
	roomXMLFiles.push_back(homePath);
	const char * Mackenzie1stPath = "../data/1st_Mackenzie.xml";
	roomXMLFiles.push_back(Mackenzie1stPath);
	const char * Mackenzie2ndPath = "../data/2nd_Mackenzie.xml";
	roomXMLFiles.push_back(Mackenzie2ndPath);
	const char * Mackenzie3rdPath = "../data/3rd_Mackenzie.xml";
	roomXMLFiles.push_back(Mackenzie3rdPath);
	
	for (int i = 0; i < roomXMLFiles.size(); i++){
		RoomInfo roomInfo;
		roomsInfo.push_back(roomInfo);
	}
	
	vector<shared_ptr<dynamic::modeling::model>> rooms;
	vector<shared_ptr<dynamic::modeling::model>> inToRoomFilters;
	vector<shared_ptr<dynamic::modeling::model>> outFromRoomFilters;
	
	//load room xml files into each RoomInfo struct
	for (int i = 0; i < roomXMLFiles.size(); i++){
		
		//loading document
		TiXmlDocument _document;
		bool result = _document.LoadFile(roomXMLFiles[i]);
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
		roomsInfo[i].ID = pElem->GetText();
		
		//acquiring ventilation rating of the room
		pElem = hRoot.FirstChild("ventilationRating").ToElement();
		if (!pElem) return 0;
		roomsInfo[i].ventilationRating = strtol(pElem->GetText(), NULL, 10);

		//acquiring social distance threshold of the room
		pElem = hRoot.FirstChild("socialDistanceThreshold").ToElement();
		if (!pElem) return 0;
		roomsInfo[i].socialDistanceThreshold = strtol(pElem->GetText(), NULL, 10);
		
		//acquiring wearing mask correctly factor of the room
		pElem = hRoot.FirstChild("wearsMaskCorrectlyFactor").ToElement();
		if (!pElem) return 0;
		roomsInfo[i].wearsMaskCorrectlyFactor = strtol(pElem->GetText(), NULL, 10);

		//acquiring social distance factor of the room
		pElem = hRoot.FirstChild("socialDistanceFactor").ToElement();
		if (!pElem) return 0;
		roomsInfo[i].socialDistanceFactor = strtol(pElem->GetText(), NULL, 10);
		
		//acquiring respiratory increase per minute of the room
		pElem = hRoot.FirstChild("respIncreasePerMin").ToElement();
		if (!pElem) return 0;
		roomsInfo[i].respIncreasePerMin = strtol(pElem->GetText(), NULL, 10);
		
		
		//The room model is initialized and added to the list of room models
		shared_ptr<dynamic::modeling::model> room;
		
		room = dynamic::translate::make_dynamic_atomic_model<RoomModel, TIME>(roomsInfo[i].ID, roomsInfo[i].ID, 
			roomsInfo[i].ventilationRating, roomsInfo[i].socialDistanceThreshold, roomsInfo[i].wearsMaskCorrectlyFactor, roomsInfo[i].socialDistanceFactor, roomsInfo[i].respIncreasePerMin);
		
		rooms.push_back(room);
		
		
		/****** filter atomic model instantiation *******************/
		//The filterIn model is initialized and added to the list of filterIn models
		shared_ptr<dynamic::modeling::model> filterIn;

		filterIn = dynamic::translate::make_dynamic_atomic_model<Filter_People_In, TIME>("filterIn"+roomsInfo[i].ID, roomsInfo[i].ID);

		inToRoomFilters.push_back(filterIn);
		
		//The filterOut model is initialized and added to the list of filterOut models
		shared_ptr<dynamic::modeling::model> filterOut;

		filterOut = dynamic::translate::make_dynamic_atomic_model<Filter_People_Out, TIME>("filterOut"+roomsInfo[i].ID, roomsInfo[i].ID);

		outFromRoomFilters.push_back(filterOut);
		
		
	}

	 /****** person atomic model instantiation *******************/
    const char * personFileName = "../data/Student1.xml";
	
	shared_ptr<dynamic::modeling::model> person;
	
    person = dynamic::translate::make_dynamic_atomic_model<Person, TIME>("person", move(personFileName));
	
	/****** probGetSick filter atomic model instantiation *******************/
	const char * personID = "Jane";
	
	shared_ptr<dynamic::modeling::model> probGetSickFilter;
	
    probGetSickFilter = dynamic::translate::make_dynamic_atomic_model<Filter_ProbGetSick, TIME>("probGetSickFilter", move(personID));
	
	/***** (5) *****/
    /*******TOP MODEL********/
    dynamic::modeling::Ports iports_TOP;
    iports_TOP = {};

    dynamic::modeling::Ports oports_TOP;
    oports_TOP = {typeid(top_out)};

    dynamic::modeling::Models submodels_TOP;
	for (int i = 0; i < rooms.size(); i++){
		submodels_TOP.push_back(rooms[i]);
		submodels_TOP.push_back(inToRoomFilters[i]);
		submodels_TOP.push_back(outFromRoomFilters[i]);
	}
    submodels_TOP.push_back(person);
	submodels_TOP.push_back(probGetSickFilter);
	

    dynamic::modeling::EICs eics_TOP;
    eics_TOP = {};  // _EIC WOULD GO HERE ; NOT NEEDED BECAUSE IT IS EMTPY IN THIS EXAMPLE

    dynamic::modeling::EOCs eocs_TOP;
	for (int i = 0; i < rooms.size(); i++){
		eocs_TOP.push_back(dynamic::translate::make_EOC<RoomModelPorts::out,top_out>(roomsInfo[i].ID));
	}

    dynamic::modeling::ICs ics_TOP;
	for (int i = 0; i < rooms.size(); i++){
		ics_TOP.push_back(dynamic::translate::make_IC<Person_ports::nextDestination,Filter_People_In_ports::inToFilter>("person","filterIn"+roomsInfo[i].ID));
		ics_TOP.push_back(dynamic::translate::make_IC<Person_ports::nextDestination,Filter_People_Out_ports::inToFilter>("person","filterOut"+roomsInfo[i].ID));
		ics_TOP.push_back(dynamic::translate::make_IC<Filter_People_In_ports::outFromFilter,RoomModelPorts::inToRoom>("filterIn"+roomsInfo[i].ID, roomsInfo[i].ID));
		ics_TOP.push_back(dynamic::translate::make_IC<Filter_People_Out_ports::outFromFilter,RoomModelPorts::outFromRoom>("filterOut"+roomsInfo[i].ID,roomsInfo[i].ID));
		ics_TOP.push_back(dynamic::translate::make_IC<RoomModelPorts::out,Filter_ProbGetSick_ports::inToFilter>(roomsInfo[i].ID,"probGetSickFilter"));
	}
	ics_TOP.push_back(dynamic::translate::make_IC<Filter_ProbGetSick_ports::outFromFilter,Person_ports::infectionProb>("probGetSickFilter","person"));

	
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
    r.run_until(NDTime("26:00:00:000"));
    return 0;
}
