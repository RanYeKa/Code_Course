// ring_buffer.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "ring_buffer.h"
#include "logger.h"

#define RED   "\033[0;31m"
#define BLUE  "\033[0;34m"
#define RESET "\033[0m"


OP_STATUS init_rbuff(rbuff* rbuff, size_t size){
    if(rbuff == NULL || size == 0){
        return FAIL;
    }
    // init lock
    pthread_mutex_init(&rbuff->lock, NULL);

    // init semaphores
    sem_init(&rbuff->sem_items, 0, 0);
    sem_init(&rbuff->sem_spaces, 0, size);

    // init buffer
    rbuff->buff_size = size;
    rbuff->buff = (sensor_msg*)malloc(size*sizeof(sensor_msg));
    if(rbuff->buff == NULL) return FAIL;
    rbuff->read_offset = 0;
    rbuff->write_offset = 0;

    return SUCCESS;
}

OP_STATUS destroy_rbuff(rbuff* rbuff){
    if(rbuff == NULL){
        return SUCCESS;
    }
    // delete mutex
    pthread_mutex_destroy(&rbuff->lock);

    // delete semaphores
    sem_destroy(&rbuff->sem_items);
    sem_destroy(&rbuff->sem_spaces);

    // free memory
    if(rbuff->buff != NULL) free(rbuff->buff);
    rbuff->buff = NULL; // protect from misuse after deletion.
    rbuff->read_offset = (ssize_t)STRUCT_DESTROYED;
    rbuff->write_offset = (ssize_t)STRUCT_DESTROYED;

    return SUCCESS;
}

OP_STATUS rbuff_write(rbuff* rbuff, sensor_msg* data){
    log_info("[DBG] Attempting to write data to ring buffer...");
    if(rbuff == NULL || rbuff->buff == NULL || data == NULL){
        return FAIL;
    }

    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_sec += READ_WRITE_TIMEOUT;
    int rc = sem_timedwait(&rbuff->sem_spaces, &ts); // wait for free slots (decrease slots)
    if(rc == -1){
        return TIMEOUT;
    }

    pthread_mutex_lock(&rbuff->lock); // lock when free;

    // write to the correct offset.
    memcpy((rbuff->buff + rbuff->write_offset), data, sizeof(sensor_msg));
    // 'offsetting'
    rbuff->write_offset = (rbuff->write_offset + 1) % rbuff->buff_size;

    pthread_mutex_unlock(&rbuff->lock);
    sem_post(&rbuff->sem_items); // increase items

    return SUCCESS;
}



OP_STATUS rbuff_read(rbuff* rbuff, sensor_msg* data){
    log_info("[DBG] Attempting to read data from ring buffer...");
    if(rbuff == NULL || rbuff->buff == NULL){
        return FAIL;
    }

    if(data == NULL){
        return FAIL; // nothing to write to.
    }

    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_sec += READ_WRITE_TIMEOUT;
    log_info("[DBG] Waiting for data...");
    int rc = sem_timedwait(&rbuff->sem_items, &ts); // wait for items (decrease items)
    log_info("[DBG] Semwait returned with code %d", rc);
    if(rc == -1){
        log_err("[ERR - %s] Timeout while waiting for data.", __func__);
        return TIMEOUT;
    }

    pthread_mutex_lock(&rbuff->lock);

    // reading
    memcpy(data, (rbuff->buff + rbuff->read_offset), sizeof(sensor_msg));
    // 'offsetting'
    rbuff->read_offset = (rbuff->read_offset + 1) % rbuff->buff_size;

    pthread_mutex_unlock(&rbuff->lock);
    sem_post(&rbuff->sem_spaces);

    return SUCCESS;
}

bool is_empty(rbuff* rbuff){
    if(rbuff == NULL){
        log_err("Ring buffer is NULL, cannot check if empty.");
        return false;
    }
    return (rbuff->read_offset == rbuff->write_offset && rbuff->read_offset == -1);
}

void print_messages(rbuff* rbuff){
    if(rbuff == NULL) {
        // nothing to print
        log_err("Ring buffer is NULL, nothing to print.");
        return;
    }

    if(is_empty(rbuff) || rbuff->buff==NULL){
        // no data
        log_info("Ring buffer is empty, nothing to print.");
        return;
    }

    ssize_t it = rbuff->read_offset;
    pthread_mutex_lock(&rbuff->lock);
    do{
        log_info("Message at [%ld] is [%s] ", it, (rbuff->buff+it)->payload);
        if(it == rbuff->write_offset){
            break;
        }
        it = (it + 1) % rbuff->buff_size;
    }while(1);
    pthread_mutex_unlock(&rbuff->lock);


}

