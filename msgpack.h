#ifdef MSGPACK_H
#define MSGPACK_H

#include <string>
#include <iostream>
#include <vector>
#include <stdlib.h>
#include <stdio.h>

using namespace std;

/*	Comparator for the message struct: MsgPack 	*/
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

#endif /* MSGPACK_H */