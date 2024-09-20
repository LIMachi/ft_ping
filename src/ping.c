/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ping.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hmartzolf <hmartzol@student.42.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/01 00:00:00 by hmartzolf         #+#    #+#             */
/*   Updated: 2023/01/01 00:00:00 by hmartzolf        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"

t_u16	checksum(const void *const data, const size_t len)
{
	t_u32	acc;
	t_u16	*ptr;
	size_t	p;

	ptr = (t_u16 *)data;
	acc = 0;
	p = len;
	while (p > 1)
	{
		acc += (t_u32) * ptr++;
		p -= 2;
	}
	if (len & 1)
		acc += (t_u32) * (t_u8 *)ptr;
	while (acc & 0xFFFF0000)
		acc = (acc & 0xFFFF) + (acc >> 16);
	return (~(t_u16)acc);
}

t_exit_code	send_ping(void)
{
	t_ping_packet	packet;

	packet = (t_ping_packet){.header = {.type = ICMP_ECHO, .un.echo = {
		.id = getpid(), .sequence = app()->sent++}}};
	*(t_time*)&packet.raw[PING_H_SZ] = now();
	packet.header.checksum = checksum(&packet, sizeof(t_ping_packet));
	if (sendto(app()->sock, &packet, sizeof(t_ping_packet), 0,
			(const struct sockaddr *)&app()->target_addr,
			sizeof(struct sockaddr_in)) == -1)
	{
		--app()->sent;
		print_e((t_str[4]){app()->app_name, ": socket error on send: ",
			strerror(errno), NULL});
		app()->error = SOCKET_ERROR;
		return (SOCKET_ERROR);
	}
	return (OK);
}
