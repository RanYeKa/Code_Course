#pragma once


#include <time.h>
#include <stdbool.h>

typedef enum {
    MSG_TEMP = 0,
    MSG_HUMIDITY,
    MSG_STATUS
} message_type_t;


typedef enum {
    STATUS_ERROR = -1,
    STATUS_OK,
    STATUS_WARNING,
} status_code_t;


typedef struct sensor_reading {
    int id;
    time_t timestamp;
    float value;
} sensor_reading_t;


typedef struct sensor_msg {
    message_type_t type;
    sensor_reading_t payload;
} sensor_msg_t;