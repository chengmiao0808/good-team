CXX = g++
CXXFLAGS = -Wall -g -std=c++11 -lpthread

dchat: dchat.o utility.o handler.o
	$(CXX) $(CXXFLAGS) -o dchat dchat.o utility.o handler.o

dchat.o: dchat.cpp dchat.h utility.h handler.h
	$(CXX) $(CXXFLAGS) -c dchat.cpp

utility.o: utility.h

handler.o: handler.h

clean:
	rm -rf *.o dchat