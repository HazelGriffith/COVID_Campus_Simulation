#include <math.h> 
#include <iostream>
#include <string>

#include "ProbGetSick.hpp"

/***************************************************/
/************* Output stream ************************/
/***************************************************/

ostream& operator << (ostream &out, const ProbGetSick &p){
	out << "The Person: " << p.personID;
	out << " has a probability of " << p.probSick << " to get sick.";
	
	return out;
}

/***************************************************/
/************* Input stream ************************/
/***************************************************/

istream& operator >> (istream &in, ProbGetSick &p){
	in >> p.personID;

	in >> p.probSick;
	
	return in;
}