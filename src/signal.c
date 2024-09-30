/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signal.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hmartzolf <hmartzol@student.42.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/01 00:00:00 by hmartzolf         #+#    #+#             */
/*   Updated: 2023/01/01 00:00:00 by hmartzolf        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"

void	sighandler(int signal)
{
	if (signal == SIGALRM)
		send_ping();
	else if (signal == SIGINT)
		app()->running = 0;
}

void	set_sig_handler(void)
{
	struct sigaction	alarm_runner;
	struct sigaction	interupt_runner;

	interupt_runner = (struct sigaction){.sa_handler = sighandler};
	sigaction(SIGINT, &interupt_runner, NULL);
	alarm_runner = (struct sigaction){.sa_handler = sighandler};
	sigaction(SIGALRM, &alarm_runner, NULL);
}
