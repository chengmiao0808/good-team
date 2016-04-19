#ifndef UTILITY_H
#define UTILITY_H

#include <string>
#include <string.h>
#include <stdio.h>      
#include <time.h>      /* time_t, struct tm, difftime, time, mktime */
#include <vector>
#include <stdlib.h>

using namespace std;


int getLocalTime();

vector<string> split(string str);

vector<string> split_helper(string str, string sep);


#endif