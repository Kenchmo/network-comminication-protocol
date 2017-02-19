//
//  thrds.h
//  network communication protocol
//
//  Created by Enkhjargalyn Nyam-Ochir on 2/18/17.
//  Copyright © 2017 Lufan Zhou. All rights reserved.
//

#ifndef thrds_h
#define thrds_h

#include <queue>
#include <iostream>
#include <iostream>
#include <cstdlib>
#include <string>
#include <pthread.h>
#include <queue>
#include <iostream>
#include <thread>


#include "spinlock.h"
#include "sleep.h"
using namespace std;



#define NOTSET -1

//req_or_resp flag
#define REQ 0
#define RESP 1

// rtype and ttype
#define IDTYPE 3
#define HASHTYPE 4

// sender

#define C1 5
#define C2 6

//acknowledged or not
#define ACKED 7
#define UNACKED 8

//datatype

#define INTARR 9
#define CHARARR 10



typedef struct msg{
    int8_t sender;
    int8_t req_or_resp;
    int8_t acked;
    int8_t type;
    void * data;
    size_t datalen;
    int8_t datatype;
} msg_t;


typedef struct msgq{
    queue<msg_t> q;
    spinlock lock;
} msgq_t;


typedef struct server{
    spinlock lock;
    msg_t msg;
}server_t;



void *server_thread(void *threadarg);
void *client1_thread(void *threadarg);
void *client2_thread(void *threadarg);
void *user1_thread(void *threadarg);
void *user2_thread(void *threadarg);


#endif /* thrds_h */


