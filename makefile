CXX = g++
CXXFLAGS = -Wall -g -std=c++11 -lpthread

dchat: dchat.o utility.o leader.o
	$(CXX) $(CXXFLAGS) -o dchat dchat.o utility.o leader.o

dchat.o: dchat.cpp dchat.h utility.h leader.h
	$(CXX) $(CXXFLAGS) -c dchat.cpp

utility.o: utility.h

leader.o: leader.h

clean:
	rm -rf *.o dchat