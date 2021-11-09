#include "../data_structures/DecisionMakerBehaviour.hpp"

#include <random>
#include <chrono>
#include <string>

using namespace std;
using namespace decision_maker_behaviour_structures;

int main(){
	DecisionMakerBehaviour person;
	int numberOfPeople;
	int numberOfRooms;
	
	cout << "Enter the number of People to be generated: ";
	cin >> numberOfPeople;
	cout << endl;

	cout << "Enter the number of rooms they can visit: ";
	cin >> numberOfRooms;
	cout << endl;
	
	vector<DecisionMakerBehaviour> people;
	people.resize(numberOfPeople);
	
	for (int i = 0; i < numberOfPeople; i++){
		string s = to_string(i+1);
		people[i] = DecisionMakerBehaviour(s, numberOfRooms);
		for (int j = 0; j < numberOfPeople; j++){
			Relationship relation;
			string otherPerson = to_string(j+1);
			string type = "";
			if (j > i){
				
				unsigned seed1 = chrono::system_clock::now().time_since_epoch().count();
				default_random_engine generator(seed1);
				uniform_int_distribution<int> relationshipDistribution(0,100);
				int r = relationshipDistribution(generator);
			
				
				if (r > 80){
					type = "friends";
				} else if (r > 60){
					type = "acquaintance";
				} else {
					type = "stranger";
				}
				
			} else if (j < i){
				type = people[j].relationship[i-1].Relationship_type;
				
			}
			
			if (j != i){
				relation = Relationship(otherPerson, type);
				people[i].relationship.push_back(relation);
			}
		}
			
			
		string studentPathStr = "../data/people/Person" + s + ".xml";
		const char * studentPath = studentPathStr.c_str();
		people[i].save(studentPath);
	}
	
	return 0;
	
}
