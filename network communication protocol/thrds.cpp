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

extern Server_class this_server;
extern Msgq * msgq1, * msgq2;

int client1_connected = 0;
int client2_connected = 0;
volatile int notify_c1 = 0;
volatile int notify_c2 = 0;

void *server_thread(void *threadarg){
    //initialize Server_class
    //int notified_flag = 0;

    //waiting for two clients to connect
    while(1){
        if(client1_connected && client2_connected){
            cout<<"client 1 & 2 connected \n";
            break;
        }
    }
    
    //start processing requests and responses
    while(1){
//        //if Server_class has unacknowledged msg
//        if(this_server.msg.acked == UNACKED && notified_flag == 0){
//            this_server.lock.lock();
//            if(this_server.msg.sender == C1){
//                notify_c2 = 1; //notify c2
//                cout<<"notified c2 \n";
//                //wait until c2 put the notify flag down
//                while(notify_c2){}
//            }
//            if(this_server.msg.sender == C2){
//                cout<<"notifying c1 \n";
//                notify_c1 = 1; //notify c2
//                //wait until c1 put the notify flag down
//                while(notify_c1){}
//            }
//            this_server.lock.unlock();
//        }
    }
    //pthread_exit(NULL);
    return NULL;
}



/**
 * INPUT: void* threadarg, the task queue
 * EFFECTS: sending the task to the other client through the Server_class
 *          receiving the response from the other client through the sever
 *
 *
 **/
void *client1_thread(void *threadarg) {
    Msgq *msgq = (Msgq *) threadarg;
    client1_connected = 1;
    
    while(1)
        if(client1_connected && client2_connected)
            break;
    
    while(1){

        this_server.lock.lock();
        
        /* see if there is msg for me */
        
        if(!this_server.msg.empty() &&this_server.msg.top().sender == C2 && this_server.msg.top().acked == UNACKED){
            
            // if the msg is a response, simply print it to screen
            if(this_server.msg.top().req_or_resp == RESP){
                if(this_server.msg.top().type == IDTYPE)
                    cout<<"CLIENT 2 RESP ID TO CLIENT 1: ";
                else if(this_server.msg.top().type == HASHTYPE)
                    cout<<"CLIENT 2 RESP HASH RESULT: ";
                this_server.msg.top().print_data();
            }
            
            // if the msg is a request,
            // 1. print to screen
            // 2. push to the msg queue, waiting for the user to process
            else if(this_server.msg.top().req_or_resp == REQ){
                
                if(this_server.msg.top().type == IDTYPE)
                    cout<<"CLIENT 1 RECEIVED ID REQ FROM CLIENT 2 \n";
                else if(this_server.msg.top().type == HASHTYPE)
                    cout<<"CLIENT 1 RECEIVED HASH REQ FROM CLIENT 2 \n";
                Msg msgtemp = this_server.msg.top();
                msgq->lock.lock();
                msgq->q.push(msgtemp);
                msgq->lock.unlock();
            }
            
            this_server.msg.top().acked = ACKED;
        }
        
        /* if there is no msg for me , see if I need to send msg */
        
        else{
            if(this_server.msg.empty() || (this_server.msg.top().acked == ACKED || this_server.msg.top().acked == NOTSET)){
                
                msgq->lock.lock();              //lock the msg queue
                
                if(msgq->q.size() != 0 && msgq->q.front().sender == C1){
                    this_server.msg.push(msgq->q.front());
                    msgq->q.pop();
                }
                msgq->lock.unlock();
            }
        }
        this_server.lock.unlock();
    }
    //pthread_exit(NULL);
    return NULL;
}



/**
 * INPUT: void* threadarg, the task queue
 * EFFECTS: sending the task to the other client through the Server_class
 *          receiving the response from the other client through the sever
 *
 *
 **/
void *client2_thread(void *threadarg){
    Msgq *msgq = (Msgq *) threadarg;
    client2_connected = 1;
    
    while(1)
        if(client1_connected && client2_connected)
            break;
    
    while(1){
        
        this_server.lock.lock();
        
        /* see if there is msg for me */
        
        if(!this_server.msg.empty() && this_server.msg.top().sender == C1 && this_server.msg.top().acked == UNACKED){
            
            // if the msg is a response, simply print it to screen
            if(this_server.msg.top().req_or_resp == RESP){
                if(this_server.msg.top().type == IDTYPE)
                    cout<<"CLIENT 1 RESP ID TO CLIENT 2: ";
                else if(this_server.msg.top().type == HASHTYPE)
                    cout<<"CLIENT 1 RESP HASH RESULT: ";
                this_server.msg.top().print_data();
            }
            
            // if the msg is a request,
            // 1. print to screen
            // 2. push to the msg queue, waiting for the user to process
            else if(this_server.msg.top().req_or_resp == REQ){
                
                if(this_server.msg.top().type == IDTYPE)
                    cout<<"CLIENT 2 RECEIVED ID REQ FROM CLIENT 1 \n";
                else if(this_server.msg.top().type == HASHTYPE)
                    cout<<"CLIENT 2 RECEIVED HASH REQ FROM CLIENT 1 \n";
                Msg msgtemp = this_server.msg.top();
                msgq->lock.lock();
                msgq->q.push(msgtemp);
                msgq->lock.unlock();
            }
            
            this_server.msg.top().acked = ACKED;
        }
        
        /* if there is no msg for me , see if I need to send msg */
        
        else{
            if(this_server.msg.empty() || (this_server.msg.top().acked == ACKED || this_server.msg.top().acked == NOTSET)){
                
                msgq->lock.lock();              //lock the msg queue
                if(msgq->q.size() != 0 && msgq->q.front().sender == C2){
                    this_server.msg.push(msgq->q.front());
                    msgq->q.pop();
                }
                msgq->lock.unlock();
            }
        }
        this_server.lock.unlock();
    }
    //pthread_exit(NULL);
    return NULL;


}


// the user thread is quivalent to computers on the two ends
void *user1_thread(void *threadarg){
    Msgq *msgq = (Msgq *) threadarg;
    int solving_flag = 0;
    Msg msgtemp;
    msgtemp.datatype = NOTSET;
    
    int8_t idtype = INTARR;
    int idlen = 5*sizeof(int);
    int * id = (int *)malloc(idlen);
    id[0] = 15;
    id[1] = 30;
    id[2] = 58;
    id[3] = 78;
    id[4] = 97;
    
    while(1){
        //if there is a request to solve, work on it
        if(solving_flag){
            //solve msgtemp, put result to msgtemp
            msgtemp.sender = C1;
            msgtemp.req_or_resp = RESP;
            msgtemp.acked = UNACKED;
            if(msgtemp.type == IDTYPE){
                if(msgtemp.data) free(msgtemp.data);
                msgtemp.data = malloc(idlen);
                memcpy(msgtemp.data, id, idlen);
                msgtemp.datalen = idlen;
                msgtemp.datatype = idtype;
            }
            else if(msgtemp.type == HASHTYPE){
                msgtemp.data = hash_func(msgtemp.data, msgtemp.datatype);
            }
            msgq->lock.lock();
            msgq->q.push(msgtemp);
            msgq->lock.unlock();
            solving_flag = 0;
        }
        else{
            msgq->lock.lock();
            // if the queue front is a request from peer
            // copy the request to local
            if(msgq->q.front().req_or_resp == REQ && msgq->q.front().sender == C2){
                msgtemp = msgq->q.front();
                msgq->q.pop();
                solving_flag = 1;
            }
            msgq->lock.unlock();
        }
        //sleep_ms(1000);
    }
    //pthread_exit(NULL);
    return NULL;
}

// the user thread is quivalent to computers on the two ends
void *user2_thread(void *threadarg){
    
    Msgq *msgq = (Msgq *) threadarg;
    int solving_flag = 0;
    Msg msgtemp;
    msgtemp.datatype = NOTSET;
    
    int idlen = 5 * sizeof(int);
    int * id = (int *)malloc(idlen);
    id[0] = 25;
    id[1] = 40;
    id[2] = 68;
    id[3] = 88;
    id[4] = 00;
    
    while(1){
        //if there is a request to solve, work on it
        if(solving_flag){
            //solve msgtemp, put result to msgtemp
            msgtemp.sender = C2;
            msgtemp.req_or_resp = RESP;
            msgtemp.acked = UNACKED;
            if(msgtemp.type == IDTYPE){
                if(msgtemp.data) free(msgtemp.data);
                msgtemp.data = malloc(idlen);
                memcpy(msgtemp.data, id, idlen);
                msgtemp.datalen = idlen;
                msgtemp.datatype = INTARR;
            }
            else if(msgtemp.type == HASHTYPE){
                msgtemp.data = hash_func(msgtemp.data, msgtemp.datatype);
            }
            msgq->lock.lock();
            msgq->q.push(msgtemp);
            msgq->lock.unlock();
            solving_flag = 0;
        }
        else{
            msgq->lock.lock();
            // if the queue front is a request from peer
            // copy the request to local
            if(msgq->q.front().req_or_resp == REQ && msgq->q.front().sender == C1){
                msgtemp = msgq->q.front();
                msgq->q.pop();
                solving_flag = 1;
            }
            msgq->lock.unlock();
        }
        //sleep_ms(1000);
    }
    //pthread_exit(NULL);
    return NULL;
}
