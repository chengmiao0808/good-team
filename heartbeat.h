#ifdef HEARTBEAT_H
#define HEARTBEAT_H
#include <string>
#include <iostream>
#include <vector>
#include <stdlib.h>
#include <stdio.h>

using namespae std;
/* The heartbeat send and receive from leader and client */
struct heartbeat {
	string IP;
	int port;
	int timestamp;

	heartbeat(string IPaddress, int portnum, int t_time) {
		IP = IPaddress;
		port = portnum;
		timestamp = t_time;
	}

	static string heartbeat(heartbeat h) {
		return h.IP + "##" + h.port + "##" + h.timestamp; 
	}

	static heartbeat deserialize(string str) {
        vector<string> arr = split(str, "##");
        string IP = arr[0];
		int port = atoi(arr[1].c_str())；
		int t_time = atoi(arr[2].c_str());
        return msgPack(IP, port, t_time);
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

#endif /* HEARTBEAT_H */