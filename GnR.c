#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <prussdrv.h>
#include <pruss_intc_mapping.h>

#define SERVO_PWM_PERIOD 20000000

void strum(unsigned int *servo_reg, int del) {
    for (int pos = 30; pos <= 60; pos++) {
        *servo_reg = SERVO_PWM_PERIOD - pos * 10000;
        usleep(del * 1000);
    }
    for (int pos = 60; pos >= 30; pos--) {
        *servo_reg = SERVO_PWM_PERIOD - pos * 10000;
        usleep(del * 1000);
    }
}

void moveTo(unsigned int *servo_reg, int from, int to) {
    if (from > to) {
        for (; from >= to; from--) {
            *servo_reg = SERVO_PWM_PERIOD - from * 10000;
            usleep(1000);
        }
    } else {
        for (; from <= to; from++) {
            *servo_reg = SERVO_PWM_PERIOD - from * 10000;
            usleep(1000);
        }
    }
    usleep(500000);
}

int main() {
    prussdrv_init();
    prussdrv_open(PRU_EVTOUT_0);

    unsigned int *servo1_reg = prussdrv_map_prumem(PRUSS0_SHARED_DATARAM, 4*0);
    unsigned int *servo2_reg = prussdrv_map_prumem(PRUSS0_SHARED_DATARAM, 4*1);

    int count = 0;
    int amp = 35;
    int range = (180 - amp)/2;
    int from = range + amp * 2 / 5;

    while (1) {
        if (count < 7) {
            if (count == 0 || count == 2 || count == 4 || count == 6) {
                moveTo(servo2_reg, from, range - 3 + amp * 2 / 4);
            } else if (count == 3) {
                moveTo(servo2_reg, from, range + 1.5);
            } else {
                moveTo(servo2_reg, from, range + amp + 1.5);
            }
            strum(servo1_reg, 10);
            usleep(500000);
        } else {
            strum(servo1_reg, 6);
            usleep(500000);
        }
        count++;
        if (count > 9) {
            count = 0;
            usleep(500000);
        }
    }

    prussdrv_exit();

    return 0;
}
