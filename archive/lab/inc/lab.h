#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <assert.h>
#include <sys/wait.h>
#include <stdint.h>

#include <pthread.h>
#include <semaphore.h>


// #include <errno.h>

#include "lab_proto.h"

#define MAX_MSG_SIZE 17
typedef uint32_t msg_len_type;


// Environment Setup Sanity Check.
void test_func(void);

//____________ Pointer____________
// Demo
void pointer_basic_demo(void);

// Modify Value
void mod_val(int *x);

// Swap
void swap_by_ref(int* x, int* y);

// pointer arithmetic demo
void pointer_arith_demo(int arr[], size_t arr_size);

// const & pointer
void pointer_and_const(void);

// pointer 2 pointer
void pointer2pointer_demo(void);


//____________ Dynamic Memory + Strings ____________

// Stack vs Heap
/**
Requirements:
create a stack variable int a = 10
allocate an int on heap
print both addresses
free the heap variable

Observe:
stack addresses near each other
heap addresses far away */
void stack_vs_heap_demo(void);

// malloc array
/**
returns NULL if n == 0
initialize values to 0
caller must free

Hint:

calloc exists
 */
int* create_int_array(size_t n);

// realloc growth
/**
new elements initialized to 0
return NULL if failure
 */
int* grow_array(int *arr, size_t old_size, size_t new_size);

// string length
/**
NULL safe
do not use strlen
*/
size_t my_strlen(const char *s);


// safe string copy
/**
always null terminate
returns length of src
*/
size_t safe_str_copy(char *dst, size_t dst_size, const char *src);


// count char in string
/**
Example: count_char("banana",'a') → 3
*/
int count_char(const char *s, char c);





//____________ Next Section — read(), write(), fork(), pipe() ____________

/*
Goal:
    Write exactly n bytes to a file descriptor.

Requirements:
    - Keep calling write() until all n bytes are written.
    - Return 0 on success.
    - Return -1 on error.
    - Be NULL-safe if buf is NULL and n > 0.

Constraints:
    - Use write().
    - Use pointer arithmetic or byte indexing.
*/
int write_exact(int fd, const void *buf, size_t n);


/*
Goal:
    Read exactly n bytes from a file descriptor.

Requirements:
    - Keep calling read() until exactly n bytes are read.
    - Return 0 on success.
    - Return -1 on error or premature EOF.
    - Be NULL-safe if buf is NULL and n > 0.

Constraints:
    - Use read().
    - Handle short reads correctly.
*/
int read_exact(int fd, void *buf, size_t n);

/*
Goal:
    Build one focused demo that combines:
    fork(), pipe(), exact I/O, framing, and EOF behavior.

Scenario:
    - Parent creates a pipe and forks.
    - Parent acts as WRITER.
    - Child acts as READER.

Requirements:
    1. Create a pipe:
         fd[0] = read end
         fd[1] = write end

    2. Call fork().

    3. In the parent:
         - Close the unused read end.
         - Send exactly one framed message into the pipe:
               [uint32_t payload_length][payload bytes]
         - Use write_exact() for both:
               - the 4-byte length
               - the payload
         - Close the write end after sending.
         - Wait for the child with waitpid().

    4. In the child:
         - Close the unused write end.
         - Read exactly 4 bytes as the payload length.
         - Validate the length against a fixed buffer capacity.
         - Read exactly payload_length bytes into a buffer.
         - Add terminating '\0' locally after reading.
         - Print:
               - child PID
               - parent PID
               - received payload
         - Then attempt one more read() from the pipe
           and verify that EOF is reached (read() == 0).
         - Close the read end before exiting.

    5. Demonstrate process identity:
         - Parent prints its PID and child PID.
         - Child prints its own PID and parent PID.

    6. Error handling:
         - Return -1 on any failure.
         - Return 0 on success.

Constraints:
    - Use only:
         pipe(), fork(), read(), write(), close(), waitpid(), getpid(), getppid()
    - Use read_exact() / write_exact() for framed transfer.
    - Do not rely on one read() or one write() completing everything.
    - Payload is sent without the terminating '\0'.
    - Use uint32_t for frame length.
    - Keep buffer fixed-size in the child.
    - Reject oversized payload length.
    - No stdio buffering for the pipe transfer itself.

What this drill is meant to prove:
    - You understand fork() branching.
    - You understand descriptor inheritance.
    - You know which pipe ends each process must close.
    - You understand that a pipe is a byte stream.
    - You know why framing is needed.
    - You understand that EOF is observed only after write ends are closed.
*/


int pipe_fork_framed_demo(void);

void thread_race_cond_demo(int numOfThreads);

/*
Goal:
    Limit concurrency using semaphore.

Requirements:
    - Create 5 threads.
    - Only allow 2 threads at a time to run "critical section".
    - Print when thread enters/exits.

Expected:
    - At most 2 threads inside at once.

Constraints:
    - Use sem_t
*/

void* thread_func(void* arg);

int semaphore_demo(void);


//____________ Pointer ____________
//____________ Pointer ____________
//____________ Pointer ____________
//____________ Pointer ____________
//____________ Pointer ____________
//____________ Pointer ____________