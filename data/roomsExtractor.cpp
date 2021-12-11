#include <iostream>
#include <fstream>
#include <string>
#include <tuple>
#include <chrono>
#include <random>
#include <vector>
#include <math.h>

#include "tinyXML/tinyxml.h"
#include "tinyXML/tinystr.h"

using namespace std;

ifstream roomData;

struct RoomInfo{
	string ID;
	int ventilationRating;
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

int main(){
	
	roomData.open("realRoomData.txt");
	
	string line;
	string name;
	float squareMetres;
	float height;
	
	size_t firstTab;
	size_t secondTab;
	size_t findSlash;
	
	unsigned seed1 = chrono::system_clock::now().time_since_epoch().count();
	default_random_engine generator(seed1);
	uniform_int_distribution<int> mainDistribution(0,100);
	
	int r = 0;
	if (roomData.is_open()){
		getline(roomData, line);
		while (!line.empty()){
			
			firstTab = line.find("\t");
			
			secondTab = line.find("\t", firstTab+1);
		
			name = line.substr(0, firstTab);
			findSlash = name.find("/");
			name = name.substr(0,findSlash) + "," + name.substr(findSlash+1, string::npos);
			
			squareMetres = stof(line.substr(firstTab+1, secondTab-firstTab-1)); 
			
			height = stof(line.substr(secondTab+1, string::npos))/1000;
			
			
		
		
			RoomInfo room;
			room.ID = name;
			
			uniform_int_distribution<int> ventilationRatingDistribution(1,7);
			r = ventilationRatingDistribution(generator);
			room.ventilationRating = r - (0.5*(r-1));
		
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
			cout << room.ID << endl;
			string roomPathStr = "rooms/" + room.ID + ".xml";
	
			const char* pFilename = roomPathStr.c_str();
			doc.SaveFile(pFilename); 
		
			getline(roomData, line);
		}
	}
	roomData.close();
	return 0;
}