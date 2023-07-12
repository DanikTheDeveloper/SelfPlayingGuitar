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

#define NOTE1_ANGLE 90
#define OTHER_NOTE_ANGLE0 0
#define OTHER_NOTE_ANGLE45 45
#define OTHER_NOTE_ANGLE60 60

#define STRUM_TIME 1000000 // 1 second

#define NOTE_COUNT 7
#define CHORD_COUNT 2

struct note_gpio_mapping {
    uint8_t gpio_pin;
    uint32_t servo_angle;
};

note_gpio_mapping chord_mappings[CHORD_COUNT][NOTE_COUNT - 1] = {
    // Chord 1
    {
        {NOTE2_GPIO, OTHER_NOTE_ANGLE0},
        {NOTE3_GPIO, OTHER_NOTE_ANGLE0},
        {NOTE4_GPIO, OTHER_NOTE_ANGLE45},
        {NOTE5_GPIO, OTHER_NOTE_ANGLE60},
        {NOTE6_GPIO, OTHER_NOTE_ANGLE60},
        {NOTE7_GPIO, OTHER_NOTE_ANGLE0},
    },
    // Chord 2
    {
        {NOTE3_GPIO, OTHER_NOTE_ANGLE0},
        {NOTE4_GPIO, OTHER_NOTE_ANGLE0},
        {NOTE5_GPIO, OTHER_NOTE_ANGLE45},
        {NOTE6_GPIO, OTHER_NOTE_ANGLE60},
        {NOTE2_GPIO, OTHER_NOTE_ANGLE60},
        {NOTE7_GPIO, OTHER_NOTE_ANGLE0},
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
            pruio_pwm_setangle(io, chord_mappings[c][i].gpio_pin, chord_mappings[c][i].servo_angle);
            if (io->Err) {
                printf("failed setting servo angle (%s)\n", io->Err);
                pruio_destroy(io);
                pthread_mutex_unlock(&pru_mutex);
                return NULL;
            }
        }
        pthread_cond_signal(&strum_done_cond);
        pthread_mutex_unlock(&pru_mutex);

        usleep(STRUM_TIME);
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

        pruio_pwm_setangle(io, NOTE1_GPIO, NOTE1_ANGLE);
        if (io->Err) {
            printf("failed setting servo angle (%s)\n", io->Err);
            pruio_destroy(io);
            pthread_mutex_unlock(&pru_mutex);
            return NULL;
        }

        pthread_mutex_unlock(&pru_mutex);
        usleep(STRUM_TIME);

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
