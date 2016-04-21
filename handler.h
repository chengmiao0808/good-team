#ifndef HANDLER_H
#define HANDLER_H

#include "dchat.h"

void handle_normal_message(dchat *p_chat, vector<string> message);  // Lin

void handle_join_request(dchat *p_chat, vector<string> message);  // Lin

void handle_forward_join_request(dchat *p_chat, vector<string> message);  // Lin

void handle_join_inform(dchat *p_chat, vector<string> message);  // Lin

void handle_join_response(dchat *p_chat, vector<string> message);	// Lin

void handle_client_leave(dchat *p_chat, vector<string> message); // Lin

void handle_election(dchat *p_chat, vector<string> message); //	Lin

void handle_new_leader(dchat *p_chat, vector<string> message);	// Lin

void handle_refuse(dchat *p_chat, vector<string> message);	// Lin

void handle_client_request(dchat *p_chat, vector<string> message);

void handle_leader_request(dchat *p_chat, vector<string> message);

void leader_handle_client_heartbeat(dchat *p_chat, vector<string> message);

void client_handle_leader_heartbeat(dchat *p_chat, vector<string> message);

#endif