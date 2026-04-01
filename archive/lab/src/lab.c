#include "lab.h"


// Environment Setup Sanity Check.
void test_func(void){
    printf("Hi there, you shit!\n");
}


//____________ Pointer____________
// Demo
void pointer_basic_demo(void)
{
    int x = 5;

    int *p = &x;

    printf("x value = %d\n", x);
    printf("x addr  = %p\n", (void*)&x);

    printf("p value = %p\n", (void*)p);
    printf("*p value = %d\n", *p);
}

// Modify Value
void mod_val(int *x){
    printf("arrived val = %d\n", *x);
    *x = (((*x * 5)+4)*3) >> 2;
    printf("BEHOLD! a magic was done - check your value.\n");
}

// Swap
void swap_by_ref(int* x, int* y){
    printf("arrived (x,y) = (%d,%d)\n", *x, *y);
    int temp = *x;
    *x = *y;
    *y = temp;
}

// pointer arithmetic demo
void pointer_arith_demo(int arr[], size_t arr_size){
    printf("lets iterate over the array with no index!\n");
    for(size_t i = 0; i < arr_size; i++){
        printf("arr[%ld] = %d\n", i, *(arr+i));
    }
    printf("nice ha?\n");
}

// const & pointer
void pointer_and_const(void){
    // printf("lets test some const location and pointers..");
    // const int *x = 5; // same as int const *x
    // assert(*x == 5);
    // x = x+4;

    // int const *K = 5;
    // K = K+4;
}

// pointer 2 pointer
void pointer2pointer_demo(void){
    int *p;
    int **p_p;
    int var = 5;
    p = &var;
    p_p = &p;

    printf("var value = %d\n", var);
    printf("p value = %p\n", (void*)p);
    printf("*p value = %d\n", *p);
    printf("p_p value = %p\n", (void*)p_p);
    printf("*p_p value = %p\n", (void*)*p_p);
    printf("**p_p value = %d\n", **p_p);
}


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
void stack_vs_heap_demo(void){
    int a_stack = 10; // stack var
    int* a_heap = malloc(sizeof(int));
    *a_heap = 10;
    printf("stack var a_stack = %d, address = %p\n", a_stack, (void*)&a_stack);
    printf("heap var a_heap = %d, address = %p\n", *a_heap, (void*)a_heap);

    free(a_heap);
}

// malloc array
/**
returns NULL if n == 0
initialize values to 0
caller must free

Hint:

calloc exists
 */
int* create_int_array(size_t n){
    if(n == 0){
        return NULL;
    }
    int *arr = malloc(n*sizeof(int));
    if(arr == NULL){
        printf("malloc failed!\n");
        return NULL;
    }
    memset(arr, 0, n*sizeof(int));
    printf("dont forget to free the array after use!\n");
    return arr;
}

// realloc growth
/**
new elements initialized to 0
return NULL if failure
 */
int* grow_array(int *arr, size_t old_size, size_t new_size){
    if(arr ==NULL){
        printf("arr is NULL, cannot grow a non existing array\n");
        return NULL;
    }
    if (new_size == 0){
        free(arr);
        return NULL;
    }

    if (old_size == 0){
        return NULL;
    }

    int* ans = NULL;
    ans = realloc(arr, new_size*sizeof(int));
    if(ans == NULL){
        printf("realloc failed!\n");
        return NULL;
    }

    size_t size_diff = (new_size > old_size)? new_size - old_size : 0;
    if(size_diff){
        // Enter here only if new_size > old_size, so we need to initialize the new elements to 0
        memset(ans+old_size, 0, (size_diff)*sizeof(int));
    }

    return ans;
}


// string length
/**
NULL safe
do not use strlen
*/
size_t my_strlen(const char *s){
    if (s==NULL){
        printf("NULL string has length 0\n");
        return 0;
    }
    size_t len = 0;
    while(*(s+len)!='\0'){
        len++;
    }
    return len;
}


// safe string copy
/**
always null terminate
returns length of src
*/
size_t safe_str_copy(char *dst, size_t dst_size, const char *src){
    if(dst_size == 0) return (size_t)-1;
    if(src == NULL){
        printf("NULL source string, nothing to copy\n");
        if(dst_size>0){
            dst[0] = '\0';
        }
        return 0;
    }
    if (dst == NULL){
        printf("NULL destination string, cannot copy\n");
        return 0;
    }

    size_t src_size = my_strlen(src);
    if(src_size > dst_size){
        printf("warning: source string is larger than destination buffer, truncating copy\n");
    }
    size_t idx = 0;
    while(idx < src_size && idx < (dst_size-1)){
        *(dst + idx) = *(src + idx);
        idx++;
    }
    *(dst+idx) = '\0';
    return src_size;
}


// count char in string
/**
Example: count_char("banana",'a') → 3
*/
int count_char(const char *s, char c){
    if(s == NULL){
        printf("NULL string, cannot count chars\n");
        return 0;
    }
    size_t idx = 0;
    int count = 0;
    while(*(s + idx) != '\0'){
        if (*(s + idx) == c){
            count++;
        }
        idx++;
    }
    return count;
}


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

int write_exact(int fd, const void *buf, size_t n){
    if (n == 0){
        printf("nothing to write, returning success\n");
        return 0;
    }
    if(!buf){
        printf("NULL buffer, cannot write\n");
        return -1; // no buff.
    }

    int zero_write_cnt = 0;
    size_t written_bytes = 0;
    while(written_bytes < n ){
        ssize_t current_written = write(fd, (const unsigned char*)buf + (written_bytes), (n- written_bytes));
        if (current_written < 0) {
            printf("write error occurred\n");
            return -1;
        }
        else if (current_written == 0){
            zero_write_cnt++;
            if (zero_write_cnt == 3){
                printf("write() returned 0 three times, assuming write failure\n");
                return -1;
            }
        }
        written_bytes += current_written;
    }

    return 0;
}


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
int read_exact(int fd, void *buf, size_t n){
    if (n == 0){
        printf("nothing to read, returning success\n");
        return 0;
    }
    if (!buf){
        printf("NULL buffer, cannot read\n");
        return -1; // no buff.
    }

    size_t remaining_bytes = n;
    while(remaining_bytes > 0){
        ssize_t bytes_read = read(fd, (unsigned char*)buf + (n - remaining_bytes), remaining_bytes);
        if (bytes_read < 0){
            // err
            printf("read error occurred\n");
            return -1;
        }
        else if (bytes_read == 0){
            // EOF
            printf("premature EOF occurred\n");
            return -1;
        }
        remaining_bytes -= bytes_read;
    }

    return 0;

}
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
typedef struct msg_t{
    uint32_t msg_len;
    char* msg_payload;
} msg;


int pipe_fork_framed_demo(void){
    // creating a PIPE
    int fd[2];
    if(pipe(fd) == -1 ){
        perror("pipe failed");
        return -1;
    }
    // buffer
    // char *buf = (char*)malloc((MAX_MSG_SIZE)*sizeof(char));

    // preparing fork
    pid_t pid;
    pid = fork();

    if(pid < 0){
        // fork failed
        perror("fork failed");
        return -1;
    }
    else if(pid == 0){
        // child READER
        printf("i'm the Child, the \"READER\" and my pid is %d\n", getpid());
        close(fd[1]); // Close write-end

        msg msg_rx;
        msg_rx.msg_payload = (char*)malloc(MAX_MSG_SIZE * sizeof(char)); // static size

        if(msg_rx.msg_payload == NULL){
            printf("malloc failed in child, cannot read message\n");
            close(fd[0]);
            return -1;
        }

        ssize_t rc;
        while((rc = read(fd[0], &msg_rx.msg_len, sizeof(msg_rx.msg_len)))>0){
            if(msg_rx.msg_len > MAX_MSG_SIZE){
                printf("[WARN] Received message length %u exceeds buffer capacity, rejecting message\n", msg_rx.msg_len);
            }
            read_exact(fd[0], msg_rx.msg_payload, msg_rx.msg_len);
            msg_rx.msg_payload[msg_rx.msg_len] = '\0';
            printf("Child PID: %d, Parent PID: %d, Received Payload: \"%s\"\n", getpid(), getppid(), msg_rx.msg_payload);
        }

        if(rc == 0){
            // finished successfully
            printf("EOF reached, no more messages to read\n");
        }
        else{
            // got -1 - some failure..
            printf("read error occurred while reading message payload\n");
        }
        free(msg_rx.msg_payload);
        close(fd[0]);
    }
    else {
        // parent WRITER
        const char* msg_arr[] = {   "",
                                    "hello",
                                    "world",
                                    "this",
                                    "is",
                                    "a",
                                    "framed",
                                    "message",
                                    "now we'll test it",
                                    "now we'll test it even more",
                                    "EOF"
                                };
        printf("i'm the Parent and my pid is %d\n", getpid());
        printf("and my child's pis is %d\n", pid);
        close(fd[0]); // Close read-end
        size_t i = 0;
        ssize_t rc = 0;
        while(strcmp(msg_arr[i], "EOF") != 0 && rc == 0){
            uint32_t msg_len = (uint32_t)my_strlen(msg_arr[i]);
            printf("Parent sending message: \"%s\" with length %u\n", msg_arr[i], msg_len);
            if((write_exact(fd[1], &msg_len, sizeof(msg_len_type))) != 0){
                printf("write header failed\n");
                rc = -1;
                break;
            }
            if((write_exact(fd[1], msg_arr[i], 4 + msg_len)) != 0){
                printf("write payload failed\n");
                rc = -1;
                break;
            }
            i++;
            sleep(1);
        }

        if(rc<0 || rc > 0){
            // failure
            printf("write error occurred while writing message\n");
        }
        else{
            // success
        }

        close(fd[1]); // close write-end
        waitpid(pid, NULL, 0); // wait for child to finish
    }

    return 0;
}

// -----------------------------------------------------------------------------------------------

typedef struct thread_id_t{
    pthread_t id;
    bool is_free;
} thread_id;

typedef struct thread_pkg_t{
    pthread_mutex_t* p_mutex;
    bool* isReady;
    pthread_barrier_t* brr;
    int* counter;
    int* idx;
    thread_id* thread_lut;
} thread_pkg;



void* count_thread(void* pkg_shared){
    // unpacking the metadata and sata for each thread.
    if(pkg_shared == NULL){
        printf("Error: thread package is NULL for thread %lu\n", pthread_self());
        return NULL;
    }
    thread_pkg* pkg_in = (thread_pkg*)pkg_shared;
    int idx_local = *pkg_in->idx;
    free(pkg_in->idx);
    pthread_mutex_lock(pkg_in->p_mutex);
    printf("Hey there! i'm thread %lu\n", pthread_self());
    int* counter = pkg_in->counter;
    bool* is_ready = pkg_in->isReady;
    // registering the thread at the LUT.
    if(!pkg_in->thread_lut[idx_local].is_free){
        printf("Error: thread LUT index %d is not free for thread %lu\n", idx_local, pthread_self());
        pthread_mutex_unlock(pkg_in->p_mutex);
        free(pkg_in);
        return NULL;
    }
    pkg_in->thread_lut[idx_local].id = pthread_self();
    pkg_in->thread_lut[idx_local].is_free = false;

    pthread_mutex_unlock(pkg_in->p_mutex);

    // while(!(*is_ready)){
    //     sleep(2); // TODO: look for better waiting: condition variable / barrier / atomic flag - need to learn!
    // }

    pthread_barrier_wait(pkg_in->brr);

    bool run = true;
    while(run){
        pthread_mutex_lock(pkg_in->p_mutex);
        printf("thread [%lu] got cnt value = %d\n",pthread_self(), *counter);
        if (*counter < 100000){
            (*counter)++;
        }
        else{
            run = false;
        }

        pthread_mutex_unlock(pkg_in->p_mutex);
        // sleep(2);
    }
    free(pkg_in);
    return NULL;
}


void thread_race_cond_demo(int numOfThreads){
    thread_pkg* pkg_shared = (thread_pkg*)malloc(sizeof(thread_pkg));
    if(pkg_shared == NULL){
        // err
        return;
    }
    pkg_shared->p_mutex = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));
    if(pthread_mutex_init(pkg_shared->p_mutex, NULL) != 0 ){
        // err
        free(pkg_shared);
        return;
    }
    pkg_shared->brr = (pthread_barrier_t*)malloc(sizeof(pthread_barrier_t));
    if(pthread_barrier_init(pkg_shared->brr, NULL, numOfThreads) != 0){
        // err
        free(pkg_shared->p_mutex);
        free(pkg_shared);
        return;
    }

    pkg_shared->thread_lut = (thread_id*)malloc(sizeof(thread_id)*numOfThreads);
    if(pkg_shared->thread_lut == NULL){
        // err
        free(pkg_shared->p_mutex);
        free(pkg_shared);
        return;
    }
    pkg_shared->counter = (int*)malloc(sizeof(int));
    if(pkg_shared->counter == NULL){
        // err
        free(pkg_shared->thread_lut);
        free(pkg_shared->p_mutex);
        free(pkg_shared);
        return;
    }
    pkg_shared->isReady = (bool*)malloc(sizeof(bool));
    if(pkg_shared->isReady == NULL){
        // err
        free(pkg_shared->counter);
        free(pkg_shared->thread_lut);
        free(pkg_shared->p_mutex);
        free(pkg_shared);
        return;
    }
    *pkg_shared->counter = 0;
    *pkg_shared->isReady = false;
    pthread_t *threads = (pthread_t*)malloc(numOfThreads*sizeof(pthread_t));
    if(threads == NULL){
        // err
        free(pkg_shared->isReady);
        free(pkg_shared->counter);
        free(pkg_shared->thread_lut);
        free(pkg_shared->p_mutex);
        free(pkg_shared);
        return;
    }
    for(int i =0; i <numOfThreads; i++){
        thread_pkg* pkg = (thread_pkg*)malloc(sizeof(thread_pkg));
        pkg_shared->thread_lut[i].is_free = true;

        pkg->p_mutex = pkg_shared->p_mutex;
        pkg->counter = pkg_shared->counter;
        pkg->isReady = pkg_shared->isReady;
        pkg->brr = pkg_shared->brr;
        pkg->thread_lut = pkg_shared->thread_lut;
        pkg->idx = (int*)malloc(sizeof(int));
        *pkg->idx = i;
        pthread_create(&threads[i], NULL, count_thread, (void*)pkg);
        usleep(5000);
    }

    // set start flag
    *pkg_shared->isReady = true;

    // join threads
    for(int i =0; i <numOfThreads; i++){
        pthread_join(threads[i], NULL);
    }

    printf("All threads finished, final counter value = %d\n", *pkg_shared->counter);
    // print lut
    printf("thread LUT:\n");
    for(int i =0; i <numOfThreads; i++){
        printf("idx %d: thread id = %lu, is_free = %d\n", i, pkg_shared->thread_lut[i].id, pkg_shared->thread_lut[i].is_free);
    }


    free(pkg_shared->counter);
    free(pkg_shared->isReady);
    free(pkg_shared->thread_lut);
    pthread_mutex_destroy(pkg_shared->p_mutex);
    free(pkg_shared->p_mutex);
    free(pkg_shared);
    free(threads);

}

typedef struct sem_pkg_t{
    sem_t* sem;
    pthread_mutex_t* p_mutex;
    ssize_t* sec_to_wait;
} sem_pkg;

void* thread_func(void* arg){
    ssize_t num_of_crit_enter = 20;
    sem_pkg* pkg_shared = (sem_pkg*)arg;
    if(pkg_shared == NULL){
        printf("Error: thread package is NULL for thread %lu\n", pthread_self());
        return NULL;
    }
    ssize_t sec_to_wait_local = *(pkg_shared->sec_to_wait);
    printf("Hey There! here is thread [%lu] ready to run...\n", pthread_self());
    while(num_of_crit_enter > 0){
        printf("Thread %lu is waiting for a pass...\n", pthread_self());
        sem_wait(pkg_shared->sem);
        printf("Thread %lu got a pass!\n", pthread_self());
        pthread_mutex_lock(pkg_shared->p_mutex);
        *(pkg_shared->sec_to_wait) = sec_to_wait_local;
        pthread_mutex_unlock(pkg_shared->p_mutex);

        while(*(pkg_shared->sec_to_wait) > 0){
            pthread_mutex_lock(pkg_shared->p_mutex);
            printf("%lu Mississippi...\n", *(pkg_shared->sec_to_wait));
            pthread_mutex_unlock(pkg_shared->p_mutex);
            // sleep(1);
            usleep(50000);
            pthread_mutex_lock(pkg_shared->p_mutex);
            *(pkg_shared->sec_to_wait) = *(pkg_shared->sec_to_wait) - 1;
            pthread_mutex_unlock(pkg_shared->p_mutex);
        }
        num_of_crit_enter--;
        sem_post(pkg_shared->sem);
    }
    printf("Thread %lu is done with its critical section entries, exiting...\n", pthread_self());
    return NULL;
}

int semaphore_demo(void){
    pthread_t* threads = (pthread_t*)malloc(5*sizeof(pthread_t));
    // args to threads:
    sem_pkg* pkg_shared = (sem_pkg*)malloc(sizeof(sem_pkg));
    if(pkg_shared == NULL){
        // err
        free(threads);
        return -1;
    }
    pkg_shared->sem = (sem_t*)malloc(sizeof(sem_t));
    pkg_shared->sec_to_wait = (ssize_t*)malloc(sizeof(ssize_t));
    if(pkg_shared->sem == NULL || pkg_shared->sec_to_wait == NULL){
        // err
        free(pkg_shared->sem);
        free(pkg_shared->sec_to_wait);
        free(pkg_shared);
        free(threads);
        return -1;
    }

    pkg_shared->p_mutex = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));
    if(pthread_mutex_init(pkg_shared->p_mutex, NULL) != 0 ){
        // err
        free(pkg_shared->p_mutex);
        free(pkg_shared->sem);
        free(pkg_shared->sec_to_wait);
        free(pkg_shared);
        free(threads);
        return -1;
    }

    *pkg_shared->sec_to_wait = 3;
    sem_init(pkg_shared->sem, 0, 2); // initial value of 2, shared between threads of the same process.

    for(int i = 0; i< 5; i ++){
        pthread_create(&threads[i], NULL, thread_func, (void*)pkg_shared);
    }


    for(int i = 0; i< 5; i ++){
        pthread_join(threads[i], NULL);
    }

    return 0;
}
