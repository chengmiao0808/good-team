CC = g++

all: dchat

member.o: member.cpp member.h utility.h
	gcc -std=c++11  -c member.cpp utility.cpp

dchat: member.o dchat.o utility.o 
	gcc -lpthread -std=c++11 -o dchat dchat.o utility.o member.o 


utility.o: utility.cpp msgpack.h utility.h 
	gcc -std=c++11  -c utility.cpp

dchat.o: dchat.cpp dchat.h heartbeat.h msgpack.h 
	gcc -lpthread -std=c++11  -c dchat.cpp


clean:
	rm -rf *.o dchat