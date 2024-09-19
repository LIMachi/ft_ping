/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   app.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hmartzolf <hmartzol@student.42.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/01 00:00:00 by hmartzolf         #+#    #+#             */
/*   Updated: 2023/01/01 00:00:00 by hmartzolf        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"

t_app	*app(void)
{
	static t_app	sapp = {
		.running = 1,
		.error = OK,
		.flags = 0,
		.target = NULL,
		.sock = -1,
		.sent = 0,
		.received = 0,
		.min = 0,
		.max = 0,
		.average = 0,
		.da2 = 0
	};

	return (&sapp);
}

t_exit_code	error(t_exit_code code)
{
	app()->error = code;
	return (code);
}
