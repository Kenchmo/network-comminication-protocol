//
//  thrds.cpp
//  network communication protocol
//
//
#include <chrono>
#include "thrds.h"
#include "sleep.h"
#include "hash.h"
using namespace std;

extern server_t this_server;
extern msgq_t * msgq1, * msgq2;

int client1_connected = 0;
int client2_connected = 0;
int notify_c1 = 0;
int notify_c2 = 0;

void *server_thread(void *threadarg){
    //initialize server
    
    this_server.lock.lock = UNLOCKED;
    this_server.msg.sender = NOTSET;
    this_server.msg.req_or_resp = NOTSET;
    this_server.msg.acked = NOTSET;

    //waiting for two clients to connect
    while(1)
        if(client1_connected && client2_connected) break;
    //start processing requests and responses
    
    while(1){
        //if server has unacknowledged msg
        if(!this_server.msg.acked){
            if(this_server.msg.sender == C1){
                notify_c2 = 1; //notify c2
                while(!this_server.msg.acked){} // wait until c2 acked the msg
            }
        }
        
    }
    //pthread_exit(NULL);
    return NULL;
}



/**
 * INPUT: void* threadarg, the task queue
 * EFFECTS: sending the task to the other client through the server
 *          receiving the response from the other client through the sever
 *
 *
 **/
void *client1_thread(void *threadarg) {
    msgq_t *msgq = (msgq_t *) threadarg;
    
    client1_connected = 1;
    while(1){
        
        if(notify_c1){                  //see if there is msg for me
            //read from msg;
            while(this_server.lock.lock == LOCKED){}   //wait until the other one is done writing
            this_server.lock.lock = LOCKED;
            notify_c1 = 0;
            if(this_server.msg.req_or_resp == RESP){
                if(this_server.msg.respmsg.rtype == IDTYPE){
                    cout<<"CLIENT 2 RESP ID: "<< *(this_server.msg.respmsg.data)<<endl;
                }
                if(this_server.msg.respmsg.rtype == HASHTYPE){
                    cout<<"CLIENT 2 RESP HASH: "<< *(this_server.msg.respmsg.data)<<endl;
                }
            }
            else if(this_server.msg.req_or_resp == REQ){
                while(msgq1->lock.lock == LOCKED){} //wait
                msgq->lock.lock = LOCKED;
                msg_t msgtemp = this_server.msg;
                msgq->v.push_back(msgtemp);
                msgq->lock.lock = UNLOCKED;
            }
            this_server.msg.acked = ACKED;
            this_server.lock.lock = UNLOCKED;
        }
        
        if(msgq->v.size() != 0){
            if(this_server.lock.lock == LOCKED) break; //don not wait, avoid deadlock
            else{   // write to server msg
                msg_t msgtemp = msgq->v[0];
                if(msgtemp.sender == C2 && msgtemp.req_or_resp == REQ) break;
                msgq->v.erase(msgq->v.begin());
                this_server.lock.lock = LOCKED;
                
                
                this_server.msg.sender = C1;
                this_server.msg.acked = UNACKED;
                this_server.msg.req_or_resp = msgtemp.req_or_resp;
                this_server.msg.reqmsg = msgtemp.reqmsg;
                this_server.msg.respmsg = msgtemp.respmsg;
                
                this_server.lock.lock = UNLOCKED;
            }
        }
    }
    //pthread_exit(NULL);
    return NULL;
}



/**
 * INPUT: void* threadarg, the task queue
 * EFFECTS: sending the task to the other client through the server
 *          receiving the response from the other client through the sever
 *
 *
 **/
void *client2_thread(void *threadarg) {
    msgq_t *msgq = (msgq_t *) threadarg;
    
    client2_connected = 1;
    while(1){
        
        if(notify_c2){                  //see if there is msg for me
            //read from msg;
            while(this_server.lock.lock == LOCKED){}   //wait until the other one is done writing
            this_server.lock.lock = LOCKED;
            notify_c2 = 0;
            if(this_server.msg.req_or_resp == RESP){
                if(this_server.msg.respmsg.rtype == IDTYPE){
                    cout<<"CLIENT 1 RESP ID: "<< *(this_server.msg.respmsg.data)<<endl;
                }
                if(this_server.msg.respmsg.rtype == HASHTYPE){
                    cout<<"CLIENT 1 RESP HASH: "<< *(this_server.msg.respmsg.data)<<endl;
                }
            }
            else if(this_server.msg.req_or_resp == REQ){
                while(msgq->lock.lock == LOCKED){} //wait
                msgq->lock.lock = LOCKED;
                msg_t msgtemp = this_server.msg;
                msgq->v.push_back(msgtemp);
                msgq->lock.lock = UNLOCKED;
            }
            this_server.msg.acked = ACKED;
            this_server.lock.lock = UNLOCKED;
        }
        
        if(msgq->v.size() != 0){
            if(this_server.lock.lock == LOCKED) break; //don not wait, avoid deadlock
            else{   // write to server msg
                msg_t msgtemp = msgq->v[0];
                if(msgtemp.sender == C1 && msgtemp.req_or_resp == REQ) break;
                msgq->v.erase(msgq->v.begin());
                this_server.lock.lock = LOCKED;
                
                this_server.msg.sender = C2;
                this_server.msg.acked = UNACKED;
                this_server.msg.req_or_resp = msgtemp.req_or_resp;
                this_server.msg.reqmsg = msgtemp.reqmsg;
                this_server.msg.respmsg = msgtemp.respmsg;
                
                this_server.lock.lock = UNLOCKED;
            }
        }
    }
    //pthread_exit(NULL);
    return NULL;
}


// the user thread is quivalent to computers on the two ends
void *user1_thread(void *threadarg) {
    msgq_t *msgq = (msgq_t *) threadarg;
    int * userid = new int(12345678);
    int * hashingarg = new int;
    while(1){

        sleep_ms(1000);
        while(msgq->lock.lock == LOCKED){} //wait
        msgq->lock.lock = LOCKED;
        size_t len = msgq->v.size();
        for(size_t i = 0; i < len; i++){
            //check if there is req from c2
            if(msgq->v[i].req_or_resp == REQ && msgq->v[i].sender == C2){
                
                if(msgq->v[i].reqmsg.ttype == IDTYPE){
                    msgq->v[i].sender = C1;
                    msgq->v[i].req_or_resp = RESP;
                    msgq->v[i].acked = UNACKED;
                    msgq->v[i].respmsg.rtype = IDTYPE;
                    msgq->v[i].respmsg.datalen = 1;
                    void * dest = msgq->v[i].respmsg.data;
                    void * src = userid;
                    memcpy(dest, src, 1);
                }
                if(msgq->v[i].reqmsg.ttype == HASHTYPE){
                    
                    void * dest = hashingarg;
                    void * src = msgq->v[i].reqmsg.data;
                    memcpy(dest, src, 1);
                    int * hashval = hash_func((int*)dest);
                    msgq->v[i].sender = C1;
                    msgq->v[i].req_or_resp = RESP;
                    msgq->v[i].acked = UNACKED;
                    msgq->v[i].respmsg.rtype = HASHTYPE;
                    msgq->v[i].respmsg.datalen = 1;
                    dest = msgq->v[i].respmsg.data;
                    src = hashval;
                    memcpy(dest, src, 1);
                    
                }
                break;
            }
        }
        msgq->lock.lock = UNLOCKED;
    
    }
    //pthread_exit(NULL);
    return NULL;
}

// the user thread is quivalent to computers on the two ends
void *user2_thread(void *threadarg) {
    msgq_t *msgq = (msgq_t *) threadarg;
    int * userid = new int(12345678);
    int * hashingarg = new int(1);
    while(1){
        sleep_ms(1000);
        while(msgq->lock.lock == LOCKED){} //wait
        msgq->lock.lock = LOCKED;
        size_t len = msgq->v.size();
        for(size_t i = 0; i < len; i++){
            //check if there is req from c2
            if(msgq->v[i].req_or_resp == REQ && msgq->v[i].sender == C1){
                
                if(msgq->v[i].reqmsg.ttype == IDTYPE){
                    msgq->v[i].sender = C2;
                    msgq->v[i].req_or_resp = RESP;
                    msgq->v[i].acked = UNACKED;
                    msgq->v[i].respmsg.rtype = IDTYPE;
                    msgq->v[i].respmsg.datalen = 1;
                    void * dest = msgq->v[i].respmsg.data;
                    void * src = userid;
                    memcpy(dest, src, 1);
                }
                if(msgq->v[i].reqmsg.ttype == HASHTYPE){
                    
                    void * dest = hashingarg;
                    void * src = msgq->v[i].reqmsg.data;
                    memcpy(dest, src, 1);
                    int * hashval = hash_func((int*)dest);
                    msgq->v[i].sender = C2;
                    msgq->v[i].req_or_resp = RESP;
                    msgq->v[i].acked = UNACKED;
                    msgq->v[i].respmsg.rtype = HASHTYPE;
                    msgq->v[i].respmsg.datalen = 1;
                    dest = msgq->v[i].respmsg.data;
                    src = hashval;
                    memcpy(dest, src, 1);
                    
                }
                break;
            }
        }
        msgq->lock.lock = UNLOCKED;
        
    }
    //pthread_exit(NULL);
    return NULL;
}
