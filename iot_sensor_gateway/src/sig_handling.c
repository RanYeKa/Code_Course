// sig_handling.c

#include "sig_handling.h"
#include <stdio.h>

volatile sig_atomic_t g_run_flag = 1;
volatile sig_atomic_t g_sig_received = 1;

int SIG_ARR[] = {
    SIGINT,
    SIGILL,
    SIGABRT,
    SIGFPE,
    SIGSEGV,
    SIGTERM
};
int NUM_SIG = sizeof(SIG_ARR) / sizeof(int);
cb_func_t* CB_ARR[] = { reset_flag, // SIGINT
                        reset_flag, // SIGILL
                        reset_flag, // SIGABRT
                        reset_flag, // SIGFPE
                        reset_flag, // SIGSEGV
                        reset_flag, // SIGTERM
}; // for signal handling callback functions



const char* sig_num_to_str(int sig){
    switch(sig){
        case SIGINT: return "SIGINT";
        case SIGILL: return "SIGILL";
        case SIGABRT: return "SIGABRT";
        case SIGFPE: return "SIGFPE";
        case SIGSEGV: return "SIGSEGV";
        case SIGTERM: return "SIGTERM";
        default: return "UNKNOWN";
    }
}



int get_flag_state(void){
    return  (int)g_run_flag;
}
void set_flag(int sig){
    g_sig_received = sig;
    g_run_flag = 1;
}
void reset_flag(int sig){
    g_sig_received = sig;
    g_run_flag = 0;
}


void link_sig_to_cb(cb_func_t* cb[], int* signals, int num_signals) {
    // Create the configuration struct
    struct sigaction sa;
    // set rules that apply to all signals
    sigemptyset(&sa.sa_mask); // No additional signals to block during handler execution
    sa.sa_flags = 0; // No special flags
    // Link each signal to its corresponding callback
    for (int i = 0; i < num_signals; i++) {
        sa.sa_handler = cb[i]; // Set the handler to the corresponding callback function
        sigaction(signals[i], &sa, NULL); // Register the handler for the signal
    }
}