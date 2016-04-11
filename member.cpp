#include "member.h"
#include <time.h>

using namespace std;

int start_a_leader(string l_addr) {
	int sock, sock2;
	struct sockaddr_in server;

	sock = socket(AF_INET, SOCK_DGRAM, 0); 
	if (sock < 0) {
		return sock;
	}

	bzero((char *) &server, sizeof(server)); 
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr();
    server.sin_port = htons();

    sock2 = bind(sock, (struct sockaddr *) &server, sizeof(server));
    if (sock2 < 0) {
    	return sock2;
    }
    return 0;
}

int start_a_regular_member(dchat *p_chat, string l_addr, string m_addr, string m_name) {
	
}