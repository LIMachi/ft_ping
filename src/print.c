#include "ft_ping.h"

void print_e(str parts[]) {
    size_t len;

    if (parts == NULL)
        return;
    for (int i = 0; parts[i] != NULL; ++i) {
        len = 0;
        while (parts[i][len] != '\0')
            ++len;
        write(2, parts[i], len);
    }
    write(2, "\n", 1);
}

void print_s(str s) {
    size_t len;

    if (s == NULL)
        return;
    len = 0;
    while (s[len] != '\0')
        ++len;
    write(1, s, len);
}

void print_u(i64 val) {
    if (val >= 10)
        print_u(val / 10);
    write(1, &"0123456789"[val % 10], 1);
}

void print_x(i64 val) {
    if (val >= 0x10)
        print_x(val >> 4);
    write(1, &"0123456789ABCDEF"[val & 0xF], 1);
}

//prints the 3 most significant digit of a number
void print_u3(i64 val) {
    char buff[3] = { '0', '0', '0' };
    unsigned i = 3;

    while (val > 0) {
        if (i > 0)
            buff[--i] = "0123456789"[val % 10];
        else {
            buff[2] = buff[1];
            buff[1] = buff[0];
            buff[0] = "0123456789"[val % 10];
        }
        val /= 10;
    }
    write(1, buff, 3);
}

void print_ms(i64 ns) {
    print_u((ns / NS_MS) % 1000);
    write(1, ".", 1);
    print_u3((ns / NS_US) % 1000);
}

void print_stats() {
    print_s("\n--- ");
    print_s(app()->target);
    print_s(" ping statistics ---\n");
    print_u(app()->sent);
    print_s(" packets transmitted, ");
    print_u(app()->received);
    print_s(" packets received, ");
    print_u((100 - (app()->received * 100) / app()->sent));
    print_s("% packet loss\n");
    if (app()->received > 0) {
        print_s("round-trip min/avg/max/stddev = ");
        print_ms(app()->min);
        print_s("/");
        print_ms(app()->average);
        print_s("/");
        print_ms(app()->max);
        print_s("/");
        print_ms((i64)sqrt((double)app()->da2 / (double)app()->received));
        print_s(" ms\n");
    }
}

void print_init() {
    print_s("PING ");
    print_s(app()->target);
    print_s(" (");
    print_s(app()->resolved_target);
    print_s("): ");
    print_u(sizeof(t_ping_msg));
    print_s(" data bytes");
    if (app()->flags == VERBOSE) {
        int id = getpid();
        print_s(", id 0x");
        print_x(id);
        print_s(" = ");
        print_u(id);
    }
    write(1, "\n", 1);
}

void print_pong(ssize_t size, i64 delta, t_ping_msg *msg) {
    print_u(size);
    print_s(" bytes from ");
    print_s(app()->resolved_target);
    print_s(": icmp_seq=");
    print_u(msg->packet.header.un.echo.sequence);
    print_s(" ttl=");
    print_u(msg->ip_header.ttl);
    print_s(" time=");
    print_ms(delta);
    print_s(" ms\n");
}