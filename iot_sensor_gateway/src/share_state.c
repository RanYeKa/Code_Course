// share_state.c

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <float.h>

#include "share_state.h"


int init_system_state(system_state* state){
    printf("function called: %s\n", __func__);
    if(state == NULL){return -1;}

    if(pthread_mutex_init(&state->state_lock, NULL) != 0){
        return -1;
    }
    state->num_sensors = NUM_OF_SENSORS;
    state->sensors = (sensor*)malloc(state->num_sensors * sizeof(sensor));
    for(size_t i = 0; i < state->num_sensors; i++){
        strcpy(state->sensors[i].name,"free");
        state->sensors[i].sensor_data.latest = 0;
        state->sensors[i].sensor_data.avg = 0;
        state->sensors[i].sensor_data.sample_counter = 0;
        state->sensors[i].sensor_data.max = 0;
        state->sensors[i].sensor_data.min = FLT_MAX;
    }

    return 0;
}


int destroy_system_state(system_state* state){
    printf("function called: %s\n", __func__);

    if(state == NULL){
        // nothing to do.
        return 0;
    }

    pthread_mutex_lock(&state->state_lock);
    free(state->sensors);
    pthread_mutex_unlock(&state->state_lock);
    pthread_mutex_destroy(&state->state_lock);

    return 0;
}


int register_sensor(system_state* state, const char* sensor_name){
    // NOTE: this function used under the lock of its caller!
    printf("function called: %s\n", __func__);
    if(state == NULL){
        return -1;
    }

    printf("Attempting to register sensor: %s\n", sensor_name);
    int idx = is_sensor_exists(state, "free");
    printf("Returned index: %d\n", idx);
    if(idx >= 0){
        // there is space
        strcpy(state->sensors[idx].name, sensor_name);
        state->sensors[idx].name[MAX_SENSOR_NAME_LEN - 1] = '\0'; // ensure null termination
        printf("Sensor %s registered at index %d\n", sensor_name, idx);
        return 0;
    }

    printf("No free slot found, need to realloc!\n");
    // need to realloc
    if(realloc(state->sensors, (state->num_sensors + NUM_OF_SENSORS) * sizeof(sensor)) == NULL){
        // print err:
        fprintf(stderr, "Failed to reallocate memory for sensors\n");
        return -1;
    }
    strcpy(state->sensors[state->num_sensors].name, sensor_name);
    state->sensors[idx].name[MAX_SENSOR_NAME_LEN - 1] = '\0'; // ensure null termination

    state->num_sensors += NUM_OF_SENSORS;

    printf("Sensor %s registered at index %d\n", sensor_name, (int)state->num_sensors);
    return 0;
}

int delete_sensor(system_state* state, const char* sensor_name){
    // return the deleted idx, -1 otherwise.
    printf("function called: %s\n", __func__);

    if(!state){
        return -1;
    }
    pthread_mutex_lock(&state->state_lock);

    int idx = is_sensor_exists(state, sensor_name);

    if(idx < 0){
        // the sensor is not in the list.
        pthread_mutex_unlock(&state->state_lock);
        return -1;
    }

    strcpy(state->sensors[idx].name, "free");
    state->sensors[idx].sensor_data.latest = 0;
    state->sensors[idx].sensor_data.avg = 0;
    state->sensors[idx].sensor_data.max = 0;
    state->sensors[idx].sensor_data.min = 0;

    pthread_mutex_unlock(&state->state_lock);
    return idx;
}

int is_sensor_exists(system_state* state, const char* sensor_name){
    printf("function called: %s\n", __func__);

    // return idx of the sensor, -1 else.
    // NOTE: this function used under the lock of its caller!
    if(!state || !sensor_name){
        return -1;
    }
    printf("arg are OK!\n");
    size_t i = 0;
    int rc = 0;
    while((i < state->num_sensors) && ((rc = strcmp(state->sensors[i].name, sensor_name)) != 0)){
        printf("index %d out of  %d was not a match, name was %s\n", (int)i, (int)state->num_sensors, state->sensors[i].name);

        i++;
    }
    printf("search was done, rc was %d\n", rc);

    return (rc == 0)? (int)i: (-1);
}

void show_sensors(system_state* state){
    // return idx of the sensor, -1 else.
    printf("function called: %s\n", __func__);

    if(!state){
        return;
    }
    pthread_mutex_lock(&state->state_lock);


    size_t i = 0;
    printf("\n");
    printf("-------------------------------------------------------\n");
    printf("-----------------   Senors List   ---------------------\n");
    printf("-------------------------------------------------------\n");
    while( i < state->num_sensors && !strcmp(state->sensors[i].name, "free")){ // print only registered sensors
        printf("Sensor [%zu/%zu] : %s", i, state->num_sensors, state->sensors[i].name);
        i++;
    }
    pthread_mutex_unlock(&state->state_lock);

}
