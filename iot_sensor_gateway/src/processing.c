// rpocessing.c
#include "processing.h"
#include "gateway.h"
#include "sig_handling.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>


void* worker_thread(void* arg){
    printf("function called: %s\n", __func__);

    care_pkg* ctx = (care_pkg*)arg;

    pthread_barrier_wait(&ctx->barrier);
    printf("Worker booting up...\n");

    // int cnt = 0;
    sensor_msg data_read;
    while(g_run_flag){
        if(rbuff_read(&ctx->rbuff, &data_read) == SUCCESS){
            // printf("Worker and just read the message: %s\n", data_read->payload);
            msg_parser(&data_read, &ctx->system);
        }
    }


    printf("Worker was killed...\n");

    return (void*)ctx; // placeholder
}

int msg_parser(void* msg, void* arg){
    printf("function called: %s\n", __func__);

    // arg is system state
    if( !msg || !arg ){
        return -1;
    }

    char* sensor_name = strtok(((sensor_msg*)msg)->payload, ":");
    if(sensor_name == NULL) return -1;
    char* val_rpt = strtok(NULL, ":");
    if(val_rpt == NULL) return -1;

    printf("[DBG] Parsed message, sensor name: %s, value: %s\n", sensor_name, val_rpt);
    data_processor(arg, sensor_name, atof(val_rpt));

    return 0;
}

int data_processor(void* arg, const char* sensor_name, float val_rpt){
    printf("function called: %s\n", __func__);

    if( !arg || !sensor_name){
        return -1;
    }
    printf("Processing data for sensor: %s, value: %f\n", sensor_name, val_rpt);
    system_state* ctx = (system_state*)arg;
    pthread_mutex_lock(&ctx->state_lock);
    int sens_idx = is_sensor_exists(ctx, sensor_name);
    if(sens_idx < 0){
        // need to register the sensor!
        int rc= register_sensor(ctx, sensor_name);
        if(rc < 0){
            printf("Failed to register sensor: %s\n", sensor_name);
            pthread_mutex_unlock(&ctx->state_lock);
            return -1;
        }
        printf("Registered new sensor: %s\n",  sensor_name);
    }
    printf("Found sensor at index: %d\n",  sens_idx);
    // current reading
    ctx->sensors[sens_idx].sensor_data.latest = val_rpt;
    // max
    ctx->sensors[sens_idx].sensor_data.max = (val_rpt > ctx->sensors[sens_idx].sensor_data.max)?
                                                val_rpt : ctx->sensors[sens_idx].sensor_data.max;
    // min
    ctx->sensors[sens_idx].sensor_data.min = (val_rpt < ctx->sensors[sens_idx].sensor_data.min)?
                                                val_rpt : ctx->sensors[sens_idx].sensor_data.min;
    // average
    float n = ctx->sensors[sens_idx].sensor_data.sample_counter + 1;
    float avg = ctx->sensors[sens_idx].sensor_data.avg;
    avg = (1.0 / n)*((n-1)*avg + val_rpt);
    ctx->sensors[sens_idx].sensor_data.avg = avg;
    ctx->sensors[sens_idx].sensor_data.sample_counter =  n;
    printf("[DBG] Updated sensor data for: %s, latest: %f, avg: %f\n",  sensor_name, ctx->sensors[sens_idx].sensor_data.latest, ctx->sensors[sens_idx].sensor_data.avg);
    printf("[DBG] Updated sensor data for: %s, max: %f, min: %f\n",  sensor_name, ctx->sensors[sens_idx].sensor_data.max, ctx->sensors[sens_idx].sensor_data.min);

    pthread_mutex_unlock(&ctx->state_lock);

    return 0;
}

