#ifndef DecisionMakerBehaviourHPP
#define DecisionMakerBehaviourHPP

#include <string>
#include <iostream>
#include <math.h>
#include <vector>
#include <assert.h>
#include <random>
#include <chrono>
#include <tuple>
#include <map>

#include "../data/tinyXML/tinyxml.h"
#include "../data/tinyXML/tinystr.h"

using namespace std;

namespace decision_maker_behaviour_structures{
	
	
	class Event{
		public:
			Event(){}
			
			Event(const string& ID, const string& type, const int& capacity, const bool& overCapacity, int& occupancy, const int& startTime, const int& endTime){
				this->ID=ID;
				this->type=type;
				this->capacity=capacity;
				this->overCapacity=overCapacity;
				this->occupancy=occupancy;
				this->startTime=startTime;
				this->endTime=endTime;
			}
		
		string ID;
		string type;
		int capacity;
		int occupancy;
		int startTime;
		int endTime;
		bool overCapacity;
		
		
	};
	
    /*******************************************/
    /***** XML Auxiliary structures  ***********/
    /*******************************************/
	
	
    class Relationship{
		public:
			Relationship(){}
			Relationship(const string& PersonID, const string& Relationship_type){
				this->PersonID=PersonID;
				this->Relationship_type=Relationship_type;
			}
    
		string PersonID;
		string Relationship_type;
    };

	class BehaviourRulesPerson{
		public:
			BehaviourRulesPerson(){}
			BehaviourRulesPerson(const string& PersonRelations, const int& SafeDistanceProbability, const int& MaskWearingProbability, const int& EnterMaxOccRoomProbability){
				this->PersonRelations=PersonRelations;
				this->SafeDistanceProbability=SafeDistanceProbability;
				this->MaskWearingProbability=MaskWearingProbability;
				this->EnterMaxOccRoomProbability=EnterMaxOccRoomProbability;
			}
			string PersonRelations;
			int SafeDistanceProbability;
			int MaskWearingProbability;
			int EnterMaxOccRoomProbability;
	};

	class NumberOfPeople{
		public:
			NumberOfPeople(){}
			NumberOfPeople(const string& group, const string& SafeDistanceProbability, const string& MaskProbability){
				this->group=group;
				this->SafeDistanceProbability=SafeDistanceProbability;
				this->MaskProbability=MaskProbability;
			}
			string group;
			string SafeDistanceProbability;
			string MaskProbability;
	};


	class BehaviourRulesRoom{
		public:
			BehaviourRulesRoom(){}
			BehaviourRulesRoom(const string& roomSize,  vector<NumberOfPeople> groupBehaviour ){
				this->roomSize=roomSize;
				this->groupBehaviour=groupBehaviour;
				//this->room_Size=room_Size;
			}
			string roomSize;
			vector<NumberOfPeople> groupBehaviour;
			//vector<RoomSize> room_Size;
	};

	class LocationPlan{
		public:
			LocationPlan(){}
			
			LocationPlan(const string& roomID, const int& timeInRoomMin, const int& startTime ){
				this->roomID = roomID;
				this->timeInRoomMin = timeInRoomMin;
				this->startTime = startTime;

			}
			
			string roomID;
			int timeInRoomMin;
			int startTime;
		
		bool isEmpty(){
			if (((roomID.compare("") == 0)&&(timeInRoomMin == 0)&&(startTime == 0))||(timeInRoomMin < 0)||(startTime < 0)){
				return true;
			} else {
				return false;
			}
		}
	};

	/*******************************************/
	/**** XML Student Behaviour structures *************/
	/*******************************************/
	class DecisionMakerBehaviour{
		public:
			inline static vector<Event> events;
			
			static void setEvents(vector<Event> new_events){
				events = new_events;
			}
		
			DecisionMakerBehaviour() = default;
			
			//constructor for randomly generating DecisionMakerBehaviour objects
			DecisionMakerBehaviour(string i_ID){
				
				unsigned seed1 = chrono::system_clock::now().time_since_epoch().count();
				default_random_engine generator(seed1);
				uniform_int_distribution<int> booleanDistribution(0,100);
				ID = i_ID;
				exposed = false;
				location = "home";
				
				//boolean parameters are set according to probability parameters
				int r = booleanDistribution(generator);
				if (r < 10){
					isSick = true;
				} else {
					isSick = false;
				}
				r = booleanDistribution(generator);
				if (r < 50){
					vaccinated = true;
				} else {
					vaccinated = false;
				}
				r = booleanDistribution(generator);
				if (r < 50){
					wearingMask = true;
				} else {
					wearingMask = false;
				}
				r = booleanDistribution(generator);
				if (r < 50){
					socialDistance = true;
				} else {
					socialDistance = false;
				}
				r = booleanDistribution(generator);
				weatherThreshold = floor(r / 10);
				r = booleanDistribution(generator);
				if (r < 50){
					eatsAlone = true;
				} else {
					eatsAlone = false;
				}
				r = booleanDistribution(generator);
				if (r < 50){
					riskyTravelBehaviour = true;
				} else {
					riskyTravelBehaviour = false;
				}
				
				
				vector<Event> availableEvents = events;
				
				vector<Event> personEvents;
				vector<int> startTimes;
				vector<int> endTimes;
				
				uniform_int_distribution<int> personEventsDistribution(1,4);
				int numberOfEvents = personEventsDistribution(generator);
				
					
				uniform_int_distribution<int> eventsDistribution(0,availableEvents.size()-1);
				int eventIndex = eventsDistribution(generator);
				
				bool scheduleConflict = true;
				
				for (int j = 0; j < numberOfEvents; j++){
				
					
					scheduleConflict = true;
					while (scheduleConflict == true){
						scheduleConflict = false;
						if (availableEvents.empty() == false){
							for (int i = 0; i < startTimes.size(); i++){
								//start and end times cannot be equal
								if ((availableEvents[eventIndex].startTime == startTimes[i])||(availableEvents[eventIndex].endTime == endTimes[i])){
									scheduleConflict = true;
									availableEvents.erase(availableEvents.begin()+eventIndex);
									uniform_int_distribution<int> eventsDistribution(0,availableEvents.size()-1);
									eventIndex = eventsDistribution(generator);
									break;
								} else if ((availableEvents[eventIndex].startTime <= startTimes[i])&&(availableEvents[eventIndex].endTime >= endTimes[i])){
									//The event can't start earlier and end later
									scheduleConflict = true;
									availableEvents.erase(availableEvents.begin()+eventIndex);
									uniform_int_distribution<int> eventsDistribution(0,availableEvents.size()-1);
									eventIndex = eventsDistribution(generator);
									break;
								} else if ((availableEvents[eventIndex].endTime >= startTimes[i])&&(availableEvents[eventIndex].endTime <= endTimes[i])){
									//The event cannot end in the middle of another event
									scheduleConflict = true;
									availableEvents.erase(availableEvents.begin()+eventIndex);
									uniform_int_distribution<int> eventsDistribution(0,availableEvents.size()-1);
									eventIndex = eventsDistribution(generator);
									break;
								} else if ((availableEvents[eventIndex].startTime >= startTimes[i])&&(availableEvents[eventIndex].startTime <= endTimes[i])){
									//The event cannot start in the middle of another event
									scheduleConflict = true;
									availableEvents.erase(availableEvents.begin()+eventIndex);
									uniform_int_distribution<int> eventsDistribution(0,availableEvents.size()-1);
									eventIndex = eventsDistribution(generator);
									break;
								} else if (availableEvents[eventIndex].endTime <= startTimes[i]){
									//The event cannot occur before the event
									scheduleConflict = true;
									availableEvents.erase(availableEvents.begin()+eventIndex);
									uniform_int_distribution<int> eventsDistribution(0,availableEvents.size()-1);
									eventIndex = eventsDistribution(generator);
									break;
								} else if ((availableEvents[eventIndex].endTime == startTimes[i])||(availableEvents[eventIndex].startTime == endTimes[i])){
									//The endtime of one event cannot match the startTime of another and vice versa
									scheduleConflict = true;
									availableEvents.erase(availableEvents.begin()+eventIndex);
									uniform_int_distribution<int> eventsDistribution(0,availableEvents.size()-1);
									eventIndex = eventsDistribution(generator);
									break;
								}
							}
							
						}
					}
					
					if (availableEvents.empty() == false){
						if (availableEvents[eventIndex].overCapacity == true){
							personEvents.push_back(availableEvents[eventIndex]);
							startTimes.push_back(availableEvents[eventIndex].startTime);
							endTimes.push_back(availableEvents[eventIndex].endTime);
							events[eventIndex].occupancy++;
						} else {
							if (availableEvents[eventIndex].occupancy < availableEvents[eventIndex].capacity){
								personEvents.push_back(availableEvents[eventIndex]);
								startTimes.push_back(availableEvents[eventIndex].startTime);
								endTimes.push_back(availableEvents[eventIndex].endTime);
								events[eventIndex].occupancy++;
							}
						}
						
						availableEvents.erase(availableEvents.begin()+eventIndex);
						uniform_int_distribution<int> eventsDistribution(0,availableEvents.size()-1);
						eventIndex = eventsDistribution(generator);
					}
					
					
				}
				
				
				
				
				bool goingHome = false;
				for (int i = 0; i < personEvents.size(); i++){
					//creating event LocationPlan
					string iD = personEvents[i].ID;
					int timeSpent = personEvents[i].endTime - personEvents[i].startTime;
					int timeStart = personEvents[i].startTime;
					LocationPlan temp = LocationPlan(iD, timeSpent, timeStart);
					locationPlan.push_back(temp);
					//creating travel LocationPlan
					iD = "Outdoors";
					if (i == personEvents.size()-1){
						timeSpent = 10;
					} else if ((personEvents[i+1].startTime - personEvents[i].endTime) < 60) {
						timeSpent = personEvents[i+1].startTime - personEvents[i].endTime;
					} else {
						
						goingHome = true;
						timeSpent = 10;
					}	
					timeStart = personEvents[i].endTime;
					temp = LocationPlan(iD, timeSpent, timeStart);
					locationPlan.push_back(temp);
					if (goingHome == true){
						//creating home event
						iD = "home";
						timeSpent = personEvents[i+1].startTime - personEvents[i].endTime - 20;
						timeStart = personEvents[i].endTime + 10;
						temp = LocationPlan(iD, timeSpent, timeStart);
						locationPlan.push_back(temp);
						//creating travel event
						iD = "Outdoors";
						timeSpent = 10;
						timeStart = personEvents[i+1].startTime - 10;
						temp = LocationPlan(iD, timeSpent, timeStart);
						locationPlan.push_back(temp);
						goingHome = false;
					}
				}
				//creating travel event to campus
				string iD;
				iD = "Outdoors";
				int timeSpent = 10;
				LocationPlan travel = LocationPlan(iD, timeSpent, (personEvents[0].startTime - timeSpent));
				LocationPlan home = LocationPlan("home", 1440 - (personEvents[personEvents.size()-1].endTime+10) + (personEvents[0].startTime - timeSpent), (personEvents[personEvents.size()-1].endTime+10));
				locationPlan.push_back(home);
				locationPlan.push_back(travel);
				currStartTime = home.startTime;
				timeInFirstLocation = home.timeInRoomMin;
				
				BehaviourRulesPerson family = BehaviourRulesPerson("friends", 20, 20, 80);
				BehaviourRulesPerson friends = BehaviourRulesPerson("acquaintance", 50, 50, 50);
				BehaviourRulesPerson stranger = BehaviourRulesPerson("stranger", 80, 80, 20);
				behaviourRulesPerson.insert(pair<string, BehaviourRulesPerson>("friends", family));
				behaviourRulesPerson.insert(pair<string, BehaviourRulesPerson>("acquaintance", friends));
				behaviourRulesPerson.insert(pair<string, BehaviourRulesPerson>("stranger", stranger));
				
				
				
			}
			string                          	ID;
			string                          	location;
			int 								currStartTime;
			int 								timeInFirstLocation;
			int									weatherThreshold;
			bool                            	isSick;
			bool 								exposed;
			bool								vaccinated;
			bool                            	wearingMask;
			bool								socialDistance;
			bool                            	eatsAlone;
			bool                            	riskyTravelBehaviour;
			vector<Relationship>            	relationship;
			map<string,BehaviourRulesPerson>    behaviourRulesPerson;
			vector<BehaviourRulesRoom>      	behaviourRulesRoom;
			vector<LocationPlan>            	locationPlan;
			LocationPlan						nextLocation;
		
		
		void setNextLocation(int timeRemaining) {
			vector<LocationPlan>::iterator iter;
			bool changed = false;
			for (iter = locationPlan.begin(); iter != locationPlan.end(); iter++){
				const LocationPlan& lp = *iter;
				//cout << "startTime:" << lp.startTime << endl;
				if ((currStartTime + timeRemaining)%1440 == lp.startTime){
					nextLocation = lp;
					changed = true;
				}
			}
			//cout << "timeRemaining:" << timeRemaining << endl;
			//cout << "currStartTime:" << currStartTime << endl;
			assert(changed == true);
		}

		void save(const char* pFilename){
	

			TiXmlDocument doc;  
			TiXmlDeclaration* decl = new TiXmlDeclaration( "1.0", "", "" );  
			doc.LinkEndChild(decl); 

     

			TiXmlElement * root = new TiXmlElement("DecisionMakerBehaviour");  
			doc.LinkEndChild( root );  

			// block: ID

			{           
				 TiXmlElement * pID = new TiXmlElement( "ID" );          
				 pID->LinkEndChild(new TiXmlText(ID.c_str()));
				 root->LinkEndChild( pID );  

			}

			// block: Location

			{           

				TiXmlElement * pLocation = new TiXmlElement( "location" );          
				pLocation->LinkEndChild(new TiXmlText(location.c_str()));
				root->LinkEndChild( pLocation );  

			}
			
			// block: currStartTime

			{           

				TiXmlElement * pCurrStartTime = new TiXmlElement( "currStartTime" );          
				string currStartTimeStr = to_string(currStartTime);
				pCurrStartTime->LinkEndChild(new TiXmlText(currStartTimeStr.c_str()));
				root->LinkEndChild( pCurrStartTime );  

			}
			
			// block: timeInFirstLocation

			{           

				TiXmlElement * pTimeInFirstLocation = new TiXmlElement( "timeInFirstLocation" );          
				string timeInFirstLocationStr = to_string(timeInFirstLocation);
				pTimeInFirstLocation->LinkEndChild(new TiXmlText(timeInFirstLocationStr.c_str()));
				root->LinkEndChild( pTimeInFirstLocation );  

			}

			// block: isSick

			{    
				TiXmlElement * pIsSick = new TiXmlElement( "isSick" );
				if (isSick){
					pIsSick->LinkEndChild(new TiXmlText("True"));
				} else {
					pIsSick->LinkEndChild(new TiXmlText("False"));
				}
				root->LinkEndChild(pIsSick);
			}    

			// block: exposed

			{    
				TiXmlElement * pExposed = new TiXmlElement( "exposed" );
				if (exposed){
					pExposed->LinkEndChild(new TiXmlText("True"));
				} else {
					pExposed->LinkEndChild(new TiXmlText("False"));
				}
				root->LinkEndChild(pExposed);
			}   

			// block: vaccinated

			{    
				TiXmlElement * pVaccinated = new TiXmlElement( "vaccinated" );
				if (vaccinated){
					pVaccinated->LinkEndChild(new TiXmlText("True"));
				} else {
					pVaccinated->LinkEndChild(new TiXmlText("False"));
				}
				root->LinkEndChild(pVaccinated);
			}   
			
			// block: wearingMaskCorrectly

			{    
				TiXmlElement * pWearingMask = new TiXmlElement( "wearingMaskCorrectly" );
				if (wearingMask){
					pWearingMask->LinkEndChild(new TiXmlText("True"));
				} else {
					pWearingMask->LinkEndChild(new TiXmlText("False"));
				}
				root->LinkEndChild(pWearingMask);
			}   
			
			// block: socialDistance

			{    
				TiXmlElement * pSocialDistance = new TiXmlElement( "socialDistance" );
				if (socialDistance){
					pSocialDistance->LinkEndChild(new TiXmlText("True"));
				} else {
					pSocialDistance->LinkEndChild(new TiXmlText("False"));
				}
				root->LinkEndChild(pSocialDistance);
			}   

			// block: weatherThreshold

			{

				TiXmlElement * pWeatherThreshold = new TiXmlElement("weatherThreshold");
				string weatherThresholdStr = to_string(weatherThreshold);
				pWeatherThreshold->LinkEndChild(new TiXmlText(weatherThresholdStr.c_str()));
				root->LinkEndChild(pWeatherThreshold);

			}
			
			//Block: Relationship
			{
				vector<Relationship>::iterator iter;
				TiXmlElement * RelationshipNode = new TiXmlElement("relationship");
				root->LinkEndChild(RelationshipNode);
			
				for(iter = relationship.begin(); iter != relationship.end(); iter++) {
					const Relationship& w=*iter;
					TiXmlElement * RelationID = new TiXmlElement("relationship");
					RelationshipNode ->LinkEndChild(RelationID);
					RelationID->SetAttribute("ID", w.PersonID.c_str());
					RelationID->SetAttribute("type", w.Relationship_type.c_str());
				}
			}
        
			//Block: BehaviourRulesPerson
			{
				map<string,BehaviourRulesPerson>::iterator iter;
				TiXmlElement * BehaviourPersonNode = new TiXmlElement("behaviourRulesPerson");
				root->LinkEndChild(BehaviourPersonNode);
			
				for(iter=behaviourRulesPerson.begin(); iter != behaviourRulesPerson.end(); iter++){
					BehaviourRulesPerson w = iter->second;
					TiXmlElement * PersonRelations = new TiXmlElement( "personRelations" );
					BehaviourPersonNode->LinkEndChild(PersonRelations);
					PersonRelations->SetAttribute("status", w.PersonRelations.c_str());
					string s = to_string(w.SafeDistanceProbability);
					PersonRelations->SetAttribute("safeDistanceProb", s.c_str());
					string m = to_string(w.MaskWearingProbability);
					PersonRelations->SetAttribute("maskWearingProb", m.c_str());
					string e = to_string(w.EnterMaxOccRoomProbability);
					PersonRelations->SetAttribute("enterMaxOccRoomProb", e.c_str());
				}
			}

			//Block: BehaviourRulesRoom
				// {
				//   vector<BehaviourRulesRoom>::iterator iter;
				//   TiXmlElement * BehaviourRoomNode = new TiXmlElement("BehaviourRulesRoom");
				//   root->LinkEndChild(BehaviourRoomNode);
				//   for(iter=behaviourRulesRoom.begin(); iter !=behaviourRulesRoom.end(); iter++)
				//   {
				//     const BehaviourRulesRoom& w=*iter;
				//     TiXmlElement * BehaviourRoomSize = new TiXmlElement( "RoomSize" );
				//     BehaviourRoomNode->LinkEndChild(BehaviourRoomSize);
				//     BehaviourRoomSize->SetAttribute("size",w.roomSize.c_str());
				//     vector<NumberOfPeople>::iterator iter2;
				//     vector<NumberOfPeople> aux_NumberOfPeople;
				//     aux_NumberOfPeople = w.groupBehaviour;
				//     for(iter2 = aux_NumberOfPeople.begin(); iter2 != aux_NumberOfPeople.end(); iter2++)
				//     {
				//       const NumberOfPeople& w2=*iter2;
				//       TiXmlElement * GroupNode = new TiXmlElement ( "NumberOfPeople" );
				//       BehaviourRoomNode->LinkEndChild(GroupNode);
				//       GroupNode->SetAttribute("group", w2.group.c_str());
				//       TiXmlElement * SafeDistance = new TiXmlElement( "SafeDistance" );
				//       GroupNode->SetAttribute("safeDistanceProb", w2.SafeDistanceProbability.c_str());
				//       BehaviourRoomNode->LinkEndChild( GroupNode );
				//       TiXmlElement * MaskWearing = new TiXmlElement(" MaskWearing ");
				//       GroupNode->SetAttribute("maskWearingProb", w2.MaskProbability.c_str());
				//       BehaviourRoomNode->LinkEndChild( GroupNode );
				//     }
				//   }
				// }
				
			//Block: locationPlan
			{
				vector<LocationPlan>::iterator iter;
				TiXmlElement * LocationPlanNode = new TiXmlElement("locationPlan");
				root->LinkEndChild(LocationPlanNode);
			
				for(iter=locationPlan.begin(); iter != locationPlan.end(); iter++){
					const LocationPlan& w=*iter;
					TiXmlElement * locationPlanE = new TiXmlElement( "locationPlan" );
					LocationPlanNode->LinkEndChild(locationPlanE);
					
					locationPlanE->SetAttribute("room", w.roomID.c_str());
					string s = to_string(w.timeInRoomMin);
					locationPlanE->SetAttribute("timeinroom", s.c_str());
					locationPlanE->SetAttribute("startTime", w.startTime);
				}
			}

			doc.SaveFile(pFilename); 
		} 
   
		void load(const char* pFilename){     
       
			TiXmlDocument _document;
			bool result = _document.LoadFile(pFilename);
			if (!result) {cout << "file not loaded " << _document.ErrorDesc() << endl; return;}   
			TiXmlHandle hDoc(&_document);
			TiXmlElement* pElem;
			TiXmlHandle hRoot(0);
			// block: root
			{
				pElem=hDoc.FirstChildElement().ToElement();
				// should always have a valid root but handle gracefully if it does
				if (!pElem) return;    
				// save this for later
				hRoot=TiXmlHandle(pElem);
			}      

			// block: Id
			{ 
				pElem=hRoot.FirstChild("ID").ToElement();
				if (!pElem) return;
				const char* pID = pElem->GetText();
				if(pID) ID = pID;
				
			}

			// block: Location
			{ 
		  
				pElem=hRoot.FirstChild("location").ToElement();
		  
				if (!pElem) return;
		  
				const char* pLocation = pElem->GetText();
				if(pLocation) location = pLocation;
				
				
			}
			
			// block: currStartTime
			{
				pElem = hRoot.FirstChild("currStartTime").ToElement();
				if (!pElem) return;
				const char* pCurrStartTime = pElem->GetText();
				if(pCurrStartTime) currStartTime = strtol(pCurrStartTime, NULL, 10);
				
			}

			// block: timeInFirstLocation
			{
				pElem = hRoot.FirstChild("timeInFirstLocation").ToElement();
				if (!pElem) return;
				const char* pTimeInFirstLocation = pElem->GetText();
				if(pTimeInFirstLocation) timeInFirstLocation = strtol(pTimeInFirstLocation, NULL, 10);
				
			}
			
			// block: IsSick
			{ 

				pElem=hRoot.FirstChild("isSick").ToElement();
				if (!pElem) return;
				const char* pIsSick = pElem->GetText();
				if(strncmp(pIsSick,"False",2)==0){
					isSick = false;
				}else{
					isSick = true;
				}
				
			}
			
			// block: exposed
			{ 

				pElem=hRoot.FirstChild("exposed").ToElement();
				if (!pElem) return;
				const char* pExposed = pElem->GetText();
				if(strncmp(pExposed,"False",2)==0){
					exposed = false;
				}else{
					exposed = true;
				}
				
			}
			
			// block: vaccinated
			{ 

				pElem=hRoot.FirstChild("vaccinated").ToElement();
				if (!pElem) return;
				const char* pVaccinated = pElem->GetText();
				if(strncmp(pVaccinated,"False",2)==0){
					vaccinated = false;
				}else{
					vaccinated = true;
				}
				
			}
	    
			// block: MaskWearing
			{ 
				pElem=hRoot.FirstChild("wearingMaskCorrectly").ToElement();
				if (!pElem) return;
				const char* pWearingMask = pElem->GetText();
				if(strncmp(pWearingMask,"False",2)==0){
					wearingMask = false;
				}else{
					wearingMask = true;
				}
				
			}
			
			// block: socialDistance
			{
				pElem = hRoot.FirstChild("socialDistance").ToElement();
				if (!pElem) return;
				const char* pSocialDistance = pElem->GetText();
				if (strncmp(pSocialDistance,"False",2) == 0){
					socialDistance = false;
				}else{
					socialDistance = true;
				}
				
			}
			
			// block: weatherThreshold
			{
				pElem = hRoot.FirstChild("weatherThreshold").ToElement();
				if (!pElem) return;
				const char* pWeatherThreshold = pElem->GetText();
				if (pWeatherThreshold) weatherThreshold = strtol(pWeatherThreshold, NULL, 10);

			}

			//Block: Relationship
			{
				relationship.clear();
				TiXmlElement* pRelationshipNode = hRoot.FirstChild( "relationship" ).FirstChild().ToElement();
				for( pRelationshipNode; pRelationshipNode; pRelationshipNode=pRelationshipNode->NextSiblingElement()){
					Relationship c;
					const char* pPerson= pRelationshipNode->Attribute("ID");
					if(pPerson) c.PersonID = pPerson;
					const char *pRelationship_type = pRelationshipNode->Attribute("type");
					if(pRelationship_type) c.Relationship_type = pRelationship_type;
					
					relationship.push_back(c);
				}
			}

			//Block: BehaviourRulesPerson
			{
				behaviourRulesPerson.clear();
				TiXmlElement* pBehaviourPersonNode = hRoot.FirstChild( "behaviourRulesPerson" ).FirstChild().ToElement();
				for(pBehaviourPersonNode; pBehaviourPersonNode; pBehaviourPersonNode=pBehaviourPersonNode->NextSiblingElement()){
					BehaviourRulesPerson c;
					if(!pBehaviourPersonNode) return;
					const char *pPersonRelations=pBehaviourPersonNode->Attribute("status");
					if(pPersonRelations) c.PersonRelations = pPersonRelations;
					const char *psafeDistanceProb=pBehaviourPersonNode->Attribute("safeDistanceProb");
					if(psafeDistanceProb) c.SafeDistanceProbability = strtol(psafeDistanceProb, NULL, 10);
					const char *pmaskWearingProb = pBehaviourPersonNode->Attribute("maskWearingProb");
					if(pmaskWearingProb) c.MaskWearingProbability = strtol(pmaskWearingProb, NULL, 10);
					const char *penterMaxOccRoomProb = pBehaviourPersonNode->Attribute("enterMaxOccRoomProb");
					if(penterMaxOccRoomProb) c.EnterMaxOccRoomProbability = strtol(penterMaxOccRoomProb, NULL, 10);

					behaviourRulesPerson.insert(pair<string,BehaviourRulesPerson>(c.PersonRelations,c));
				}
			}

			//Block: BehaviourRulesRoom
			{
				behaviourRulesRoom.clear();
				TiXmlElement* pBehaviourRoomNode = hRoot.FirstChild( "behaviourRulesRoom" ).FirstChild().ToElement();
				for(pBehaviourRoomNode; pBehaviourRoomNode; pBehaviourRoomNode=pBehaviourRoomNode->NextSiblingElement()){
					BehaviourRulesRoom c;
					if(!pBehaviourRoomNode) return;
					const char *pBehaviourRoomSize = pBehaviourRoomNode->Attribute("size");
					if(pBehaviourRoomSize) c.roomSize = pBehaviourRoomSize;
					
					TiXmlElement* pGroupBehaviourNode=pBehaviourRoomNode->FirstChildElement();
				
					for(pGroupBehaviourNode; pGroupBehaviourNode; pGroupBehaviourNode=pGroupBehaviourNode->NextSiblingElement()){
						NumberOfPeople d;
						if(!pGroupBehaviourNode) return;
						const char* pgroupBehaviours=pGroupBehaviourNode->Attribute("group");
						if (pgroupBehaviours) d.group = pgroupBehaviours;

						const char *psafeDistanceProb=pGroupBehaviourNode->Attribute("safeDistanceProb");
						if(psafeDistanceProb) d.SafeDistanceProbability = psafeDistanceProb;
						const char *pmaskWearingProb = pGroupBehaviourNode->Attribute("maskWearingProb");
						if(pmaskWearingProb) d.MaskProbability = pmaskWearingProb;
						
						c.groupBehaviour.push_back(d);
					}
				behaviourRulesRoom.push_back(c);
				}
			}
        
			//Block: LocationPlan
			{
				locationPlan.clear(); // trash existing list    
				TiXmlElement* pLocationPlanNode = hRoot.FirstChild("locationPlan").FirstChild().ToElement();
				for( pLocationPlanNode; pLocationPlanNode; pLocationPlanNode=pLocationPlanNode->NextSiblingElement()){
					LocationPlan c;
					const char* pRoomID= pLocationPlanNode->Attribute("room");
					if(pRoomID) c.roomID = pRoomID;
					const char *pTime_in_room = pLocationPlanNode->Attribute("timeinroom");
					if(pTime_in_room) c.timeInRoomMin =strtol(pTime_in_room,NULL,10);
					const char *pStart_Time = pLocationPlanNode->Attribute("startTime");
					if(pStart_Time) c.startTime = strtol(pStart_Time,NULL,10);
					
					locationPlan.push_back(c);
				}
			}

      
		}
	};
}   

#endif //PersonInfoHPP
