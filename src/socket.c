#include "ft_ping.h"

t_exit_code resolve_sock() {
    struct addrinfo req = {
        .ai_family = AF_INET,
		.ai_socktype = SOCK_RAW,
		.ai_protocol = IPPROTO_ICMP
    };
    struct addrinfo *info;

    if (getaddrinfo(app()->target, NULL, &req, &info)) {
        print_e((str[4]){app()->app_name, ": unknown target: ", app()->target, NULL});
        return app()->error = UNKNOWN_TARGET;
    }
    app()->target_addr = *(struct sockaddr_in*)info->ai_addr;
    freeaddrinfo(info);
    if (inet_ntop(AF_INET, &app()->target_addr.sin_addr, app()->resolved_target, INET_ADDRSTRLEN) == NULL) {
        print_e((str[4]){app()->app_name, ": inet_ntop error: ", strerror(errno), NULL});
        return app()->error = INET_ERROR;
    }
    return OK;
}

t_exit_code connect_sock() {
    const int ttl = 64;

    if ((app()->socket = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) == -1) {
        print_e((str[4]){app()->app_name, ": socket error: ", strerror(errno), NULL});
        return SOCKET_ERROR;
    }
    if (setsockopt(app()->socket, IPPROTO_IP, IP_TTL, &ttl, sizeof(int)) == -1) {
        print_e((str[4]){app()->app_name, ": socket error: ", strerror(errno), NULL});
        close(app()->socket);
        return SOCKET_ERROR;
    }
    return OK;
}