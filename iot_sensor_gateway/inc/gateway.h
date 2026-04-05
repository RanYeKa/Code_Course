#pragma once
// gateway.h

#include <pthread.h>
#include "ring_buffer.h"
#include "share_state.h"

#define BUFF_ELEM_TYPE sensor_packet_t // NOTE: change here for other kind of buffer.
#define BUFF_SIZE 10

typedef struct care_pkg_ {
    pthread_barrier_t barrier; // for threads to start together.
    rbuff rbuff; // struct to hold data to be written / read and processed.
    system_state system; // contain the system params
} care_pkg_t;


int gateway_run(void);