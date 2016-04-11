#ifndef MSGQUE_H
#define MSGQUE_H

#include <iostream>
#include <string>
#include <queue>
#include <deque>
#include "msgPack.h" 			// for message struct: MsgPack

using namespace std;


/*	Comparator for the message struct: MsgPack 	*/
struct msgSorter{
	bool operator() (const msgPack &lhs, const msgPack &rhs) const{
		return lhs.time < rhs.time;
	}
};
msgSorter ms;

/*	The Holdback Queue Storing the MsgPacks	*/
class holdbackQueue{
    deque<msgPack>  msgQueue;            // message package list
    //deque<MsgPack>::iterator it;   	   
	int numMsg;


public:
	holdbackQueue(){	numMsg = 0;}	// default constructor
	msgPack topMsg(){	return msgQueue.front();} // return  Top MsgPack

	bool insertMsg(msgPack &mp){
		msgQueue.push_back(mp);
		numMsg++;
		sort( msgQueue.begin() , msgQueue.end() , ms);
//		SortQueue();
		return true;
	}

	bool deleteMsg(msgPack &mp){
		for(auto it = msgQueue.begin(); it! = msgQueue.end(); ++it){
			if( it->username == mp.username){
				msgQueue.erase(it);
				numMsg--;
				sort( msgQueue.begin() , msgQueue.end() , ms);
				//		SortQueue();
				return true;
           	}
		}
		return false;
	}
	
};

#endif