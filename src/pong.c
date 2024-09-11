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

int			valid_pong(void *packet, ssize_t size)
{
	struct icmphdr	*head;

	if (size < sizeof(struct icmphdr))
		return (0);
	head = packet;
	if (head->type == ICMP_ECHOREPLY && size >= sizeof(t_ping_packet))
		return head->un.echo.id == getpid();
	return (0);
}

t_exit_code	receive_pong(void) {
	t_ping_msg buffer;
	ssize_t received;
	t_i64 delta;

	struct msghdr msg_reader = {
		.msg_iov = (struct iovec [1]){{
			.iov_base = &buffer,
			.iov_len = sizeof(buffer)
		}},
		.msg_iovlen = 1
	};

	if ((received = recvmsg(app()->sock, &msg_reader, MSG_DONTWAIT)) == -1) {
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