#include "handler.h"
#include <ifaddrs.h>
#include <time.h>

using namespace std;


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
      // cout<<p_ifa->ifa_name<<endl;
      // cout<<"host is: "<<host<<endl;
      if (strcmp(p_ifa->ifa_name, "em1") == 0) {  //en0 for Mac
        string my_ip = string(host);
        freeifaddrs(p_ifaddrs);
        // cout<<"my_ip is: "<<my_ip<<endl;
        return my_ip;
      }
    }
    p_ifa = p_ifa->ifa_next;
  }
}

int bind_socket(dchat *p_chat, string my_addr) {
  vector<string> vec = split_helper(my_addr, ":");
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

void dchat::start_new_group(string l_name) {
  is_leader = true;
  my_name = l_name;

  while (1) {
    srand((unsigned)time(NULL));
    int portno = rand() % 2000 + 8000;
    string l_addr = get_ip_address()+":"+to_string(portno);
    int n = bind_socket(this, l_addr);
    if (n == 0) {
      leader_addr = l_addr;
      my_addr = l_addr;
      all_members_list[l_addr] = l_name;
      cout<<l_name<<" started a new chat, listening on "<<l_addr<<"\n"
        <<"Succeeded, current users:\n"<<l_name<<" "<<l_addr<<" (Leader)\n"
        <<"Waiting for others to join...\n";
      break;
    }
  }
}

int start_a_regular_member(dchat *p_chat, string l_addr, string m_addr, string m_name) {
  p_chat->has_joined = false;
  vector<string> vec_me = split_helper(m_addr, ":");
  string ip_addr_me = vec_me.front();
  string portno_me = vec_me.back();

  int n = bind_socket(p_chat, m_addr);
  if (n < 0)
    return n;
  p_chat->current_stamp = 0;
  string msg = "join_request#$"+ m_name + "#$" + m_addr;
  send_handler(msg, l_addr, p_chat);
  p_chat->leader_last_alive = getLocalTime();
  
  char buff[2048];
  bzero(buff, 2048);
  struct timeval tv;
  tv.tv_sec = 45;
  if (setsockopt(p_chat->sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0) {
    error("Error with setsockopt!\n");
  }
  p_chat->num = recvfrom(p_chat->sock, buff, 2048, 0, (struct sockaddr *) &(p_chat->other), (socklen_t *) &(p_chat->len));
  if (p_chat->num < 0) {
    error("Recvfrom timeout!\n");
  }

  tv.tv_sec = 88888888;
  if (setsockopt(p_chat->sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0) {
    error("Error with setsockopt!\n");
  }
  vector<string> message = split(buff);
  handle_join_inform(p_chat, message);

  p_chat->leader_last_alive = getLocalTime();
  return 0;
}

void dchat::join_a_group(string m_name, string l_addr) {
	is_leader = false;
  my_name = m_name;
  while (1) {
    srand((unsigned)time(NULL));
    int portno = rand() % 2000 + 8000;
    string m_addr = get_ip_address()+":"+to_string(portno);
    cout<<m_name<<" joining a new chat on "<<l_addr<<", listening on\n"<<m_addr<<"\n";
    my_addr = m_addr;
    int n = start_a_regular_member(this, l_addr, m_addr, m_name);
    if (n == 0) {
      if (has_joined) {
        cout<<"Succeeded, current users:\n";
        typedef map<string, string>::iterator it_type;
        for (it_type iter = all_members_list.begin(); iter != all_members_list.end(); iter++) {
          cout<<iter->second<<" "<<iter->first;
          if (iter->first == leader_addr)
            cout<<" (Leader)";
          cout<<"\n";
        }
        break;
      } 
    }
  }
}

void check_queue(dchat *p_chat, deque<string> my_que) {
  int i;
  for (i = 0; my_que.at(i).empty()!=1; ) {
    vector<string> message = split(my_que.at(i));
    if (message[0] == "normal") {
      handle_normal_message(p_chat, message);
    }
    else if (message[0] == "join_inform") {
      handle_join_inform(p_chat, message);
    }
    else {
      handle_client_leave(p_chat, message);
    }
    my_que.pop_front();
    my_que.emplace_back();
  }
}

void leader_receive_handler(dchat* p_chat, string msg) {
  vector<string> message = split(msg);

  if (message[0] == "client_heartbeat") {
    p_chat->member_last_alive[message[1]] = getLocalTime();
  } 
  else if(message[0] == "join_request") {
    cout<<"Recvfrom get:\t"<<msg<<endl;
    handle_join_request(p_chat, message);
  }
  else if (message[0] == "client_request") {
    cout<<"Recvfrom get:\t"<<msg<<endl;
    handle_client_request(p_chat, message);
  }
  else {
    cout<<"Recvfrom get:\t"<<msg<<endl;
    if (p_chat->current_member_stamp[message[2]] == stoi(message[1])) {
      // cout<<"match the member stamp"<<endl;
      p_chat->member_event_queue[message[2]].at(0) = msg;
      check_queue(p_chat, p_chat->member_event_queue[message[2]]);
    }
    else {
      if (p_chat->current_member_stamp[message[2]] < stoi(message[1])) {
        int i = stoi(message[1]) - p_chat->current_member_stamp[message[2]];
        cout<<"doesn't match the member stamp, i = "<<i<<endl;
        p_chat->member_event_queue[message[2]].at(i) = msg;
        string req = "leader_request#$" + p_chat->my_addr + "#$" + p_chat->my_name + "#$" + to_string(p_chat->current_member_stamp[message[2]]);
        send_handler(req, message[2], p_chat);
      }
    }
  }
}

void client_receive_handler(dchat* p_chat, string msg) {
  vector<string> message = split(msg);

  if (message[0] == "leader_heartbeat") {
    p_chat->leader_last_alive = getLocalTime();
  }
  else if (message[0] == "join_request") {
    cout<<"Recvfrom get:\t"<<msg<<endl;
    handle_join_request(p_chat, message);
  }
  else if (message[0] == "leader_request") {
    cout<<"Recvfrom get:\t"<<msg<<endl;
    handle_leader_request(p_chat, message);
  }
  else if (message[0] == "election") {
    cout<<"Recvfrom get:\t"<<msg<<endl;
    string refuse = "refuse#$";
    send_handler(refuse, message[1], p_chat);
  }
  else if (message[0] == "refuse") {
    cout<<"Recvfrom get:\t"<<msg<<endl;
    struct timeval tv;
    tv.tv_sec = 88888888;
    if (setsockopt(p_chat->sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0) {
      error("Error with setsockopt!\n");
    }
  }
  else if (message[0] == "new_leader") {
    cout<<"Recvfrom get:\t"<<msg<<endl;
    handle_new_leader(p_chat, message);
  }
  else {
    cout<<"Recvfrom get:\t"<<msg<<endl;
    if (p_chat->leader_stamp == stoi(message[1])) {
      // cout<<"match the leader stamp"<<endl;
      p_chat->leader_event_queue.at(0) = msg;
      check_queue(p_chat, p_chat->leader_event_queue);
    }
    else {
      if (p_chat->leader_stamp < stoi(message[1])) {
        int i = stoi(message[1]) - p_chat->leader_stamp;
        cout<<"doesn't match the leader stamp, i = "<<i<<endl;
        p_chat->leader_event_queue.at(i) = msg;
        string req = "client_request#$" + p_chat->my_addr + "#$" + p_chat->my_name + "#$" + to_string(p_chat->leader_stamp);
        send_handler(req, p_chat->leader_addr, p_chat);
      }
    }
  }
}

void *recv_msgs(void *threadarg) {
  dchat *p_chat = (dchat *) threadarg;

  for (;;) {
    char buff[2048];
    bzero(buff, 2048);
    bool flag = true;
    p_chat->num = recvfrom(p_chat->sock, buff, 2048, 0, (struct sockaddr *) &(p_chat->other), (socklen_t *) &(p_chat->len));
    if (p_chat->num < 0) {
      flag = false;
      struct timeval tv;
      tv.tv_sec = 88888888;
      if (setsockopt(p_chat->sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0) {
        error("Error with setsockopt!\n");
      }
      p_chat->is_leader = true;
      p_chat->is_election = false;
      p_chat->leader_addr = p_chat->my_addr;
      string new_leader_msg = "new_leader#$" + p_chat->my_addr + "#$" + p_chat->my_name;
      broadcast(p_chat, new_leader_msg);
      cout<<"NOTICE new leader "<<p_chat->my_name<<" is listening on "<<p_chat->my_addr<<endl;
    }
    
    if (p_chat->is_leader) {
      if (flag) {
        leader_receive_handler(p_chat, (string) buff);
      }
    } else {
      client_receive_handler(p_chat, (string) buff);
    }
  }
  pthread_exit(NULL);
}


/*  normal_message: when client sends a normal message. 
    command#$time_stamp#$user_ip:user_port#$user_name#$message (command is normal)
*/
void *send_msgs(void *threadarg) {
  dchat *p_chat = (dchat *) threadarg;

  for(;;) {
    if (p_chat->is_leader) {
      string line;
      getline(cin, line);
      line = p_chat->my_name + ":\t" + line;
      string msg = "normal#$" 
                  + to_string(p_chat->current_stamp)+ "#$" 
                  + p_chat->my_addr+ "#$" 
                  + p_chat->my_name + "#$" 
                  + line;

      p_chat->msgs[p_chat->current_stamp] = msg;
      p_chat->current_stamp++;
      p_chat->leader_stamp = p_chat->current_stamp;

      broadcast(p_chat, msg);
      cout<<line<<endl;

    } else { // non-leader member
      string line;
      getline(cin, line);
      line = p_chat->my_name + ":\t" + line;

      string msg = "normal#$" 
                  + to_string(p_chat->current_stamp)+ "#$" 
                  + p_chat->my_addr+ "#$" 
                  + p_chat->my_name + "#$" 
                  + line;
      
      if (!p_chat->is_election && !p_chat->is_leader) {
        p_chat->msgs[p_chat->current_stamp] = msg;
        p_chat->current_stamp++;
        send_handler(msg, p_chat->leader_addr, p_chat);
      }
      if (p_chat->is_leader) {
        p_chat->msgs[p_chat->current_stamp] = msg;
        p_chat->current_stamp++;
        p_chat->leader_stamp = p_chat->current_stamp;
        
        broadcast(p_chat, msg);
        cout<<line<<endl;
      }
    }
  }
  pthread_exit(NULL);
}

void *send_heart_beat(void *threadarg) {
  dchat *p_chat = (dchat *) threadarg;
  
  while (true) {
    usleep(3000000);
    if (!p_chat->is_election) {
      if (p_chat->is_leader) {
        string msg = "leader_heartbeat#$" + p_chat->my_addr;
        broadcast(p_chat, msg);
      }
      else {
        string msg = "client_heartbeat#$" + p_chat->my_addr;
        send_handler(msg, p_chat->leader_addr, p_chat);
      }
    }
  }
  pthread_exit(NULL);
}

void *check_alive(void* threadarg) {
  dchat *p_chat = (dchat *) threadarg;

  while (true) {
    if (!p_chat->is_election) {
      if (p_chat->is_leader) {
        for (auto iter = p_chat->member_last_alive.begin(); iter != p_chat->member_last_alive.end(); ) {
          if (getLocalTime() - (iter->second) > 30) {
            string name = p_chat->all_members_list[iter->first];
            string addr = iter->first;
            p_chat->all_members_list.erase(iter->first);
            p_chat->member_event_queue.erase(iter->first);
            p_chat->member_last_alive.erase(iter++);

            string msg = "NOTICE " + name + " left the chat or crashed";
            cout<<msg<<endl;
            msg = "client_leave#$" + to_string(p_chat->current_stamp) + "#$" + addr + "#$" + msg;
            p_chat->msgs[p_chat->current_stamp] = msg;
            p_chat->current_stamp++;
            p_chat->leader_stamp = p_chat->current_stamp;
            broadcast(p_chat, msg);
          } else {
            ++iter;
          }
        }
      }
      else {
        if (getLocalTime() - p_chat->leader_last_alive > 30) {
          if (p_chat->has_joined) {
            string name = p_chat->all_members_list[p_chat->leader_addr];
            cout<<"NOTICE "<<name<<" (leader) left the chat or crashed"<<endl;
            start_election(p_chat);
          }
          else {
            string msg = "join_request#$" + p_chat->my_name + "#$" + p_chat->my_addr;
            send_handler(msg, p_chat->leader_addr, p_chat);
            p_chat->leader_last_alive = getLocalTime();
          }
        }
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
  pthread_t threads[4];

  pthread_create(&threads[0], NULL, recv_msgs, (void *)p_dchat);
  pthread_create(&threads[1], NULL, send_msgs, (void *)p_dchat);

  pthread_create(&threads[2], NULL, send_heart_beat, (void *)p_dchat);
  pthread_create(&threads[3], NULL, check_alive, (void *)p_dchat);
  
  pthread_join(threads[0], NULL);
  pthread_join(threads[1], NULL);
  pthread_join(threads[2], NULL);
  pthread_join(threads[3], NULL);
	return 0;
}
