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

void	set_sig_handler(int signal, void (*handler)(int))
{
	struct sigaction	runner;

	runner = (struct sigaction){.sa_handler = handler};
	sigaction(signal, &runner, NULL);
}

void	sighandler(int signal)
{
	if (signal == SIGALRM)
		send_ping();
	else if (signal == SIGINT)
		app()->running = 0;
}
