// rpocessing.c
#include "processing.h"
#include "gateway.h"
#include "sig_handling.h"
#include "logger.h"
#include "protocol_sensor.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>


void* worker_thread(void* arg){
    log_info("function called: %s", __func__);

    care_pkg_t* ctx = (care_pkg_t*)arg;
    BUFF_ELEM_TYPE msg;

    pthread_barrier_wait(&ctx->barrier);
    log_info("Worker booting up...");

    // BUFF_ELEM_TYPE
    protocol_error_t msg_status;
    uint8_t data_read[sizeof(BUFF_ELEM_TYPE)] = {0};
    while(g_run_flag){
        if(rbuff_read(&ctx->rbuff, data_read) == SUCCESS){
            // log_info("Worker and just read the message: %s", data_read->payload);
            // msg_parser(&data_read, &ctx->system);
            if((msg_status = protocol_parse_msg(data_read, sizeof(BUFF_ELEM_TYPE), &msg)) != PROTOCOL_SUCCESS){
                // handle failed read
                log_warn("Failed to parse message with error code: %d. Ignoring this message.", msg_status);
                continue;
            }
            // process arrived data: msg, arg{system state}
            data_processor(&ctx->system, &msg);
        }

        memset(data_read, 0, sizeof(BUFF_ELEM_TYPE)); // Clear the buffer for the next read
    }


    log_info("Worker was killed...exiting thread.");

    return (void*)ctx; // placeholder
}


/**
    Receive the system state and sensor packet and the follow:
    - check if sensor id exists - if not, register it.
    - log its report
    - calc avg / max / min
 */
int data_processor(void* arg, void* data) {
    log_info("function called: %s", __func__);

    if( !arg || !data){
        return -1;
    }

    system_state* system = (system_state*)arg;
    BUFF_ELEM_TYPE* sensor_data = (BUFF_ELEM_TYPE*)data;

    log_info("Processing data for sensor id: %d", sensor_data->device_id);
    log_info("payload: msg_type[%d], sensor_type[%d], payload[%f]", sensor_data->msg_type, sensor_data->sensor_type, sensor_data->payload);

    pthread_mutex_lock(&system->state_lock);
    int sens_idx = is_sensor_exists(system, sensor_data->device_id);
    if(sens_idx < 0){
        // need to register the sensor!
        int rc= register_sensor(system, sensor_data->device_id);
        if(rc < 0){
            log_err("Failed to register sensor: %d", sensor_data->device_id);
            pthread_mutex_unlock(&system->state_lock);
            return -1;
        }
        log_dbg("Registered new sensor with device id: %d", sensor_data->device_id);
    }

    log_info("Found sensor at index: %d", sens_idx);
    // current reading
    system->sensors[sens_idx].sensor_data.latest = sensor_data->payload;
    // max
    system->sensors[sens_idx].sensor_data.max = (sensor_data->payload > system->sensors[sens_idx].sensor_data.max)?
                                                sensor_data->payload : system->sensors[sens_idx].sensor_data.max;
    // min
    system->sensors[sens_idx].sensor_data.min = (sensor_data->payload < system->sensors[sens_idx].sensor_data.min)?
                                                sensor_data->payload : system->sensors[sens_idx].sensor_data.min;
    // average
    float n = system->sensors[sens_idx].sensor_data.sample_counter + 1;
    float avg = system->sensors[sens_idx].sensor_data.avg;
    avg = (1.0 / n)*((n-1)*avg + sensor_data->payload);
    system->sensors[sens_idx].sensor_data.avg = avg;
    system->sensors[sens_idx].sensor_data.sample_counter =  n;
    log_dbg("info update for sensor %d", sensor_data->device_id);
    log_dbg("latest: %f | avg: %f | max: %f | min: %f",
                                    system->sensors[sens_idx].sensor_data.latest,
                                    system->sensors[sens_idx].sensor_data.avg,
                                    system->sensors[sens_idx].sensor_data.max,
                                    system->sensors[sens_idx].sensor_data.min
            );

    pthread_mutex_unlock(&system->state_lock);

    return 0;
}

