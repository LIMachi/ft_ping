#include "ft_ping.h"

void set_sig_handler(int signal, void (*handler)(int)) {
    struct sigaction runner = {
        .sa_handler = handler
    };

    sigaction(signal, &runner, NULL);
}

int strmatch(str s1, str s2) {
    if (s1 == NULL || s2 == NULL)
        return 0;
    int i = 0;
    while (s1[i] == s2[i] && s1[i] != '\0')
        ++i;
    return s1[i] == s2[i];
}

u16 checksum(const void *const data, const size_t len) {
    u32 acc = 0;
    u16 *ptr = (u16*)data;

    for (size_t p = len; p > 1; p -= 2)
        acc += (u32)*ptr++;
    if (len & 1)
        acc += (u32)*(u8*)ptr;
    while (acc & 0xFFFF0000)
        acc = (acc & 0xFFFF) + (acc >> 16);
    return ~(u16)acc;
}

t_exit_code send_ping() {
    t_ping_packet packet = {
        .header = {
            .type = ICMP_ECHO,
            .un.echo = {
                .id = getpid(),
                .sequence = app()->sent++
            }
        },
        .timestamp = now()
    };

    packet.header.checksum = checksum(&packet, sizeof(t_ping_packet));
    if (sendto(app()->socket, &packet, sizeof(t_ping_packet), 0, (const struct sockaddr*)&app()->target_addr, sizeof(struct sockaddr_in)) == -1) {
        --app()->sent;
        //TODO: print error
        return app()->error = SOCKET_ERROR;
    }
    return OK;
}

int valid_pong(void *packet, ssize_t size) {
    struct icmphdr *head;

    if (size < sizeof(struct icmphdr))
        return 0;
    head = packet;
    if (head->type == ICMP_ECHOREPLY && size >= sizeof(t_ping_packet))
        return head->un.echo.id == getpid();
    return 0;
}

t_exit_code receive_pong() {
    t_ping_msg buffer;
    ssize_t received;
    i64 delta;

    struct msghdr msg_reader = {
        .msg_iov = (struct iovec [1]){{
            .iov_base = &buffer,
            .iov_len = sizeof(buffer)
        }},
        .msg_iovlen = 1
    };

    if ((received = recvmsg(app()->socket, &msg_reader, MSG_DONTWAIT)) == -1) {
        if (errno != EAGAIN && errno != EWOULDBLOCK) {
            //TODO: error
            return app()->error = SOCKET_ERROR;
        }
        return OK;
    }

    if (received <= sizeof(struct iphdr)) {
        //TODO: error
        return app()->error = INVALID_MESSAGE;
    }
    if (valid_pong(&buffer.packet, received - (ssize_t)sizeof(struct iphdr))) {
        delta = delta_time(buffer.packet.timestamp, now());
        insert_pong(delta);
        if (app()->flags != QUIET)
            print_pong(received, delta, &buffer);
    }
    return OK;
}

t_exit_code parse_args(int argc, str argv[]) {
    int get_flags = 1;

    for (int i = 1; i < argc; ++i) {
        if (argv[i][0] == '-' && argv[i][1] != '\0' && get_flags) {
            if (argv[i][1] == '-') {
                if (argv[i][2] != '\0') { //--<arg> long arg format
                    if (strmatch(&argv[i][2], "help") || strmatch(&argv[i][2], "usage")) {
                        app()->flags = HELP;
                        return OK;
                    } else if (strmatch(&argv[i][2], "verbose"))
                        app()->flags |= VERBOSE;
                    else if (strmatch(&argv[i][2], "quiet"))
                        app()->flags |= QUIET;
                    else {
                        print_e((str[4]){argv[0], ": usage error: invalid arg ", &argv[i][0], NULL});
                        return app()->error = UNKNOWN_ARG;
                    }
                } else //--, end of arg parsing
                    get_flags = 0;
            } else { //-<args> short arg format
                for (int j = 1; argv[i][j] != '\0'; ++j)
                    switch (argv[i][j]) {
                        case 'h':
                        case 'u':
                        case '?':
                            app()->flags = HELP;
                            return OK;
                        case 'v':
                            app()->flags |= VERBOSE;
                            break;
                        case 'q':
                            app()->flags |= QUIET;
                            break;
                        default:
                            print_e((str[4]){argv[0], ": usage error: invalid arg(s) ", &argv[i][j], NULL});
                            return app()->error = UNKNOWN_ARG;
                    }
            }
        } else { //non - preceded arg, probably the address
            if (app()->target == NULL)
                app()->target = argv[i];
            else
                return app()->error = MULTIPLE_DESTINATION;
        }
    }
    return OK;
}

t_app *app() {
    static t_app sapp = {
        .running = 1,
        .error = OK,
        .flags = NONE,
        .target = NULL,
        .socket = -1,
        .sent = 0,
        .received = 0,
        .min = 0,
        .max = 0,
        .average = 0,
        .da2 = 0
    };

    return &sapp;
}

void sighandler(int signal) {
    if (signal == SIGALRM)
        send_ping();
    else if (signal == SIGINT)
        app()->running = 0;
}

int main(int argc, str argv[]) {
    app()->app_name = argv[0];

    if (parse_args(argc, argv))
        return app()->error;

    if (app()->flags == HELP) {
        print_s("Usage\n  ");
        print_s(argv[0]);
        print_s(" [options] <destination>\n\nOptions:\n"
                "  <destination>                      dns name or ip address\n"
                "  -h / -u / -? / --help / --usage    show this usage\n"
                "  -v / --verbose                     show extra packet information\n"
                "  -q / --quiet                       quiet output (only show statistics at the end)\n\n");
        return OK;
    }
    if (app()->target == NULL) {
        print_e((str[3]){app()->app_name, ": usage error: missing destination option", NULL});
        return MISSING_DESTINATION;
    }
    if (getuid() != 0) {
        print_e((str[3]){app()->app_name, ": usage error: need to be run as root", NULL});
        return NEED_ROOT;
    }
    if (resolve_sock() || connect_sock())
        return app()->error;
    set_sig_handler(SIGALRM, &sighandler);
    set_sig_handler(SIGINT, &sighandler);
    set_timer(1, 0);
    print_init();
    while (app()->running && app()->error == OK)
        receive_pong();
    print_stats();
    close(app()->socket);
    return app()->error;
}