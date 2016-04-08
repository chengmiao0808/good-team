#include "dchat.h"
#include "member.h"
#include <ifaddrs.h>
#include <time.h>

using namespace std;

void error(char *err) {
  perror(err);
  exit(1);
}

string dchat::get_ip_address() {
	struct ifaddrs *p_ifaddrs, *p_ifa;
  	int p_family, n;
  	char host[NI_MAXHOST];

  	if (getifaddrs(&p_ifaddrs) == -1) {
    	error("Error with getifaddrs!\n");
  	}

  	p_ifa = p_ifaddrs;
  	while(1) {
  		if (p_ifa == NULL)
  			break;
  		if (p_ifa->ifa_addr == NULL)
  			continue;
  		p_family = p_ifa->ifa_addr->sa_family;
  		if (p_family == AF_INET) {
  			n = getnameinfo(p_ifa->ifa_addr,
      				sizeof(struct sockaddr_in),
                    host, NI_MAXHOST,
                    NULL, 0, NI_NUMERICHOST);
      		if (n != 0) {
      			error("Error with getnameinfo!\n");
      		}
      		if (strcmp(p_ifa->ifa_name,"eth0") == 0) {
        		freeifaddrs(p_ifaddrs);
        		return string(host);
      		}
  		}
  		p_ifa = p_ifa->ifa_next;
  	}
}

void dchat::start_new_group(string l_name) {
	is_leader = true;
}

void dchat::join_a_group(tring m_name, string l_addr) {
	is_leader = false;
}

int main(int argc, char *argv[]) {
	if (argc != 2 && argc != 3)
	{
		error("Error! The number of arguments must be 1 or 2!\n");
	}
	if (argc == 2) {
		dchat *p_dchat = new dechat();
		p_dchat->start_new_group(argv[1]);
	}
	else {
		dchat *p_dchat = new dechat();
		p_dchat->join_a_group(argv[1], argv[2]);
	}

	return 0;
}
