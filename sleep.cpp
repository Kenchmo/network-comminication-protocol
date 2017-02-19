//
//  sleep.cpp
//  network communication protocol
//
//  Created by Enkhjargalyn Nyam-Ochir on 2/18/17.
//  Copyright © 2017 Lufan Zhou. All rights reserved.
//

#include "sleep.h"

void sleep_ms(int milliseconds) // cross-platform sleep function
{
#ifdef WIN32
    Sleep(milliseconds);
#elif _POSIX_C_SOURCE >= 199309L
    struct timespec ts;
    ts.tv_sec = milliseconds / 1000;
    ts.tv_nsec = (milliseconds % 1000) * 1000000;
    nanosleep(&ts, NULL);
#else
    usleep(milliseconds * 1000);
#endif
}
