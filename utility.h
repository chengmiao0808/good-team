#ifndef UTILITY_H
#define UTILITY_H

#include <string>
#include <string.h>
#include <stdio.h>      
#include <time.h>      /* time_t, struct tm, difftime, time, mktime */
#include <vector>
#include <stdlib.h>

using namespace std;

/*	Comparator for the message class: MsgPack 	*/
class msgpack {
public:
	string IP;
	int port;
	string username;
	int t_time;
	int command;
	//command:
	//0: join
	//1: leave
	string msg;
	
	msgpack () {};

	msgpack(string IPaddress, int portNum, string name, int timestamp, int cmd, string message) {
		IP = IPaddress;
		port = portNum;
		username = name;
		t_time = timestamp;
		command = cmd;
		msg = message;
	} 
    
};

int getLocalTime();

string serialize(msgpack m);

msgpack deserialize(string str);

vector<string> split(string str, string sep);


#endif