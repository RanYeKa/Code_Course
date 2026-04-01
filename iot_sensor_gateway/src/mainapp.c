// mainapp.c
#include <stdio.h>
#include <unistd.h>

#include "daemonize.h"
#include "gateway.h"

int main(void){
    daemonize_process();
    printf("***------------------------------------------------------------****\n");
    printf("To stop the loop, please type 'kill -SIGINT %d' in another terminal.\n", getpid());
    printf("you can alway use this: 'kill -INT \"$(pgrep main_app)\"' ");
    printf("***------------------------------------------------------------****\n");

    fflush(stdout); // FORCE the OS to write this to the file right now!
    gateway_run();


    printf("dummy mainapp finished after some SIG [%s] handling.", sig_num_to_str((int)g_sig_received));
    exit(EXIT_SUCCESS);
}