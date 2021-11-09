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
			DecisionMakerBehaviour() = default;
			
			//constructor for randomly generating DecisionMakerBehaviour objects
			DecisionMakerBehaviour(string i_ID, int numberOfRooms){
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
				
				int maxTimeInDay = 810; //minutes. This was found as Carleton lectures start at 8:30 am and end at 10 pm, so 13.5 hours *60 min = 810 min
				
				//The amount of time spent at Carleton is randomly generated as a multiple of 30, as events at Carleton rarely end at the quarter hour
				uniform_int_distribution<int> carletonTimeDistribution(1,maxTimeInDay/30);
				
				//total time spent at Carleton
				int timeAtCarleton = carletonTimeDistribution(generator)*30;
				assert(timeAtCarleton <= maxTimeInDay);
				assert(timeAtCarleton >= 30);
				
				int numberOf60MinEvents = 0;
				int numberOf30MinEvents = 0;
				int numberOf180MinEvents = 0;
				int numberOfTravelingEvents = 0;
				vector<int> travelingEventTimes;
				
				//This section randomly determines how many 180, 30, 60, and/or 90 minute event durations are scheduled
				uniform_int_distribution<int> ninetyMinDistribution(0,floor(timeAtCarleton/90));
				int numberOf90MinEvents = ninetyMinDistribution(generator);
				assert(numberOf90MinEvents >= 0);
				assert(numberOf90MinEvents <= floor(timeAtCarleton/90));
				
				int tempTimeAtCarleton = timeAtCarleton;
				tempTimeAtCarleton -= numberOf90MinEvents*90;
				
				//random travelling times are generated and added

				uniform_int_distribution<int> travelTimesDistribution(1,10);
				//travelingEventTimes.push_back(travelTimesDistribution(generator));
				int travelingTime;
				
				assert(tempTimeAtCarleton >= 0);
				
				
				
				
				if (tempTimeAtCarleton >= 180){
					//If there are any remaining 180 min time slots then some may be generated
					uniform_int_distribution<int> oneEightyMinDistribution(0,floor(tempTimeAtCarleton/180));
					numberOf180MinEvents = oneEightyMinDistribution(generator);
					
					if (numberOf180MinEvents > 2){
						numberOf180MinEvents = 2;
					}
					
					assert(numberOf180MinEvents >= 0);
					assert(numberOf180MinEvents <= floor(tempTimeAtCarleton/180));
					tempTimeAtCarleton -= numberOf180MinEvents*180;
					
					assert(tempTimeAtCarleton >= 0);
				} 

				if (tempTimeAtCarleton >= 60){
					
					//If there are any remaining 60 min time slots then some may be generated
					uniform_int_distribution<int> sixtyMinDistribution(0,floor(tempTimeAtCarleton/60));
					numberOf60MinEvents = sixtyMinDistribution(generator);
					assert(numberOf60MinEvents >= 0);
					assert(numberOf60MinEvents <= floor(tempTimeAtCarleton/60));
					tempTimeAtCarleton -= numberOf60MinEvents*60;
					
					assert(tempTimeAtCarleton >= 0);
				}
					
				if (tempTimeAtCarleton >= 30){
						
					numberOf30MinEvents = floor(tempTimeAtCarleton/30);
					assert(numberOf30MinEvents >= 0);
					assert(numberOf30MinEvents <= floor(tempTimeAtCarleton/30));
					tempTimeAtCarleton -= numberOf30MinEvents*30;
					
					assert(tempTimeAtCarleton >= 0);
					
				}
				
				int numberOfEvents = numberOf90MinEvents + numberOf180MinEvents + numberOf60MinEvents + numberOf30MinEvents;
				int sum = 0;
				for (int i = 0; i < numberOfEvents; i++){
					travelingTime = travelTimesDistribution(generator);
					travelingEventTimes.push_back(travelingTime);
					sum += travelingTime;
				}
				while (sum > 0){
					if (sum >= 180){
						if (numberOf180MinEvents > 0){
							sum -= 180;
							numberOf180MinEvents--;
							numberOfEvents--;
						} else if (numberOf90MinEvents > 0){
							sum -= 90;
							numberOf90MinEvents--;
						} else if (numberOf60MinEvents > 0){
							sum -= 60;
							numberOf60MinEvents--;
						} else {
							sum -= 30;
							numberOf30MinEvents--;
						}
						numberOfEvents--;
					} else if (sum >= 90){
						if (numberOf90MinEvents > 0){
							sum -= 90;
							numberOf90MinEvents--;
							numberOfEvents--;
						} else if (numberOf60MinEvents > 0){
							sum -= 60;
							numberOf60MinEvents--;
							numberOfEvents--;
						} else if (numberOf30MinEvents > 0){
							sum -= 30;
							numberOf30MinEvents--;
							numberOfEvents--;
						}
						
					} else if (sum >= 60){
						if (numberOf60MinEvents > 0){
							sum -= 60;
							numberOf60MinEvents--;
							numberOfEvents--;
						} else {
							sum -= 30;
							numberOf30MinEvents--;
							numberOfEvents--;
						}
						
					} else if ((sum >= 30)&&(numberOf30MinEvents > 0)){
						sum -= 30;
						numberOf30MinEvents--;
						numberOfEvents--;
						
					} else if (timeAtCarleton + sum <= maxTimeInDay){
						timeAtCarleton += sum;
						sum = 0;
					} else {
						if (numberOf30MinEvents > 0){
							numberOf30MinEvents--;
							
						} else if (numberOf60MinEvents > 0){
							numberOf60MinEvents--;
						} else if (numberOf90MinEvents > 0){
							numberOf90MinEvents--;
						} else {
							numberOf180MinEvents--;
						}
						numberOfEvents--;
						sum = 0;
					}
				}
				travelingEventTimes.resize(numberOfEvents);
				
				//randomly generates the time the person arrives at Carleton
				uniform_int_distribution<int> arrivalTimeDistribution(510,(1320-timeAtCarleton));
				int beginningOfTimeAtCarleton = arrivalTimeDistribution(generator);
				
				//randomly generates the list of rooms the person will visit during events
				
				vector<int> locationIDs;
				locationIDs.resize(numberOfEvents);
				uniform_int_distribution<int> roomIDDistribution(1,numberOfRooms);
				for (int i = 0; i < numberOfEvents; i++){
					locationIDs[i] = roomIDDistribution(generator);
					if (i > 0){
						//This makes sure that no two adjacent events share a room, as the person would be re-entering the same room.
						if (locationIDs[i] == locationIDs[i-1]){
							if (locationIDs[i] == numberOfRooms){
								locationIDs[i] -= 1;
							} else {
								locationIDs[i] += 1;
							}
						} 
					}
				}
				for (int i = 1; i < numberOfEvents-1; i++){
					assert(locationIDs[i] != locationIDs[i+1]);
				}
				
				//This section distributes the number of 30, 60, and 90 minute events across all of the events to be generated
				vector<int> startTimes;
				vector<int> timeInRooms;
				startTimes.resize(numberOfEvents);
				timeInRooms.resize(numberOfEvents);
				//sum is used to verify that the correct times are used
				sum = 0;
				
				startTimes[0] = beginningOfTimeAtCarleton;
				assert(startTimes[0] <= 1290);
				
				vector<tuple<int, int>> events;
				if (numberOf180MinEvents > 0){
					events.push_back( tuple<int, int>(180,numberOf180MinEvents));
				}
				if (numberOf90MinEvents > 0){
					events.push_back( tuple<int, int>(90,numberOf90MinEvents));
				}
				if (numberOf60MinEvents > 0){
					events.push_back( tuple<int, int>(60,numberOf60MinEvents));
				}
				if (numberOf30MinEvents > 0){
					events.push_back( tuple<int, int>(30,numberOf30MinEvents));
				}

				for (int i = 0; i < events.size(); i++){
				//	cout << get<0>(events[i]) << " " << get<1>(events[i]) << endl;
				}
				
				int elapsedTime = 0;
				int elementSelected = 0;
				
				for (int i = 0; i < numberOfEvents; i++){
					
					uniform_int_distribution<int> eventsSelectionDistribution(0, events.size()-1);
					elementSelected = eventsSelectionDistribution(generator);
					elapsedTime = get<0>(events[elementSelected]);
					get<1>(events[elementSelected]) -= 1;
					//cout << get<1>(events[elementSelected]) << endl;
					assert(get<1>(events[elementSelected]) >= 0);
					
					if (get<1>(events[elementSelected]) < 1){
						events.erase(events.begin()+elementSelected);
					}
						
					if (i > 0){
						startTimes[i] = startTimes[i-1] + timeInRooms[i-1] + travelingEventTimes[i-1];
					}
					
					sum += elapsedTime;
					timeInRooms[i] = elapsedTime;
				}
					
				assert(sum == (numberOf180MinEvents*180 + numberOf90MinEvents*90 + numberOf60MinEvents*60 + numberOf30MinEvents*30));
				
				//The locationPlan vector is initialized for the DecisionMakerBehaviour object
				//locationPlan.resize(numberOfEvents);
				for (int i = 0; i < numberOfEvents; i++){
					string iD = to_string(locationIDs[i]);
					int timeSpent = timeInRooms[i];
					int timeStart = startTimes[i];
					LocationPlan temp = LocationPlan(iD, timeSpent, timeStart);
					locationPlan.push_back(temp);
					if (riskyTravelBehaviour){
						iD = "Tunnels";
					} else {
						iD = "Outdoors";
					}
					timeSpent = travelingEventTimes[i];
					timeStart = startTimes[i] + timeInRooms[i];
					temp = LocationPlan(iD, timeSpent, timeStart);
					locationPlan.push_back(temp);
				}
				string iD;
				if (riskyTravelBehaviour){
					iD = "Tunnels";
				} else {
					iD = "Outdoors";
				}
				int timeSpent = travelTimesDistribution(generator);
				LocationPlan travel = LocationPlan(iD, timeSpent, (startTimes[0] - timeSpent));
				LocationPlan home = LocationPlan("home", 1440 - (startTimes[numberOfEvents-1]+timeInRooms[numberOfEvents-1]+travelingEventTimes[numberOfEvents-1]) + (startTimes[0] - timeSpent), startTimes[numberOfEvents-1]+timeInRooms[numberOfEvents-1]+travelingEventTimes[numberOfEvents-1]);
				locationPlan.push_back(home);
				locationPlan.push_back(travel);
				currStartTime = home.startTime;
				timeInFirstLocation = home.timeInRoomMin;
				
				BehaviourRulesPerson family = BehaviourRulesPerson("friends", 20, 20, 80);
				BehaviourRulesPerson friends = BehaviourRulesPerson("acquaintances", 50, 50, 50);
				BehaviourRulesPerson stranger = BehaviourRulesPerson("stranger", 80, 80, 20);
				behaviourRulesPerson.insert(pair<string, BehaviourRulesPerson>("friends", family));
				behaviourRulesPerson.insert(pair<string, BehaviourRulesPerson>("acquaintances", friends));
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
