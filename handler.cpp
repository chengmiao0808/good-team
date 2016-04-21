#include "handler.h"

using namespace std;

/*
	Helper function from dchat.cpp & utility.cpp
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/ 
int getLocalTime(){
	time_t timer;
	time(&timer);  // get current time
	return (int)timer;
}

vector<string> split_helper(string str, string sep) {
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

vector<string> split(string str) {
	return split_helper(str, "#$");
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

//      error("Error with sendto!\n");
  }
}

void broadcast(dchat *p_chat, string msg) {

  for (auto iter = p_chat->all_members_list.begin(); iter != p_chat->all_members_list.end(); iter++) {
  //cout<<"\t this iter: \t"<< iter->first << endl;
    if(iter->first == p_chat->leader_addr) continue; //dont send to leader herself
    send_handler(msg, iter->first, p_chat);
  }
}

/*	<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
	Helper function from dchat.cpp & utility.cpp*/


void handle_normal_message(dchat* p_chat, vector<string> message){ 

  //command#$time_stamp#$user_name#$user_ip:user_port#$message
  vector<string> vec = message;
  string cmd = vec[0];
  int msg_time = stoi(vec[1]); 
  string user_name =vec[2];
  string user_addr =vec[3];
  string msg = vec[4];

  // update count of the memeber
  p_chat->current_member_stamp[user_addr]++;

  // broadcast messages
  // int curr_time = p_chat->current_member_stamp[user_name];
  // 	p_chat->member_event_queue[user_name].at(0) = msg;

  	// for(auto iter = p_chat->member_event_queue[user_name].begin(); 
  	// 	iter != p_chat->member_event_queue[user_name].end(); ){

	  	// use leader's timestamp
  		// string line = iter;
  		// iter = p_chat->member_event_queue[user_name].erase(iter);
	  	string line = vec[0] + "#$" + to_string(p_chat->leader_stamp)+ "#$" + vec[2]+ "#$" + vec[3] + "#$" + vec[4];
      p_chat->msgs[p_chat->leader_stamp] = line;
      p_chat->leader_stamp++;
	  	broadcast(p_chat, line);
	   // }
}


/*  client_join_request: when client sends a join request to leader/other client. 
    command#$time_stamp#$user_name#$my_ip:my_port (command is join_request)
*/
void handle_join_request(dchat* p_chat,  vector<string> message){

  // 1. add new member into the map
  string new_user_name = message[2];
  string new_user_addr = message[3];
  deque<string> new_deque(10);
  p_chat->all_members_list[new_user_addr] = new_user_name;    //add new_user into the list 
  p_chat->member_event_queue[new_user_name] = new_deque;      //new_user's msg queue
  p_chat->current_member_stamp[new_user_addr] = 0;            //new_user's msg count
  p_chat->member_last_alive[new_user_name] = getLocalTime();  //new_user's last alive time

  // 2. broadcast the "Notice xxxx joined on xxxxx""
  p_chat->leader_stamp++;
  string line = "NOTICE " + new_user_name + " joined on " + new_user_addr;
  line = "join_inform#$" 
        + to_string(p_chat->leader_stamp)+ "#$" 
        + new_user_name + "#$" 
        + new_user_addr + "#$" 
        + line;
  broadcast(p_chat, line);
  p_chat->leader_stamp++;

  // 3. send member list to the new member
    /* Create the member list */
  string memeber_list = "";
  for (auto iter = p_chat->all_members_list.begin(); iter != p_chat->all_members_list.end(); iter++) {
    memeber_list += "#$"  + iter->first;
    memeber_list+= "#$"  + iter->second;
  } 

  string response = "join_inform#$" 
        + to_string(p_chat->leader_stamp)+ "#$" 
        + p_chat->leader_addr + memeber_list;

  send_handler(response, new_user_addr, p_chat);
  p_chat->leader_stamp++;
}

/*
    command#$time_stamp#$user_name#$my_ip:my_port (command is join_request)
*/
void handle_forward_join_request(dchat *p_chat, vector<string> message){
  string forward_msg = message[0] + message[1] + message[2] + message[3];
  string other_addr = p_chat->leader_addr;
  send_handler(forward_msg, other_addr, p_chat); 
}

/*
	command#$time_stamp#$client_ip:client_port#$username#$message (command is join_inform)
*/
void handle_join_inform(dchat *p_chat, vector<string> message){
  string cmd = message[0];
  int leader_timestamp = stoi(message[1]); 
  string new_user_addr = message[2];
  string new_user_name = message[3];
  string msg = message[4];

  cout<<msg<<endl;
  p_chat->current_stamp = leader_timestamp;
  p_chat->all_members_list[new_user_addr] = new_user_name;

}  

/*command#$time_stamp#$leader_ip:leader_port#$first_member_ip:first_member_port#$first_member_name#$...(command is join_response)
*/
void handle_join_response(dchat *p_chat, vector<string> message){
  
  string cmd = message[0];
  int leader_timestamp = stoi(message[1]); 
  string leader_addr = message[2];

  p_chat->current_stamp = leader_timestamp;
  p_chat->leader_addr = leader_addr;
  for(int index = 3; index<message.size(); index+=2) {
    string key = message[index];
    string val = message[index+1];
    p_chat->all_members_list[key] = val;
  }
}	

/* 	client_leave: when leader detect a client leaves or crashes, 
	he would send a leave message to every client. 
	command#$time_stamp#$client_leave_ip:client_leave_port#$message (command is client_leave)
*/
void handle_client_leave(dchat *p_chat, vector<string> message){
  string cmd = message[0];
  int leader_timestamp = stoi(message[1]); 
  string left_user_addr = message[2];
  string msg = message[3];

  p_chat->current_stamp = leader_timestamp;
  p_chat->all_members_list.erase(left_user_addr);
  cout<<msg<<endl;
}
