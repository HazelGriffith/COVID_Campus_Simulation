#ifndef PersonInfoHPP
#define PersonInfoHPP

#include "DecisionMakerBehaviour.hpp"

#include <string>
#include <iostream>
#include <math.h>
#include <map>

using namespace std;
using namespace decision_maker_behaviour_structures;

struct PersonInfo{
	
	PersonInfo() {}
	
	PersonInfo(string i_personID, bool i_isSick, bool i_exposed, bool i_vaccinated, bool i_wearsMaskCorrectly, 
				string i_roomIDEntering, int i_timeEntering, string i_roomIDLeaving, int i_timeLeaving, bool i_socialDistance, int i_weatherThreshold, int i_minsUntilLeaving, 
				vector<Relationship> i_relationships, map<string,BehaviourRulesPerson> i_relationshipBehaviour)
				:personID(i_personID), isSick(i_isSick), exposed(i_exposed), vaccinated(i_vaccinated), wearsMaskCorrectly(i_wearsMaskCorrectly),
				  roomIDEntering(i_roomIDEntering), timeEntering(i_timeEntering), roomIDLeaving(i_roomIDLeaving), timeLeaving(i_timeLeaving), socialDistance(i_socialDistance),
				  weatherThreshold(i_weatherThreshold), minsUntilLeaving(i_minsUntilLeaving), relationships(i_relationships), relationshipBehaviour(i_relationshipBehaviour) {}
				  
	
	
	string personID;
	bool isSick;
	bool exposed;
	bool vaccinated;
	bool wearsMaskCorrectly;
	string roomIDEntering;
	int timeEntering;
	string roomIDLeaving;
	int timeLeaving;
	bool socialDistance;
	int weatherThreshold;
	long int minsUntilLeaving;
	vector<Relationship> relationships;
	map<string,BehaviourRulesPerson> relationshipBehaviour;
};

ostream& operator << (ostream &out, const PersonInfo &p);

istream& operator >> (istream &in, PersonInfo &p);

#endif //PersonInfoHPP