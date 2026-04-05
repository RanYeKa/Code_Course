// protocol_sensor.c

#include "protocol_sensor.h"
#include "logger.h"

#include <string.h>

// Calculates CRC-16-CCITT
uint16_t calculate_crc16(const uint8_t *data, int length) {
    uint16_t crc = 0xFFFF; // Initial value

    for (int i = 0; i < length; i++) {
        crc ^= (uint16_t)data[i] << 8;
        for (int j = 0; j < 8; j++) {
            if (crc & 0x8000) {
                crc = (crc << 1) ^ 0x1021; // Polynomial
            } else {
                crc <<= 1;
            }
        }
    }
    return crc;
}

// Parses raw bytes into the struct. Returns true if valid and checksum matches.
protocol_error_t protocol_parse_msg(const uint8_t *raw_bytes, int length, sensor_packet_t *parsed_pkt){
    if(!raw_bytes || !parsed_pkt){
        log_err("Null pointer passed to %s", __func__);
        return PROTOCOL_ERR_NULL;
    }

    if(length != sizeof(sensor_packet_t)){
        log_err("Invalid length");
        return PROTOCOL_ERR_INVALID_LENGTH;
    }

    // check for of the message validity:
    if(raw_bytes[0] != PACKET_MAGIC_BYTE){
        log_err("Invalid magic byte: %02X", raw_bytes[0]);
        return PROTOCOL_ERR_INVALID_MAGIC;
    }

    memcpy(parsed_pkt, raw_bytes, length);

    // checksum / CRC test:
    int payload_length = sizeof(sensor_packet_t) - sizeof(parsed_pkt->checksum);
    uint16_t calc_crc = calculate_crc16(raw_bytes, payload_length);

    if(calc_crc != parsed_pkt->checksum){
        log_err("Checksum mismatch: calculated %04X, expected %04X", calc_crc, parsed_pkt->checksum);
        return PROTOCOL_ERR_CHECKSUM_MISMATCH;
    }


    return PROTOCOL_SUCCESS;
}

// Builds raw bytes from a struct. Calculates the checksum and injects the magic byte.
protocol_error_t protocol_build_msg(const sensor_packet_t *pkt, uint8_t *raw_bytes_out, int *out_length){
    if(!pkt || !raw_bytes_out || !out_length){
        log_err("Null pointer passed to %s", __func__);
        return PROTOCOL_ERR_NULL;
    }

    *out_length = sizeof(sensor_packet_t);
    memcpy(raw_bytes_out, pkt, *out_length);

    raw_bytes_out[0] = PACKET_MAGIC_BYTE; // Ensure magic byte is set

    int payload_len = sizeof(sensor_packet_t)-sizeof(uint16_t);
    uint16_t checksum = calculate_crc16(raw_bytes_out, payload_len);

    sensor_packet_t* out_pkt_ptr = (sensor_packet_t*)raw_bytes_out;
    out_pkt_ptr->checksum = checksum;

    return PROTOCOL_SUCCESS;
}