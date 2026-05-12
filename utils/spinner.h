#ifndef SPINNER_H
#define SPINNER_H

#include <pthread.h>

typedef struct {
    volatile int running;
    const char* message;
    pthread_t thread;
} Spinner;

void spinner_start(Spinner* sp, const char* message);
void spinner_stop(Spinner* sp, const char* doneMessage);

#endif