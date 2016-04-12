#ifdef MSGPACK_H
#define MSGPACK_H

#include <string>
#include <iostream>
#include <vector>
#include <stdlib.h>
#include <stdio.h>

using namespae std;

/*	Comparator for the message struct: MsgPack 	*/
struct msgpack{
	string IP;
	int port;
	string username;
	int time;
	int command;
	//command:
	//0: join
	//1: leave


	string msg;

	msgpack() {};

	msgpack(string IPaddress, int portNum, string name, int timestamp, int cmd, string message) {
		IP = IPaddress;
		port = portNum;
		username = name;
		time = timestamp;
		command = cmd;
		msg = message;
	} 

	static string serialize(msgpack m) {
        return m.IP + "##" + to_string(m.port) + "##" + m.username + "##" + to_string(m.time)
        + "##" + to_string(m.command) + "##" + m.message;
    }

    static msgpack deserialize(string str) {
        vector<string> arr = split(str, "##");
        string IP = arr[0];
		int port = atoi(arr[1].c_str())；
		string username = arr[2];
		int time = atoi(arr[3].c_str());
		int command = atoi(arr[4].c_str());
		string msg = arr[5];
        return msgPack(IP, port, username, time, command, msg);
    }
    
    static vector<string> split(string str, string sep) {
        vector<string> arr;
        char* curr;
        char* cstr = const_cast<char*>(str.c_str());
        curr = strtok(cstr, sep.c_str());
        while (curr != NULL) {
            arr.push_back(string(curr));
            curr = strtok(NULL, sep.c_str());
        }
        return arr;
    }
};

#endif /* MSG_PACK_H */