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
#include <vector>
#include <deque>
#include <iterator>
#include <pthread.h>

#include "utility.h"


using namespace std;


class dchat
{
public:
	// Store all the members' info in the current group
	map<string, string> all_members_list; // <"ip_address:port", "name">
	// Store the leader's "ip_address:port" info
	string leader_addr;
	// Store my own "ip_address:port" info
	string my_addr;
	string my_name;
	bool is_leader;
	bool has_joined;

	int current_stamp; // for event total ordering
	map<int, string> msgs; // Store previous messages

	// for the leader side
	map<string, int> current_member_stamp;
	map<string, deque<string>> member_event_queue;
	map<string, int> member_last_alive; //<"ip_address:port", "last_alive_time">

	// for the client side
	int leader_stamp;
	int leader_last_alive;
	deque<string> leader_event_queue;
	
	// for election
	bool is_election;

	int sock, sock2, num, len;
    struct sockaddr_in me;
    struct sockaddr_in other;


	dchat() {
		current_stamp = 0;
		leader_stamp = 0;
		is_election = false;
		all_members_list = map<string, string>();
		current_member_stamp = map<string, int>();
		member_last_alive = map<string, int>();
		msgs = map<int, string>();
		member_event_queue = map<string, deque<string>>();
		deque<string> new_deque(9999999);
		leader_event_queue = new_deque;
	}
	string get_ip_address();
	void start_new_group(string l_name);
	void join_a_group(string m_name, string l_addr);
};


#endif
