#ifdef MSGPACK_H
#define MSGPACK_H

#include <string>
#include <iostream>
#include <vector>
#include <stdlib.h>
#include <stdio.h>

using namespae std;

/*	Comparator for the message struct: MsgPack 	*/
struct msgpack {
	string IP;
	int port;
	string username;
	int time;
	int command;
	//command:
	//0: join
	//1: leave
	string msg;
	
	msgpack(string IPaddress, int portNum, string name, int timestamp, int cmd, string message) {
		IP = IPaddress;
		port = portNum;
		username = name;
		time = timestamp;
		command = cmd;
		msg = message;
	} 
    
};

#endif /* MSG_PACK_H */