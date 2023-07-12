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

#define NOTE_COUNT 7
#define CHORD_COUNT 2

struct note_gpio_mapping {
    uint8_t gpio_pin;
    uint32_t pwm_freq;
    uint32_t pwm_duty;
};

note_gpio_mapping chord_mappings[CHORD_COUNT][NOTE_COUNT - 1] = {
    // Chord 1
    {
        {NOTE2_GPIO, OTHER_NOTE_FREQ, OTHER_NOTE_TIME},
        {NOTE3_GPIO, OTHER_NOTE_FREQ, OTHER_NOTE_TIME},
        {NOTE4_GPIO, OTHER_NOTE_FREQ, OTHER_NOTE_TIME},
        {NOTE5_GPIO, OTHER_NOTE_FREQ, OTHER_NOTE_TIME},
        {NOTE6_GPIO, OTHER_NOTE_FREQ, OTHER_NOTE_TIME},
        {NOTE7_GPIO, OTHER_NOTE_FREQ, OTHER_NOTE_TIME},
    },
    // Chord 2
    {
        {NOTE3_GPIO, OTHER_NOTE_FREQ, OTHER_NOTE_TIME},
        {NOTE4_GPIO, OTHER_NOTE_FREQ, OTHER_NOTE_TIME},
        {NOTE5_GPIO, OTHER_NOTE_FREQ, OTHER_NOTE_TIME},
        {NOTE6_GPIO, OTHER_NOTE_FREQ, OTHER_NOTE_TIME},
        {NOTE2_GPIO, OTHER_NOTE_FREQ, OTHER_NOTE_TIME},
        {NOTE7_GPIO, OTHER_NOTE_FREQ, OTHER_NOTE_TIME},
    }
};

pthread_mutex_t pru_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t strum_done_cond = PTHREAD_COND_INITIALIZER;

void* playChord(void *vargp)
{
    for (int c = 0; c < CHORD_COUNT; c++)
    {
        pthread_mutex_lock(&pru_mutex);

        pruIo *io = pruio_new(PRUIO_DEF_ACTIVE, 0x98, 0, 1);
        if (io->Err) {
            printf("pruio_new failed (%s)\n", io->Err);
            pruio_destroy(io);
            pthread_mutex_unlock(&pru_mutex);
            return NULL;
        }

        for (int i = 0; i < NOTE_COUNT - 1; i++)
        {
            pruio_cap_config(io, chord_mappings[c][i].gpio_pin, chord_mappings[c][i].pwm_freq);
            if (io->Err) {
                printf("failed setting CAP mode (%s)\n", io->Err);
                pruio_destroy(io);
                pthread_mutex_unlock(&pru_mutex);
                return NULL;
            }
        }
        pthread_cond_signal(&strum_done_cond);
        pthread_mutex_unlock(&pru_mutex);

        usleep(STRUM_TIME); // sleep for strum time
        pruio_destroy(io);
    }
    return NULL;
}

void* playNote1(void *vargp)
{
    for (int c = 0; c < CHORD_COUNT; c++)
    {
        pthread_mutex_lock(&pru_mutex);
        pthread_cond_wait(&strum_done_cond, &pru_mutex);

        pruIo *io = pruio_new(PRUIO_DEF_ACTIVE, 0x98, 0, 1);
        if (io->Err) {
            printf("pruio_new failed (%s)\n", io->Err);
            pruio_destroy(io);
            pthread_mutex_unlock(&pru_mutex);
            return NULL;
        }

        pruio_cap_config(io, NOTE1_GPIO, NOTE1_FREQ);
        if (io->Err) {
            printf("failed setting CAP mode (%s)\n", io->Err);
            pruio_destroy(io);
            pthread_mutex_unlock(&pru_mutex);
            return NULL;
        }

        pthread_mutex_unlock(&pru_mutex);
        usleep(STRUM_TIME); // sleep for strum time

        pruio_destroy(io);
    }
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
