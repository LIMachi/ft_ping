#include "ft_ping.h"

void insert_pong(t_ping_stats *stats, double new) {
	double delta, off;

	if (new < stats->min || stats->received == 0)
		stats->min = new;
	if (new > stats->max || stats->received == 0)
		stats->max = new;
	++stats->received;
	delta = new - stats->average;
	stats->average += delta / (double)stats->received;
	off = new - stats->average;
	stats->m2 += delta * off;
	stats->deviation = sqrt(stats->m2 / (double)stats->received);
}

t_time now() {
	t_time out;

	//I use CLOCK_BOOTTIME as it is the more stable and cannot be changed by the system while ping is running, nor is it halted when the program is suspended
	if (clock_gettime(CLOCK_REALTIME, &out) == -1) {
		print_e("clock_gettime error: ", strerror(errno));
		exit(-1);
	}
	return out;
}

//return the delta time in ms since the given time (using the current time)
double delta_time(t_time prev) {
	t_time t = now();

	t.tv_nsec -= prev.tv_nsec;
	if ((t.tv_sec -= prev.tv_sec) < 0) {
		t.tv_nsec += 1000000000;
		--t.tv_sec;
	}
	return (double)t.tv_sec * 1000. + (double)t.tv_nsec / 1000000.;
}

void sighandler(int signal) {
    if (signal == SIGALRM)
        print_s("got signal!\n");
    else if (signal == SIGINT) {
        print_s("stopping\n");
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
                print_s("got n\n");
                break;
            case 't':
                print_s("got t with ");
                print_s(optarg);
                print_s("\n");
                break;
            case 1:
                print_s("got non arg ");
                print_s(optarg);
                print_s("\n");
                break;
            case '?':
                print_s("got unknown\n");
                break;
        }
    }
}

int main(int argc, char *const argv[]) {
    //args(argc, argv);
    //set_timer(1, 0);
    //set_sig_handler(SIGALRM, sighandler);
    //set_sig_handler(SIGINT, sighandler);
    //while (1) {}

    //double average = 0;
    //double count = 0;
    //average = cumulating_average(average, ++count, 5);
    //average = cumulating_average(average, ++count, 10);
    //average = cumulating_average(average, ++count, 15);
	//printf("average %f\n", average);
	//printf("size: %lu\n", sizeof(struct timeval));
	//struct timeval test = {};
	//long lcount = 0;
	//test = cumulating_average_of_time(test, ++lcount, (struct timeval){
	//	.tv_sec = 0,
	//	.tv_usec = 54000
	//});
	//test = cumulating_average_of_time(test, ++lcount, (struct timeval){
	//	.tv_sec = 0,
	//	.tv_usec = 54500
	//});
	//test = cumulating_average_of_time(test, ++lcount, (struct timeval){
	//	.tv_sec = 0,
	//	.tv_usec = 57000
	//});
	//test = cumulating_average_of_time(test, ++lcount, (struct timeval){
	//	.tv_sec = 0,
	//	.tv_usec = 48000
	//});
	//printf("average %ld, %ld\n", test.tv_sec, test.tv_usec);

	//t_time snap = now();
	//for (int i = 0; i < 100000000; ++i);
	//double delta = delta_time(snap);
	//printf("got delta: %.3fms\n", delta);

	t_ping_stats stats = {
		.sent = 1
	};
	t_time sent = now();
	for (int i = 0; i < 100000; ++i);
	insert_pong(&stats, delta_time(sent));
	print_stats(&stats);
	++stats.sent;
	sent = now();
	for (int i = 0; i < 100005; ++i);
	insert_pong(&stats, delta_time(sent));
	print_stats(&stats);
	++stats.sent;
	sent = now();
	for (int i = 0; i < 100000; ++i);
	insert_pong(&stats, delta_time(sent));
	print_stats(&stats);
	++stats.sent;
	sent = now();
	for (int i = 0; i < 100010; ++i);
	insert_pong(&stats, delta_time(sent));
	print_stats(&stats);
}