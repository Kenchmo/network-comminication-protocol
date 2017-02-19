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


#define LOCKED 1
#define UNLOCKED 0

#define NOTSET -1

//req_or_resp flag
#define REQ 0
#define RESP 1

// rtype and ttype
#define IDTYPE 1
#define HASHTYPE 2

// sender

#define C1  1
#define C2 2

//acknowledged or not
#define ACKED 1
#define UNACKED 0

//datatype

#define INTARR 1
#define CHARARR 2



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


