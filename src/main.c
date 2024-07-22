#include <stdio.h>
#include <sys/time.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

void sighandler(int signal) {
    if (signal == SIGALRM)
        printf("got signal!\n");
    else if (signal == SIGINT) {
        printf("stopping\n");
        exit(0);
    }
}

void set_sig_handler(int signal, void (*handler)(int)) {
    struct sigaction runner = {
        .sa_handler = handler
    };
    
    sigaction(signal, &runner, NULL);
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

void args(int argc, char *const argv[]) {
    opterr = 0;
    while(1) {
        switch (getopt(argc, argv, "-nt:")) {
            case -1: return;
            case 'n':
                printf("got n\n");
                break;
            case 't':
                printf("got t with %s\n", optarg);
                break;
            case 1:
                printf("got non arg %s\n", optarg);
                break;
            case '?':
                printf("got unknown\n");
                break;
        }
    }
}

int main(int argc, char *const argv[]) {
    args(argc, argv);
    set_timer(1, 0);
    set_sig_handler(SIGALRM, sighandler);
    set_sig_handler(SIGINT, sighandler);
    while (1) {}
}