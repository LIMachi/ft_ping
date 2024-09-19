/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_ping.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hmartzolf <hmartzol@student.42.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/01 00:00:00 by hmartzolf         #+#    #+#             */
/*   Updated: 2023/01/01 00:00:00 by hmartzolf        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_PING_H
# define FT_PING_H

# include <sys/time.h>
# include <signal.h>
# include <stdlib.h>
# include <unistd.h>
# include <string.h>
# include <errno.h>
# include <time.h>
# include <math.h>
# include <netinet/ip_icmp.h>
# include <netdb.h>
# include <sys/socket.h>
# include <arpa/inet.h>

typedef __int64_t		t_i64;
typedef __uint32_t		t_u32;
typedef __uint16_t		t_u16;
typedef __uint8_t		t_u8;
typedef const char*		t_str;

///helper typedefs to not write 'struct X' everywhere
//total size: 16 (no matter the platform or endian)
typedef struct timespec	t_time;
//total size: u8 * 2 + u16 + u32 so 8
typedef struct icmphdr	t_ping_head;
//total size: u8 * 4 + u16 * 4 + u32 * 2 so 20
typedef struct iphdr	t_ip_head;

# define TIME_SZ 16
# define PING_H_SZ 8
# define IP_H_SZ 20

typedef union u_ping_packet {
	t_u8		raw[PING_H_SZ + TIME_SZ];
	t_ping_head	header;
}						t_ping_packet;

typedef union u_ping_msg {
	t_u8		raw[IP_H_SZ + PING_H_SZ + TIME_SZ];
	t_ip_head	header;
}						t_ping_msg;

# define NS_US 1000
# define NS_MS 1000000
# define NS_S  1000000000

# define QUIET 1
# define VERBOSE 2

typedef enum e_exit_code {
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
}						t_exit_code;

typedef struct s_app {
	int					running;
	t_exit_code			error;
	t_str				app_name;
	int					flags;
	t_str				target;
	struct sockaddr_in	target_addr;
	char				resolved_target[INET_ADDRSTRLEN + 1];
	int					sock;
	t_i64				sent;
	t_i64				received;
	t_i64				min;
	t_i64				max;
	t_i64				average;
	t_i64				da2;
}						t_app;

//main.c
///singleton to get access to the app
///(will be used to make this code comply with the 42's norm latter)
t_app				*app(void);
///
t_exit_code			error(t_exit_code code);

//print_utils.c
///print multiple strings to the stderr fd
void				print_e(t_str parts[]);
///helper function to print a null terminated string
void				print_s(t_str s);
///helper function to print an unsigned int recursively
void				print_u(t_i64 val);
///print the 3 most sgnificant digits of a number
void				print_u3(t_i64 val);
///helper function to print an unsigned int recursively in hexadecimal
void				print_x(t_i64 val);

//print.c
///print the header (recap of the target and packet size)
void				print_init(void);
///print the stats in a similar fashion to the standard ping command
void				print_stats(void);
///print the information of the currently received pong message
///(if not in quiet mode)
void				print_pong(ssize_t size, t_i64 delta, t_ping_msg *msg);

//timing.c
///update the stats by inserting a new delta time (in nanoseconds)
///will update the min and max field if needed
///will update the average and the squared deviation,
///and increase the received field
void				insert_pong(t_i64 new);
///return the current time has given by the realtime clock
///(computer clock, changing it while ping runs might cause invalid intervals)
t_time				now(void);
///calculate the delta time in nanosecond between now and a previous time stamp
///i64 is enough to store a delta of up to 213503 years in nanoseconds,
///we should be good on that front :P
t_i64				delta_time(t_time prev, t_time now);
///start a timer that will emit a SIGALRM every time it expires (looping)
void				set_timer(int secs, int micros);

//socket.c
///
t_exit_code			resolve_sock(void);
///
t_exit_code			connect_sock(void);

//signal.c
///
void				sighandler(int signal);
///
void				set_sig_handler(int signal, void (*handler)(int));

//args.c
///
t_exit_code			parse_args(int argc, t_str argv[]);

//ping.c
///
t_u16				checksum(const void *data, size_t len);
///
t_exit_code			send_ping(void);

//pong.c
///
t_exit_code			receive_pong(void);

#endif