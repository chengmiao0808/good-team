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



int start_a_leader(dchat *p_chat, string l_addr, string l_name) {
  cout<<"start_a_leader is called!"<<endl;
  cout<<l_addr<<endl;
  vector<string> vec = split(l_addr, ":");
  string ip_addr = vec.front();
  string portno = vec.back();
  

  p_chat->sock = socket(AF_INET, SOCK_DGRAM, 0); 
  if (p_chat->sock < 0) {
    error("Error with socket!\n");
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

  p_chat->sock = socket(AF_INET, SOCK_DGRAM, 0); 
  if (p_chat->sock < 0) {
    error("Error with socket!\n");
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
  p_chat->is_leader = false;

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
    error("Error with sendto!\n");
  }

  p_chat->len = sizeof(p_chat->other);
  bzero((char *) &(p_chat->other), p_chat->len);
  bzero(buff, 2048);
  p_chat->num = recvfrom(p_chat->sock, buff, 2048, 0, (struct sockaddr *) &(p_chat->other), (socklen_t *) &(p_chat->len));
  if (p_chat->num < 0) {
    error("Error with recvfrom!\n");
  }
cout<<"Received the msg from the leader: \t"<<buff<<endl;   
  string msg_recv = buff;
  msg_pack = deserialize(msg_recv);
  string members = msg_pack.msg;
cout<<"memebers: "<<members<<endl; // Problem here! should not be empty
  vector<string> vec = split(members, "\t");
  p_chat->leader = vec.back();
  vec.pop_back();
cout<<"HERE"<<endl;
  while (!vec.empty()) {
    string key = vec.back();
    vec.pop_back();
    string val = vec.back();
    vec.pop_back();
    p_chat->all_members_list[key] = val;
  }
cout<<"HERE"<<endl;

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
  my_name = l_name;

  while (1) {
    srand((unsigned)time(NULL));
    int portno = rand() % 2000 + 8000;
    string l_addr = get_ip_address()+":"+to_string(portno);
    int n = start_a_leader(this, l_addr, l_name);
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


void broadcast(dchat *p_chat, string msg) {

  for (auto iter = p_chat->all_members_list.begin(); iter != p_chat->all_members_list.end(); iter++) {
  cout<<"\t this iter: \t"<< iter->first << endl;

    vector<string> vec_other = split(iter->first, ":");
    string ip_addr = vec_other.front();
    string portno = vec_other.back();

    bzero((char *) &(p_chat->other), sizeof(p_chat->other)); 
    p_chat->other.sin_family = AF_INET;
    p_chat->other.sin_addr.s_addr = inet_addr(ip_addr.c_str());
    p_chat->other.sin_port = htons(stoi(portno));

    /*  Combining the sending string  & Get args for sendto() function*/
    char buff[2048];
    bzero(buff, 2048);
  cout<<"\t p_chat->leader \t"<< p_chat->leader<<endl;
    vector<string> myvec = split(p_chat->leader, ":");
    string ip_addr_me = myvec.front();
    string portno_me = myvec.back();
    int currtime = getLocalTime();  
    msgpack msg_pack(ip_addr_me, stoi(portno_me), p_chat->my_name, currtime, 0, msg);
    string msg_sent = serialize(msg_pack);
    strcpy(buff, msg_sent.c_str());

    p_chat->num = sendto(p_chat->sock, buff, strlen(buff), 0, (struct sockaddr *) &(p_chat->other), sizeof(p_chat->other));
    if (p_chat->num < 0) {
      perror("sendto returns value < 0 \n");
    }
    cout<<"broadcasted the message!\n"<<endl;
  }
}

void *recv_msgs(void *threadarg) {
  dchat *p_chat = (dchat *) threadarg;

  //mtx.lock();
  if(p_chat->is_leader == true){
    bzero((char *) &(p_chat->other), p_chat->len);
    char buff[2048];
    bzero(buff, 2048);
    p_chat->num = recvfrom(p_chat->sock, buff, 2048, 0, (struct sockaddr *) &(p_chat->other), (socklen_t *) &(p_chat->len));
    cout<<"Received the msg: \t"<<buff<<endl;    
    if (p_chat->num < 0){
      perror("recvfrom returns value < 0 \n");
      pthread_exit(NULL);
    }
    /*  Unpacked the msg  */
    msgpack msg_pack = deserialize(buff);
    if(msg_pack.command == 1){

      // JOIN THE GROUP
      p_chat->all_members_list[msg_pack.IP+":"+ to_string(msg_pack.port)] = msg_pack.username;
      cout<<"A NEW MEMBER"<<p_chat->all_members_list[msg_pack.IP+":"+ to_string(msg_pack.port)]<< endl;
      char nbuff[2048];
      bzero(nbuff, 2048);
    cout<<"\t p_chat->leader \t"<< p_chat->leader<<endl;
      vector<string> myvec = split(p_chat->leader, ":");
      string ip_addr_me = myvec.front();
      string portno_me = myvec.back();
      int currtime = getLocalTime();  


      string ip_addr = msg_pack.IP;
      string portno = to_string(msg_pack.port);
      bzero((char *) &(p_chat->other), sizeof(p_chat->other)); 
      p_chat->other.sin_family = AF_INET;
      p_chat->other.sin_addr.s_addr = inet_addr(ip_addr.c_str());
      p_chat->other.sin_port = htons(stoi(portno));
//SHOULD SEND BACK MEMBER LIST
      msgpack msg_pack(ip_addr_me, stoi(portno_me), p_chat->my_name, currtime, 0, "sddssksdjls \tlin");
      string msg_sent = serialize(msg_pack);
      strcpy(nbuff, msg_sent.c_str());
      cout<<nbuff<<endl;

      p_chat->num = sendto(p_chat->sock, nbuff, strlen(nbuff), 0, (struct sockaddr *) &(p_chat->other), sizeof(p_chat->other));
      if (p_chat->num < 0) {
        perror("sendto returns value < 0 \n");
      }


    }else{
      string buff_str = buff;
      broadcast(p_chat, buff_str);    
      cout<<"Successfully broadcast received msg: \t"<<buff<<endl;    
    }

  }else{ // regular memeber
    bzero((char *) &(p_chat->other), p_chat->len);
    char buff[2048];
    bzero(buff, 2048);
    p_chat->num = recvfrom(p_chat->sock, buff, 2048, 0, (struct sockaddr *) &(p_chat->other), (socklen_t *) &(p_chat->len));
    if (p_chat->num < 0) {
      perror("recvfrom returns value < 0 \n");
      pthread_exit(NULL);
    }
    /*  Unpacked the msg  */
    msgpack msg_pack = deserialize(buff);
    
    if(msg_pack.command == 1){ 
    /* directly forward the join request to the leader */
      vector<string> vec_other = split(p_chat->leader, ":");
      string ip_addr_other = vec_other.front();
      string portno_other = vec_other.back();
      p_chat->is_leader = false;

      bzero((char *) &(p_chat->other), sizeof(p_chat->other)); 
      p_chat->other.sin_family = AF_INET;
      p_chat->other.sin_addr.s_addr = inet_addr(ip_addr_other.c_str());
      p_chat->other.sin_port = htons(stoi(portno_other));

      p_chat->num = sendto(p_chat->sock, buff, strlen(buff), 0, (struct sockaddr *) &(p_chat->other), sizeof(p_chat->other));
      if (p_chat->num < 0) {
        perror("sendto returns value < 0 \n");
        pthread_exit(NULL);
      }

    }else{
      cout<<"Message from server:\t"<<buff<<endl; 
    }    

  }
  //mtx.unlock();


  pthread_exit(NULL);
}


void *send_msgs(void *threadarg) {
  dchat *p_chat = (dchat *) threadarg;

  //mtx.lock();
  if(p_chat->is_leader == true){

    cout<<"What's on your mind?(You are the leader)"<<endl;
    string line;
    getline(cin, line);
    broadcast( p_chat, line);
    cout<<"Successfully broadcast leader's msg\n"<<endl;    

  }else{// non-leader member

    cout<<"What's on your mind?(You are not the leader)"<<endl;
    string line;
    getline(cin, line);

    /*  Combining the sending string  & Get args for sendto() function*/
    char buff[2048];
    bzero(buff, 2048);
    vector<string> myvec = split(p_chat->my_addr, ":");
    string ip_addr_me = myvec.front();
    string portno_me = myvec.back();
    int currtime = getLocalTime(); 
    msgpack msg_pack(ip_addr_me, stoi(portno_me), p_chat->my_name, currtime, 0, line);
    string msg_sent = serialize(msg_pack);
    strcpy(buff, msg_sent.c_str());

    p_chat->num = sendto(p_chat->sock, buff, strlen(buff), 0, (struct sockaddr *) &(p_chat->other), sizeof(p_chat->other));
    if (p_chat->num < 0) {
      perror("sendto returns value < 0 \n");
      pthread_exit(NULL);
    }   
  }
  //mtx.unlock();

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
  pthread_t threads[2];

  pthread_create(&threads[0], NULL, recv_msgs, (void *)p_dchat);
  pthread_create(&threads[1], NULL, send_msgs, (void *)p_dchat);
  
  pthread_join(threads[0], NULL);
  pthread_join(threads[1], NULL);
	return 0;
}
