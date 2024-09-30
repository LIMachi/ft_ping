/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   socket.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hmartzolf <hmartzol@student.42.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/01 00:00:00 by hmartzolf         #+#    #+#             */
/*   Updated: 2023/01/01 00:00:00 by hmartzolf        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"

t_exit_code	resolve_sock(void)
{
	const struct addrinfo	req = {
		.ai_family = AF_INET,
		.ai_socktype = SOCK_RAW,
		.ai_protocol = IPPROTO_ICMP
	};
	struct addrinfo			*info;

	if (getaddrinfo(app()->target, NULL, &req, &info))
	{
		print(2, (t_str[]){app()->app_name, ": unknown target: ", app()->target,
			"\n", NULL});
		app()->error = UNKNOWN_TARGET;
		return (UNKNOWN_TARGET);
	}
	app()->target_addr = *(struct sockaddr_in *)info->ai_addr;
	freeaddrinfo(info);
	if (inet_ntop(AF_INET, &app()->target_addr.sin_addr,
			app()->resolved_target, INET_ADDRSTRLEN) == NULL)
	{
		print(2, (t_str[]){app()->app_name, ": inet_ntop error: ",
			strerror(errno), "\n", NULL});
		app()->error = INET_ERROR;
		return (INET_ERROR);
	}
	return (OK);
}

//tmp_sock is there because the norm fails to parse `app()->sock = `
t_exit_code	connect_sock(void)
{
	int			tmp_sock;

	tmp_sock = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	if (tmp_sock == -1)
	{
		print(2, (t_str[]){app()->app_name, ": socket error: ", strerror(errno),
			"\n", NULL});
		app()->error = SOCKET_ERROR;
		return (SOCKET_ERROR);
	}
	app()->sock = tmp_sock;
	if (app()->ttl > 255)
		app()->ttl = 255;
	if (setsockopt(tmp_sock, IPPROTO_IP, IP_TTL, &app()->ttl,
			sizeof(int)) == -1)
	{
		print(2, (t_str[]){app()->app_name, ": socket error: ", strerror(errno),
			"\n", NULL});
		close(tmp_sock);
		app()->error = SOCKET_ERROR;
		return (SOCKET_ERROR);
	}
	return (OK);
}
