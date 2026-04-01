// network.c
#include "network.h"
#include "gateway.h"
#include "sig_handling.h"

#include <stdio.h>
#include <unistd.h>

#include <sys/socket.h> // The core socket API
#include <netinet/in.h> // Structures for IP addresses
#include <arpa/inet.h>  // Functions to convert IP strings to bytes
#include <string.h>     // For memset
#include <sys/time.h>   // For our timeout struct


void* udp_listener_thread(void* arg){
    printf("function called: %s\n", __func__);

    care_pkg* ctx = (care_pkg*)arg;

    pthread_barrier_wait(&ctx->barrier);

    // 1. Create the Mailbox (AF_INET = IPv4, SOCK_DGRAM = UDP)
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if(sockfd < 0){
        printf("Socket creation Failed!\n");
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
        printf("Failed to bind socket to port 8080!\n");
        close(sockfd);
        return (void*)ctx;
    }
    printf("Listener bound to Port 8080. Waiting for data...\n");

    // 5. The Main Loop
    while (g_run_flag) {
        sensor_msg msg;
        memset(&msg, 0, sizeof(msg)); // Clear the buffer

        struct sockaddr_in client_addr; // To store who sent the postcard
        socklen_t client_len = sizeof(client_addr);

        // Reach into the mailbox
        ssize_t bytes_read = recvfrom(sockfd, msg.payload, MAX_MSG_LENGTH - 1, 0,
                                        (struct sockaddr*)&client_addr, &client_len);

        if (bytes_read > 0) {
            // We got a postcard! Ensure it is a valid string.
            msg.payload[bytes_read] = '\0';
            printf("Listener caught: [%s]\n", msg.payload);
            if(rbuff_write(&ctx->rbuff, &msg) != SUCCESS){
                printf("Failed to write to ring buffer!\n");
            }
        }
    }

    // 6. Graceful cleanup
    close(sockfd);
    printf("Listener closed socket and was killed...\n");

    return (void*)ctx; // placeholder
}