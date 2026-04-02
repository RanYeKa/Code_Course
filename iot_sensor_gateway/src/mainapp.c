// mainapp.c
#include <stdio.h>
#include <unistd.h>

#include "daemonize.h"
#include "gateway.h"
#include "logger.h"

#define LOG_DIR_PATH "/home/ranyeh/EmbeddedCenter/Code_Course/iot_sensor_gateway/logs/"
#define LOG_FILE_PATH LOG_DIR_PATH "gateway_app.log"

int main(void){
    daemonize_process();
    logger_init(LOG_FILE_PATH);
    log_info("***--------------------------------------------------------------***");
    log_info("*** To stop the loop, please type 'kill -SIGINT %d' in another terminal.", getpid());
    log_info("*** you can alway use this: 'kill -INT \"$(pgrep main_app)\"' ***");
    log_info("***------------------------------------------------------------***\n");

    gateway_run();


    log_info("dummy mainapp finished after some SIG [%s] handling.", sig_num_to_str((int)g_sig_received));
    exit(EXIT_SUCCESS);
}