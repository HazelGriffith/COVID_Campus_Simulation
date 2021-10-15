#include <math.h> 
#include <iostream>
#include <string>

#include "WeatherInfo.hpp"

/***************************************************/
/************* Output stream ************************/
/***************************************************/

ostream& operator << (ostream &out, const WeatherInfo &w){
	out << "The weather has changed from " << w.prevState;
	out << " to " << w.newState;
	
	return out;
}

/***************************************************/
/************* Input stream ************************/
/***************************************************/

istream& operator >> (istream &in, WeatherInfo &w){
	in >> w.prevState;
	in >> w.newState;
	
	return in;
}