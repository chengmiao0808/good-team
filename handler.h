#ifndef HANDLER_H
#define HANDLER_H

#include "dchat.h"

void error(string err);

void send_handler(string msg, string other_addr, dchat *p_chat);

void broadcast(dchat *p_chat, string msg);

void handle_normal_message(dchat *p_chat, vector<string> message);

void handle_join_request(dchat *p_chat, vector<string> message);

void handle_forward_join_request(dchat *p_chat, vector<string> message);

void handle_join_inform(dchat *p_chat, vector<string> message);

void handle_client_leave(dchat *p_chat, vector<string> message);

void handle_election(dchat *p_chat, vector<string> message);

void handle_new_leader(dchat *p_chat, vector<string> message);

void handle_refuse(dchat *p_chat, vector<string> message);

void handle_client_request(dchat *p_chat, vector<string> message);

void handle_leader_request(dchat *p_chat, vector<string> message);


#endif