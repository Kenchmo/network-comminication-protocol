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



class Msg{
public:
    int8_t sender;
    int8_t req_or_resp;
    int8_t acked;
    int8_t type;
    void * data;
    size_t datalen;
    int8_t datatype;
    
    Msg(){
        sender = NOTSET;
        req_or_resp = NOTSET;
        acked = NOTSET;
        type = NOTSET;
        data = NULL;
        datalen = 0;
        datatype = NOTSET;
    };
    ~Msg(){
        if(data != NULL) free(data);
        data = NULL;
    };
    
    Msg & operator= (const Msg & other){
        sender = other.sender;
        req_or_resp = other.req_or_resp;
        acked = other.acked;
        type = other.type;
        if(data != NULL){
            free(data);
            data = NULL;
        }
        if(other.data != NULL){
            data = malloc(other.datalen);
            datalen = other.datalen;
            memcpy(data, other.data, datalen);
            datatype = other.datatype;
        }
        else{
            data = NULL;
            datalen = 0;
            datatype = NOTSET;
        }
        return *this;
    };
    
    void print_data(){
        if(datatype == INTARR){
            for(int i = 0; i < datalen / sizeof(int); i++)
                cout<<((int*)data)[i]<<" ";
            cout<<endl;
        }
        if(datatype == CHARARR){
            for(int i = 0; i < datalen / sizeof(char); i++)
                cout<<((char*)data)[i]<<" ";
            cout<<endl;
        }
    };
};


class Msgq{
public:
    queue<Msg> q;
    spinlock lock;
};


class Server_class{
public:
    
    spinlock lock;
    Msg msg;
    
    Server_class(){};
    ~Server_class(){
        if(msg.data != NULL) free(msg.data);
    };
    Server_class & operator= (const Server_class & other){
        msg = other.msg;
        return *this;
    };
    

};



void *server_thread(void *threadarg);
void *client1_thread(void *threadarg);
void *client2_thread(void *threadarg);
void *user1_thread(void *threadarg);
void *user2_thread(void *threadarg);


#endif /* thrds_h */


