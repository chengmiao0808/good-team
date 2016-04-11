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
      if (strcmp(p_ifa->ifa_name, "eth0") == 0) {
        freeifaddrs(p_ifaddrs);
        return string(host);
      }
  	}
  	p_ifa = p_ifa->ifa_next;
  }
}

void dchat::start_new_group(string l_name) {
	is_leader = true;
  while (1) {
    srand((unsigned)time(NULL));
    int portno = rand() % 2000 + 8000;
    string l_addr = get_ip_address()+":"+to_string(portno);
    int n = start_a_leader(l_addr);
    if (n == 0) {
      leader = l_addr;
      cout<<l_name<<" started a new chat, listening on "<<l_addr<<"\n"
        <<"Succeeded, current users:\n"<<l_name<<" "<<l_addr<<" (Leader)\n"
        <<"Waiting for others to join...\n";
      break;
    }
  }
}

void dchat::join_a_group(string m_name, string l_addr) {
	is_leader = false;
  while (1) {
    srand((unsigned)time(NULL));
    int portno = rand() % 2000 + 8000;
    string m_addr = get_ip_address()+":"+to_string(portno);
    int n = start_a_regular_member(this, l_addr, m_addr, m_name);
    if (n == 0) {
      client = m_addr;
      cout<<m_name<<" started a new chat, on "<<l_addr<<", listening on\n"<<m_addr<<"\n"
        <<"Succeeded, current users:\n";
      typedef map<string, string>>::iterator it_type;
      for (it_type iter = all_members_list.begin(); iter != all_members_list.end(); iter++) {
        cout<<iter->second<<" "<<iter->first;
        if (iter == all_members_list.begin())
          cout<<" (Leader)";
        cout<<"\n";
      }
      break;
    }
  }
}

int main(int argc, char *argv[]) {
	if (argc != 2 && argc != 3)
	{
		error("Error! Please input: ./dchat USER or ./dchat USER ADDR:PORT\n");
	}
	if (argc == 2) {
		dchat *p_dchat = new dechat();
		p_dchat->start_new_group(string(argv[1]);
	}
	else {
		dchat *p_dchat = new dechat();
		p_dchat->join_a_group(string(argv[1]), string(argv[2]));
	}

	return 0;
}
