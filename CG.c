#include <stdio.h>
#include <prussdrv.h>
#include <pruss_intc_mapping.h>

// Defining PWM period for servo motors (in ns)
#define SERVO_PWM_PERIOD 20000000

// Defining the servo motor positions for C major and G major chords
#define C_MAJOR_POSITIONS {1450000, 1300000, 1200000, 1100000, 1050000, 1000000, 950000}
#define G_MAJOR_POSITIONS {1300000, 1200000, 1100000, 1000000, 900000, 850000, 800000}

void play_chord(int positions[7]) {
    unsigned int* servo_ctl_regs[7];

    // Each servo is controlled by a separate PRU shared RAM register
    for (int i = 0; i < 7; i++) {
        servo_ctl_regs[i] = prussdrv_map_prumem(PRUSS0_SHARED_DATARAM, 4*i);
        *servo_ctl_regs[i] = positions[i];
    }

    // Delay for 2 seconds (enough time for the servo motors to move)
    usleep(2000000);

    for (int i = 0; i < 7; i++) {
        *servo_ctl_regs[i] = SERVO_PWM_PERIOD - positions[i];
    }

    // Delay for 2 seconds (enough time for the strings to ring)
    usleep(2000000);
}

int main() {
    prussdrv_init();
    prussdrv_open(PRU_EVTOUT_0);

    int c_major_positions[7] = C_MAJOR_POSITIONS;
    int g_major_positions[7] = G_MAJOR_POSITIONS;

    play_chord(c_major_positions);
    play_chord(g_major_positions);

    prussdrv_exit();

    return 0;
}
