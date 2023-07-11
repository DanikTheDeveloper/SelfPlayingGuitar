#include <stdio.h>
#include <prussdrv.h>
#include <pruss_intc_mapping.h>
#include <unistd.h>
#include <pthread.h>

// Defining PWM period for servo motors (in ns)
#define SERVO_PWM_PERIOD 20000000

// Note positions for the melody (simplified for one octave)
#define E5_POSITION 1450000
#define D5_POSITION 1300000
#define C5_POSITION 1200000
#define B4_POSITION 1100000
#define A4_POSITION 1000000

// Array of note positions in the melody
#define MELODY {E5_POSITION, D5_POSITION, C5_POSITION, B4_POSITION, A4_POSITION}

// Global variable to keep track of the current note
int current_note_position = 0;

// Strumming thread function
void* strumming_thread(void* arg) {
    unsigned int* strumming_servo_ctl_reg = prussdrv_map_prumem(PRUSS0_SHARED_DATARAM, 4*1);
    while (1) {
        // Play the strumming motion
        *strumming_servo_ctl_reg = current_note_position;
        usleep(500000);
        *strumming_servo_ctl_reg = SERVO_PWM_PERIOD - current_note_position;
    }
    return NULL;
}

// Function to play a single note
void play_note(int note_position) {
    unsigned int* fretting_servo_ctl_reg = prussdrv_map_prumem(PRUSS0_SHARED_DATARAM, 4*0);

    // Set the global note position
    current_note_position = note_position;

    // Move the fretting servo
    *fretting_servo_ctl_reg = note_position;

    // Wait for the note to ring
    usleep(500000);

    *fretting_servo_ctl_reg = SERVO_PWM_PERIOD - note_position;

    // Wait for the string to stop ringing
    usleep(500000);
}

// Function to play a melody
void play_melody(int melody[], size_t melody_length) {
    for (int i = 0; i < melody_length; i++) {
        play_note(melody[i]);
    }
}

int main() {
    prussdrv_init();
    prussdrv_open(PRU_EVTOUT_0);

    // Create the strumming thread
    pthread_t strumming_thread_id;
    pthread_create(&strumming_thread_id, NULL, strumming_thread, NULL);

    // Play the melody
    int melody[] = MELODY;
    size_t melody_length = sizeof(melody) / sizeof(melody[0]);
    play_melody(melody, melody_length);

    // Cancel the strumming thread once we're done
    pthread_cancel(strumming_thread_id);

    prussdrv_exit();

    return 0;
}
