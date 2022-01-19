#include "randomPeopleXMLGenerator.hpp"

#include <string>
#include <math.h>
#include <vector>
#include <assert.h>
#include <random>
#include <iostream>
#include <fstream>
#include <tuple>
#include <chrono>
#include <bits/stdc++.h>

#include "tinyXML/tinyxml.h"
#include "tinyXML/tinystr.h"
#include "../data_structures/DecisionMakerBehaviour.hpp"

using namespace std;

using namespace decision_maker_behaviour_structures;

ifstream roomData;

struct RoomInfo{
	string ID;
	float ventilationRating;
	int socialDistanceThreshold;
	int wearsMaskFactor;
	int socialDistanceFactor;
	int vaccinatedFactor;
	int sickPeopleCO2Factor;
	int highCO2FactorThresholds[4];
	int highCO2Factors[4];
	int respIncreasePerMin;
	float squareMetres;
	float height;
	int maxOccupancy;
};

/*bool compareEvent(Event e1, Event e2){
	return (e1.startTime <= e2.startTime);
}*/

int main(){
	
	vector<Event> eventsCreated;
	
	roomData.open("realRoomData.csv");
	
	string line;
	string name;
	float squareMetres;
	float height;
	float ACH;
	
	size_t firstComma;
	size_t secondComma;
	size_t thirdComma;
	size_t findSlash;
	
	unsigned seed1 = chrono::system_clock::now().time_since_epoch().count();
	default_random_engine generator(seed1);
	uniform_int_distribution<int> mainDistribution(0,100);
	
	int r = 0;
	
	if (roomData.is_open()){
		getline(roomData, line);
		getline(roomData, line);
		while (!line.empty()){
			
			firstComma = line.find(",");
			
			secondComma = line.find(",", firstComma+1);
			
			thirdComma = line.find(",", secondComma+1);
		
			name = line.substr(0, firstComma);
			//findSlash = name.find("/");
			//name = name.substr(0,findSlash) + "," + name.substr(findSlash+1, string::npos);
			
			squareMetres = stof(line.substr(firstComma+1, secondComma-firstComma-1)); 
			
			height = stof(line.substr(secondComma+1, thirdComma-secondComma-1))/1000;
			
			ACH = stof(line.substr(thirdComma+1, string::npos));
		
		
			RoomInfo room;
			room.ID = name;
			
			room.ventilationRating = ACH;
		
			room.squareMetres = squareMetres;
				
			room.height = height;
	
			room.socialDistanceThreshold = floor(room.squareMetres/2);
			room.maxOccupancy = floor(room.socialDistanceThreshold * 0.75);
	
			r = mainDistribution(generator);
			if (r < 50){
				room.wearsMaskFactor = 1;
			} else {
				room.wearsMaskFactor = 2;
			}
	
			r = mainDistribution(generator);
			if (r < 50){
				room.socialDistanceFactor = 1;
			} else {
				room.socialDistanceFactor = 2;
			}
	
			uniform_int_distribution<int> vaccinatedDistribution(1,4);
			r = vaccinatedDistribution(generator);
			room.vaccinatedFactor = r*-1;
	
			uniform_int_distribution<int> sickPeopleCO2Distribution(1,3);
			r = sickPeopleCO2Distribution(generator);
			room.sickPeopleCO2Factor = r;
	
			uniform_int_distribution<int> highCO2FactorThresholdsDistribution(1000,2000);
			for (int j = 0; j < 4; j++){
				if (j == 0){
					r = highCO2FactorThresholdsDistribution(generator);
				} else {
					uniform_int_distribution<int> highCO2FactorThresholdsDistribution(r,2000);
					r = highCO2FactorThresholdsDistribution(generator);
				}
				room.highCO2FactorThresholds[j] = r;
				room.highCO2Factors[j] = j+1;
			}
	
			room.respIncreasePerMin = 340000;
	
			int numberOfEvents;
			int duration;
			string eventType;
			if (room.maxOccupancy >= 60){
				eventType = "Lecture";
				numberOfEvents = 8;
				duration = 90;
			} else if (room.maxOccupancy  >= 40){
				eventType = "Lab";
				numberOfEvents = 4;
				duration = 180;
			} else {
				eventType = "Tutorial";
				numberOfEvents = 11;
				duration = 60;
			}
		
			for (int j = 0; j < numberOfEvents; j++){
				Event event;
				event.ID = room.ID;
				event.type = eventType;
				event.capacity = room.maxOccupancy;
				r = mainDistribution(generator);
				if (r > 70){
					event.overCapacity = true;
				} else {
					event.overCapacity = false;
				}
				event.occupancy = 0;
				event.startTime = 510+(j*duration)+(j*10);
				event.endTime = event.startTime + duration ;
				eventsCreated.push_back(event);
			}
	
	
			TiXmlDocument doc;  
			TiXmlDeclaration* decl = new TiXmlDeclaration( "1.0", "", "" );  
			doc.LinkEndChild(decl); 
     

			TiXmlElement * root = new TiXmlElement("RoomParameters");  
			doc.LinkEndChild( root );  

			// block: ID

			{           
				TiXmlElement * pID = new TiXmlElement( "ID" );      		
				pID->LinkEndChild(new TiXmlText(room.ID.c_str()));
				root->LinkEndChild( pID );  

			}

			// block: ventilationRating

			{           

				TiXmlElement * pVentilationRating = new TiXmlElement( "ventilationRating" );  
				string ventilationRatingStr = to_string(room.ventilationRating);			
				pVentilationRating->LinkEndChild(new TiXmlText(ventilationRatingStr.c_str()));
				root->LinkEndChild( pVentilationRating );  

			}
			
			// block: socialDistanceThreshold

			{           

				TiXmlElement * pSocialDistanceThreshold = new TiXmlElement( "socialDistanceThreshold" );   
				string socialDistanceThresholdStr = to_string(room.socialDistanceThreshold);	
				pSocialDistanceThreshold->LinkEndChild(new TiXmlText(socialDistanceThresholdStr.c_str()));
				root->LinkEndChild( pSocialDistanceThreshold );  

			}
		
			// block: maxOccupancy

			{           

				TiXmlElement * pMaxOccupancy = new TiXmlElement( "maxOccupancy" );   
				string maxOccupancyStr = to_string(room.maxOccupancy);	
				pMaxOccupancy->LinkEndChild(new TiXmlText(maxOccupancyStr.c_str()));
				root->LinkEndChild( pMaxOccupancy );  

			}
			
			// block: wearsMaskFactor

			{           

				TiXmlElement * pWearsMaskFactor = new TiXmlElement( "wearsMaskFactor" );          
				string wearsMaskFactorStr = to_string(room.wearsMaskFactor);
				pWearsMaskFactor->LinkEndChild(new TiXmlText(wearsMaskFactorStr.c_str()));
				root->LinkEndChild( pWearsMaskFactor );  

			}

			// block: socialDistanceFactor

			{    
				TiXmlElement * pSocialDistanceFactor = new TiXmlElement( "socialDistanceFactor" );          
				string socialDistanceFactorStr = to_string(room.socialDistanceFactor);
				pSocialDistanceFactor->LinkEndChild(new TiXmlText(socialDistanceFactorStr.c_str()));
				root->LinkEndChild( pSocialDistanceFactor );  
			}    

			// block: vaccinatedFactor

			{    
				TiXmlElement * pVaccinatedFactor = new TiXmlElement( "vaccinatedFactor" );          
				string vaccinatedFactorStr = to_string(room.vaccinatedFactor);
				pVaccinatedFactor->LinkEndChild(new TiXmlText(vaccinatedFactorStr.c_str()));
				root->LinkEndChild( pVaccinatedFactor );  
			}   

			// block: sickPeopleCO2Factor
	
			{    
				TiXmlElement * pSickPeopleCO2Factor = new TiXmlElement( "sickPeopleCO2Factor" );          
				string sickPeopleCO2FactorStr = to_string(room.sickPeopleCO2Factor);
				pSickPeopleCO2Factor->LinkEndChild(new TiXmlText(sickPeopleCO2FactorStr.c_str()));
				root->LinkEndChild( pSickPeopleCO2Factor );
			}   
			
			// block: wearingMaskCorrectly

			{    
			
				TiXmlElement * pHighCO2FactorThresholds = new TiXmlElement( "highCO2FactorThresholds" ); 
				string highCO2FactorThresholdsStr = "";
				for (int i = 0; i < 4; i++){
					highCO2FactorThresholdsStr += to_string(room.highCO2FactorThresholds[i]);
					if (i != 3){
						highCO2FactorThresholdsStr += ",";
					}
				}
			
				pHighCO2FactorThresholds->LinkEndChild(new TiXmlText(highCO2FactorThresholdsStr.c_str()));
				root->LinkEndChild( pHighCO2FactorThresholds );
			}   
			
			// block: highCO2Factors

			{    
				TiXmlElement * pHighCO2Factors = new TiXmlElement( "highCO2Factors" ); 
				string highCO2FactorsStr = "";
				for (int i = 0; i < 4; i++){
					highCO2FactorsStr += to_string(room.highCO2Factors[i]);
					if (i != 3){
						highCO2FactorsStr += ",";
					}
				}
			
				pHighCO2Factors->LinkEndChild(new TiXmlText(highCO2FactorsStr.c_str()));
				root->LinkEndChild( pHighCO2Factors );
			}   

			//Block: respIncreasePerMin
		
			{
				TiXmlElement * pRespIncreasePerMin = new TiXmlElement( "respIncreasePerMin" );          
				string respIncreasePerMinStr = to_string(room.respIncreasePerMin);
				pRespIncreasePerMin->LinkEndChild(new TiXmlText(respIncreasePerMinStr.c_str()));
				root->LinkEndChild( pRespIncreasePerMin );
			}
        
			//Block: squareMetres
		
			{
				TiXmlElement * pSquareMetres = new TiXmlElement( "squareMetres" );          
				string squareMetresStr = to_string(room.squareMetres);
				pSquareMetres->LinkEndChild(new TiXmlText(squareMetresStr.c_str()));
				root->LinkEndChild( pSquareMetres );
			}

			//Block: height
		
			{
				TiXmlElement * pHeight = new TiXmlElement( "height" );          
				string heightStr = to_string(room.height);
				pHeight->LinkEndChild(new TiXmlText(heightStr.c_str()));
				root->LinkEndChild( pHeight );
			}
			
			string roomPathStr = "rooms/" + room.ID + ".xml";
			
			const char* pFilename = roomPathStr.c_str();
			doc.SaveFile(pFilename); 
		
			getline(roomData, line);
		}
	}
	
	roomData.close();
	
	//sort(events.begin(), events.end(), compareEvent);
	generatePeopleXML(eventsCreated);
	
	return 0;
}