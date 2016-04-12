#ifndef MEMBER_H
#define MEMBER_H

#include "dchat.h"

int start_a_leader(dchat *p_chat, string l_addr);

int start_a_regular_member(dchat *p_chat, string l_addr, string m_addr, string m_name);

#endif