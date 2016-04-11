#include "member.h"

using namespace std;

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
	
}