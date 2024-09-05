#ifndef FT_PING
#define FT_PING

#include <sys/time.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <math.h>
#include <netinet/ip_icmp.h>
#include <netdb.h>
#include <sys/socket.h>
#include <arpa/inet.h>

typedef __int64_t i64;
typedef __uint32_t u32;
typedef __uint16_t u16;
typedef __uint8_t u8;
typedef const char* str;

///helper typedef to not write 'struct timespec' everywhere
typedef struct timespec t_time;

#pragma pack(push, 1)
typedef struct {
    struct icmphdr  header;  //type == ICMP_ECHO -> ping, type == ICMP_ECHOREPLY -> pong
    t_time timestamp;
} t_ping_packet;

typedef struct {
    struct iphdr  ip_header;
    t_ping_packet packet;
} t_ping_msg;
#pragma pack(pop)

#define NS_US 1000
#define NS_MS 1000000
#define NS_S  1000000000

typedef enum {
    NONE = 0,
    QUIET = 1,
    VERBOSE = 2,
    HELP = 3
} t_flags;

typedef enum {
    UNKNOWN_ERROR = -1,
    OK = 0,
    UNKNOWN_ARG,
    MULTIPLE_DESTINATION,
    MISSING_DESTINATION,
    NEED_ROOT,
    UNKNOWN_TARGET,
    INET_ERROR,
    SOCKET_ERROR,
    INVALID_MESSAGE
} t_exit_code;

typedef struct {
    int running;
    t_exit_code error;
    str app_name;
    t_flags flags;
    str target;
    struct sockaddr_in target_addr;
    char resolved_target[INET_ADDRSTRLEN + 1];
    int socket;
    i64 sent;
    i64 received;
    i64 min;
    i64 max;
    i64 average;
    i64 da2;
} t_app;

//main.c
///singleton to get access to the app (will be used to make this code comply with the 42's norm latter)
t_app *app();

//print.c
///print the header (recap of the target and packet size)
void print_init();
///print the stats in a similar fashion to the standard ping command
void print_stats();
///print the information of the currently received pong message (if not in quiet mode)
void print_pong(ssize_t size, i64 delta, t_ping_msg *msg);
///print multiple strings to the stderr fd
void print_e(str parts[]);
///helper function to print a null terminated string
void print_s(str s);
///helper function to print an unsigned int recursively
void print_u(i64 val);

//timing.c
///update the stats by inserting a new delta time (in nanoseconds)
///will update the min and max field if needed
///will update the average and the squared deviation, and increase the received field
void insert_pong(i64 new);
///return the current time has given by the realtime clock (computer clock, changing it while ping runs might cause invalid intervals)
t_time now();
///calculate the delta time in nanosecond between now and a previous time stamp
///i64 is enough to store a delta of up to 213503 years in nanoseconds, we should be good on that front :P
i64 delta_time(t_time prev, t_time now);
///start a timer that will emit a SIGALRM every time it expires (looping)
void set_timer(int secs, int micros);

//socket.c
///
t_exit_code resolve_sock();
///
t_exit_code connect_sock();

#endif