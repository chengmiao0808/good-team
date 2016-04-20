#include "dchat.h"
#include "utility.h"



/**
 * Bully Comparison based on alphetic order of "IP:PORT"
 */
bool bully_compare(string me, string candidate){
  if(me.compare(candidate) > 0){
    // reject
    return true;
  }else {
    // agree
    return false;
  }
}

/*
election: when client detect leader leaves or crashes, 
he would send a election message to all clients that have higher ip:port than himself. 
command#$time_stamp#$client_ip:client:port (command is election)
*/
void handle_election(dchat* p_chat, string message){

  //command#$time_stamp#$client_ip:client:port (command is election)
  vector<string> vec = split(message);
  string cmd = vec[0];
  int msg_time = atoi(vec[1]); 
  string informer_addr =vec[2];

  if(bully_compare(p_chat->my_addr, informer_addr)){
  	// I bully this one

  	// 1. send back "okay": command#$time_stamp#$client_ip:client_port. (command is refuse)
  	string line = "refuse" + "#$" + to_string(p_chat->current_stamp) + "#$" + informer_addr;
  	// 2. send election to others with higher priority:   	
  	for (auto iter = p_chat->all_members_list.begin(); iter != p_chat->all_members_list.end(); iter++) {
  	  if( !bully_compare(p_chat->my_addr, iter->first) ){
  	    send_handler(msg, iter->first, p_chat);
  	  }
  	}
  	// 3. wait for the result
  	bool is_new_leader = true;
  	int start_time = getLocalTime();
  	while( getLocalTime() - start < 10000000 ){

  	}
  	if(is_new_leader){
  		//
  	}

  }else{ // impossible to have this situation
  	continue;
  }

}


void hold_election(dchat* p_chat){
	// broadcast election start to those with higher priority

}



/*
new_leader_create: when a new leader is elected, 
we would multicast the msg to every client. 
command#$time_stamp#$leader_ip:leader_port#$leader_name. (command is new_leader)
*/



/*
refuse: when client's election is refused, 
he would send a message back to the client who sends election. 
command#$time_stamp#$client_ip:client_port. (command is refuse)
*/
void refuse(dchat* p_chat, string user_name, string user_addr){ //user is the one who inform me


}