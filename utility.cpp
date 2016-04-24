#include "utility.h"
#include <stdio.h>      
#include <time.h>      /* time_t, struct tm, difftime, time, mktime */
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

const string ALPHABET = "aAbBcCdDeEfFgGhHiIjJkKlLmMnNoOpPqQrRsStTuUvVwWxXyYzZ0123456789 ";


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
	char temp[2048];
	strcpy(temp, str.c_str());
    vector<string> arr;
    char* curr;
    char* cstr = const_cast<char*>(temp);
    curr = strtok(cstr, sep.c_str());
    while (curr != NULL) {
        arr.push_back(string(curr));
        curr = strtok(NULL, sep.c_str());
    }
    return arr;
}

string encrypt(string input) {
	int key = 5; // assuming change by 5
	string result = "";
	for(int i = 0; i < input.length() ; i++) {
		for(int j = 0; j <= ALPHABET.length(); j++) {
			if(input[i] == ALPHABET[j]) {
				result += ALPHABET[(j + key) % ALPHABET.length()];
				break;
			}
		}
	}

	return result;
}

string decrypt(string input) {
	int key = 5; // assuming change by 5
	string result = "";
	for(int i = 0; i < input.length(); i++) {
		for(int j = 0; j <= ALPHABET.length(); j++) {
			if(input[i] == ALPHABET[j]) {
				j < key ? result += ALPHABET[((j + ALPHABET.length()) - key) % ALPHABET.length()] : result += ALPHABET[(j - key) % ALPHABET.length()];
				break;
			}
		}
	}

	return result;
}


string substitute_encrypt(string input) {
	unsigned int offset = 5;
	string result = "";
	for (int i = 0; i < input.length(); i++) {
		//	If it is non-alphabet then continue
		if (!isalpha(input[i]))
			continue;

		// calculate the start
		char start = islower(input[i]) ? 'a' : 'A';
		unsigned int alpha_offset  = input[i] - start;
		unsigned int new_alpha_offset  = alpha_offset + offset ;
		input[i] = start + new_alpha_offset % 26;
	}
	return input;
}

string substitute_decrypt(string input) {
	unsigned int offset = 5;
	string result = "";
	for (int i = 0; i < input.length(); i++) {
		//	If it is non-alphabet then continue
		if ( !isalpha(input[i]))
			continue;
		
		// calculate the start
		char start = islower(input[i]) ? 'a' : 'A';
		unsigned int alpha_offset = input[i] - start ;
		int new_alpha_offset = alpha_offset  - offset ;
		
		if (new_alpha_offset < 0) new_alpha_offset += 26;
		
		input[i] = start + (new_alpha_offset % 26);
	}
	return input;
}

