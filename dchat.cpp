#include "dchat.h"
#include "utility.h"
#include <ifaddrs.h>
#include <time.h>
#include <mutex>          // std::mutex

using namespace std;

mutex mtx;

void error(string err) {
  perror(err.c_str());
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

int bind_socket(dchat *p_chat, string my_addr) {
  vector<string> vec = split(my_addr, ":");
  string ip_addr = vec.front();
  string portno = vec.back();
  
  p_chat->sock = socket(AF_INET, SOCK_DGRAM, 0); 
  if (p_chat->sock < 0) {
    return p_chat->sock;
  }

  //initialize my information
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

  int n = bind_socket(p_chat, m_addr);
  if (n < 0)
    return n;

  string msg = "join_request" + "#$" + m_name + "#$" + m_addr;
  send_handler(msg, l_addr, p_chat);
  return 0;
}

void dchat::start_new_group(string l_name) {
  cout<<"dchat::start_new_group is called!\n";
	is_leader = true;
  my_name = l_name;

  while (1) {
    srand((unsigned)time(NULL));
    int portno = rand() % 2000 + 8000;
    string l_addr = get_ip_address()+":"+to_string(portno);
    int n = bind_socket(this, l_addr);
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
  my_name = m_name;
  while (1) {
    srand((unsigned)time(NULL));
    int portno = rand() % 2000 + 8000;
    string m_addr = get_ip_address()+":"+to_string(portno);
    cout<<m_name<<" joining a new chat on "<<l_addr<<", listening on\n"<<m_addr<<"\n";
    int n = start_a_regular_member(this, l_addr, m_addr, m_name);
    if (n == 0) {
      my_addr = m_addr;
      cout<<"Succeeded, current users:\n";
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

void send_handler(string msg, string other_addr, dchat *p_chat) {
  vector<string> vec_other = split(other_addr, ":");
  string ip_addr_other = vec_other.front();
  string portno_other = vec_other.back();

  bzero((char *) &(p_chat->other), sizeof(p_chat->other)); 
  p_chat->other.sin_family = AF_INET;
  p_chat->other.sin_addr.s_addr = inet_addr(ip_addr_other.c_str());
  p_chat->other.sin_port = htons(stoi(portno_other));

  char buff[2048];
  bzero(buff, 2048);
  strcpy(buff, msg.c_str());

  p_chat->num = sendto(p_chat->sock, buff, strlen(buff), 0, (struct sockaddr *) &(p_chat->other), sizeof(p_chat->other));
  if (p_chat->num < 0) {
      error("Error with sendto!\n");
  }
}

void broadcast(dchat *p_chat, string msg) {

  for (auto iter = p_chat->all_members_list.begin(); iter != p_chat->all_members_list.end(); iter++) {
  //cout<<"\t this iter: \t"<< iter->first << endl;
    //if(iter->first == p_chat->leader) continue; //dont send to leader herself
    send_handler(msg, iter->first, p_chat);
  }
}

void *recv_msgs(void *threadarg) {
  dchat *p_chat = (dchat *) threadarg;

  for (;;) {
    char buff[2048];
    bzero(buff, 2048);
    p_chat->num = recvfrom(p_chat->sock, buff, 2048, 0, (struct sockaddr *) &(p_chat->other), (socklen_t *) &(p_chat->len));
    if (p_chat-> num < 0) {
      error("Error with recvfrom!\n");
    }
    if (p_chat->is_leader) {
      leader_receive_handler(p_chat, (string) buff);
    } else {
      client_receive_handler(p_chat, (string) buff);
    }
  }

  pthread_exit(NULL);
}

void check_queue(dchat *p_chat, deque<string> my_que) {
  int i;
<<<<<<< HEAD
    for (i = 0; my_que.at(i).empty()!=1; i++) {
      vector<string> message = split(my_que.at(i));
      switch (message[0]) {
        case "normal" :
          handle_normal_request(p_chat, message);
          break;
        case "join_request" :
          handle_join_request(p_chat, message);
          break;
        case "join_inform" :
          handle_join_inform(p_chat, message);
          break;
        case "join_response" :
          handle_join_response(p_chat, message);
          break;
        case "client_leave" :
          handle_client_leave(p_chat, message);
          break;
        case "election" :
          handle_election(p_chat, message);
          break;
        case "new_leader" :
          handle_new_leader(p_chat, message);
          break;
        case "refuse" :
          handle_refuse(p_chat, message);
          break;
        case "client_request" :
          handle_client_request(p_chat, message);
          break;
        case "leader_request" :
          handle_leader_request(p_chat, message);
          break;
      }
=======
  for (i = 0; my_que.at(i).empty()!=1; i++) {
    vector<string> message = split(my_que.at(i));
    switch (message[0]) {
      case "normal" :

      case "client_request" :

      case "join_request" :

      case "leader_request" :

      case "join_request" :

      case "join_inform" :

      case "join_response" :

      case "client_leave" :

      case "election" :

      case "new_leader" :


>>>>>>> 6570dd8ec97be04d52e56b2354450aeeda2017c0
    }
  }
}

void leader_receive_handler(dchat* p_chat, string msg) {
  vector<string> message = split(msg);
  switch (message[0]) {
    case "client_heartbeat" :
      leader_handle_client_heartbeat(p_chat, message);
      break;
    default:
      if (p_chat->current_member_stamp[message[2]] == stoi(message[1])) {
        p_chat->member_event_queue[message[2]].at(i) = msg;
        check_queue(p_chat, p_chat->member_event_queue[message[2]]);
      }
      else {

      }
      break;
    }
}

void client_receive_handler(dchat* p_chat, string msg) {
  vector<string> message = split(msg);
  switch (message[0]) {
      case "leader_heartbeat" :
        client_handle_leader_heartbeat(p_chat, message);
        break;
      default:
        if (p_chat->leader_stamp == stoi(message[1])) {
          p_chat->leader_event_queue[message[2]].at(i) = msg;
          check_queue(p_chat, p_chat->leader_event_queue);
        }
        else {

        }
        break;
  }
}


void *send_msgs(void *threadarg) {
  dchat *p_chat = (dchat *) threadarg;

  for(;;) {
    //mtx.lock();
    if (p_chat->is_leader == true) {

      //cout<<"What's on your mind?(You are the leader)"<<endl;
      string line;
      getline(cin, line);
      line = p_chat->my_name + ":\t" + line;
      broadcast( p_chat, line);
      //cout<<"Successfully broadcast leader's msg\n"<<endl;    

    } else {// non-leader member

      //cout<<"What's on your mind?(You are not the leader)"<<endl;
      string line;
      getline(cin, line);
      line = p_chat->my_name + ":\t" + line;

      string msg = "normal" + "#$" + to_string(p_chat->current_stamp) + "#$" + line;
      p_chat->current_stamp++;
      send_handler(msg, p_chat->leader, p_chat);
    }
    //mtx.unlock();
  }
  pthread_exit(NULL);
}

void *send_heart_beat(void *threadarg) {
  dchat *p_chat = (dchat *) threadarg;
  //cout << "HERE" << endl;
  while (true) {
    usleep(1000000);

    string msg = "client_heartbeat" + "#$";
    send_handler(msg, p_chat->leader, p_chat);
  }
  pthread_exit(NULL);
}

void *check_alive(void* threadarg) {
  dchat *p_chat = (dchat *) threadarg;
  while (1) {

    for (auto iter = p_chat->last_alive.begin(); iter != p_chat->last_alive.end(); ) {
      if (getLocalTime() - (iter->second) > 2) {
        string name = p_chat->all_members_list[iter->first];
        p_chat->all_members_list.erase(iter->first);
        p_chat->last_alive.erase(iter++);

        string msg = "NOTICE " + name + " left the chat or crashed";
        msg = "client_leave" + "#$" + to_string(getLocalTime()) + "#$" + msg;
        broadcast(p_chat, msg);
      } else {
        ++iter;
      }
    }
  }
  pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
	if (argc != 2 && argc != 3)
	{
		error("Error! Please input: ./dchat USER or ./dchat USER ADDR:PORT\n");
	}
  dchat *p_dchat = new dchat();
	if (argc == 2) {
		p_dchat->start_new_group(string(argv[1]));
	}
	else {
		p_dchat->join_a_group(string(argv[1]), string(argv[2]));
	}
  pthread_t threads[3];

  pthread_create(&threads[0], NULL, recv_msgs, (void *)p_dchat);
  pthread_create(&threads[1], NULL, send_msgs, (void *)p_dchat);

  if (p_dchat->is_leader) {
    pthread_create(&threads[2], NULL, check_alive, (void *)p_dchat);
  } else {
    pthread_create(&threads[2], NULL, send_heart_beat, (void *)p_dchat);
  }
  
  pthread_join(threads[0], NULL);
  pthread_join(threads[1], NULL);
  pthread_join(threads[2], NULL);
	return 0;
}
