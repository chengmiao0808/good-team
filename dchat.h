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
	// Store all the members' info in the current group
	map<string, string> all_members_list; // <"ip_address:port", "name">
	// Store the leader's "ip_address:port" info
	string leader;

public:
	dchat() {
		int heartbeat_time = 5;
		all_members_list = map<string, string>();
	}
	void start_new_group(string addr, string name);
	void join_a_group(string add1, string add2, string name);
	void elect_a_leader();
};