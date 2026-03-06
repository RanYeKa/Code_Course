#include "loops.h"
#include "sensor.h"


int myRandom(int max) {
    srand(time(NULL)); // Seed the random number generator with the current time
    int ans_temp = rand(); // Generate a random number between 0 and max-1
    // printf("Generated random number (before modulo): %d\n", ans_temp); // Print the generated random number before modulo
    int ans = ans_temp % max; // Generate a random number between 0 and max-1
    // printf("Generated random number in range [0, %d): %d\n", max, ans); // Print the generated random number
    return ans;
}


int main(void){
    // int n = 5;
    // int k = 3;

    // printf("Loop Sum for %d: %d\n", n, loop_sum(n, 5));
    // printf("Loop Product for %d and %d: %d\n", n, k, loop_product(n, k));
    // printf("Countdown For %d: ", n);
    // loop_countdown_for(n);
    // printf("\nCountdown While %d: ", n);
    // loop_countdown_while(n);
    // printf("\n");

    printf("Creating a sensor reading...\n");
    for(int read_i = 0; read_i <= NUM_OF_READINGS; read_i++){
        message_type_t type = (message_type_t)(myRandom(3)); // Randomly select between MSG_TEMP, MSG_HUMIDITY, and MSG_STATUS
        float value = 0;
        if (type == MSG_STATUS) {
            // For status messages, we want to randomly select between STATUS_ERROR, STATUS_OK, and STATUS_WARNING
            status_code_t status = (status_code_t)(myRandom(3) - 1); // Randomly select between -1, 0, and 1
            value = (float)status;
        } else {
            value = (float)(rand() % 100);
        }
        sensor_msg_t reading = sensor_msg_make(type, read_i, value, time(NULL)); // Simulate sensor reading with random value and current timestamp in ms
        sensor_msg_print(&reading);
        loop_countdown_while_no_new_line(READING_INTERVAL_SECONDS); // Simulate delay between readings
        if (read_i < NUM_OF_READINGS) printf("\033[%dA\033[J", 5); // Move cursor up and clear line for next reading (5 lines for the sensor message)
    }

    return 0;
}