#ifndef MEMBER_H
#define MEMBER_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <string>
#include <iostream>
#include <map>
#include <set>
#include <vector>
#include <deque>
#include <sysexits.h>
#include <signal.h>

int start_a_leader(string l_addr, string l_name);

int start_a_regular_member(string l_addr, string m_addr, string m_name);

#endif