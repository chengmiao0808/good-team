#include "utility.h"
#include <stdio.h>      
#include <time.h>      /* time_t, struct tm, difftime, time, mktime */
#include <stdio.h>
#include <stdlib.h>

using namespace std;

/*
 * Get local current time
 */
int getLocalTime(){
	time_t timer;
	time(&timer);  // get current time
	return (int)timer;
}

vector<string> split(string str) {
	return split_helper(str, "#$");
}

vector<string> split_helper(string str, string sep) {
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
