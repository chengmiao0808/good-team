#ifndef UTILITY_H
#define UTILITY_H

#include <stdio.h>      
#include <time.h>      /* time_t, struct tm, difftime, time, mktime */
using namespace std;

int getLocalTime();
int splitString(string str, string delimeter);

#endif