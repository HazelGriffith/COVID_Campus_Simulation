#include <iostream>
#include <ifstream>
#include <string>

using namespace std;

int main(){
	string line;
	string goalLine;
	bool 
	
	ifstream stateLog;
	ifstream outputLog;
	
	stateLog.open("../simulation_results/simulation_test_output_ProbGetSick.txt");
	outputLog.open("../simulation_results/simulation_test_output_state.txt");
	
	size_t found;
	if ((stateLog.is_open())&&(outputLog.is_open())){
		getLine(stateLog, line);
		found = line.find_first_not_of("0123456789:");
		if (found == string::npos){
			goalLine = line;
		
		stateLog.close();
		outputLog.close();
	} else {
		cout << "Failed to open logger files" << endl;
	}
	
	return 0;
	
}