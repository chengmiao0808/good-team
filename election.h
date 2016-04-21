#include "dchat.h"
#include "utility.h"



/**
 * Bully Comparison based on alphetic order of "IP:PORT"
 */
bool bully_compare(string me, string candidate){
  if(me.compare(candidate) > 0){ // reject
    return true;
  }else{ 						// agree
    return false;
  }
}



/*
election: when client detect leader leaves or crashes, 
he would send a election message to all clients that have higher ip:port than himself. 
command#$time_stamp#$client_ip:client:port (command is election)
*/
void handle_election(dchat* p_chat, string message){

  p_chat->all_members_list.erase(p_chat->leader_addr);

  // command#$time_stamp#$client_ip:client:port (command is election)
  vector<string> vec = split(message);
  string cmd = vec[0];
  int msg_time = atoi(vec[1]); 
  string informer_addr =vec[2];

  // I bully this informer, or I am the 1st one who start the election 
  if(bully_compare(p_chat->my_addr, informer_addr) || message.empty() ){

  	// 1. send back "okay": command#$time_stamp#$client_ip:client_port. (command is refuse)
  	if(!message.empty()){
  	  string okay_msg = "refuse#$" + to_string(p_chat->current_stamp) + "#$" + my_addr;
	  send_handler(okay_msg, informer_addr, p_chat);
	}

  	// 2. send election to others with higher priority: 
  	//  	command#$time_stamp#$client_ip:client:port (command is election)
	string elect_msg = "election#$" + to_string(p_chat->current_stamp) + "#$" + my_addr;
  	for (auto iter = p_chat->all_members_list.begin(); iter != p_chat->all_members_list.end(); iter++) {
  	  if( !bully_compare(p_chat->my_addr, iter->first) ){
  	    send_handler( elect_msg, iter->first, p_chat);
  	  }
  	}

  	// 3. wait for the result
  	p_chat->is_leader = true;
  	int start_time = getLocalTime();
  	while( getLocalTime() - start < 50000000 ){
	    char buff[2048];
	    bzero(buff, 2048);
	    p_chat->num = recvfrom(p_chat->sock, buff, 2048, 0, (struct sockaddr *) &(p_chat->other), (socklen_t *) &(p_chat->len));
	    if (p_chat-> num < 0) {
	      error("Error with recvfrom!\n");
	    }
	    vector<string> message = split(buff);
  		if (strcmp(message[0], "refuse") == 0) {
    	  p_chat->is_leader = false;
  		} 
  	}
  	// 4. If you win, broadcast it!
  	// new_leader_create: when a new leader is elected, 
  	// we would multicast the msg to every client. 
  	// command#$time_stamp#$leader_ip:leader_port#$leader_name. (command is new_leader) 
  	if(p_chat->is_leader){
  	  p_chat->leader_addr = p_chat->my_addr;
  	  p_chat->leader_stamp = p_chat->current_stamp;
  	  string winner_line = "new_leader#$" + to_string(p_chat->current_stamp) + "#$" + my_addr + "#$" + my_name;
	  cout<<"Notice "<< " new leader "<<new_leader_name<<" is hearing on "<<new_leader_addr<<endl; 
  	  broadcast(p_chat, winner_line);	
  	}

  }else{ // impossible to have this situation
  	continue;
  }

}

/*	new_leader_create: when a new leader is elected, 
	we would multicast the msg to every client. 
	command#$time_stamp#$leader_ip:leader_port#$leader_name. (command is new_leader)
*/
void handle_new_leader(p_chat, message){
  vector<string> vec = split(message);
  string cmd = vec[0];
  int msg_time = atoi(vec[1]); 
  string new_leader_addr =vec[2];
  string new_leader_name =vec[3];
  p_chat->leader_addr = new_leader_addr;
  p_chat->leader_name = new_leader_name;
  cout<<"Notice "<< " new leader "<<new_leader_name<<" is hearing on "<<new_leader_addr<<endl; 
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