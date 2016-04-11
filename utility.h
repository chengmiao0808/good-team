#ifndef UTILITY_H
#define UTILITY_H

#include <stdio.h>      
#include <time.h>      /* time_t, struct tm, difftime, time, mktime */
using namespace std;


class utility{


public :

	int getLocalTime();

};

/*
 * Get local current time
 */
int utility::getLocalTime(){
	time_t timer;
	time(&timer);  // get current time
	return (int)timer;
}

#endif