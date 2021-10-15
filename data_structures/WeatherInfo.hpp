#ifndef WeatherInfoHPP
#define WeatherInfoHPP

#include <string>
#include <iostream>
#include <math.h>

using namespace std;

struct WeatherInfo{
	WeatherInfo() {}
	
	WeatherInfo(int i_prevState, int i_newState)
				:prevState(i_prevState), newState(i_newState) {}
				  

	int prevState, newState;

};

ostream& operator << (ostream &out, const WeatherInfo &w);

istream& operator >> (istream &in, WeatherInfo &w);

#endif //WeatherInfoHPP