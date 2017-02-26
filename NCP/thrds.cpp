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

int8_t client1_connected = 0;
int8_t client2_connected = 0;
volatile int notify_c1 = 0;
volatile int notify_c2 = 0;

/* deleted server thread because the server is literally a msg box (as declared as this_server above)*/



/** client thread
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
                msgq->lock.lock();
                msgq->q.push_back((this_server.msg.top()));
                msgq->lock.unlock();
            }
            
            this_server.msg.top().acked = ACKED;
        }
        
        /* if there is no msg for me , see if I need to send msg */
        
        else{
            if(this_server.msg.empty() || (this_server.msg.top().acked == ACKED || this_server.msg.top().acked == NOTSET)){
                
                msgq->lock.lock();              //lock the msg queue
                
                if(msgq->q.size() != 0 && msgq->q[msgq->count].sender == C1){
                    this_server.msg.push(msgq->q[msgq->count]);
                    msgq->count++;
                }
                msgq->lock.unlock();
            }
        }
        this_server.lock.unlock();
    }
    //pthread_exit(NULL);
    return NULL;
}



/** client thread
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
                msgq->lock.lock();
                msgq->q.push_back((this_server.msg.top()));
                msgq->lock.unlock();
            }
            
            this_server.msg.top().acked = ACKED;
        }
        
        /* if there is no msg for me , see if I need to send msg */
        
        else{
            if(this_server.msg.empty() || (this_server.msg.top().acked == ACKED || this_server.msg.top().acked == NOTSET)){
                
                msgq->lock.lock();              //lock the msg queue
                if(msgq->q.size() != 0 && msgq->q[msgq->count].sender == C2){
                    this_server.msg.push(msgq->q[msgq->count]);
                    msgq->count++;
                }
                msgq->lock.unlock();
            }
        }
        this_server.lock.unlock();
    }
    //pthread_exit(NULL);
    return NULL;


}
/** user thread
  * INPUT: void * threadarg: the pointer to the msg queue
  * EFFECTS: 1. put the incoming requests to the msg queue
  *          2. deal the the requests from peer and put the answer to the msg queue
 **/

void *user1_thread(void *threadarg){
    Msgq *msgq = (Msgq *) threadarg;
    int solving_flag = 0;
    stack<Msg> msgtemp;
    
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
            msgtemp.top().sender = C1;
            msgtemp.top().req_or_resp = RESP;
            msgtemp.top().acked = UNACKED;
            if(msgtemp.top().type == IDTYPE){
                if(msgtemp.top().data) free(msgtemp.top().data);
                msgtemp.top().data = malloc(idlen);
                memcpy(msgtemp.top().data, id, idlen);
                msgtemp.top().datalen = idlen;
                msgtemp.top().datatype = INTARR;
            }
            else if(msgtemp.top().type == HASHTYPE){
                void * temp = malloc(msgtemp.top().datalen);
                memcpy(temp, msgtemp.top().data, msgtemp.top().datalen);
                free(msgtemp.top().data);
                *((int*)msgtemp.top().data) = hash_func(temp, msgtemp.top().datatype, msgtemp.top().hashtype);
                msgtemp.top().datalen = sizeof(int);
                msgtemp.top().datatype = INTARR;
            }
            msgq->lock.lock();
            msgq->q.push_back(msgtemp.top());
            msgq->lock.unlock();
            solving_flag = 0;
        }
        else{
            msgq->lock.lock();
            // if the queue front is a request from peer
            // copy the request to local
            if(msgq->q[msgq->count].req_or_resp == REQ && msgq->q[msgq->count].sender == C2){
                msgtemp.push(msgq->q[msgq->count]);
                msgq->count++;
                solving_flag = 1;
            }
            msgq->lock.unlock();
        }
        //sleep_ms(1000);
    }
    //pthread_exit(NULL);
    return NULL;
}


/** user thread
 * INPUT: void * threadarg: the pointer to the msg queue
 * EFFECTS: 1. put the incoming requests to the msg queue
 *          2. deal the the requests from peer and put the answer to the msg queue
 **/
void *user2_thread(void *threadarg){
    
    Msgq *msgq = (Msgq *) threadarg;
    int solving_flag = 0;
    stack<Msg> msgtemp;
    
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
            msgtemp.top().sender = C2;
            msgtemp.top().req_or_resp = RESP;
            msgtemp.top().acked = UNACKED;
            if(msgtemp.top().type == IDTYPE){
                if(msgtemp.top().data) free(msgtemp.top().data);
                msgtemp.top().data = malloc(idlen);
                memcpy(msgtemp.top().data, id, idlen);
                msgtemp.top().datalen = idlen;
                msgtemp.top().datatype = INTARR;
            }
            else if(msgtemp.top().type == HASHTYPE){
                void * temp = malloc(msgtemp.top().datalen);
                memcpy(temp, msgtemp.top().data, msgtemp.top().datalen);
                free(msgtemp.top().data);
                *((int*)msgtemp.top().data) = hash_func(temp, msgtemp.top().datatype, msgtemp.top().hashtype);
                msgtemp.top().datalen = sizeof(int);
                msgtemp.top().datatype = INTARR;
            }
            msgq->lock.lock();
            msgq->q.push_back(msgtemp.top());
            msgq->lock.unlock();
            solving_flag = 0;
        }
        else{
            msgq->lock.lock();
            // if the queue front is a request from peer
            // copy the request to local
            if(msgq->q[msgq->count].req_or_resp == REQ && msgq->q[msgq->count].sender == C1){
                msgtemp.push(msgq->q[msgq->count]);
                msgq->count++;
                solving_flag = 1;
            }
            msgq->lock.unlock();
        }
        //sleep_ms(1000);
    }
    //pthread_exit(NULL);
    return NULL;
}
