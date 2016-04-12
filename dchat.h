#ifndef DCHAT_H
#define DCHAT_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <string>
#include <iostream>
#include <map>
#include <set>
#include <vector>
#include <deque>
#include <sysexits.h>
#include <signal.h>
#include <iterator>
#include <pthread.h>

class dchat
{
public:
	// Store all the members' info in the current group
	map<string, string> all_members_list; // <"ip_address:port", "name">
	// Store the leader's "ip_address:port" info
	string leader;
	// Store my own "ip_address:port" info
	string my_addr;
	string my_name;
	bool is_leader;
	int sock, sock2, num, len;
    struct sockaddr_in me;
    struct sockaddr_in other;

	dchat() {
		all_members_list = map<string, string>();
	}
	string get_ip_address();
	void start_new_group(string l_name);
	void join_a_group(string m_name, string l_addr);
	// void elect_a_leader();
};

#endif