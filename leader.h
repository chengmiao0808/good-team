#include "dchat.h"
#include "utility.h"


/*	Sequencer for normal message request	*/
void handle_normal_request(dchat* p_chat, string message){ 

  //command#$time_stamp#$user_name#$user_ip:user_port#$message
  vector<string> vec = split(message);
  string cmd = vec[0];
  int msg_time = atoi(vec[1]); 
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
	  	line = vec[0] + "#$" + to_string(p_chat->leader_stamp)+ "#$" + vec[2]+ "#$" + vec[3] + "#$" + line;
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
  new_user_name = message[2];
  new_user_addr = message[3];
  p_chat->all_members_list[new_user_addr] = new_user_name;
  p_chat->current_member_stamp[new_user_addr] = 1; //new_user's count initiate

  // 2. broadcast the "Notice xxxx joined on xxxxx""
  string line = "NOTICE " + new_user_name + " joined on " + new_user_addr;
  line = "join_inform" + "#$" 
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

  string line = "join_inform" + "#$" 
        + to_string(p_chat->leader_stamp)+ "#$" 
        + p_chat->leader_addr + memeber_list;

  send_handler(line, new_user_addr, p_chat);
  p_chat->leader_stamp++;
}


/*  client_join_request: when client sends a join request to leader/other client. 
    command#$time_stamp#$user_name#$my_ip:my_port (command is join_request)
*/
void client_join_request(dchat* p_chat, string new_user_addr, string new_user_name){
  
}


/*  client_join_inform: when leader receives a join request,
    multicast the join_form to current client. 
    command#$time_stamp#$username#$client_ip:client_port#$message (command is join_inform)
 */
void client_join_inform(dchat* p_chat, string new_user_name, string new_user_addr){
  

}

/*  client_join_response: when leader receives a join request, 
    send the msg back to the client who's joinning. 
    command#$time_stamp#$leader_ip:leader_port#$first_member_ip:first_member_port#$first_member_name#$...(command is join_response)
*/
void client_join_response(dchat* p_chat, string new_user_addr){


}


