#pragma once
// gateway.h

#include <pthread.h>
#include "ring_buffer.h"
#include "share_state.h"


#define BUFF_SIZE 2

typedef struct care_pkg_t {
    pthread_barrier_t barrier;
    rbuff rbuff;
    system_state system;
} care_pkg;


int gateway_run(void);