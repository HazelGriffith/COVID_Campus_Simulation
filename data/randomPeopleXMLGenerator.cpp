#include "../data_structures/DecisionMakerBehaviour.hpp"

using namespace std;
using namespace decision_maker_behaviour_structures;

int main(){
	DecisionMakerBehaviour person;
	
	int numberOfPeople = 1000;
	int numberOfRooms = 3;
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