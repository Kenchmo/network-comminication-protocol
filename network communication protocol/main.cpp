#include "thrds.h"
#include <iostream>
#include <cstdlib>
#include <string>
#include <signal.h>
#include <pthread.h>
#include <vector>

using namespace std;

pthread_t server_trd;
pthread_t client1_trd, client2_trd;
pthread_t user1_trd, user2_trd;
server_t this_server;
msgq_t * msgq1, * msgq2;

int main () {
    msgq1 = new msgq_t;
    msgq2 = new msgq_t;
    msg_t tempmsg;
    int * hash1 = new int(12345678);
    int * hash2 = new int(87654321);
    
    tempmsg.sender = C1;
    tempmsg.req_or_resp = REQ;
    tempmsg.acked = UNACKED;
    tempmsg.reqmsg.ttype = IDTYPE;
    tempmsg.reqmsg.datalen = 1;

    
    for(int i = 0; i < 10; i++){
        msgq1->v.push_back(tempmsg);
    }
    
    tempmsg.sender = C1;
    tempmsg.req_or_resp = REQ;
    tempmsg.acked = UNACKED;
    tempmsg.reqmsg.ttype = HASHTYPE;
    tempmsg.reqmsg.datalen = 1;
    tempmsg.reqmsg.data = hash1;
    
    for(int i = 0; i < 3; i++){
        msgq1->v.push_back(tempmsg);
    }
    
    tempmsg.sender = C2;
    tempmsg.req_or_resp = REQ;
    tempmsg.acked = UNACKED;
    tempmsg.reqmsg.ttype = IDTYPE;
    tempmsg.reqmsg.datalen = 1;
    
    
    for(int i = 0; i < 10; i++){
        msgq2->v.push_back(tempmsg);
    }
    
    tempmsg.sender = C2;
    tempmsg.req_or_resp = REQ;
    tempmsg.acked = UNACKED;
    tempmsg.reqmsg.ttype = HASHTYPE;
    tempmsg.reqmsg.datalen = 1;
    tempmsg.reqmsg.data = hash2;
    
    for(int i = 0; i < 3; i++){
        msgq2->v.push_back(tempmsg);
    }
    

    int rc;
    
    cout <<"main() : creating server thread " << endl;
    rc = pthread_create(&server_trd, NULL, server_thread, (void *)NULL);
    if (rc){
        cout << "Error: unable to create thread," << rc << endl;
        exit(-1);
    }
    
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
