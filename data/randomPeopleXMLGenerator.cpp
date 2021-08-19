#include "../data_structures/DecisionMakerBehaviour.hpp"

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
		string studentPathStr = "../data/people/Person" + s + ".xml";
		const char * studentPath = studentPathStr.c_str();
		people[i].save(studentPath);
	}
	
	return 0;
	
}