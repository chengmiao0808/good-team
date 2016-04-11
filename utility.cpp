#include <stdio.h>      
#include <time.h>      /* time_t, struct tm, difftime, time, mktime */
using namespace std;


/*
 * Get local current time
 */
int getLocalTime(){
	time_t timer;
	time(&timer);  // get current time
	return (int)timer;
}
