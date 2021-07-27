#ifndef PersonBehaviour_HPP
#define PersonBehaviour_HPP

#include <assert.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <cstring>
#include <list>
#include <vector>
#include <map>
#include <chrono>
#include <algorithm>
#include <limits>
#include <math.h>
#include <memory>
#include <iomanip>

#include "tinyXML/tinyxml.h"
#include "tinyXML/tinystr.h"




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
    LocationPlan(const string& Room, const long& timeInRoomMin ){
      this->Room=Room;
      this->timeInRoomMin=timeInRoomMin;

    }
    string Room;
    long timeInRoomMin;

  };



  /*******************************************/
  /**** XML Student Behaviour structures *************/
  /*******************************************/
  class DecisionMakerBehaviour{
  public:
    DecisionMakerBehaviour() = default;
    long                            iD;
    string                          location;
    bool                            isSick;
    long                            distance;
    bool                            wearingMask;
    vector<Relationship>            relationship;
    vector<BehaviourRulesPerson>    behaviourRulesPerson;
    vector<BehaviourRulesRoom>      behaviourRulesRoom;
    vector<LocationPlan>            locationPlan;


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
          for(iter = relationship.begin(); iter != relationship.end(); iter++)
          {
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
          for(iter=behaviourRulesPerson.begin(); iter != behaviourRulesPerson.end(); iter++)
          {
            const BehaviourRulesPerson& w=*iter;
            TiXmlElement * PersonRelations = new TiXmlElement( "PersonRelations" );
            BehaviourPersonNode->LinkEndChild(PersonRelations);
            PersonRelations->SetAttribute("status", w.PersonRelations.c_str());
            TiXmlElement * SafeDistance = new TiXmlElement( "SafeDistance" );
            SafeDistance->SetAttribute("probability", w.SafeDistanceProbability.c_str());
            BehaviourPersonNode->LinkEndChild( SafeDistance );
            TiXmlElement * MaskWearing = new TiXmlElement("MaskWearing");
            MaskWearing->SetAttribute("chance", w.MaskWearingProbability.c_str());
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
        //       GroupNode->SetAttribute("probability", w2.SafeDistanceProbability.c_str());
        //       BehaviourRoomNode->LinkEndChild( GroupNode );
        //       TiXmlElement * MaskWearing = new TiXmlElement(" MaskWearing ");
        //       GroupNode->SetAttribute("chance", w2.MaskProbability.c_str());
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
          iD =strtol(pID,NULL,10);
          cout<<setfill('0') << setw(4)<< iD <<endl;
        }

        // block: Location
        { 
		  
          pElem=hRoot.FirstChild("Location").ToElement();
		  
          if (!pElem) return;
		  
          const char* pLocation = pElem->GetText();
          if(pLocation) location = pLocation;
		  cout << location << endl;
        }

        // block: IsSick
        { 
          //char isSick = 0;
          //bool IsSick = isSick != 0;

          // const char* IsSick = pElem->GetText();
          // isSick = IsSick && strncmp(IsSick,"true",5)==0;

          pElem=hRoot.FirstChild("IsSick").ToElement();
          if (!pElem) return;
          const char* pIsSick = pElem->GetText();
          if(strncmp(pIsSick,"False",2)==0){
            isSick = false;
          }else{
            isSick = true;
          }
          cout<< *pIsSick <<endl;
        }

        // block: Distance
        { 
          pElem=hRoot.FirstChild("Distance").ToElement();
          if (!pElem) return;
          const char* pDistance = pElem->GetText();
          distance =strtol(pDistance,NULL,10);
		  cout << distance << endl;
        }
				// block: MaskWearing
        { 
          pElem=hRoot.FirstChild("WearingMask").ToElement();
          if (!pElem) return;
          const char* pWearingMask = pElem->GetText();
          if(strncmp(pWearingMask,"False",2)==0){
            wearingMask = false;
          }else{
            wearingMask = true;
          }
		  cout << *pWearingMask << endl;
        }


        //Block: Relationship
        {
          relationship.clear(); // trash existing list    
          TiXmlElement* pRelationshipNode = hRoot.FirstChild( "Relationship" ).FirstChild().ToElement();
          for( pRelationshipNode; pRelationshipNode; pRelationshipNode=pRelationshipNode->NextSiblingElement())
          {
            Relationship c;
            const char* pPerson= pRelationshipNode->Attribute("id");
            if(pPerson) c.PersonID = pPerson;
            const char *pRelationship_type = pRelationshipNode->Attribute("type");
            if(pRelationship_type) c.Relationship_type = pRelationship_type;
            cout <<c.PersonID <<c.Relationship_type <<endl;
            relationship.push_back(c);
          }
        }

        //Block: BehaviourRulesPerson
        {
          behaviourRulesPerson.clear();
          TiXmlElement* pBehaviourPersonNode = hRoot.FirstChild( "BehaviourRulesPerson" ).FirstChild().ToElement();
          for(pBehaviourPersonNode; pBehaviourPersonNode; pBehaviourPersonNode=pBehaviourPersonNode->NextSiblingElement())
          {
            BehaviourRulesPerson c;
            if(!pBehaviourPersonNode) return;
            const char *pPersonRelations=pBehaviourPersonNode->Attribute("status");
            if(pPersonRelations) c.PersonRelations = pPersonRelations;
            const char *pProbability=pBehaviourPersonNode->Attribute("probability");
            if(pProbability) c.SafeDistanceProbability = pProbability;
            const char *pChance = pBehaviourPersonNode->Attribute("chance");
            if(pChance) c.MaskWearingProbability = pChance;

            behaviourRulesPerson.push_back(c);
          }
        }

        {
          behaviourRulesRoom.clear();
          TiXmlElement* pBehaviourRoomNode = hRoot.FirstChild( "BehaviourRulesRoom" ).FirstChild().ToElement();
          for(pBehaviourRoomNode; pBehaviourRoomNode; pBehaviourRoomNode=pBehaviourRoomNode->NextSiblingElement())
          {
            BehaviourRulesRoom c;
            if(!pBehaviourRoomNode) return;
            const char *pBehaviourRoomSize = pBehaviourRoomNode->Attribute("size");
            if(pBehaviourRoomSize) c.roomSize = pBehaviourRoomSize;
            cout <<c.roomSize <<endl;
            TiXmlElement* pGroupBehaviourNode=pBehaviourRoomNode->FirstChildElement();
            for(pGroupBehaviourNode; pGroupBehaviourNode; pGroupBehaviourNode=pGroupBehaviourNode->NextSiblingElement())
            {
              NumberOfPeople d;
              if(!pGroupBehaviourNode) return;
              const char* pgroupBehaviours=pGroupBehaviourNode->Attribute("group");
              if (pgroupBehaviours) d.group = pgroupBehaviours;

              const char *pProbability=pGroupBehaviourNode->Attribute("probability");
              if(pProbability) d.SafeDistanceProbability = pProbability;
              const char *pChance = pGroupBehaviourNode->Attribute("chance");
              if(pChance) d.MaskProbability = pChance;
              cout <<d.group <<" " <<d.SafeDistanceProbability <<" " <<d.MaskProbability <<endl;
              c.groupBehaviour.push_back(d);
            }
            behaviourRulesRoom.push_back(c);
          }
        }
        
        //Block: LocationPlan
        {
          locationPlan.clear(); // trash existing list    
          TiXmlElement* pLocationPlanNode = hRoot.FirstChild("LocationPlan").FirstChild().ToElement();
          for( pLocationPlanNode; pLocationPlanNode; pLocationPlanNode=pLocationPlanNode->NextSiblingElement())
          {
            LocationPlan c;
            const char* pRoom= pLocationPlanNode->Attribute("room");
            if(pRoom) c.Room = pRoom;
            const char *pTime_in_room = pLocationPlanNode->Attribute("timeinroom");
            //if(pTime_in_room) c.timeInRoomMin = pTime_in_room;
            c.timeInRoomMin =strtol(pTime_in_room,NULL,10);
            cout <<c.Room << " " <<c.timeInRoomMin <<endl;
            locationPlan.push_back(c);
          }
        }

      
   }	
 
};
}


#endif // PersonBehaviour_HPP