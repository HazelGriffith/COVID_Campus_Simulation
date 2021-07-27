CC=g++
CFLAGS=-std=c++17

bin_folder := $(shell mkdir -p bin)
build_folder := $(shell mkdir -p build)

#TARGET TO COMPILE ALL THE TESTS TOGETHER (NOT SIMULATOR)

tinystr.o: tinyXML/tinystr.cpp
	$(CC) -g -c $(CFLAGS) tinyXML/tinystr.cpp -o tinystr.o

tinyxml.o: tinyXML/tinyxml.cpp
	$(CC) -g -c $(CFLAGS) tinyXML/tinyxml.cpp -o tinyxml.o

tinyxmlparser.o: tinyXML/tinyxmlparser.cpp
	$(CC) -g -c $(CFLAGS) tinyXML/tinyxmlparser.cpp -o tinyxmlparser.o
	
tinyxmlerror.o: tinyXML/tinyxmlerror.cpp
	$(CC) -g -c $(CFLAGS) tinyXML/tinyxmlerror.cpp -o tinyxmlerror.o

mainLoadingSavingTest.o: mainLoadingSavingTest.cpp
	$(CC) -g -c $(CFLAGS) mainLoadingSavingTest.cpp -o build/mainLoadingSavingTest.o

#TARGET TO COMPILE ONLY ABP SIMULATOR
test: mainLoadingSavingTest.o tinystr.o tinyxml.o tinyxmlparser.o tinyxmlerror.o
	$(CC) -g -o SAVING_LOADING_TEST build/mainLoadingSavingTest.o tinystr.o tinyxml.o tinyxmlparser.o tinyxmlerror.o
	
#CLEAN COMMANDS
clean: 
	rm -f bin/* build/*