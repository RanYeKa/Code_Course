// network.c
#include "network.h"
#include "gateway.h"
#include "sig_handling.h"
#include "logger.h"
#include "protocol_sensor.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/socket.h> // The core socket API
#include <netinet/in.h> // Structures for IP addresses
#include <arpa/inet.h>  // Functions to convert IP strings to bytes
#include <string.h>     // For memset
#include <sys/time.h>   // For our timeout struct


void* udp_listener_thread(void* arg){
    log_info("function called: %s", __func__);

    care_pkg_t* ctx = (care_pkg_t*)arg;

    pthread_barrier_wait(&ctx->barrier);

    // 1. Create the Mailbox (AF_INET = IPv4, SOCK_DGRAM = UDP)
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if(sockfd < 0){
        log_err("Socket creation Failed!");
        return (void*)ctx;
    }

    // 2. Set the 1-second Timeout Rule
    struct timeval tv;
    tv.tv_sec =1;
    tv.tv_usec = 0;
    setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);

    // 3. Configure the Apartment Address
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr)); // Zero out the memory first!
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(8080); // Port 8080

    // 4. Bolt it to the door
    if (bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        log_err("Failed to bind socket to port 8080!");
        close(sockfd);
        return (void*)ctx;
    }
    log_info("Listener bound to Port 8080. Waiting for data...");

    // 5. The Main Loop
    uint8_t raw_msg[sizeof(BUFF_ELEM_TYPE)];
    while (g_run_flag) {
        memset(raw_msg, 0, sizeof(BUFF_ELEM_TYPE)); // Clear the buffer

        struct sockaddr_in client_addr; // To store who sent the postcard
        socklen_t client_len = sizeof(client_addr);

        // Reach into the mailbox
        ssize_t bytes_read = recvfrom(sockfd, raw_msg, sizeof(BUFF_ELEM_TYPE), 0,
                                        (struct sockaddr*)&client_addr, &client_len);

        if (bytes_read == sizeof(BUFF_ELEM_TYPE)) {
            // We got a postcard! Ensure it is a valid string.
            log_info("Listener caught: %zd bytes from %s:%d", bytes_read,
                    inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
            if(rbuff_write(&ctx->rbuff, raw_msg) != SUCCESS){
                log_err("Failed to write to ring buffer!");
            }
        }else if(bytes_read == -1){
                log_err("recvfrom error or timeout occurred.");

        }else {
            log_warn("Received unexpected packet size: %zd bytes. Ignoring.", bytes_read);
        }
    }

    // 6. Graceful cleanup
    close(sockfd);
    log_info("Listener closed socket and was killed...");

    return (void*)ctx; // placeholder
}