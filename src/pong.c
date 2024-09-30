/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pong.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hmartzolf <hmartzol@student.42.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/01 00:00:00 by hmartzolf         #+#    #+#             */
/*   Updated: 2023/01/01 00:00:00 by hmartzolf        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"

//TODO: checksum test, handle special cases in verbose mode
int	valid_pong(void *packet, ssize_t size)
{
	struct icmphdr	*head;

	if (size < sizeof(struct icmphdr))
		return (0);
	head = packet;
	if (head->type == ICMP_ECHOREPLY && size >= PING_H_SZ)
		return (head->un.echo.id == getpid());
	return (0);
}

int	handle_receive_error(ssize_t received)
{
	if (received == -1)
	{
		if (errno != EAGAIN && errno != EWOULDBLOCK)
		{
			print_e((t_str[4]){app()->app_name, ": socket error on receive: ",
				strerror(errno), NULL});
			app()->error = SOCKET_ERROR;
			return (1);
		}
		return (1);
	}
	if (received <= sizeof(struct iphdr))
	{
		print_e((t_str[3]){app()->app_name, ": malformed/truncated message",
			NULL});
		app()->error = INVALID_MESSAGE;
		return (1);
	}
	return (0);
}

t_exit_code	receive_pong(void)
{
	t_ping_msg		buffer;
	ssize_t			received;
	t_i64			delta;
	struct msghdr	msg_reader;

	msg_reader = (struct msghdr){
		.msg_iov = (struct iovec [1]){{
		.iov_base = &buffer,
		.iov_len = sizeof(buffer)}},
		.msg_iovlen = 1};
	received = recvmsg(app()->sock, &msg_reader, MSG_DONTWAIT);
	if (handle_receive_error(received))
		return (app()->error);
	if (valid_pong(&buffer.raw[IP_H_SZ], received - (ssize_t)IP_H_SZ))
	{
		delta = delta_time(*(t_time *)&buffer.raw[IP_H_SZ + PING_H_SZ], now());
		insert_pong(delta);
		if ((app()->flags & QUIET) != QUIET)
			print_pong(received - IP_H_SZ, delta, &buffer);
		if (app()->deadline > 0 && app()->received >= app()->deadline)
			app()->running = 0;
	}
	return (OK);
}
