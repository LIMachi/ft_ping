#include "ft_ping.h"

void insert_pong(i64 new) {
    i64 delta;
    i64 off;

    if (new < app()->min || app()->received == 0)
        app()->min = new;
    if (new > app()->max || app()->received == 0)
        app()->max = new;
    ++app()->received;
    delta = new - app()->average;
    app()->average += delta / app()->received;
    off = new - app()->average;
    app()->da2 += delta * off;
}

t_time now() {
    t_time out;

    if (clock_gettime(CLOCK_REALTIME, &out) == -1) {
        print_e((str[3]){"clock_gettime error: ", strerror(errno), NULL});
        exit(-1);
    }
    return out;
}

//return the delta time in ns since the given time (using the current time)
i64 delta_time(t_time prev, t_time now) {
    now.tv_nsec -= prev.tv_nsec;
    if ((now.tv_sec -= prev.tv_sec) < 0) {
        now.tv_nsec += NS_S;
        --now.tv_sec;
    }
    return (i64)now.tv_sec * NS_S + (i64)now.tv_nsec;
}

void set_timer(int secs, int micros) {
    struct itimerval timer = {
        .it_interval = {
            .tv_sec = secs,
            .tv_usec = micros
        },
        .it_value = {
            .tv_sec = secs,
            .tv_usec = micros
        }
    };

    setitimer(ITIMER_REAL, &timer, NULL);
}