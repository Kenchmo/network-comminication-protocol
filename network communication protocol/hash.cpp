//
//  hash.cpp
//  network communication protocol
//
//

#include "hash.h"


// These are two dummy hashing fuctions
//

int hash_func(void * hasharg, size_t datalen, int8_t hash_method){
    if(hash_method == HASHING1) return hashing1(hasharg, datalen);
    if(hash_method == HASHING2) return hashing2(hasharg, datalen);
    return 1;
}


int hashing1(void * hasharg, size_t datalen){
    int sum = 0;
    int m = 1024;
    for(int i = 0; i < datalen; i++){
        sum += ((int *)hasharg)[i];
        sum %= m;
    }
    return sum;
}

int hashing2(void * hasharg, size_t datalen){
    int pro = 0;
    int m = 1024;
    for(int i = 0; i < datalen / 2; i++){
        pro *= ((int *)hasharg)[i*2];
        pro %= m;
    }
    return pro;
}

