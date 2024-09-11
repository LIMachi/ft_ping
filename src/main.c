/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
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
		.flags = NONE,
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

int	main(int argc, t_str argv[])
{
	if (parse_args(argc, argv))
		return (app()->error);
	if (app()->target == NULL)
	{
		print_e((t_str[3]){app()->app_name,
			": usage error: missing destination option", NULL});
		return (MISSING_DESTINATION);
	}
	if (getuid() != 0)
	{
		print_e((t_str[3]){app()->app_name,
			": usage error: need to be run as root", NULL});
		return (NEED_ROOT);
	}
	if (resolve_sock() || connect_sock())
		return (app()->error);
	set_sig_handler(SIGALRM, &sighandler);
	set_sig_handler(SIGINT, &sighandler);
	set_timer(1, 0);
	print_init();
	while (app()->running && app()->error == OK)
		receive_pong();
	print_stats();
	close(app()->sock);
	return (app()->error);
}
