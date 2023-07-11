#include <stdio.h>
#include <prussdrv.h>
#include <pruss_intc_mapping.h>
#include <unistd.h>

// Defining PWM period for servo motors (in ns)
#define SERVO_PWM_PERIOD 20000000

// Note positions for the melody (simplified for one octave)
#define E5_POSITION 1450000
#define D5_POSITION 1300000
#define E5_POSITION 1200000
#define D5_POSITION 1100000
#define E5_POSITION 1000000
#define B4_POSITION 950000
#define D5_POSITION 900000
#define C5_POSITION 850000
#define A4_POSITION 800000

// Array of note positions in the melody
#define FUR_ELISE_MELODY {E5_POSITION, D5_POSITION, E5_POSITION, D5_POSITION, E5_POSITION, B4_POSITION, D5_POSITION, C5_POSITION, A4_POSITION}

void play_note(int note_position) {
    unsigned int* servo_ctl_reg;

    // The melody is played on one string (one servo)
    servo_ctl_reg = prussdrv_map_prumem(PRUSS0_SHARED_DATARAM, 4*0);
    *servo_ctl_reg = note_position;

    // Delay for 500 milliseconds (enough time for the servo motor to move and the string to ring)
    usleep(500000);

    *servo_ctl_reg = SERVO_PWM_PERIOD - note_position;

    // Delay for 500 milliseconds (enough time for the string to stop ringing)
    usleep(500000);
}

void play_melody(int melody[], size_t melody_length) {
    for (int i = 0; i < melody_length; i++) {
        play_note(melody[i]);
    }
}

int main() {
    prussdrv_init();
    prussdrv_open(PRU_EVTOUT_0);

    int fur_elise_melody[] = FUR_ELISE_MELODY;
    size_t melody_length = sizeof(fur_elise_melody) / sizeof(fur_elise_melody[0]);

    play_melody(fur_elise_melody, melody_length);

    prussdrv_exit();

    return 0;
}
