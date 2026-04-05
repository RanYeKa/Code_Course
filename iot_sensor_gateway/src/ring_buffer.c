// ring_buffer.c
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>

#include "ring_buffer.h"
#include "logger.h"

#define RED   "\033[0;31m"
#define BLUE  "\033[0;34m"
#define RESET "\033[0m"



OP_STATUS init_rbuff(rbuff* rbuff, size_t size, size_t type_size){
    if(rbuff == NULL || size == 0 || (type_size == 0)){
        return FAIL;
    }
    // init lock
    pthread_mutex_init(&rbuff->lock, NULL);

    // init semaphores
    sem_init(&rbuff->sem_items, 0, 0);
    sem_init(&rbuff->sem_spaces, 0, size);

    // init buffer
    rbuff->buff_size = size;
    rbuff->elem_size = type_size;
    rbuff->buff = malloc((size) * (type_size));
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

OP_STATUS rbuff_write(rbuff* rbuff, void* data){
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
    uint8_t *write_offset = ((uint8_t*)rbuff->buff) + (rbuff->write_offset*rbuff->elem_size);
    memcpy(write_offset, data, rbuff->elem_size);
    // 'offsetting'
    rbuff->write_offset = (rbuff->write_offset + 1) % rbuff->buff_size;

    pthread_mutex_unlock(&rbuff->lock);
    sem_post(&rbuff->sem_items); // increase items

    return SUCCESS;
}



OP_STATUS rbuff_read(rbuff* rbuff, void* data){
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
    uint8_t *read_offset = ((uint8_t*)rbuff->buff) + (rbuff->read_offset*rbuff->elem_size);
    memcpy(data, read_offset, rbuff->elem_size);
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
    return (rbuff->read_offset == rbuff->write_offset);
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

    ssize_t msg_it = rbuff->read_offset;
    size_t hex_it = 0;
    pthread_mutex_lock(&rbuff->lock);
    do{
        uint8_t* next_hexdump = ((uint8_t* )rbuff->buff) + (msg_it*rbuff->elem_size);
        while(hex_it < rbuff->elem_size){
            raw_print("%02X ", next_hexdump[hex_it]);
            hex_it++;
        }
        if(msg_it == rbuff->write_offset){
            break;
        }
        hex_it = 0;
        msg_it = (msg_it + 1) % rbuff->buff_size;
    }while(1);
    pthread_mutex_unlock(&rbuff->lock);

}

