#include "handler.h"
#include "utility.h"

using namespace std;

void error(string err) {
  perror(err.c_str());
  exit(1);
}

void send_handler(string msg, string other_addr, dchat *p_chat) {
  vector<string> vec_other = split_helper(other_addr, ":");
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
  //cout<<"will broadcast: \t<"<< msg<<"> to:"<<endl;
  for (auto iter = p_chat->all_members_list.begin(); iter != p_chat->all_members_list.end(); iter++) {
  //cout<<"\t this iter: \t"<< iter->first << endl;
    if (iter->first == p_chat->leader_addr) continue; //dont send to leader herself
    //cout<<iter->second<<" on "<<iter->first<<endl;
    send_handler(msg, iter->first, p_chat);
  }
}

void handle_normal_message(dchat* p_chat, vector<string> message) { 

  //command#$time_stamp#$user_ip:user_port#$user_name#$message
  vector<string> vec = message;
  string cmd = vec[0];
  // int msg_time = stoi(vec[1]); 
  string user_addr =vec[2];
  string user_name =vec[3];
  string msg = vec[4];

  if (p_chat->is_leader) {
    // update stamp of the member
    p_chat->current_member_stamp[user_addr]++;

    // broadcast messages
    string line = vec[0] + "#$" + to_string(p_chat->current_stamp)+ "#$" + vec[2]+ "#$" + vec[3] + "#$" + vec[4];
    p_chat->msgs[p_chat->current_stamp] = line;
    p_chat->current_stamp++;
    p_chat->leader_stamp = p_chat->current_stamp;
    broadcast(p_chat, line);
    cout<<msg<<endl;
  } else {
    p_chat->leader_stamp++;
    cout << msg << endl;
  }
 
}


/*  client_join_request: when client sends a join request to leader/other client. 
    command#$user_name#$my_ip:my_port (command is join_request)
*/
void handle_join_request(dchat* p_chat,  vector<string> message) {

  if (p_chat->is_leader) {
    // 1. add new member into the map
    string new_user_name = message[1];
    string new_user_addr = message[2];
    deque<string> new_deque(100);
    p_chat->all_members_list[new_user_addr] = new_user_name;    //add new_user into the list 
    p_chat->member_event_queue[new_user_addr] = new_deque;      //new_user's msg queue
    p_chat->current_member_stamp[new_user_addr] = 0;            //new_user's msg count
    p_chat->member_last_alive[new_user_addr] = getLocalTime();  //new_user's last alive time
    cout<<"NOTICE "<<new_user_name<<" joined on "<<new_user_addr<<endl;

    // 2. send member list to the new member
    /* Create the member list  <addr#$name> */ 
    string member_list = "";
    for (auto iter = p_chat->all_members_list.begin(); iter != p_chat->all_members_list.end(); iter++) {
      member_list += "#$"  + iter->first;
      member_list += "#$"  + iter->second;
    }

    string inform = "join_inform#$" 
        + to_string(p_chat->current_stamp)+ "#$" 
        + p_chat->leader_addr + "#$" + new_user_addr + member_list;

    p_chat->msgs[p_chat->current_stamp] = inform;

    // 3. broadcast the "NOTICE xxxx joined on xxxxx""
    broadcast(p_chat, inform);
    p_chat->current_stamp++;
    p_chat->leader_stamp = p_chat->current_stamp;
  }
  else {
    string forward_msg = message[0] + "#$" + message[1] + "#$" + message[2];
    send_handler(forward_msg, p_chat->leader_addr, p_chat); 
  }
}

/*
	command#$time_stamp#$leader_ip:leader_port#$new_user_addr#$all_member_addrs (command is join_inform)
*/
void handle_join_inform(dchat *p_chat, vector<string> message){
  string cmd = message[0];
  int leader_timestamp = stoi(message[1]) + 1;  // add 1 for leader_stamp
  string leader_addr = message[2];
  string new_user_addr = message[3];

  p_chat->leader_addr = leader_addr;
  p_chat->leader_stamp = leader_timestamp;
  for(int index = 4; index<message.size(); index+=2) {
    string key = message[index];
    string val = message[index+1];
    p_chat->all_members_list[key] = val;
  }
  if (p_chat->my_addr == new_user_addr) {
    p_chat->has_joined = true;
  }
    
  cout<<"NOTICE "<<p_chat->all_members_list[new_user_addr]<<" joined on "<<new_user_addr<<endl;

}  

/* 	client_leave: when leader detect a client leaves or crashes, 
	he would send a leave message to every client. 
	command#$time_stamp#$client_leave_ip:client_leave_port#$message (command is client_leave)
*/
void handle_client_leave(dchat *p_chat, vector<string> message){
  string cmd = message[0];
  int leader_timestamp = stoi(message[1]) + 1;  // add 1 for leader_stamp
  string left_user_addr = message[2];
  string msg = message[3];

  p_chat->leader_stamp = leader_timestamp;
  p_chat->all_members_list.erase(left_user_addr);
  cout<<msg<<endl;
}


/**
 * Bully Comparison based on alphetic order of "IP:PORT"
 */
bool bully_compare(string me, string candidate) {
  if (me.compare(candidate) > 0) { // reject
    return true;
  } else { 						// agree
    return false;
  }
}


/*
election: when client detect leader leaves or crashes, 
he would send a election message to all clients that have higher ip:port than himself. 
command#$time_stamp#$client_ip:client:port (command is election)
*/
void handle_election(dchat* p_chat, vector<string> message) {
  cout<<"election"<<message.size()<<endl;
  if(p_chat->is_election == true) return;

  p_chat->is_election = true;
  bool is_winner = true;
  p_chat->all_members_list.erase(p_chat->leader_addr);
  p_chat->leader_addr = "";

  if(!message.empty()){
    cout<<"Others notice there is an new election"<<endl;

    // command#$time_stamp#$client_ip:client:port (command is election)    
    vector<string> vec = message;
    string cmd = vec[0];
    int msg_time = stoi(vec[1]); 
    string informer_addr =vec[2];

    // I bully this informer, or I am the 1st one who start the election 
    if(bully_compare(p_chat->my_addr, informer_addr) ){

      // 1. send back "okay": command#$time_stamp#$client_ip:client_port. (command is refuse)
      string okay_msg = "refuse#$" + to_string(p_chat->current_stamp) + "#$" +p_chat->my_addr;
      send_handler(okay_msg, informer_addr, p_chat);

      // 2. send election to others with higher priority: 
      //    command#$time_stamp#$client_ip:client:port (command is election)
      string elect_msg = "election#$" + to_string(p_chat->current_stamp) + "#$" + p_chat->my_addr;
      for (auto iter = p_chat->all_members_list.begin(); iter != p_chat->all_members_list.end(); iter++) {
        if( !bully_compare(p_chat->my_addr, iter->first) ){
          send_handler( elect_msg, iter->first, p_chat);
        }
      }

    }else{ 
      /*  Others inform you as an election loser
          Impossible to have this condition 
      */
    }

  }else{ // Input vector.size() = 0   : FOUND LEADER DEAD & START A NEW ELECTION

      /* 1. send election to others with higher priority: 
          command#$time_stamp#$client_ip:client:port (command is election)
      */
      string elect_msg = "election#$" + to_string(p_chat->current_stamp) + "#$" + p_chat->my_addr;
      cout<<elect_msg<<endl;
      for (auto iter = p_chat->all_members_list.begin(); iter != p_chat->all_members_list.end(); iter++) {
        if( !bully_compare(p_chat->my_addr, iter->first) && p_chat->my_addr != iter->first){
          cout<<"send to:\t"<<iter->first<<iter->second<<endl;
          send_handler( elect_msg, iter->first, p_chat);
        }
      }    
  }

  	// 3. wait for others' messages
    int count = 3;
  	while( count > 0){
      count--;

      //Receive message from others
      struct timeval tv;
      tv.tv_sec = 1;
      tv.tv_usec = 0;
      if (setsockopt(p_chat->sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0) {
        error("Recvfrom timeout!\n");
      }

      cout<<"here0"<<endl;
      
      char temp[2048];
      bzero(temp, 2048);
      p_chat->num = recvfrom(p_chat->sock, temp, 2048, 0, (struct sockaddr *) &(p_chat->other), (socklen_t *) &(p_chat->len));
      cout<<"here0.1"<<endl;
      if (p_chat-> num < 0) {
        error("Error with recvfrom!\n");
      }

      cout<<"here1"<<endl;

      tv.tv_sec = 99999999;
      if (setsockopt(p_chat->sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0) {
        error("Recvfrom timeout!\n");
      }

      cout<<"here2"<<endl;

      cout<<"election recvfrom: \t"<< temp<<endl;
	    if(temp[0] != '/0'){
        vector<string> message = split(temp);
    		if (message[0].compare("refuse") == 0) {// refuse message
      	  is_winner = false;
    		}else{ //other unexpected message
        
        }
        count++;
      }

  	}

  	/* 4. If you win, broadcast it!
  	   new_leader_create: when a new leader is elected,  we would multicast the msg to every client. 
  	   command#$time_stamp#$leader_ip:leader_port#$leader_name. (command is new_leader) 
    */
    if(is_winner == true){
      cout<<"You win!"<<endl;
      p_chat->is_election = false;

      /************TO DO: Initiate the status as a leader ****************/
      p_chat->is_leader = true;
    	p_chat->leader_addr = p_chat->my_addr;
    	p_chat->leader_stamp = p_chat->current_stamp; // not sure about this?
      /************TO DO: Initiate the status as a leader ****************/

    	string winner_line = "new_leader#$" + to_string(p_chat->leader_stamp) + "#$" + p_chat->my_addr + "#$" +p_chat->my_name;
  	  cout<<"Notice "<< " new leader "<<p_chat-> my_name<<" is hearing on "<<p_chat-> my_addr<<endl; 
      broadcast(p_chat, winner_line);	

    }else{
      return;
    }  	

}

/*	new_leader_create: when a new leader is elected, we would multicast the msg to every client. 
	command#$time_stamp#$leader_ip:leader_port#$leader_name. (command is new_leader)
*/
void handle_new_leader(dchat* p_chat,vector<string> message){

  p_chat->is_election = false;

/************TO DO: Initiate the status for a new leader ****************/
  vector<string> vec = message;
  string cmd = vec[0];
  p_chat->leader_stamp = stoi(vec[1]); // not sure about this?
  string new_leader_addr =vec[2];
  string new_leader_name = vec[3];


  p_chat->leader_addr = new_leader_addr;
/************TO DO: Initiate the status for a new leader ****************/
  cout<<"NOTICE new leader "<<new_leader_name<<" is listening on "<<new_leader_addr<<endl; 
}

void handle_refuse(dchat *p_chat, vector<string> message) {

}

// command#$client_ip:client_port#$client_name#$time_stamp_of_msg_asked. (command is client_request)
void handle_client_request(dchat *p_chat, vector<string> message) {
  string client_addr = message[1];
  // string client_name = message[2];
  int time_stamp = stoi(message[3]);
  string msg = p_chat->msgs[time_stamp];
  send_handler(msg, client_addr, p_chat);
}

// command#$leader_ip:leader_port#$leader_name#$time_stamp_of_msg_asked. (command is leader_request)
void handle_leader_request(dchat *p_chat, vector<string> message) {
  string leader_addr = message[1];
  // string leader_name = message[2];
  int time_stamp = stoi(message[3]);
  string msg = p_chat->msgs[time_stamp];
  send_handler(msg, leader_addr, p_chat);
}
