
/** Network Communication Protocol **/



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

    
    /* to be implemented, the request message parser*/

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
