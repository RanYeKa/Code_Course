#include "sensor.h"

sensor_msg_t sensor_msg_make(message_type_t sense_type ,int id, float value, time_t raw_ts){
    sensor_msg_t new_reading;
    new_reading.type = sense_type;
    new_reading.payload.value = value;
    new_reading.payload.id = id;
    new_reading.payload.timestamp = raw_ts;
    return new_reading;
}

void sensor_msg_print(const void *msg){
    if(!msg){
        printf("Invalid sensor reading.\n");
        return;
    }

    sensor_msg_t *input = (sensor_msg_t*)msg;

    printf("Sensor ID: %d\n", input->payload.id);
    struct tm *info = localtime(&input->payload.timestamp);
    char buffer[9]; // Enough for "HH:MM:SS\0"
    strftime(buffer, sizeof(buffer), "%H:%M:%S", info); // Convert ms to seconds for strftime
    printf("Timestamp: %s\n", buffer);
    printf("Message Type: %s\n", msg_type_str(input->type));
    switch(input->type) {
        case MSG_TEMP:
            printf("Temperature: %.2f °C\n", input->payload.value);
            break;
        case MSG_HUMIDITY:
            printf("Humidity: %.2f %%\n", input->payload.value);
            break;
        case MSG_STATUS:
            printf("Status: %s\n", status_str((status_code_t)(int)input->payload.value));
            break;
        default:
            printf("Unknown message type.\n");
    }
}

