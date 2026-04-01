#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#include "proto.h"

#define NUM_OF_READINGS 10
#define READING_INTERVAL_SECONDS 1




sensor_msg_t sensor_msg_make(message_type_t sense_type ,int id, float value, time_t raw_ts);
void sensor_msg_print(const void *msg);

static inline const char* msg_type_str(message_type_t t) {
    switch (t) {
        case MSG_TEMP:     return "TEMP";
        case MSG_HUMIDITY: return "HUMIDITY";
        case MSG_STATUS:   return "STATUS";
        default:           return "UNKNOWN";
    }
}


static inline const char* status_str(status_code_t s) {
    switch (s) {
        case STATUS_ERROR:   return "ERROR";
        case STATUS_OK:      return "OK";
        case STATUS_WARNING: return "WARNING";
        default:             return "UNKNOWN";
    }
}