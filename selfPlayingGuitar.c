#include <iostream>
#include <unistd.h>
#include <pthread.h>
#include <pruio.h>

// Hypothetically, we map each GPIO to the specific servo
#define STRUM_SERVO_GPIO 60
#define STRING1_SERVO_GPIO 50
#define STRING2_SERVO_GPIO 51
#define STRING3_SERVO_GPIO 30
#define STRING4_SERVO_GPIO 31
#define STRING5_SERVO_GPIO 48
#define STRING6_SERVO_GPIO 49

#define STRUM_PWM_FREQ 50    // 50Hz
#define STRUM_PWM_DUTY 2000  // 2ms
#define STRING_PWM_FREQ 50   // 50Hz
#define STRING_PWM_DUTY 1500 // 1.5ms

#define STRUM_TIME 1000000 // 1 second

#define FRET_COUNT 7

struct servo_gpio_mapping {
    uint8_t gpio_pin;
    uint32_t pwm_freq;
    uint32_t pwm_duty;
};

servo_gpio_mapping servo_mappings[FRET_COUNT] = {
    {STRING1_SERVO_GPIO, STRING_PWM_FREQ, STRING_PWM_DUTY},
    {STRING2_SERVO_GPIO, STRING_PWM_FREQ, STRING_PWM_DUTY},
    {STRING3_SERVO_GPIO, STRING_PWM_FREQ, STRING_PWM_DUTY},
    {STRING4_SERVO_GPIO, STRING_PWM_FREQ, STRING_PWM_DUTY},
    {STRING5_SERVO_GPIO, STRING_PWM_FREQ, STRING_PWM_DUTY},
    {STRING6_SERVO_GPIO, STRING_PWM_FREQ, STRING_PWM_DUTY},
    {STRUM_SERVO_GPIO, STRUM_PWM_FREQ, STRUM_PWM_DUTY},
};

void* chordFunction(void *vargp)
{
    pruIo *io = pruio_new(PRUIO_DEF_ACTIVE, 0x98, 0, 1);
    if (io->Err) {
        printf("pruio_new failed (%s)\n", io->Err);
        pruio_destroy(io);
        return NULL;
    }

    for (int i = 0; i < FRET_COUNT - 1; i++)
    {
        pruio_cap_config(io, servo_mappings[i].gpio_pin, servo_mappings[i].pwm_freq);
        if (io->Err) {
            printf("failed setting CAP mode (%s)\n", io->Err);
            pruio_destroy(io);
            return NULL;
        }
        usleep(STRUM_TIME); // sleep for strum time
    }
    pruio_destroy(io);
    return NULL;
}

void* strumFunction(void *vargp)
{
    pruIo *io = pruio_new(PRUIO_DEF_ACTIVE, 0x98, 0, 1);
    if (io->Err) {
        printf("pruio_new failed (%s)\n", io->Err);
        pruio_destroy(io);
        return NULL;
    }

    pruio_cap_config(io, servo_mappings[FRET_COUNT - 1].gpio_pin, servo_mappings[FRET_COUNT - 1].pwm_freq);
    if (io->Err) {
        printf("failed setting CAP mode (%s)\n", io->Err);
        pruio_destroy(io);
        return NULL;
    }
    usleep(STRUM_TIME); // sleep for strum time

    pruio_destroy(io);
    return NULL;
}

int main()
{
    pthread_t thread_id1, thread_id2;
    pthread_create(&thread_id1, NULL, chordFunction, NULL);
    pthread_create(&thread_id2, NULL, strumFunction, NULL);
    pthread_join(thread_id1, NULL);
    pthread_join(thread_id2, NULL);

    return 0;
}
