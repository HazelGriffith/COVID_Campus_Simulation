#ifndef PersonInfoHPP
#define PersonInfoHPP

#include <string>
#include <iostream>
#include <math.h>

using namespace std;

struct PersonInfo{
	
	PersonInfo() {}
	
	PersonInfo(string i_personID, bool i_isSick, bool i_wearsMaskCorrectly, 
				string i_roomIDEntering, string i_roomIDLeaving, bool i_socialDistance, int i_minsUntilLeaving)
				:personID(i_personID), isSick(i_isSick), wearsMaskCorrectly(i_wearsMaskCorrectly),
				  roomIDEntering(i_roomIDEntering), roomIDLeaving(i_roomIDLeaving), socialDistance(i_socialDistance),
				  minsUntilLeaving(i_minsUntilLeaving) {}
				  
	
	
	string personID;
	bool isSick;
	bool wearsMaskCorrectly;
	string roomIDEntering;
	string roomIDLeaving;
	bool socialDistance;
	int minsUntilLeaving;
	//NDTime timeUntilLeaving;
};

ostream& operator << (ostream &out, const PersonInfo &p);

istream& operator >> (istream &in, PersonInfo &p);

#endif //PersonInfoHPP