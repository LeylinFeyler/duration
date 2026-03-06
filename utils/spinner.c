#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include "spinner.h"

char spinner_seq[] = {'|','/','-','\\'};

// ===== spinner functions =====
void* spinner_thread(void* arg) {
    Spinner* sp = (Spinner*)arg;
    int i = 0;
    while(sp->running) {
        printf("\r%s %c", sp->message, spinner_seq[i++ % 4]);
        fflush(stdout);
        struct timespec ts = {0, 100000000}; // 0.1s sleep
        nanosleep(&ts, NULL);
    }
    return NULL;
}

// start spinner animation
void spinner_start(Spinner* sp, const char* message) {
    sp->running = 1;
    sp->message = message;
    pthread_t tid;
    pthread_create(&tid, NULL, spinner_thread, sp);
    pthread_detach(tid);
}

// stop spinner animation and optionally print a done message
void spinner_stop(Spinner* sp, const char* doneMessage) {
    sp->running = 0;
    sleep(200000); // small delay to let spinner finish
    if(doneMessage && strlen(doneMessage) > 0) {
        printf("\r%s%*s\n", doneMessage, 80 - (int)strlen(doneMessage), "");
    } else {
        printf("\r%*s\r", 80, "");
    }
    fflush(stdout);
}