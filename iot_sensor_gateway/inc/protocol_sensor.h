#pragma once
// protocol_sensor.h
#include <time.h>
#include <stdbool.h>
#include <stdint.h>


// The "Knock-Knock" byte. If a UDP packet doesn't start with this, we drop it instantly.
#define PACKET_MAGIC_BYTE 0xAA

// Error Codes
typedef enum protocol_error_ {
    PROTOCOL_SUCCESS = 0,
    PROTOCOL_ERR_INVALID_MAGIC = -1,
    PROTOCOL_ERR_CHECKSUM_MISMATCH = -2,
    PROTOCOL_ERR_INVALID_LENGTH = -3,
    PROTOCOL_ERR_NULL = -4
} protocol_error_t;

// Message Types for 2-way communication
typedef enum msg_type_ {
    MSG_DATA_REPORT = 0x01,        // Sensor reporting data (e.g., temperature)
    MSG_STATUS_REQ,  // Gateway asking Sensor if it's alive
    MSG_CONFIG       // Gateway setting config (e.g., update frequency)
} msg_type_t;

typedef enum report_type_ {
    TEMPERATURE = 0x01,
    HUMIDITY = 0x02,
    LUMENS = 0x03,
    PRESSURE = 0x04,
    // TBD: add more as needed, e.g., RELAY_STATUS, etc.
    RELAY_STATUS = 0x05
} report_type_t;



// The tightly packed binary payload (Exactly 9 bytes)
#pragma pack(push, 1)
typedef struct {
    uint8_t  magic_byte;    // Always 0xAA
    uint8_t  msg_type;      // MSG_DATA, MSG_STATUS_REQ, etc.
    uint16_t device_id;     // Unique to each physical board (e.g., 4001)
    uint8_t  sensor_type;   // What is this data? (e.g., 1=Temp, 2=Relay)
    float    payload;       // The actual reading (e.g., 25.5)
    uint16_t checksum;      // CRC-16 of everything above
} sensor_packet_t;
#pragma pack(pop)

// --- API Functions for the Gateway ---

// _______ Utils:
// Calculates CRC-16-CCITT
uint16_t calculate_crc16(const uint8_t *data, int length);
// _______


// Parses raw bytes into the struct. Returns true if valid and checksum matches.
protocol_error_t protocol_parse_msg(const uint8_t *raw_bytes, int length, sensor_packet_t *parsed_pkt);

// Builds raw bytes from a struct. Calculates the checksum and injects the magic byte.
protocol_error_t protocol_build_msg(const sensor_packet_t *pkt, uint8_t *raw_bytes_out, int *out_length);
