#include <string>
#include <iostream>

using namespace std;

int main(){
	float elap_min = 30;
	float ACHVentilation = 4.475;
	float volumeOfRoom = 439.96875;
	int numberPeople = 0;
	float respIncPerMin = 340000;
	float CO2Concentration = 575;
	
	int peoplePerTimeSlot[48] = {0,0,0,0,0,0,0,0,0,0,0,2,2,2,2,2,6,11,15,15,23,34,29,4,6,9,21,39,33,25,23,18,5,34,32,19,4,4,4,3,3,2,2,3,3,2,2,3};
	
	float averageCO2 = 575;
		
	float ventilationRatePerMin = ((ACHVentilation*volumeOfRoom)/60)*1000;
		
	float CO2InPerMin = ventilationRatePerMin * averageCO2;
	
	int timeStep = 10;
	
	float CO2OutPerMin;
		
	float CO2ConcentrationFlow;
	
	for (int i = 0; i < 48; i++){
		
		float CO2PeoplePerMin = respIncPerMin * peoplePerTimeSlot[i];
		
		float temp = elap_min;
	
		
		while (temp >= timeStep){
			CO2OutPerMin = ventilationRatePerMin*CO2Concentration;
			CO2ConcentrationFlow = ((CO2InPerMin + CO2PeoplePerMin - CO2OutPerMin) * timeStep)/(volumeOfRoom*1000);
			CO2Concentration = CO2Concentration + CO2ConcentrationFlow;

			temp = temp - timeStep;

		}
	
		CO2OutPerMin = ventilationRatePerMin*CO2Concentration;
		CO2ConcentrationFlow = ((CO2InPerMin + CO2PeoplePerMin - CO2OutPerMin) * temp)/(volumeOfRoom*1000);
		CO2Concentration = CO2Concentration + CO2ConcentrationFlow;
	
		cout << CO2Concentration << endl;
	}
	
	return 0;
}