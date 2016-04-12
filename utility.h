#ifndef UTILITY_H
#define UTILITY_H

#include <string>
#include <stdio.h>      
#include <time.h>      /* time_t, struct tm, difftime, time, mktime */
#include <vector>
#include <stdlib.h>
#include "msgpack.h"
using namespace std;

int getLocalTime();
string serialize(msgpack m);
msgpack deserilize(string str);
vector<string> split(string str, string sep);

#endif