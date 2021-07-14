#ifndef ProbGetSickHPP
#define ProbGetSickHPP

#include <string>
#include <iostream>
#include <math.h>

using namespace std;

struct ProbGetSick{
	ProbGetSick() {}
	
	ProbGetSick(string i_personID, float i_probSick)
				:personID(i_personID), probSick(i_probSick) {}
				  

	string personID;
	float probSick;
	//ProbGetSick *next = NULL;
};

ostream& operator << (ostream &out, const ProbGetSick &p);

istream& operator >> (istream &in, ProbGetSick &p);

#endif //ProbGetSickHPP