#ifndef DecisionMakerBehaviourHPP
#define DecisionMakerBehaviourHPP

#include <string>
#include <iostream>
#include <math.h>
#include <vector>
#include <assert.h>

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
			BehaviourRulesPerson(const string& PersonRelations, const string& SafeDistanceProbability, const string& MaskWearingProbability){
				this->PersonRelations=PersonRelations;
				this->SafeDistanceProbability=SafeDistanceProbability;
				this->MaskWearingProbability=MaskWearingProbability;
			}
			string PersonRelations;
			string SafeDistanceProbability;
			string MaskWearingProbability;
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
			if ((roomID.compare("") == 0)&&(timeInRoomMin == 0)&&(startTime == 0)){
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
			string                          ID;
			string							previousLocation;
			string                          location;
			int 							currStartTime;
			int 							timeInFirstLocation;
			bool                            isSick;
			bool                            wearingMask;
			bool							socialDistance;
			bool                            eatsAlone;
			int                             riskyTravelBehaviourFactor;
			vector<Relationship>            relationship;
			vector<BehaviourRulesPerson>    behaviourRulesPerson;
			vector<BehaviourRulesRoom>      behaviourRulesRoom;
			vector<LocationPlan>            locationPlan;
			LocationPlan					nextLocation;
		
		void setNextLocation(int timeRemaining) {
			vector<LocationPlan>::iterator iter;
			bool changed = false;
			for (iter = locationPlan.begin(); iter != locationPlan.end(); iter++){
				const LocationPlan& lp = *iter;
				
				if ((currStartTime + timeRemaining)%1440 == lp.startTime){
					nextLocation = lp;
					changed = true;
				}
			}
			cout << ID << currStartTime << endl;
			cout << ID << timeRemaining << endl;
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
				// TiXmlElement * pID = new TiXmlElement( "ID" );          
				// pID->LinkEndChild(new TiXmlText(iD.c_str()));
				// root->LinkEndChild( pID );  

			}

			// block: Location

			{           

				TiXmlElement * pLocation = new TiXmlElement( "Location" );          
				pLocation->LinkEndChild(new TiXmlText(location.c_str()));
				root->LinkEndChild( pLocation );  

			}

			// block: isSick

			{    
				//char* isSick = "true";
				//bool IsSick = isSick && strncmp(isSick,"true");==0;

				// TiXmlElement * pIsSick = new TiXmlElement( "IsSick" );
				// pIsSick->LinkEndChild(new TiXmlText(isSick.c_str()));          
				// root->LinkEndChild(pIsSick);
			}      

			//block: SafeDistanceProbability

			// {    

			//   TiXmlElement * pSafeDistanceProbability = new TiXmlElement( "ProbabilityOfSafeDistance" );          
			//   pSafeDistanceProbability->LinkEndChild(new TiXmlText(probabilityOfSafeDistance.c_str()));
			//   root->LinkEndChild( pSafeDistanceProbability ); 

			// } 

			// // block: MaskProbability

			// {    

			//   TiXmlElement * pMaskProbability = new TiXmlElement( "ProbabilityOfWearingMask" );          
			//   pMaskProbability->LinkEndChild(new TiXmlText(probabilityOfWearingMask.c_str()));
			//   root->LinkEndChild( pMaskProbability ); 

			// }     

			//Block: Relationship  ISSUE WITH SAVE
			{
				vector<Relationship>::iterator iter;
				TiXmlElement * RelationshipNode = new TiXmlElement("Relationship");
				root -> LinkEndChild(RelationshipNode);
			
				for(iter = relationship.begin(); iter != relationship.end(); iter++) {
					const Relationship& w=*iter;
					TiXmlElement * RelationID = new TiXmlElement("Person");
					RelationshipNode ->LinkEndChild(RelationID);
					RelationID->SetAttribute("ID", w.PersonID.c_str());
					TiXmlElement * RelationshipType = new TiXmlElement ( "Relationship" );
					RelationshipNode->LinkEndChild(RelationshipType);
					RelationshipNode->SetAttribute("type", w.Relationship_type.c_str());
				}
			}
        
			//Block: BehaviourRulesPerson
			{
				vector<BehaviourRulesPerson>::iterator iter;
				TiXmlElement * BehaviourPersonNode = new TiXmlElement("BehaviourRulesPerson");
				root->LinkEndChild(BehaviourPersonNode);
			
				for(iter=behaviourRulesPerson.begin(); iter != behaviourRulesPerson.end(); iter++){
					const BehaviourRulesPerson& w=*iter;
					TiXmlElement * PersonRelations = new TiXmlElement( "PersonRelations" );
					BehaviourPersonNode->LinkEndChild(PersonRelations);
					PersonRelations->SetAttribute("status", w.PersonRelations.c_str());
					TiXmlElement * SafeDistance = new TiXmlElement( "SafeDistance" );
					SafeDistance->SetAttribute("safeDistanceProb", w.SafeDistanceProbability.c_str());
					BehaviourPersonNode->LinkEndChild( SafeDistance );
					TiXmlElement * MaskWearing = new TiXmlElement("MaskWearing");
					MaskWearing->SetAttribute("maskWearingProb", w.MaskWearingProbability.c_str());
					BehaviourPersonNode->LinkEndChild( MaskWearing );
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


			//Block: Relationship
			{
				relationship.clear();
				TiXmlElement* pRelationshipNode = hRoot.FirstChild( "relationship" ).FirstChild().ToElement();
				for( pRelationshipNode; pRelationshipNode; pRelationshipNode=pRelationshipNode->NextSiblingElement()){
					Relationship c;
					const char* pPerson= pRelationshipNode->Attribute("id");
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
					if(psafeDistanceProb) c.SafeDistanceProbability = psafeDistanceProb;
					const char *pmaskWearingProb = pBehaviourPersonNode->Attribute("maskWearingProb");
					if(pmaskWearingProb) c.MaskWearingProbability = pmaskWearingProb;

					behaviourRulesPerson.push_back(c);
				}
			}

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