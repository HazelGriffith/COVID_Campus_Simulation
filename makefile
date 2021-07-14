CC=g++
CFLAGS=-std=c++17

INCLUDECADMIUM=-I../../cadmium/include
INCLUDEDESTIMES=-I../../DESTimes/include

#CREATE BIN AND BUILD FOLDERS TO SAVE THE COMPILED FILES DURING RUNTIME
bin_folder := $(shell mkdir -p bin)
build_folder := $(shell mkdir -p build)
results_folder := $(shell mkdir -p simulation_results)

#TARGET TO COMPILE ALL THE TESTS TOGETHER (NOT SIMULATOR)
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

tests: main_filter_People_In_test.o main_filter_People_Out_test.o main_room_test.o PersonInfo.o ProbGetSick.o
		$(CC) -g -o bin/FILTER_PEOPLE_IN_TEST build/main_filter_People_In_test.o build/PersonInfo.o
		$(CC) -g -o bin/FILTER_PEOPLE_OUT_TEST build/main_filter_People_Out_test.o build/PersonInfo.o
		$(CC) -g -o bin/ROOM_TEST build/main_room_test.o build/PersonInfo.o build/ProbGetSick.o

#TARGET TO COMPILE ONLY ABP SIMULATOR
simulator: main_top.o PersonInfo.o ProbGetSick.o 
	$(CC) -g -o bin/ROOM_SIM build/main_top.o build/PersonInfo.o build/ProbGetSick.o
	
#TARGET TO COMPILE EVERYTHING (ABP SIMULATOR + TESTS TOGETHER)
all: simulator tests
	
#CLEAN COMMANDS
clean: 
	rm -f bin/* build/*