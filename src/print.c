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

void	print_stats(void)
{
	t_i64		percent;
	t_i64		stddev;
	t_strbuff	buffs[11];

	if (app()->sent > 0)
		percent = (100 - (app()->received * 100) / app()->sent);
	else
		percent = 0;
	print(1, (t_str[]){"\n--- ", app()->target, " ping statistics ---\n",
		butoa(buffs[0], app()->sent, 10), " packets transmitted, ",
		butoa(buffs[1], app()->received, 10), " packets received, ",
		butoa(buffs[2], percent, 10), "% packet loss\n", NULL});
	if (app()->received > 0)
	{
		stddev = (t_i64)sqrt((double)app()->da2 / (double)app()->received);
		print(1, (t_str[]){"round-trip min/avg/max/stddev = ",
			butoa(buffs[3], (app()->min / NS_MS) % 1000, 10), ".",
			bu3(buffs[4], (app()->min / NS_US) % 1000), "/",
			butoa(buffs[5], (app()->average / NS_MS) % 1000, 10), ".",
			bu3(buffs[6], (app()->average / NS_US) % 1000), "/",
			butoa(buffs[7], (app()->max / NS_MS) % 1000, 10), ".",
			bu3(buffs[8], (app()->max / NS_US) % 1000), "/",
			butoa(buffs[9], (stddev / NS_MS) % 1000, 10), ".",
			bu3(buffs[10], (stddev / NS_US) % 1000), " ms\n", NULL});
	}
}

void	print_init(void)
{
	int			id;
	t_strbuff	buffs[4];

	print(1, (t_str[]){"PING ", app()->target, " (", app()->resolved_target,
		"): ", butoa(buffs[0], app()->pack_size, 10), "(",
		butoa(buffs[1], app()->pack_size + PING_H_SZ + IP_H_SZ, 10),
		") data bytes", NULL});
	if ((app()->flags & VERBOSE) == VERBOSE)
	{
		id = getpid();
		print(1, (t_str[]){", id 0x", butoa(buffs[2], id, 16), " = ",
			butoa(buffs[3], id, 10), NULL});
	}
	write(1, "\n", 1);
}

void	print_pong(ssize_t size, t_i64 delta, t_ping_msg *msg)
{
	t_strbuff	buffs[5];

	print(1, (t_str[]){butoa(buffs[0], size, 10), " bytes from ",
		app()->resolved_target, ": icmp_seq=", butoa(buffs[1],
			((t_ping_head *) &msg->raw[IP_H_SZ])->un.echo.sequence, 10),
		" ttl=", butoa(buffs[2], msg->header.ttl, 10), " time=",
		butoa(buffs[3], (delta / NS_MS) % 1000, 10), ".",
		bu3(buffs[4], (delta / NS_US) % 1000), " ms\n", NULL});
}
