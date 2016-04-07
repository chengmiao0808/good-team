#include <iostream>
#include <string>
#include <deque>
#include "msgpack.h" 			// for message struct: MsgPack

using namespace std;



/*	Comparator for the message struct: MsgPack 	*/
struct MsgComparator{
	bool operator() (const MsgPack& lhs, const MsgPack& rhs) const{
		return lhs->getTime() < rhs->getTime();
	}
};


/*	The Holdback Queue Storing the MsgPacks	*/
class HoldbackQueue{
    deque<MsgPack>  MsgQueue;            // message package list
    deque<MsgPack>::iterator it;   	   
	int NumMsg;

public:
	HoldbackQueue(){	NumMsg = 0;}	// default constructor
	MsgPack TopMsg(){	return MsgQueue.front();} // default constructor

	bool InsertMsg(MsgPack mp){
		MsgQueue.push_back(mp);
		NumMsg++;
		sort( MsgQueue.begin() , MsgQueue.end() , MsgComparator);
//		SortQueue();
		return true;
	}

	bool DeleteMsg(MsgPack mp){
		for( it = MsgQueue.begin(); it! = MsgPack.end(); ++it){
           	if( it->getID() == mp.getID()){
               	MsgQueue.erase(it);
               	NumMsg--;
				sort( MsgQueue.begin() , MsgQueue.end() , MsgComparator);
				//		SortQueue();
              	 	return true;
           	}
       	}
        return false;
	}

};
