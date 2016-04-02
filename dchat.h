#include <stdio.h>
#include <stdlib.h>
#include <sysexits.h>
#include <signal.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string>
#include <iostream>
#include <map>
#include <set>

using namespace std;

class dchat
{
	int heartbeat_time;
	int group_ids[100];
	// Store all the members' info in the chat room
	map<string, string> all_members_list; // <"ip_address:port", "group_id&name">
	// Store all the members in certain groups
	map<int, set<string>> group_members; // <group_id, set("ip_address:port")>
	// Store the leader's info for each group
	map<int, string> group_leaders; // <group_id, leader's "ip_address:port">

public:
	dchat() {
		int heartbeat_time = 5;
		for (int i = 0; i < 100; i++)
			group_ids[i] = 0;
	}
	void start_new_group(string addr, string name);
	void join_a_group(string add1, string add2, string name);
	void elect_a_leader();
};