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

randomPeopleXMLGenerator.o: randomPeopleXMLGenerator.cpp
	$(CC) -g -c $(CFLAGS) randomPeopleXMLGenerator.cpp -o build/randomPeopleXMLGenerator.o

#TARGET TO COMPILE ONLY ABP SIMULATOR
generator: randomPeopleXMLGenerator.o tinystr.o tinyxml.o tinyxmlparser.o tinyxmlerror.o
	$(CC) -g -o PEOPLE_XML_GENERATOR build/randomPeopleXMLGenerator.o tinystr.o tinyxml.o tinyxmlparser.o tinyxmlerror.o
	
#CLEAN COMMANDS
clean: 
	rm -f bin/* build/*