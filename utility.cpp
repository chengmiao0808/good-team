#include "utility.h"
#include <stdio.h>      
#include <time.h>      /* time_t, struct tm, difftime, time, mktime */
#include <stdio.h>
#include <stdlib.h>
#include "msgpack.h"

using namespace std;


/*
 * Get local current time
 */
int getLocalTime(){
	time_t timer;
	time(&timer);  // get current time
	return (int)timer;
}

/*
 * Serilize a string to msgpack
 */
string serialize(msgpack m) {
	return m.IP + "##" + to_string(m.port) + "##" + m.username + "##" + to_string(m.time)
	+ "##" + to_string(m.command) + "##" + m.message;
}

/*
 *  Deserilize a msgpack to string
 */
msgpack deserialize(string str) {
	vector<string> arr = split(str, "##");
	string IP = arr[0];
	int port = atoi(arr[1].c_str());
	string username = arr[2];
	int time = atoi(arr[3].c_str());
	int command = atoi(arr[4].c_str());
	string msg = arr[5];
    return msgpack(IP, port, username, time, command, msg);
}

vector<string> split(string str, string sep) {
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
