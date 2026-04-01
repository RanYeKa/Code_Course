// share_state.h
#pragma once

#include <pthread.h>
#include <signal.h>

#define NUM_OF_SENSORS 4
#define MAX_SENSOR_NAME_LEN 32

typedef struct sensor_data_stats_t{
    float latest;
    float avg;
    float sample_counter;
    float max;
    float min;
    // TBD: expected, range, etc..
} sensor_data_stats;

typedef struct sensor_t{
    char name[MAX_SENSOR_NAME_LEN];
    sensor_data_stats sensor_data;
    // TBD: expected, range, etc..
} sensor;

typedef struct system_state_t{
    // manage and store the latest data and stats for each sensor type.
    pthread_mutex_t state_lock;
    time_t time_stamp;
    size_t num_sensors;
    sensor* sensors; // registration table
} system_state;


int init_system_state(system_state* state);
int destroy_system_state(system_state* state);

int register_sensor(system_state* state, const char* sensor_name);
int delete_sensor(system_state* state, const char* sensor_name);
int is_sensor_exists(system_state* state, const char* sensor_name);
void show_sensors(system_state* state);

