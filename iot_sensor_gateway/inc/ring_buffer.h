// ring_buffer.h
#pragma once

#include <stddef.h>
#include <stdbool.h>
#include <semaphore.h>
#include <pthread.h>

#define MALLOC_BUFFER(type, count) (type *)malloc((count) * sizeof(type))

#define STRUCT_DESTROYED -1
#define READ_WRITE_TIMEOUT 3


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
    size_t elem_size;
    void* buff;
    ssize_t read_offset;
    ssize_t write_offset;
} rbuff;


OP_STATUS init_rbuff(rbuff* rbuff, size_t size, size_t type_size);
OP_STATUS destroy_rbuff(rbuff* rbuff);

OP_STATUS rbuff_write(rbuff* rbuff, void* data);
OP_STATUS rbuff_read(rbuff* rbuff, void* data);

bool is_empty(rbuff* rbuff);
void print_messages(rbuff* rbuff);


