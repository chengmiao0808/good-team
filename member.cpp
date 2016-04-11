#include "member.h"
#include "msgPack.h"
#include "utility.h"

using namespace std;

vector<string> split(string str, string sep) {
        vector<string> arr;
        char* curr;
        char* cstr = const_cast<char*>(str.c_str());
        curr = strtok(cstr, sep.c_str());
        while (curr != NULL) {
            arr.push_back(string(curr));
            curr = strtok(NULL, sep.c_str());
        }
        return arr;
    }

int start_a_leader(string l_addr) {
	int sock, sock2;
	struct sockaddr_in server;

	char *p_laddr = new char[l_addr.length()+1];
	strcpy(p_laddr, l_addr.c_str());

	char *ip_addr, *portno;
	ip_addr = strtok(p_laddr, ":");
	portno = strtok(NULL, ":");

	sock = socket(AF_INET, SOCK_DGRAM, 0); 
	if (sock < 0) {
		return sock;
	}

	bzero((char *) &server, sizeof(server)); 
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(ip_addr);
    server.sin_port = htons(atoi(portno));

    sock2 = bind(sock, (struct sockaddr *) &server, sizeof(server));
    if (sock2 < 0) {
    	return sock2;
    }
    return 0;
}

int start_a_regular_member(dchat *p_chat, string l_addr, string m_addr, string m_name) {
	int sock, sock2, num, len;
	struct sockaddr_in me;
	struct sockaddr_in other;

	char *p_maddr = new char[m_addr.length()+1];
	strcpy(p_maddr, m_addr.c_str());

	char *ip_addr_me, *portno_me;
	ip_addr_me = strtok(p_maddr, ":");
	portno_me = strtok(NULL, ":");

	sock = socket(AF_INET, SOCK_DGRAM, 0); 
	if (sock < 0) {
		return sock;
	}

	bzero((char *) &me, sizeof(me)); 
    me.sin_family = AF_INET;
    me.sin_addr.s_addr = inet_addr(ip_addr_me);
    me.sin_port = htons(atoi(portno_me));

    sock2 = bind(sock, (struct sockaddr *) &server, sizeof(server));
    if (sock2 < 0) {
    	return sock2;
    }

	char *p_laddr = new char[l_addr.length()+1];
	strcpy(p_laddr, l_addr.c_str());

	char *ip_addr_other, *portno_other;
	ip_addr_other = strtok(p_laddr, ":");
	portno_other = strtok(NULL, ":");

	bzero((char *) &other, sizeof(other)); 
    other.sin_family = AF_INET;
    other.sin_addr.s_addr = inet_addr(ip_addr_other);
    other.sin_port = htons(atoi(portno_other));

    char buff[2048];
    bzero(buff, 2048);

    int currtime = getLocalTime();//get current time with utility function
    struct msgPack msgpack(ip_addr_me, atoi(portno_me), m_name, currtime, 1, "N/A");
    string msg_sent = msgPack::serialize(msgpack);
    strcpy(buff, msg_sent.c_str());    

    num = sendto(sock, buff, strlen(buff), 0, (struct sockaddr *) &other, sizeof(other));
    if (num < 0) {
    	return num;
    }
    len = sizeof(other);
    bzero((char *) &other, len);
    bzero(buff, 2048);
   	num = recvfrom(sock, buff, 2048, 0, (struct sockaddr *) &other, &len);
   	if (num < 0) {
   		return num;
   	}
    string msg_recv = buff;
    struct msgPack msg_pack = msgPack::deserialize(msg_recv);
    string members = msg_pack.msg;
    vector<string> vec = split(members, "\t");
    while (!vec.empty()) {
        p_chat->all_members_list[vec.pop_back()] = vec.pop_back();
    }
    


    return 0;
}