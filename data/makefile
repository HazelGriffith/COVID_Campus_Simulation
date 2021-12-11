CC=g++
CFLAGS=-std=c++17

INCLUDEDESTIMES=-I../../../DESTimes/include

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

parser.o: parser.cpp
	$(CC) -g -c $(CFLAGS) $(INCLUDEDESTIMES) parser.cpp -o build/parser.o

CO2Validation.o: CO2Validation.cpp
	$(CC) -g -c $(CFLAGS) CO2Validation.cpp -o build/CO2Validation.o

randomRoomsXMLGenerator.o: randomRoomsXMLGenerator.cpp
	$(CC) -g -c $(CFLAGS) randomRoomsXMLGenerator.cpp -o build/randomRoomsXMLGenerator.o

randomGenerator.o: randomGenerator.cpp
	$(CC) -g -c $(CFLAGS) randomGenerator.cpp -o build/randomGenerator.o

roomsExtractor.o: roomsExtractor.cpp
	$(CC) -g -c $(CFLAGS) roomsExtractor.cpp -o build/roomsExtractor.o

#TARGET TO COMPILE ONLY People XML generator
peopleGenerator: randomPeopleXMLGenerator.o tinystr.o tinyxml.o tinyxmlparser.o tinyxmlerror.o
	$(CC) -g -o PEOPLE_XML_GENERATOR build/randomPeopleXMLGenerator.o tinystr.o tinyxml.o tinyxmlparser.o tinyxmlerror.o
	
#TARGET TO COMPILE ONLY PARSER
parser: parser.o
	$(CC) -g -o PARSER build/parser.o

#TARGET TO COMPILE ONLY CO2 CONCENTRATION VALIDATION
validator: CO2Validation.o
	$(CC) -g -o CO2_VALIDATION build/CO2Validation.o

#TARGET TO COMPILE ONLY Rooms XML generator
roomsGenerator: randomRoomsXMLGenerator.o tinystr.o tinyxml.o tinyxmlparser.o tinyxmlerror.o
	$(CC) -g -o ROOMS_XML_GENERATOR build/randomRoomsXMLGenerator.o tinystr.o tinyxml.o tinyxmlparser.o tinyxmlerror.o
	
#TARGET TO COMPILE ONLY GENERATOR
generator: randomGenerator.o tinystr.o tinyxml.o tinyxmlparser.o tinyxmlerror.o
	$(CC) -g -o GENERATOR build/randomGenerator.o tinystr.o tinyxml.o tinyxmlparser.o tinyxmlerror.o

#TARGET TO COMPILE ONLY ROOMS EXTRACTOR
extractor: roomsExtractor.o tinystr.o tinyxml.o tinyxmlparser.o tinyxmlerror.o
	$(CC) -g -o EXTRACTOR build/roomsExtractor.o tinystr.o tinyxml.o tinyxmlparser.o tinyxmlerror.o

#CLEAN COMMANDS
clean: 
	rm -f bin/* build/*