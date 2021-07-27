#include "../data_structures/DecisionMakerBehaviour.hpp"

using namespace std;
using namespace decision_maker_behaviour_structures;

int main(){
	DecisionMakerBehaviour person;
	const char * in = "Student1.xml";
	const char * out = "TESTSAVE.xml";
	person.load(in);
	
	person.save(out);
	
	return 0;
	
}