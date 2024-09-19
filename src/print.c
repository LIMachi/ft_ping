/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hmartzolf <hmartzol@student.42.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/01 00:00:00 by hmartzolf         #+#    #+#             */
/*   Updated: 2023/01/01 00:00:00 by hmartzolf        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"

void	print_ms(t_i64 ns)
{
	print_u((ns / NS_MS) % 1000);
	write(1, ".", 1);
	print_u3((ns / NS_US) % 1000);
}

void	print_stats(void)
{
	print_s("\n--- ");
	print_s(app()->target);
	print_s(" ping statistics ---\n");
	print_u(app()->sent);
	print_s(" packets transmitted, ");
	print_u(app()->received);
	print_s(" packets received, ");
	print_u((100 - (app()->received * 100) / app()->sent));
	print_s("% packet loss\n");
	if (app()->received > 0)
	{
		print_s("round-trip min/avg/max/stddev = ");
		print_ms(app()->min);
		print_s("/");
		print_ms(app()->average);
		print_s("/");
		print_ms(app()->max);
		print_s("/");
		print_ms((t_i64)sqrt((double)app()->da2 / (double)app()->received));
		print_s(" ms\n");
	}
}

void	print_init(void)
{
	int	id;

	print_s("PING ");
	print_s(app()->target);
	print_s(" (");
	print_s(app()->resolved_target);
	print_s("): ");
	print_u(sizeof(t_ping_msg));
	print_s(" data bytes");
	if (app()->flags == VERBOSE)
	{
		id = getpid();
		print_s(", id 0x");
		print_x(id);
		print_s(" = ");
		print_u(id);
	}
	write(1, "\n", 1);
}

void	print_pong(ssize_t size, t_i64 delta, t_ping_msg *msg)
{
	print_u(size);
	print_s(" bytes from ");
	print_s(app()->resolved_target);
	print_s(": icmp_seq=");
	print_u(((t_ping_head*)&msg->raw[IP_H_SZ])->un.echo.sequence);
	print_s(" ttl=");
	print_u(msg->header.ttl);
	print_s(" time=");
	print_ms(delta);
	print_s(" ms\n");
}
