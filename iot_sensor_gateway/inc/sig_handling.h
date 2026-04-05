// sig_handling.h
#pragma once

#include <signal.h>
#include <stddef.h> // for NULL


typedef void (cb_func_t)(int);
extern volatile sig_atomic_t g_run_flag;
extern volatile sig_atomic_t g_sig_received;




/**
TODO: create more handling functions:

Signal  |	Logic Category  |   Suggested Callback
SIGTERM |	Graceful Exit   |	set_stop_flag
SIGINT	|	Manual Stop     |	set_stop_flag
SIGHUP	|	Maintenance     |	reload_config
SIGUSR1	|	Custom          |	trigger_sensor_poll
SIGSEGV	|	Fatal Error	    |	log_and_die
SIGPIPE	|	Network	        |	SIG_IGN (Ignore)

*/

const char* sig_num_to_str(int sig);

int get_flag_state(void); // get current value
void set_flag(int sig); // set to 1
void reset_flag(int sig); // set to 0

extern int SIG_ARR[];
extern int NUM_SIG;
extern cb_func_t* CB_ARR[];

void link_sig_to_cb(cb_func_t* cb[], int* signals, int num_of_signals);