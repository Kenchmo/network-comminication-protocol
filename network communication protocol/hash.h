//
//  hash.h
//  network communication protocol
//
//

#ifndef hash_h
#define hash_h

#include <cstdlib>

//hashing type
#define HASHING1 1
#define HASHING2 2

int hash_func(void * hasharg, size_t datalen, int8_t hash_method);
int hashing1(void * hasharg, size_t datalen);
int hashing2(void * hasharg, size_t datalen);
#endif /* hash_h */
