#include <iostream>
#include <unistd.h>
#include <pthread.h>
#include <pruio.h>

#define NOTE1_GPIO 60
#define NOTE2_GPIO 50
#define NOTE3_GPIO 51
#define NOTE4_GPIO 30
#define NOTE5_GPIO 31
#define NOTE6_GPIO 48
#define NOTE7_GPIO 49

#define NOTE1_FREQ 50   // 50Hz
#define NOTE1_TIME 2000 // 2ms
#define OTHER_NOTE_FREQ 50 // 50Hz
#define OTHER_NOTE_TIME 1500 // 1.5ms

#define STRUM_TIME 1000000 // 1 second

//Frequencies 
#define NOTE1_FREQ 500   // 500Hz
#define NOTE2_FREQ 450   // 450Hz
#define NOTE3_FREQ 400   // 400Hz
#define NOTE4_FREQ 350   // 350Hz
#define NOTE5_FREQ 300   // 300Hz
#define NOTE6_FREQ 250   // 250Hz
#define NOTE7_FREQ 200   // 200Hz

#define NOTE1_TIME 2000 // 2ms
#define NOTE2_TIME 1800 // 1.8ms
#define NOTE3_TIME 1600 // 1.6ms
#define NOTE4_TIME 1400 // 1.4ms
#define NOTE5_TIME 1200 // 1.2ms
#define NOTE6_TIME 1000 // 1ms
#define NOTE7_TIME 800  // 0.8ms

#define NOTE_COUNT 7

struct note_gpio_mapping {
    uint8_t gpio_pin;
    uint32_t pwm_freq;
    uint32_t pwm_duty;
};

note_gpio_mapping note_mappings[NOTE_COUNT] = {
    {NOTE2_GPIO, NOTE2_FREQ, NOTE2_TIME},
    {NOTE3_GPIO, NOTE3_FREQ, NOTE3_TIME},
    {NOTE4_GPIO, NOTE4_FREQ, NOTE4_TIME},
    {NOTE5_GPIO, NOTE5_FREQ, NOTE5_TIME},
    {NOTE6_GPIO, NOTE6_FREQ, NOTE6_TIME},
    {NOTE7_GPIO, NOTE7_FREQ, NOTE7_TIME},
    {NOTE1_GPIO, NOTE1_FREQ, NOTE1_TIME}
};

void* playChord(void *vargp)
{
    pruIo *io = pruio_new(PRUIO_DEF_ACTIVE, 0x98, 0, 1);
    if (io->Err) {
        printf("pruio_new failed (%s)\n", io->Err);
        pruio_destroy(io);
        return NULL;
    }

    for (int i = 0; i < NOTE_COUNT - 1; i++)
    {
        pruio_cap_config(io, note_mappings[i].gpio_pin, note_mappings[i].pwm_freq);
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

void* playNote1(void *vargp)
{
    pruIo *io = pruio_new(PRUIO_DEF_ACTIVE, 0x98, 0, 1);
    if (io->Err) {
        printf("pruio_new failed (%s)\n", io->Err);
        pruio_destroy(io);
        return NULL;
    }

    pruio_cap_config(io, note_mappings[NOTE_COUNT - 1].gpio_pin, note_mappings[NOTE_COUNT - 1].pwm_freq);
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
    pthread_create(&thread_id1, NULL, playChord, NULL);
    pthread_create(&thread_id2, NULL, playNote1, NULL);
    pthread_join(thread_id1, NULL);
    pthread_join(thread_id2, NULL);

    return 0;
}
