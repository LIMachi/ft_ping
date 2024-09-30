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

void	fill(char *buff, int len)
{
	char	c;
	int		i;

	i = 0;
	c = 'a';
	while (i < len)
	{
		buff[i] = c;
		++c;
		if (c > 'z')
			c = 'a';
		++i;
	}
}

int	check_fill(const char *buff, ssize_t len)
{
	char	c;
	ssize_t	l;

	if (len >= TIME_SZ)
	{
		len -= TIME_SZ;
		buff += TIME_SZ;
	}
	l = 0;
	c = 'a';
	while (l < len)
	{
		if (buff[l] != c)
		{
			print(2, (t_str[]){app()->app_name, ": error: received pong has "
				"malicious payload (passed checksum but payload is invalid)\n",
				NULL});
			return (0);
		}
		++c;
		if (c > 'z')
			c = 'a';
		++l;
	}
	return (1);
}

t_exit_code	send_ping(void)
{
	t_ping_packet	packet;

	packet = (t_ping_packet){.header = {.type = ICMP_ECHO, .un.echo = {
		.id = getpid(), .sequence = app()->sent++}}};
	app()->last_sent = now();
	if (app()->pack_size >= TIME_SZ)
	{
		*(t_time *) &packet.raw[PING_H_SZ] = app()->last_sent;
		fill((char *)&packet.raw[PING_H_SZ + TIME_SZ],
			app()->pack_size - TIME_SZ);
	}
	else
		fill((char *)&packet.raw[PING_H_SZ], app()->pack_size);
	packet.header.checksum = checksum(&packet, PING_H_SZ + app()->pack_size);
	if (sendto(app()->sock, &packet, PING_H_SZ + app()->pack_size, 0,
			(const struct sockaddr *)&app()->target_addr,
			sizeof(struct sockaddr_in)) == -1)
	{
		--app()->sent;
		print(2, (t_str[]){app()->app_name, ": socket error on send: ",
			strerror(errno), "\n", NULL});
		app()->error = SOCKET_ERROR;
		return (SOCKET_ERROR);
	}
	return (OK);
}
