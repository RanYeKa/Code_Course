#pragma once
#include "proto.h"

#define INVALID_SIZE 0xFFFFFFFF

typedef void (*msg_print_func)(const void *msg);

typedef enum{
    STORAGE_OK = 0, // Storage is functioning normally
    STORAGE_FULL,   // Storage is full and cannot accept new messages
    STORAGE_EMPTY,  // Storage is empty and there are no messages to pop
    STORAGE_ERROR   // An error occurred during storage operations
} storage_status_t;

typedef enum{
    OPERATION_SUCCESS = 0, // Operation completed successfully
    OPERATION_FAILURE     // Operation failed due to an error
} storage_operation_status_t;


typedef struct msg_elem{
    time_t storage_time; // Timestamp of when the message was stored
    sensor_msg_t sensor_msg; // type + payload
} msg_elem_t;

typedef struct msg_storage{
    size_t capacity; // Maximum number of messages the storage can hold
    size_t count; // Current number of messages stored
    size_t head; // Index of the oldest message (for circular buffer)
    size_t tail; // Index of the next position to store a new message
    unsigned long total_pushed; // Total number of messages pushed into the storage
    unsigned long total_popped; // Total number of messages popped from the storage
    storage_status_t status; // Current status of the storage
    msg_elem_t* storage; // Array of message elements (circular buffer)
} msg_storage_t;

// creation and deletion and health check
storage_status_t msg_storage_init(msg_storage_t* msg_storage, size_t capacity);
storage_status_t msg_storage_health_check(msg_storage_t* msg_storage);
void msg_storage_cleanup(msg_storage_t* msg_storage);
void msg_storage_delete(msg_storage_t* msg_storage);

// operations
storage_operation_status_t msg_storage_push(msg_storage_t* msg_storage, const sensor_msg_t* msg);
storage_operation_status_t msg_storage_pop(msg_storage_t* msg_storage, sensor_msg_t* msg);

// getters
size_t msg_storage_get_count(const msg_storage_t* msg_storage);
size_t msg_storage_get_capacity(const msg_storage_t* msg_storage);
unsigned long msg_storage_get_total_pushed(const msg_storage_t* msg_storage);
unsigned long msg_storage_get_total_popped(const msg_storage_t* msg_storage);
storage_status_t msg_storage_get_status(const msg_storage_t* msg_storage);
time_t msg_storage_get_head_time(const msg_storage_t* msg_storage);
time_t msg_storage_get_tail_time(const msg_storage_t* msg_storage);

// boolean operations
bool msg_storage_is_full(const msg_storage_t* msg_storage);
bool msg_storage_is_empty(const msg_storage_t* msg_storage);


// presentation functions
void msg_storage_print(const msg_storage_t* msg_storage, msg_print_func print_func);