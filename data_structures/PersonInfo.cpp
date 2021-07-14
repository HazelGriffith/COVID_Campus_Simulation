#include <math.h> 
#include <iostream>
#include <string>

#include "PersonInfo.hpp"

/***************************************************/
/************* Output stream ************************/
/***************************************************/

ostream& operator << (ostream &out, const PersonInfo &p){
	out << "The Person: " << p.personID;
	
	if (p.isSick){
		out << " is sick";
	} else {
		out << " is not sick";
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
	
	out << " They are leaving the room with ID " << p.roomIDLeaving;
	out << " They are entering the room with ID " << p.roomIDEntering;
	//out << " and staying for " << p.minsUntilLeaving << " minutes.";
	out << " and staying for " << p.timeUntilLeaving;
	
	return out;
}

/***************************************************/
/************* Input stream ************************/
/***************************************************/

istream& operator >> (istream &in, PersonInfo &p){
	string temp;
	int timeInMinutes;
	int hours;
	int mins;
	int secs;
	int msecs;
	
	//cout << "Enter person's ID ";
	in >> p.personID;
	
	//cout << "Enter T if the person is sick, F if not ";
	in >> temp;
	if (temp.compare("T") == 0){
		p.isSick = true;
	} else {
		p.isSick = false;
	}
	
	//cout << "Enter T if the person wears a mask correctly, F if not ";
	in >> temp;
	if (temp.compare("T") == 0){
		p.wearsMaskCorrectly = true;
	} else {
		p.wearsMaskCorrectly = false;
	}
	
	//cout << "Enter T if the person practices social distance, F if not ";
	in >> temp;
	if (temp.compare("T") == 0){
		p.socialDistance = true;
	} else {
		p.socialDistance = false;
	}
	
	//cout << "Enter the ID of the room the person is entering ";
	in >> p.roomIDEntering;
	
	in >> p.roomIDLeaving;
	
	//cout << "Enter the time in minutes that the person stays in the room for ";
	//in >> p.minsUntilLeaving;
	hours = floor(timeInMinutes/60);
	timeInMinutes = timeInMinutes - hours*60;
	mins = floor(timeInMinutes);
	timeInMinutes = timeInMinutes - mins;
	secs = floor(timeInMinutes*60);
	timeInMinutes = timeInMinutes - (secs/60);
	msecs = (timeInMinutes*60)*1000;
	NDTime time({hours, mins, secs, msecs});
	p.timeUntilLeaving = time;
	
	return in;
	
}