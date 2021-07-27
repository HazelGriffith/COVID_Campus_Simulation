CC=g++
CFLAGS=-std=c++17

INCLUDECADMIUM=-I../../cadmium/include
INCLUDEDESTIMES=-I../../DESTimes/include

#CREATE BIN AND BUILD FOLDERS TO SAVE THE COMPILED FILES DURING RUNTIME
bin_folder := $(shell mkdir -p bin)
build_folder := $(shell mkdir -p build)
results_folder := $(shell mkdir -p simulation_results)

#TARGET TO COMPILE ALL THE TESTS TOGETHER (NOT SIMULATOR)
tinystr.o: data/tinyXML/tinystr.cpp
	$(CC) -g -c $(CFLAGS) data/tinyXML/tinystr.cpp -o build/tinystr.o

tinyxml.o: data/tinyXML/tinyxml.cpp
	$(CC) -g -c $(CFLAGS) data/tinyXML/tinyxml.cpp -o build/tinyxml.o

tinyxmlparser.o: data/tinyXML/tinyxmlparser.cpp
	$(CC) -g -c $(CFLAGS) data/tinyXML/tinyxmlparser.cpp -o build/tinyxmlparser.o
	
tinyxmlerror.o: data/tinyXML/tinyxmlerror.cpp
	$(CC) -g -c $(CFLAGS) data/tinyXML/tinyxmlerror.cpp -o build/tinyxmlerror.o
	
PersonInfo.o: data_structures/PersonInfo.cpp
	$(CC) -g -c $(CFLAGS) $(INCLUDECADMIUM) $(INCLUDEDESTIMES) data_structures/PersonInfo.cpp -o build/PersonInfo.o
	
ProbGetSick.o: data_structures/ProbGetSick.cpp
	$(CC) -g -c $(CFLAGS) $(INCLUDECADMIUM) $(INCLUDEDESTIMES) data_structures/ProbGetSick.cpp -o build/ProbGetSick.o
	
main_top.o: top_model/main.cpp
	$(CC) -g -c $(CFLAGS) $(INCLUDECADMIUM) $(INCLUDEDESTIMES) top_model/main.cpp -o build/main_top.o
	
main_filter_People_In_test.o: test/main_filter_People_In_test.cpp
	$(CC) -g -c $(CFLAGS) $(INCLUDECADMIUM) $(INCLUDEDESTIMES) test/main_filter_People_In_test.cpp -o build/main_filter_People_In_test.o
	
main_filter_People_Out_test.o: test/main_filter_People_Out_test.cpp
	$(CC) -g -c $(CFLAGS) $(INCLUDECADMIUM) $(INCLUDEDESTIMES) test/main_filter_People_Out_test.cpp -o build/main_filter_People_Out_test.o
	
main_room_test.o: test/main_room_test.cpp
	$(CC) -g -c $(CFLAGS) $(INCLUDECADMIUM) $(INCLUDEDESTIMES) test/main_room_test.cpp -o build/main_room_test.o
	
main_multiple_room_test.o: test/main_multiple_room_test.cpp
	$(CC) -g -c $(CFLAGS) $(INCLUDECADMIUM) $(INCLUDEDESTIMES) test/main_multiple_room_test.cpp -o build/main_multiple_room_test.o

main_filter_probGetSick_test.o: test/main_filter_probGetSick_test.cpp
	$(CC) -g -c $(CFLAGS) $(INCLUDECADMIUM) $(INCLUDEDESTIMES) test/main_filter_probGetSick_test.cpp -o build/main_filter_probGetSick_test.o

main_person_test.o: test/main_person_test.cpp
	$(CC) -g -c $(CFLAGS) $(INCLUDECADMIUM) $(INCLUDEDESTIMES) test/main_person_test.cpp -o build/main_person_test.o
	
main_personAndFilter_test.o: test/main_personAndFilter_test.cpp
	$(CC) -g -c $(CFLAGS) $(INCLUDECADMIUM) $(INCLUDEDESTIMES) test/main_personAndFilter_test.cpp -o build/main_personAndFilter_test.o

tests: main_filter_People_In_test.o main_filter_People_Out_test.o main_room_test.o main_multiple_room_test.o main_filter_probGetSick_test.o main_person_test.o main_personAndFilter_test.o PersonInfo.o ProbGetSick.o tinystr.o tinyxml.o tinyxmlparser.o tinyxmlerror.o
		$(CC) -g -o bin/FILTER_PEOPLE_IN_TEST build/main_filter_People_In_test.o build/PersonInfo.o
		$(CC) -g -o bin/FILTER_PEOPLE_OUT_TEST build/main_filter_People_Out_test.o build/PersonInfo.o
		$(CC) -g -o bin/ROOM_TEST build/main_room_test.o build/PersonInfo.o build/ProbGetSick.o
		$(CC) -g -o bin/MULTIPLE_ROOM_TEST build/main_multiple_room_test.o build/PersonInfo.o build/ProbGetSick.o build/tinystr.o build/tinyxml.o build/tinyxmlparser.o build/tinyxmlerror.o
		$(CC) -g -o bin/FILTER_PROBGETSICK_TEST build/main_filter_probGetSick_test.o build/ProbGetSick.o
		$(CC) -g -o bin/PERSON_TEST build/main_person_test.o build/ProbGetSick.o build/PersonInfo.o build/tinystr.o build/tinyxml.o build/tinyxmlparser.o build/tinyxmlerror.o
		$(CC) -g -o bin/PERSON_AND_FILTER_TEST build/main_personAndFilter_test.o build/ProbGetSick.o build/PersonInfo.o build/tinystr.o build/tinyxml.o build/tinyxmlparser.o build/tinyxmlerror.o

#TARGET TO COMPILE ONLY ABP SIMULATOR
simulator: main_top.o PersonInfo.o ProbGetSick.o tinystr.o tinyxml.o tinyxmlparser.o tinyxmlerror.o
	$(CC) -g -o bin/ROOM_SIM build/main_top.o build/PersonInfo.o build/ProbGetSick.o build/tinystr.o build/tinyxml.o build/tinyxmlparser.o build/tinyxmlerror.o
	
#TARGET TO COMPILE EVERYTHING (ABP SIMULATOR + TESTS TOGETHER)
all: simulator tests
	
#CLEAN COMMANDS
clean: 
	rm -f bin/* build/*