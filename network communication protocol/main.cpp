
/** Network Communication Protocol
 *
 *
 *
 *
 *
 *
 **/


#include "thrds.h"
#include <iostream>
#include <cstdlib>
#include <string>
#include <signal.h>
#include <pthread.h>
#include <vector>

using namespace std;

//pthread_t server_trd;
pthread_t client1_trd, client2_trd;
pthread_t user1_trd, user2_trd;

// the server is just a message box
Server_class this_server;

Msgq * msgq1, * msgq2;

int main () {
    msgq1 = new Msgq;
    msgq2 = new Msgq;
    Msg tempmsg1, tempmsg2;// tempmsg3, tempmsg4;
    Msg tempmsg;

    /* the trivial inputs to make the code run in the fisrt request and response round*/
    tempmsg1.sender = C1;
    tempmsg1.req_or_resp = REQ;
    tempmsg1.acked = UNACKED;
    tempmsg1.datatype = INTARR;
    tempmsg1.type = IDTYPE;
    tempmsg1.data = NULL;
    
    tempmsg2.sender = C1;
    tempmsg2.req_or_resp = REQ;
    tempmsg2.acked = UNACKED;
    tempmsg2.datatype = INTARR;
    tempmsg2.type = HASHTYPE;
    tempmsg2.data = malloc(sizeof(int));
    tempmsg2.datalen = sizeof(int);
    
    msgq1->q.push_back(tempmsg1);
    msgq1->q.push_back(tempmsg2);
    //msgq1->q.push(&tempmsg3);
    //msgq1->q.push(&tempmsg4);
    
//    tempmsg.sender = C1;
//    tempmsg.req_or_resp = REQ;
//    tempmsg.acked = UNACKED;
//    tempmsg.datatype = INTARR;
//    tempmsg.type = HASHTYPE;
//    tempmsg.data = malloc(10*sizeof(int));
//    for(int i = 0; i < 10; i++)
//        ((int*)tempmsg.data)[i] = i;
//    tempmsg.datalen = 10;
//    msgq1->q.push(tempmsg);
    
    tempmsg.sender = C2;
    tempmsg.req_or_resp = REQ;
    tempmsg.datatype = INTARR;
    tempmsg.acked = UNACKED;
    tempmsg.type = IDTYPE;
    tempmsg.data = NULL;
    tempmsg.hashtype = HASHING2;
    msgq2->q.push_back(tempmsg);
    


    int rc;
    

    
    /*network client threads */
    
    //sleep(2);
    cout <<"main() : creating client1 thread " << endl;
    rc = pthread_create(&client1_trd, NULL, client1_thread, (void *)msgq1);
    if (rc){
        cout << "Error: unable to create thread," << rc << endl;
        exit(-1);
    }

    cout <<"main() : creating client2 thread " << endl;
    rc = pthread_create(&client2_trd, NULL, client2_thread, (void *)msgq2);
    if (rc){
        cout << "Error: unable to create thread," << rc << endl;
        exit(-1);
    }
    
    /* human threads
     * the human threads are supposed to take in the input requests and put them on the msg queue
     * this code is highly unstable, so the trivial inputs are given at the begining of the main function 
     * To be implemented: in order to simulate real time input from human, the human thread needs to read from
     * a request sending schedule, and set up a clock, putting the requst to msgq according to the clock.
     */
    
    cout <<"main() : creating user1 thread " << endl;
    rc = pthread_create(&user1_trd, NULL, user1_thread, (void *)msgq1);
    if (rc){
        cout << "Error: unable to create thread," << rc << endl;
        exit(-1);
    }
    
    cout <<"main() : creating user2 thread " << endl;
    rc = pthread_create(&user2_trd, NULL, user2_thread, (void *)msgq2);
    if (rc){
        cout << "Error: unable to create thread," << rc << endl;
        exit(-1);
    }
    
    
    
    
    pthread_exit(NULL);
}
