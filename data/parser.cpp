#include <iostream>
#include <fstream>
#include <string>
#include <NDTime.hpp>
#include <tuple>

using namespace std;

ifstream stateLog;
ifstream outputLog;
ofstream parsedData;

int main(){
	string line;
	string currTimeStamp;
	string nextTimeStamp = "";
	
	streampos timeStampPos;
	streampos tempPos;
	
	stateLog.open("../simulation_results/simulation_test_output_ProbGetSick.txt");
	outputLog.open("../simulation_results/simulation_test_output_state.txt");
	
	size_t found;
	size_t room;
	size_t start;
	size_t end;
	
	vector <vector<float>> CO2ConcentrationPerRoom;
	vector <vector<string>> occupancyPerRoom;
	vector <tuple<NDTime, string, float>> infectionProbabilitiesPerRoom;
	vector <NDTime> timeStamps;
	if ((stateLog.is_open())&&(outputLog.is_open())){
		int numberOfRooms;
		cout << "Enter the number of rooms: ";
		cin >> numberOfRooms;
		cout << endl;
		CO2ConcentrationPerRoom.resize(numberOfRooms+1);
		occupancyPerRoom.resize(numberOfRooms+2);
		
		int numberOfPeople;
		cout << "Enter the number of people: ";
		cin >> numberOfPeople;
		cout << endl;
		
		getline(outputLog, line);
		
		found = line.find_first_not_of("0123456789:");
		int k = 1;
		
		while (!line.empty()){
			//Obtain the last section of a given timeStamp
			//locates first timeStamp
			while (found != string::npos){
				getline(outputLog, line);
				found = line.find_first_not_of("0123456789:");
			}
			currTimeStamp = line;
			timeStamps.push_back(NDTime(currTimeStamp));
			timeStampPos = outputLog.tellg();

			//cycles through proceeding timeStamps until a different one is found
			nextTimeStamp = currTimeStamp;
			while (nextTimeStamp.compare(currTimeStamp) == 0){
				tempPos = timeStampPos;
				getline(outputLog, line);
				found = line.find_first_not_of("0123456789:");
				while ((found != string::npos)&&(!line.empty())){
					getline(outputLog, line);
					timeStampPos = outputLog.tellg();
					getline(outputLog, line);
					found = line.find_first_not_of("0123456789:");
				}
				nextTimeStamp = line;
				
			}
			
			//The stream position is set to that of the last timeStamp that matches the original
			outputLog.clear();
			outputLog.seekg(tempPos);
			getline(outputLog, line);
			getline(outputLog, line);
			room = line.find("Room");
			int count = 0;
			while (room != string::npos){
				
				found = line.find("the room: ");
				end = line.find(" average");
				start = found + 10;
				string value = line.substr(start, end-start);
				CO2ConcentrationPerRoom[count].push_back(stof(value));
				
				found = line.find("people ID's in room: ");
				end = line.find(" students travelling at same time");
				start = found + 21;
				value = line.substr(start, end-start);
				if (value.empty()){
					value = "none";
				}
				occupancyPerRoom[count].push_back(value);
			
				for (int i = 0; i < 6; i++){
					getline(outputLog, line);
				}
				room = line.find("Room");
				count++;
			}
			
		
			for (int i = 0; i < numberOfPeople; i++){
				for (int j = 0; j < 4; j++){
					getline(outputLog, line);
				}
			}
			
			if (!line.empty()){
				found = line.find("people ID's in room: ");
				end = string::npos;
				start = found + 21;
				string value = line.substr(start, end);
				if (value.empty()){
					value = "none";
				}
				occupancyPerRoom[occupancyPerRoom.size()-1].push_back(value);
			}
			
			for (int i = 0; i < 5; i++){
				getline(outputLog, line);
			}
			
			getline(outputLog, line);
			found = line.find_first_not_of("0123456789:");
		}
		
		getline(stateLog, line);
		found = line.find_first_not_of("0123456789:");
		while (!line.empty()){
			//Obtain the last section of a given timeStamp
			//locates first timeStamp
			
			while (found != string::npos){

				found = line.find("RoomModelPorts::out");

				if (found != string::npos){
					string roomName;
					found = line.rfind("Room");
					end = string::npos;
					roomName = line.substr(found, end);
					
					
					NDTime timeStamp(currTimeStamp);
					
					found = line.find("probability of ");
					end = line.find(" to get sick");
					start = found + 15;
					string value = line.substr(start, end-start);
					
					tuple<NDTime, string, float> currInfectionProb (timeStamp, roomName, stof(value));
					infectionProbabilitiesPerRoom.push_back(currInfectionProb);
					
					size_t moreProbs = line.find(".,");
					while (moreProbs != string::npos){
					
						found = line.find("probability of ",moreProbs+2);
						end = line.find(" to get sick",moreProbs+2);
						start = found + 15;
						string value = line.substr(start, end-start);
					
						tuple<NDTime, string, float> currInfectionProb (timeStamp, roomName, stof(value));
						infectionProbabilitiesPerRoom.push_back(currInfectionProb);
						
						size_t nextProbs = moreProbs;
						moreProbs = line.find(".,",nextProbs+2);
						
					}
				}
				getline(stateLog, line);

				found = line.find_first_not_of("0123456789:");

			}
			currTimeStamp = line;
			getline(stateLog, line);
			found = line.find_first_not_of("0123456789:");
			
		}
		parsedData.open("ParsedData.txt");
		if (parsedData.is_open()){
			
			for (int i = 0; i < CO2ConcentrationPerRoom.size(); i++){
				for (int j = 0; j < CO2ConcentrationPerRoom[i].size(); j++){
					if (i < CO2ConcentrationPerRoom.size()-1){
						parsedData << "Room" << i+1 << " CO2: " << CO2ConcentrationPerRoom[i][j] << endl;
					} else {
						parsedData << "RoomTunnels CO2: " << CO2ConcentrationPerRoom[i][j] << endl;
					}
				}
			}
			for (int i = 0; i < occupancyPerRoom.size(); i++){
				for (int j = 0; j < occupancyPerRoom[i].size(); j++){
					if (i < occupancyPerRoom.size()-2){
						parsedData << "Room" << i+1 << " occupancy: " << occupancyPerRoom[i][j] << endl;
					} else if (i < occupancyPerRoom.size()-1){
						parsedData << "RoomTunnels occupancy: " << occupancyPerRoom[i][j] << endl;
					} else {
						parsedData << "Outdoors occupancy: " << occupancyPerRoom[i][j] << endl;
					}
				}
			}
			for (int i = 0; i < timeStamps.size(); i++){
				parsedData << timeStamps[i] << endl;
			}
			for (int i = 0; i < infectionProbabilitiesPerRoom.size(); i++){
				parsedData << get<0>(infectionProbabilitiesPerRoom[i]) << " " << get<1>(infectionProbabilitiesPerRoom[i]) << " " << to_string(get<2>(infectionProbabilitiesPerRoom[i])) << endl;
			}
			parsedData.close();
		} else {
			cout << "Failed to open output file" << endl;
		}
	} else {
		cout << "Failed to open logger files" << endl;
	}
	
	stateLog.close();
	outputLog.close();
	
	return 0;
	
}