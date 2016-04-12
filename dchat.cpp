#include "dchat.h"
#include "utility.h"
#include <ifaddrs.h>
#include <time.h>

using namespace std;

void error(string err) {
  perror(err.c_str());
  exit(1);
}

int start_a_leader(dchat *p_chat, string l_addr) {
  cout<<"start_a_leader is called!"<<endl;
  cout<<l_addr<<endl;
  vector<string> vec = split(l_addr, ":");
  string ip_addr = vec.front();
  string portno = vec.back();
  cout<<ip_addr<<endl;
  cout<<portno<<endl;

  p_chat->sock = socket(AF_INET, SOCK_DGRAM, 0); 
  if (p_chat->sock < 0) {
    return p_chat->sock;
  }

  bzero((char *) &(p_chat->me), sizeof(p_chat->me)); 
  p_chat->me.sin_family = AF_INET;
  p_chat->me.sin_addr.s_addr = inet_addr(ip_addr.c_str());
  p_chat->me.sin_port = htons(stoi(portno));

  p_chat->sock2 = ::bind(p_chat->sock, (struct sockaddr *) &(p_chat->me), sizeof(p_chat->me));
  if (p_chat->sock2 < 0) {
    return p_chat->sock2;
  }
  return 0;
}

int start_a_regular_member(dchat *p_chat, string l_addr, string m_addr, string m_name) {
  vector<string> vec_me = split(m_addr, ":");
  string ip_addr_me = vec_me.front();
  string portno_me = vec_me.back();

  p_chat->sock = socket(AF_INET, SOCK_DGRAM, 0); 
  if (p_chat->sock < 0) {
    return p_chat->sock;
  }

  bzero((char *) &(p_chat->me), sizeof(p_chat->me)); 
  p_chat->me.sin_family = AF_INET;
  p_chat->me.sin_addr.s_addr = inet_addr(ip_addr_me.c_str());
  p_chat->me.sin_port = htons(stoi(portno_me));

  p_chat->sock2 = ::bind(p_chat->sock, (struct sockaddr *) &(p_chat->me), sizeof(p_chat->me));
  if (p_chat->sock2 < 0) {
    return p_chat->sock2;
  }

  vector<string> vec_other = split(l_addr, ":");
  string ip_addr_other = vec_other.front();
  string portno_other = vec_other.back();

  bzero((char *) &(p_chat->other), sizeof(p_chat->other)); 
  p_chat->other.sin_family = AF_INET;
  p_chat->other.sin_addr.s_addr = inet_addr(ip_addr_other.c_str());
  p_chat->other.sin_port = htons(stoi(portno_other));

  char buff[2048];
  bzero(buff, 2048);

  int currtime = getLocalTime();  //get current time with utility function
  msgpack msg_pack(ip_addr_me, stoi(portno_me), m_name, currtime, 1, "N/A");
  string msg_sent = serialize(msg_pack);
  strcpy(buff, msg_sent.c_str());

  p_chat->num = sendto(p_chat->sock, buff, strlen(buff), 0, (struct sockaddr *) &(p_chat->other), sizeof(p_chat->other));
  if (p_chat->num < 0) {
    return p_chat->num;
  }

  p_chat->len = sizeof(p_chat->other);
  bzero((char *) &(p_chat->other), p_chat->len);
  bzero(buff, 2048);
  p_chat->num = recvfrom(p_chat->sock, buff, 2048, 0, (struct sockaddr *) &(p_chat->other), (socklen_t *) &(p_chat->len));
  if (p_chat->num < 0) {
    return p_chat->num;
  }
  string msg_recv = buff;
  msg_pack = deserialize(msg_recv);
  string members = msg_pack.msg;
  vector<string> vec = split(members, "\t");
  p_chat->leader = vec.back();
  vec.pop_back();
  while (!vec.empty()) {
    string key = vec.back();
    vec.pop_back();
    string val = vec.back();
    vec.pop_back();
    p_chat->all_members_list[key] = val;
  }

  return 0;
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
      cout<<p_ifa->ifa_name<<endl;
      cout<<"host is: "<<host<<endl;
      if (strcmp(p_ifa->ifa_name, "en0") == 0) {
        string my_ip = string(host);
        freeifaddrs(p_ifaddrs);
        cout<<"my_ip is: "<<my_ip<<endl;
        return my_ip;
      }
  	}
  	p_ifa = p_ifa->ifa_next;
  }
}

void dchat::start_new_group(string l_name) {
  cout<<"dchat::start_new_group is called!\n";
	is_leader = true;
  while (1) {
    srand((unsigned)time(NULL));
    int portno = rand() % 2000 + 8000;
    string l_addr = get_ip_address()+":"+to_string(portno);
    int n = start_a_leader(this, l_addr);
    if (n == 0) {
      leader = l_addr;
      all_members_list[l_addr] = l_name;
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
      my_addr = m_addr;
      cout<<m_name<<" started a new chat, on "<<l_addr<<", listening on\n"<<m_addr<<"\n"
        <<"Succeeded, current users:\n";
      typedef map<string, string>::iterator it_type;
      for (it_type iter = all_members_list.begin(); iter != all_members_list.end(); iter++) {
        cout<<iter->second<<" "<<iter->first;
        if (iter->first == leader)
          cout<<" (Leader)";
        cout<<"\n";
      }
      break;
    }
  }
}

void *recv_msgs(void *pt) {

  pthread_exit(NULL);
}

void *send_msgs(void *pt) {
  
  pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
	if (argc != 2 && argc != 3)
	{
		error("Error! Please input: ./dchat USER or ./dchat USER ADDR:PORT\n");
	}
	if (argc == 2) {
		dchat *p_dchat = new dchat();
		p_dchat->start_new_group(string(argv[1]));
	}
	else {
		dchat *p_dchat = new dchat();
		p_dchat->join_a_group(string(argv[1]), string(argv[2]));
	}
  pthread_t threads[2];

  pthread_create(&threads[0], NULL, recv_msgs, NULL);
  pthread_create(&threads[1], NULL, send_msgs, NULL);
  
  pthread_join(threads[0], NULL);
  pthread_join(threads[1], NULL);
	return 0;
}
