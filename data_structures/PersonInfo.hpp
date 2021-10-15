#ifndef PersonInfoHPP
#define PersonInfoHPP

#include <string>
#include <iostream>
#include <math.h>

using namespace std;

struct PersonInfo{
	
	PersonInfo() {}
	
	PersonInfo(string i_personID, bool i_isSick, bool i_exposed, bool i_vaccinated, bool i_wearsMaskCorrectly, 
				string i_roomIDEntering, string i_roomIDLeaving, bool i_socialDistance, int i_weatherThreshold, int i_minsUntilLeaving)
				:personID(i_personID), isSick(i_isSick), exposed(i_exposed), vaccinated(i_vaccinated), wearsMaskCorrectly(i_wearsMaskCorrectly),
				  roomIDEntering(i_roomIDEntering), roomIDLeaving(i_roomIDLeaving), socialDistance(i_socialDistance), weatherThreshold(i_weatherThreshold),
				  minsUntilLeaving(i_minsUntilLeaving) {}
				  
	
	
	string personID;
	bool isSick;
	bool exposed;
	bool vaccinated;
	bool wearsMaskCorrectly;
	string roomIDEntering;
	string roomIDLeaving;
	bool socialDistance;
	int weatherThreshold;
	long int minsUntilLeaving;
	//NDTime timeUntilLeaving;
};

ostream& operator << (ostream &out, const PersonInfo &p);

istream& operator >> (istream &in, PersonInfo &p);

#endif //PersonInfoHPP