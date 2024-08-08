#include <sys/time.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <math.h>

//since the expected timings of a ping are between 0.01ms (local) and 1s (hard limit, since we ping every second)
//we can store the time as a double (which makes the average and deviation calculation a lot simpler)
typedef struct {
	unsigned sent;
	unsigned received;
	double min;
	double max;
	double average;
	double deviation;
	double m2;
} t_ping_stats;

typedef enum {
	NONE = 0,
	QUIET = 1,
	VERBOSE = 2
} t_flags;

//helper typedef to not write 'struct timespec' everywhere
typedef struct timespec t_time;

void print_stats(t_ping_stats *stats);
void print_e(char *pre, char *err);
void print_s(char *str);