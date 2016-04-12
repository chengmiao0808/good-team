#ifdef SENDER_H
#define SENDER_H

#include <string>
#include <iostream>
#include <vector>
#include <stdlib.h>
#include <stdio.h>

using namespace std;

/*	Comparator for the message struct: MsgPack 	*/
struct msg_sorter{
	bool operator() (const msgpack &lhs, const msgpack &rhs) const{
		return lhs.time < rhs.time;
	}
};
msgsorter ms;

struct sender
{	deque<msgpack> msg_queue;
	int num_msg;

	sender() {
		num_msg = 0;
	}

	msgpack peek() {
		return msg_queue.front();
	}

	bool offer(msgpack &m) {
		msg_queue.push_back(m);
		num_msg++;
		return true;
	}
	
	bool pool() {
		sort( msg_queue.begin() , msg_queue.end() , ms);
		num_msg--;
		msgpack = peek();
		msg_queue.pop_front();
		return true;
	}
};

#endif /* SENDER_H */

