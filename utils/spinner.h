#ifndef SPINNER_H
#define SPINNER_H

typedef struct {
    volatile int running;
    const char* message;
} Spinner;

extern char spinner_seq[];

// thread function for spinner animation
void* spinner_thread(void* arg);

// start spinner animation
void spinner_start(Spinner* sp, const char* message);

// stop spinner animation and optionally print a done message
void spinner_stop(Spinner* sp, const char* doneMessage);

#endif