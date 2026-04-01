// ring_buffer.h
#pragma once

#include <stddef.h>
#include <stdbool.h>
#include <semaphore.h>
#include <pthread.h>

#define MAX_MSG_LENGTH 256
#define STRUCT_DESTROYED -1
#define READ_WRITE_TIMEOUT 3

typedef struct sensor_msg_t{
    char payload[MAX_MSG_LENGTH];
} sensor_msg;

typedef enum OP_STATUS_E {
    FAIL = -1,
    SUCCESS = 0,
    INVALID,
    ERROR,
    EMPTY,
    TIMEOUT,
    UNKOWN_STATUS
} OP_STATUS;


typedef struct ring_buffer_t {
    sem_t sem_spaces;
    sem_t sem_items;
    pthread_mutex_t lock;
    size_t buff_size;
    sensor_msg* buff;
    ssize_t read_offset;
    ssize_t write_offset;
} rbuff;


OP_STATUS init_rbuff(rbuff* rbuff, size_t size);
OP_STATUS destroy_rbuff(rbuff* rbuff);

OP_STATUS rbuff_write(rbuff* rbuff, sensor_msg* data);
OP_STATUS rbuff_read(rbuff* rbuff, sensor_msg* data);

bool is_empty(rbuff* rbuff);
void print_messages(rbuff* rbuff);


