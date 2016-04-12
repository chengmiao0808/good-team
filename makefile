CC = g++

all: dchat

dchat: dchat.o utility.o
	g++ -std=c++11 -o dchat dchat.o utility.o member.o


dchat.o: dchat.cpp dchat.h heartbeat.h msgque.h msgPack.h
	g++ -std=c++11  -c dchat.cpp

utility.o: utility.cpp utility.h msgque.h msgPack.h
	g++ -std=c++11  -c utility.cpp

member.o: member.cpp member.h msgque.h msgPack.h
	g++ -std=c++11  -c member.cpp


clean:
	rm -rf $(COMMON_OBJ) dchat.o dchat 