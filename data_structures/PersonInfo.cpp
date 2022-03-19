#include <math.h> 
#include <iostream>
#include <string>

#include "PersonInfo.hpp"

using namespace decision_maker_behaviour_structures;
using namespace std;

/***************************************************/
/************* Output stream ************************/
/***************************************************/

ostream& operator << (ostream &out, const PersonInfo &p){
	out << "The Person: " << p.personID;
	
	if (p.isSick){
		out << " is sick";
	} else {
		out << " is not sick";
		if (p.exposed){
			out << ", has been exposed";
		} else {
			out << ", has not been exposed";
		}
	}
	
	if (p.vaccinated){
		out << ", is vaccinated";
	} else {
		out << ", is not vaccinated";
	}
	
	if (p.wearsMaskCorrectly){
		out << ", wears a mask correctly";
	} else {
		out << ", wears a mask incorrectly";
	}
	
	if (p.socialDistance){
		out << ", and does socially distance themselves.";
	} else {
		out << ", and does not socially distance themselves.";
	}
	
	out << " They have a weather threshold of " << p.weatherThreshold;
	
	out << " They are leaving the room with ID " << p.roomIDLeaving;
	out << " They are entering the room with ID " << p.roomIDEntering;
	out << " and staying for " << p.minsUntilLeaving << " minutes.";
	//out << " and staying for " << p.timeUntilLeaving;
	
	return out;
}

/***************************************************/
/************* Input stream ************************/
/***************************************************/

istream& operator >> (istream &in, PersonInfo &p){
	string tempStr;
	int tempInt;
	int timeInMinutes;
	int hours;
	int mins;
	int secs;
	int msecs;
	
	//cout << "Enter person's ID ";
	in >> p.personID;
	
	//cout << "Enter T if the person is sick, F if not ";
	in >> tempStr;
	if (tempStr.compare("T") == 0){
		p.isSick = true;
	} else {
		p.isSick = false;
	}
	
	in >> tempStr;
	if (tempStr.compare("T") == 0){
		p.exposed = true;
	} else {
		p.exposed = false;
	}
	
	in >> tempStr;
	if (tempStr.compare("T") == 0){
		p.vaccinated = true;
	} else {
		p.vaccinated = false;
	}
	
	//cout << "Enter T if the person wears a mask correctly, F if not ";
	in >> tempStr;
	if (tempStr.compare("T") == 0){
		p.wearsMaskCorrectly = true;
	} else {
		p.wearsMaskCorrectly = false;
	}
	
	//cout << "Enter T if the person practices social distance, F if not ";
	in >> tempStr;
	if (tempStr.compare("T") == 0){
		p.socialDistance = true;
	} else {
		p.socialDistance = false;
	}
	
	//cout << "Enter the weather threshold of the person ";
	in >> p.weatherThreshold;
	
	//cout << "Enter the ID of the room the person is entering ";
	in >> p.roomIDEntering;
	
	in >> tempStr;
	
	p.timeEntering = stoi(tempStr);
	
	in >> p.roomIDLeaving;
	
	in >> tempStr;
	
	p.timeLeaving = stoi(tempStr);
	
	in >> p.minsUntilLeaving;
	
	map<string,BehaviourRulesPerson> behaviourRulesPerson;

	BehaviourRulesPerson family = BehaviourRulesPerson("friends", 20, 20, 80);
	BehaviourRulesPerson friends = BehaviourRulesPerson("acquaintance", 50, 50, 50);
	BehaviourRulesPerson stranger = BehaviourRulesPerson("stranger", 80, 80, 20);
	behaviourRulesPerson.insert(pair<string, BehaviourRulesPerson>("friends", family));
	behaviourRulesPerson.insert(pair<string, BehaviourRulesPerson>("acquaintance", friends));
	behaviourRulesPerson.insert(pair<string, BehaviourRulesPerson>("stranger", stranger));
	p.relationshipBehaviour = behaviourRulesPerson;
	
	string ID;
	string relationType;
	in >> ID;
	in >> relationType;
	Relationship r = Relationship(ID, relationType);
	p.relationships.push_back(r);
	
	in >> ID;
	in >> relationType;
	r = Relationship(ID, relationType);
	p.relationships.push_back(r);
	
	return in;
	
}