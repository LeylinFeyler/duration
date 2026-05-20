#include "spinner.h"
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

// spinner animation characters
char spinner_seq[] = {'|', '/', '-', '\\'};

// runs in separate thread and prints spinner frame
void *spinner_thread(void *arg) {
    Spinner *sp = (Spinner *)arg;
    int i       = 0;

    // loop while spinner is active
    while (sp->running) {
        printf("\r%s %c", sp->message, spinner_seq[i++ % 4]);
        fflush(stdout);

        // small delay between frames
        struct timespec ts = {0, 100000000}; // 100ms
        nanosleep(&ts, NULL);
    }

    return NULL;
}

// start spinner thread
void spinner_start(Spinner *sp, const char *message) {
    sp->running = 1;
    sp->message = message;

    pthread_create(&sp->thread, NULL, spinner_thread, sp);
}

// stop spinner thread and wait for it to finish
void spinner_stop(Spinner *sp, const char *doneMessage) {
    sp->running = 0;

    pthread_join(sp->thread, NULL);

    // print final message or clear line
    if (doneMessage && strlen(doneMessage) > 0) {
        printf("\r%s%*s\n", doneMessage, 80 - (int)strlen(doneMessage), "");
    } else {
        printf("\r%*s\r", 80, "");
    }

    fflush(stdout);
}