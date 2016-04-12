#include "member.h"
#include "msgpack.h"
#include "utility.h"

using namespace std;

vector<string> split(string str, string sep) {

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

int start_a_leader(dchat *p_chat, string l_addr) {

    char *p_laddr = new char[l_addr.length()+1];
    strcpy(p_laddr, l_addr.c_str());

    char *ip_addr, *portno;
    ip_addr = strtok(p_laddr, ":");
    portno = strtok(NULL, ":");

    p_chat->sock = socket(AF_INET, SOCK_DGRAM, 0); 
    if (p_chat->sock < 0) {
        return p_chat->sock;
    }

    bzero((char *) &(p_chat->me), sizeof(p_chat->me)); 
    p_chat->me.sin_family = AF_INET;
    p_chat->me.sin_addr.s_addr = inet_addr(ip_addr);
    p_chat->me.sin_port = htons(atoi(portno));

    p_chat->sock2 = ::bind(p_chat->sock, (struct sockaddr *) &(p_chat->me), sizeof(p_chat->me));
    if (p_chat->sock2 < 0) {
        return p_chat->sock2;
    }
    return 0;
}

int start_a_regular_member(dchat *p_chat, string l_addr, string m_addr, string m_name) {

    char *p_maddr = new char[m_addr.length()+1];
    strcpy(p_maddr, m_addr.c_str());

    char *ip_addr_me, *portno_me;
    ip_addr_me = strtok(p_maddr, ":");
    portno_me = strtok(NULL, ":");

    p_chat->sock = socket(AF_INET, SOCK_DGRAM, 0); 
    if (p_chat->sock < 0) {
        return p_chat->sock;
    }

    bzero((char *) &(p_chat->me), sizeof(p_chat->me)); 
    p_chat->me.sin_family = AF_INET;
    p_chat->me.sin_addr.s_addr = inet_addr(ip_addr_me);
    p_chat->me.sin_port = htons(atoi(portno_me));

    p_chat->sock2 = ::bind(p_chat->sock, (struct sockaddr *) &(p_chat->me), sizeof(p_chat->me));
    if (p_chat->sock2 < 0) {
        return p_chat->sock2;
    }

    char *p_laddr = new char[l_addr.length()+1];
    strcpy(p_laddr, l_addr.c_str());

    char *ip_addr_other, *portno_other;
    ip_addr_other = strtok(p_laddr, ":");
    portno_other = strtok(NULL, ":");

    bzero((char *) &(p_chat->other), sizeof(p_chat->other)); 
    p_chat->other.sin_family = AF_INET;
    p_chat->other.sin_addr.s_addr = inet_addr(ip_addr_other);
    p_chat->other.sin_port = htons(atoi(portno_other));

    char buff[2048];
    bzero(buff, 2048);

    int currtime = getLocalTime();  //get current time with utility function
    msgpack msg_pack(ip_addr_me, atoi(portno_me), m_name, currtime, 1, "N/A");
    string msg_sent = utility::serialize(msg_pack);
    strcpy(buff, msg_sent.c_str());

    p_chat->num = sendto(p_chat->sock, buff, strlen(buff), 0, (struct sockaddr *) &(p_chat->other), sizeof(p_chat->other));
    if (p_chat->num < 0) {
        return p_chat->num;
    }
    p_chat->len = sizeof(p_chat->other);
    bzero((char *) &(p_chat->other), p_chat->len);
    bzero(buff, 2048);
    p_chat->num = recvfrom(p_chat->sock, buff, 2048, 0, (struct sockaddr *) &(p_chat->other), &(p_chat->len));
    if (p_chat->num < 0) {
        return p_chat->num;
    }
    string msg_recv = buff;
    msg_pack = utility::deserialize(msg_recv);
    string members = msg_pack.msg;
    vector<string> vec = split(members, "\t");
    p_chat->leader = vec.pop_back();
    while (!vec.empty()) {
        p_chat->all_members_list[vec.pop_back()] = vec.pop_back();
    }

    return 0;
}