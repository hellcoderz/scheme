#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "repl.h"
#include "object.h"
#include "gc.h"

double startup_time;

static void dispose(void) {
    dispose_obj();
    gc_finalize();
}

static int setup_startup_time(void) {
    struct timespec ts;
    clockid_t clock = CLOCK_MONOTONIC;
    int ret;
    
    ret = clock_gettime(clock, &ts);
    if (ret != 0) {
        return -1;
    }
    startup_time = ts.tv_sec * 1000.0 + ts.tv_nsec / 1000000.0;
    return 0;
}

static void init_random(void) {
    clockid_t clock = CLOCK_MONOTONIC;
    struct timespec ts;

    clock_gettime(clock, &ts);
    srandom((int)ts.tv_sec + (int)ts.tv_nsec);
}

static int init(void) {
    int ret;

    ret = gc_init(-1); /* -1: default heap size */
    if (ret != 0) {
        fprintf(stderr, "gc system failed to initialize\n");
        return ret;
    }

    ret = init_obj();
    if (ret != 0) {
        return ret;
    }

    ret = setup_startup_time();
    if (ret != 0) {
        return ret;
    }

    init_random();
    return 0;
}

int main(int argc, char **argv) {
    int ret;

    ret = init();
    if (ret != 0) {
        fprintf(stderr, "%s\n",
                "failed to intialize object system");
        return ret;
    }

    ret = sc_repl();

    dispose();

    return ret;
}

