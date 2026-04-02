// daemonize.c
#include "daemonize.h"

pid_t daemonize_process(void) {
    pid_t pid = fork();

    // if(get_flag_state() != 0) reset_flag(0); // Initialize the flag to 0 before daemonizing
    // Link signals to callbacks
    link_sig_to_cb(CB_ARR, SIG_ARR, NUM_SIG);

    if (pid < 0) exit(EXIT_FAILURE);
    if (pid > 0) exit(EXIT_SUCCESS); // Parent exits, terminal returns to prompt

    // Child continues here
    if (setsid() < 0) exit(EXIT_FAILURE);

    // Resolve ~/temp/log.log path
    char log_path[256];
    const char* home = getenv("HOME");
    if (home == NULL) exit(EXIT_FAILURE);
    snprintf(log_path, sizeof(log_path), "%s/temp/gateway_sys.log", home);

    // Re-open with O_TRUNC to "clean" the file immediately
    int fd = open(log_path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd >= 0) {
        dup2(fd, STDOUT_FILENO); // Redirect stdout
        dup2(fd, STDERR_FILENO); // Redirect stderr
        close(fd);
    }

    // Fully detach stdin
    int dev_null = open("/dev/null", O_RDONLY);
    if (dev_null >= 0) {
        dup2(dev_null, STDIN_FILENO);
        close(dev_null);
    }
    // Disable buffering for stdout and stderr to ensure immediate log writes
    setvbuf(stdout, NULL, _IONBF, 0);
    return (pid = getpid());
    // send SIGINT to the Daemon
    // kill(getpid(), SIGINT); // This will trigger the signal handler to set the flag
}
