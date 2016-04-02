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
#include <vector>
#include <deque>

using namespace std;

class client {
public:
	string client_name;
	string client_ip;
	int client_port;

	string leader_name;
	string leader_ip;
	int leader_port;
	bool is_leader;

	map<string, string> client_addrlst; //<client_name, client addr+port> for leader

	/*	Methods	*/
	void client(string cl_name, string cl_ip, int cl_port,
		string l_name, string l_ip, int l_port){

		this->client_name = cl_name;
		this->client_ip = cl_ip;
		this->clien_port = cl_port;
		this->leader_name = l_name;
		this->leader_ip = l_ip;
		this->leader_port = l_port
		this->is_leader = 0;
	}
	void join(string, int, string, string, int);
	void send_message(string){
		if(this->is_leader == FALSE){
			//code for sending msg to leader...	
		}else{
			//code for broadcasting to all clients...
			for(map<string,string>::iterator it=0, it<client_addrlst.size(),it++){
				
				
			}
		}
	}
	void turn_leader(map<string, string> cl_addrlst){
		this->client_addrlst = cl_addrlst;
		this->is_leader = 1;
	}

}