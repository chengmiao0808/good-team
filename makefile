CC = g++

all: dchat

member.o: member.cpp member.h msgpack.h utility.o
	gcc -std=c++11  -c member.cpp

dchat: member.o dchat.o utility.o 
	gcc -std=c++11 -o dchat dchat.o utility.o member.o -lpthread


utility.o: utility.cpp utility.h msgpack.h
	gcc -std=c++11  -c utility.cpp

dchat.o: dchat.cpp dchat.h heartbeat.h msgpack.h 
	gcc -std=c++11  -c dchat.cpp


clean:
	rm -rf *.o dchat