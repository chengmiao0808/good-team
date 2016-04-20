#include "dchat.h"
#include "utility.h"


/*	Sequencer for normal message request	*/
void leader_handle_normal_request(dchat* p_chat, string message){ 

  //command#$time_stamp#$user_name#$user_ip:user_port#$message (command is normal)
  vector<string> vec = split(message);
  string cmd = vec[0];
  int msg_time = atoi(vec[1]); 
  string user_name =vec[2];
  string user_addr =vec[3];
  string msg = vec[4];

  // broadcast messages
  int curr_time = p_chat->current_member_stamp[user_name];
  if( msg_time == curr_time){

  	p_chat->member_event_queue[user_name].at(0) = msg;

  	for(auto iter = p_chat->member_event_queue[user_name].begin(); 
  		iter != p_chat->member_event_queue[user_name].end(); ){

	  	// use leader's timestamp
  		string temp = iter;
  		iter = p_chat->member_event_queue[user_name].erase(iter);
	  	string line = vec[0] + "#$" + to_string(p_chat->leader_stamp)+ "#$" + vec[2]+ "#$" + vec[3] + "#$" + temp;
	  	broadcast(p_chat, line);
	}

  }else if( curr_time < msg_time ){
    
    int index = msg_time - curr_time;
    p_chat->member_event_queue[user_name].at(index) = msg;

  }

}