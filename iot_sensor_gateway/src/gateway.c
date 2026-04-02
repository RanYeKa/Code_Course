// gateway.c
#include "gateway.h"


#include "daemonize.h" // needed for g_run_flag
#include "processing.h" // needed for thread func
#include "network.h" // needed for thread func
#include "logger.h"

int gateway_run(void){
    log_info("function called: %s", __func__);

    // alocation
    care_pkg* ctx = (care_pkg*)malloc(sizeof(care_pkg));
    if(!ctx) return -1;

    // init
    pthread_barrier_init(&ctx->barrier, NULL, 3);
    if (init_rbuff(&ctx->rbuff, (size_t)BUFF_SIZE) != SUCCESS) return -1;
    if (init_system_state(&ctx->system) != SUCCESS) return -1;

    pthread_t threads[2];

    pthread_create(&threads[0], NULL, worker_thread, ctx);
    pthread_create(&threads[1], NULL, udp_listener_thread, ctx);
    log_dbg("Threads created, waiting at the barrier...");
    pthread_barrier_wait(&ctx->barrier);
    while(g_run_flag){
        sleep(1);
    }

    // Wait for the threads to naturally exit their loops
    log_dbg("Main loop exiting, waiting for threads to finish...");
    pthread_join(threads[0], NULL);
    pthread_join(threads[1], NULL);

    free(ctx);
    return 0;

}


