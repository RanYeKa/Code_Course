#include "proto.h"
#include "msg_store.h"


// creation and deletion and health check
storage_status_t msg_storage_init(msg_storage_t* msg_storage, size_t capacity){
    if(msg_storage == NULL || capacity == 0){
        msg_storage = NULL;
        printf("Error: Invalid parameters for msg_storage_init\n");
        return STORAGE_ERROR;
    }
    msg_storage->capacity = capacity;
    msg_storage->count = 0;
    msg_storage->head = -1; // Using -1 to indicate that the storage is initially empty and there is no valid head index
    msg_storage->tail = -1; // Using -1 to indicate that the storage is initially empty and there is no valid tail index
    msg_storage->total_pushed = 0;
    msg_storage->total_popped = 0;
    msg_storage->status = STORAGE_OK;
    msg_storage->storage = (msg_elem_t*)malloc(capacity * sizeof(msg_elem_t));
    if(msg_storage->storage == NULL){
        free(msg_storage->storage);
        printf("Error: Memory allocation failed for msg_storage_init\n");
        return STORAGE_ERROR;
    }
    return STORAGE_EMPTY;
}

storage_status_t msg_storage_health_check(msg_storage_t* msg_storage){
    bool test = true;
    test &= (msg_storage->status != STORAGE_ERROR);
    test &= (msg_storage->storage != NULL);
    // storage_status_t ans = (test == false)? STORAGE_ERROR : STORAGE_OK;
    return (storage_status_t)((test == false)? STORAGE_ERROR : STORAGE_OK);
}

void msg_storage_cleanup(msg_storage_t* msg_storage){
    if (!msg_storage){
        printf("Error: Invalid parameter for msg_storage_cleanup\n");
        return;
    }
    // reinitialize the storage to reset all fields and clear the stored messages
    msg_storage->count = 0;
    msg_storage->head = -1; // Using -1 to indicate that the storage is initially empty and there is no valid head index
    msg_storage->tail = -1; // Using -1 to indicate that the storage is initially empty and there is no valid tail index
    msg_storage->total_pushed = 0;
    msg_storage->total_popped = 0;
    msg_storage->status = STORAGE_EMPTY;
    if(msg_storage->storage == NULL){
        free(msg_storage->storage);
        printf("Error: Memory allocation failed for msg_storage_init\n");
        msg_storage->status = STORAGE_ERROR;
        return;
    }
    // Clear the stored messages by setting the storage array to zero
    memset(msg_storage->storage, 0, msg_storage->capacity * sizeof(msg_elem_t));
}

void msg_storage_delete(msg_storage_t* msg_storage){
    if (!msg_storage) return;
    msg_storage->count = -1;
    msg_storage->head = -1;
    msg_storage->tail = -1;
    msg_storage->total_pushed = -1;
    msg_storage->total_popped = -1;
    msg_storage->status = STORAGE_ERROR;

    if (!msg_storage->storage) return;
    free(msg_storage->storage);
    msg_storage->storage = NULL;
}

// operations
storage_operation_status_t msg_storage_push(msg_storage_t* msg_storage, const sensor_msg_t* msg);  // TODO
storage_operation_status_t msg_storage_pop(msg_storage_t* msg_storage, sensor_msg_t* msg);  // TODO

// boolean operations
bool msg_storage_is_full(const msg_storage_t* msg_storage){
    if (!msg_storage){
        return false;
    }
    return (msg_storage->capacity == msg_storage->count);
}

bool msg_storage_is_empty(const msg_storage_t* msg_storage){
    if (!msg_storage){
        return false;
    }
    return (msg_storage->count == 0);
}

// getters
size_t msg_storage_get_count(const msg_storage_t* msg_storage){
    if (!msg_storage){
        return -1;
    }
    return msg_storage->count;
}

size_t msg_storage_get_capacity(const msg_storage_t* msg_storage){
    if (!msg_storage){
        return -1;
    }
    return msg_storage->capacity;
}

unsigned long msg_storage_get_total_pushed(const msg_storage_t* msg_storage){
    if (!msg_storage){
        return -1;
    }
    return msg_storage->total_pushed;
}
unsigned long msg_storage_get_total_popped(const msg_storage_t* msg_storage){
    if (!msg_storage){
        return -1;
    }
    return msg_storage->total_popped;
}

storage_status_t msg_storage_get_status(const msg_storage_t* msg_storage){
    if (!msg_storage){
        return STORAGE_ERROR;
    }
    return msg_storage->status;
}

time_t msg_storage_get_head_time(const msg_storage_t* msg_storage){
    if (!msg_storage || msg_storage->count == 0){
        return -1;
    }
    return msg_storage->storage[msg_storage->head].storage_time;
}

time_t msg_storage_get_tail_time(const msg_storage_t* msg_storage){
    if (!msg_storage || msg_storage->count == 0){
        return -1;
    }
    return msg_storage->storage[msg_storage->tail].storage_time;
}

// Presentation functions
void msg_storage_print(const msg_storage_t* msg_storage, msg_print_func print_func){
    // Note: printing func pointer is passed as an argument to support ANY type of message printing, not just sensor_msg_t.
    if(!msg_storage){
        printf("Error: Invalid parameter for msg_storage_print\n");
        return;
    }
    // print storage status and statistics
    printf("Storage Status: %d\n", msg_storage->status);
    printf("Storage Capacity: %zu\n", msg_storage->capacity);
    printf("Current Count: %zu\n", msg_storage->count);
    printf("Total Pushed: %lu\n", msg_storage->total_pushed);
    printf("Total Popped: %lu\n", msg_storage->total_popped);

    // print storage snapshot
    printf("Storage Snapshot:\n");
    if(msg_storage->count == 0){
        printf("Storage is empty.\n");
        return;
    }
    for(size_t i = 0; i < msg_storage->count; i++) {
        size_t index = (msg_storage->head + i) % msg_storage->capacity; // Calculate the actual index in the circular buffer
        printf("Message %ld:\n", i + 1);
        printf("Storage Time: %s", ctime(&msg_storage->storage[index].storage_time)); // ctime adds a newline character
        print_func(&(msg_storage->storage[index].sensor_msg)); // Use the provided function pointer to print the message
    }
}
