CXX = g++
CXXFLAGS = -Wall -g

dchat: dchat.o utility.o
	$(CXX) $(CXXFLAGS) -o dchat dchat.o utility.o

dchat.o: dchat.cpp dchat.h utility.h
	$(CXX) $(CXXFLAGS) -c dchat.cpp

utility.o: utility.h

clean:
	rm -rf *.o dchat