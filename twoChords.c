#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include "pru.h"
#include "servo.h"

#define NUM_SERVOS 7
#define STRUMMING_SERVO 6
#define NUM_CHORDS 2

// The positions of the servos for two simple chords.
int chord_mapping[NUM_CHORDS][NUM_SERVOS - 1] = {
    {1, 2, 3, 4, 5, 6}, // Chord 1
    {6, 5, 4, 3, 2, 1}  // Chord 2
};

void *servo_thread_function(void *arg) {
    int i, j;

    while(1) {
        // Loop over the chords
        for(j = 0; j < NUM_CHORDS; j++) {
            // Move each servo to the position for the current chord
            for(i = 0; i < NUM_SERVOS - 1; i++) {
                servo_set_position(i, chord_mapping[j][i]);
                usleep(50000); // Delay to allow servo to move
            }

            // Strum the guitar
            servo_set_position(STRUMMING_SERVO, 0);
            usleep(50000); // Delay to allow servo to move
            servo_set_position(STRUMMING_SERVO, 1);
            usleep(50000); // Delay to allow servo to move

            sleep(1); // Delay before starting the next chord
        }
    }

    return NULL;
}

int main() {
    pthread_t servo_thread;
    int i;

    // Initialize the PRU and the servos
    if(pru_init() != 0 || servo_init() != 0) {
        printf("Failed to initialize the PRU or the servos.\n");
        return 1;
    }

    // Configure the PRU and the servos
    if(pru_configure() != 0) {
        printf("Failed to configure the PRU.\n");
        return 1;
    }

    for(i = 0; i < NUM_SERVOS; i++) {
        if(servo_configure(i) != 0) {
            printf("Failed to configure servo %d.\n", i);
            return 1;
        }
    }

    // Start the servo control thread
    if(pthread_create(&servo_thread, NULL, &servo_thread_function, NULL) != 0) {
        printf("Failed to create the servo control thread.\n");
        return 1;
    }

    // Main loop: do nothing
    while(1) {
        sleep(1);
    }

    // Clean up (this code will never be reached in this example)
    pthread_cancel(servo_thread);
    pthread_join(servo_thread, NULL);

    for(i = 0; i < NUM_SERVOS; i++) {
        servo_cleanup(i);
    }

    pru_cleanup();

    return 0;
}
