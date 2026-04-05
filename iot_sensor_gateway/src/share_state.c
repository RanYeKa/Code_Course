// share_state.c

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <float.h>

#include "share_state.h"
#include "logger.h"

int init_system_state(system_state* state){
    log_info("function called: %s", __func__);
    if(state == NULL){return -1;}

    if(pthread_mutex_init(&state->state_lock, NULL) != 0){
        return -1;
    }
    state->num_sensors = NUM_OF_SENSORS;
    state->sensors = (sensor*)malloc(state->num_sensors * sizeof(sensor));
    for(size_t i = 0; i < state->num_sensors; i++){
        state->sensors[i].sensor_id = SENSOR_SLOT_FREE;
        state->sensors[i].sensor_data.latest = 0;
        state->sensors[i].sensor_data.avg = 0;
        state->sensors[i].sensor_data.sample_counter = 0;
        state->sensors[i].sensor_data.max = 0;
        state->sensors[i].sensor_data.min = FLT_MAX;
    }

    return 0;
}


int destroy_system_state(system_state* state){
    log_info("function called: %s", __func__);

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


int register_sensor(system_state* state,  uint16_t device_id){
    // NOTE: this function used under the lock of its caller!
    log_info("function called: %s", __func__);
    if(state == NULL){
        return -1;
    }

    log_info("Attempting to register sensor: %d", device_id);
    int idx = is_sensor_exists(state, SENSOR_SLOT_FREE);
    log_dbg("Returned index: %d", idx);
    if(idx >= 0){
        // there is space
        state->sensors[idx].sensor_id = device_id;
        log_info("Sensor %d registered at index %d", device_id, idx);
        return 0;
    }

    // need to realloc

    log_info("No free slot found, need to realloc!");
    // increase space.
    if(realloc(state->sensors, (state->num_sensors + NUM_OF_SENSORS) * sizeof(sensor)) == NULL){
        // print err:
        log_err("Failed to reallocate memory for sensors");
        return -1;
    }

    // save the registered idx.
    size_t register_idx = state->num_sensors; // the first idx of the new batch.
    // update the count of sensors.
    state->num_sensors += NUM_OF_SENSORS;

    // init the new slots:
    for(size_t i = register_idx; i < state->num_sensors; i++){
        state->sensors[i].sensor_id = SENSOR_SLOT_FREE;
        state->sensors[i].sensor_data.latest = 0;
        state->sensors[i].sensor_data.avg = 0;
        state->sensors[i].sensor_data.sample_counter = 0;
        state->sensors[i].sensor_data.max = 0;
        state->sensors[i].sensor_data.min = FLT_MAX;
    }
    // register the new sensor in the first slot of the new batch.
    state->sensors[register_idx].sensor_id = device_id;
    log_info("Sensor %d registered at index %d", device_id, register_idx);
    return 0;
}

int delete_sensor(system_state* state, uint16_t device_id){
    // return the deleted idx, -1 otherwise.
    log_info("function called: %s", __func__);

    if(!state){
        return -1;
    }
    pthread_mutex_lock(&state->state_lock);

    int idx = is_sensor_exists(state, device_id);

    if(idx < 0){
        // the sensor is not in the list.
        pthread_mutex_unlock(&state->state_lock);
        return -1;
    }

    state->sensors[idx].sensor_id = SENSOR_SLOT_FREE;
    state->sensors[idx].sensor_data.latest = 0;
    state->sensors[idx].sensor_data.avg = 0;
    state->sensors[idx].sensor_data.max = 0;
    state->sensors[idx].sensor_data.min = 0;

    pthread_mutex_unlock(&state->state_lock);
    log_dbg("Sensor with device id %d deleted from index %d", device_id, idx);
    return idx;
}

int is_sensor_exists(system_state* state, uint16_t device_id){
    log_info("function called: %s", __func__);

    // return idx of the sensor, -1 else.
    // NOTE: this function used under the lock of its caller!
    if(!state || device_id < 1){
        return -1;
    }

    log_info("args are OK!");
    size_t i = 0;
    int rc = 0;
    while((i < state->num_sensors) && (state->sensors[i].sensor_id != device_id)){
        log_dbg("index %d out of %d was not a match, id is %d", (int)i, (int)state->num_sensors, state->sensors[i].sensor_id);

        i++;
    }
    log_info("search was done, rc is %d", rc);

    return (rc == 0)? (int)i: (-1);
}

void show_sensors(system_state* state){
    // return idx of the sensor, -1 else.
    log_info("function called: %s", __func__);

    if(!state){
        return;
    }
    pthread_mutex_lock(&state->state_lock);


    size_t i = 0;
    log_info("\n");
    log_info("-------------------------------------------------------");
    log_info("-----------------   Senors List   ---------------------");
    log_info("-------------------------------------------------------");
    while( i < state->num_sensors && state->sensors[i].sensor_id != SENSOR_SLOT_FREE){
        log_info("Sensor ID: %d, latest: %f, avg: %f, max: %f, min: %f",
                            state->sensors[i].sensor_id, state->sensors[i].sensor_data.latest,
                            state->sensors[i].sensor_data.avg, state->sensors[i].sensor_data.max,
                            state->sensors[i].sensor_data.min
                );
        i++;
    }
    pthread_mutex_unlock(&state->state_lock);

}
