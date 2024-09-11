/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   timing.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hmartzolf <hmartzol@student.42.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/01 00:00:00 by hmartzolf         #+#    #+#             */
/*   Updated: 2023/01/01 00:00:00 by hmartzolf        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"

void	insert_pong(t_i64 new)
{
	t_i64	delta;
	t_i64	off;

	if (new < app()->min || app()->received == 0)
		app()->min = new;
	if (new > app()->max || app()->received == 0)
		app()->max = new;
	++app()->received;
	delta = new - app()->average;
	app()->average += delta / app()->received;
	off = new - app()->average;
	app()->da2 += delta * off;
}

t_time	now(void)
{
	t_time	out;

	if (clock_gettime(CLOCK_REALTIME, &out) == -1)
	{
		print_e((t_str[3]){"clock_gettime error: ", strerror(errno), NULL});
		exit(-1);
	}
	return (out);
}

//return the delta time in ns since the given time (using the current time)
t_i64	delta_time(t_time prev, t_time now)
{
	now.tv_nsec -= prev.tv_nsec;
	now.tv_sec -= prev.tv_sec;
	if (now.tv_sec < 0)
	{
		now.tv_nsec += NS_S;
		--now.tv_sec;
	}
	return ((t_i64)now.tv_sec * NS_S + (t_i64)now.tv_nsec);
}

void	set_timer(int secs, int micros)
{
	struct itimerval	timer;

	timer = (struct itimerval){
		.it_interval = {
		.tv_sec = secs,
		.tv_usec = micros
	},
		.it_value = {
		.tv_sec = secs,
		.tv_usec = micros
	}};
	setitimer(ITIMER_REAL, &timer, NULL);
}
