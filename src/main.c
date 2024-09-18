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
#include "alloc_less_argv_parser.h"

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

t_exit_code	usage(const char *name)
{
	print_s("Usage\n  ");
	print_s(name);
	print_s(" [options] <destination>\n\nOptions:\n"
		"  <destination>                      "
		"dns name or ip address\n"
		"  -h / -u / -? / --help / --usage    "
		"show this usage\n"
		"  -v / --verbose                     "
		"show extra packet information\n"
		"  -q / --quiet                       "
		"quiet output (only show statistics at the end)\n\n");
	return (OK);
}

int	finalize(void *state, int ret, const int argc, t_csa argv)
{

}

t_exit_code	args(const int argc, t_csa argv)
{
	int							ret;
	static t_arg_parser_node	root = {0, NULL, NULL};

	if (argc < 2)
		return (usage(argv[0]));
	root.choices = (t_arg_parser_choice [4]){
	{0, 'h', "--help", 0, NULL, &root, NULL},
	{0, 'q', "--quiet", 0, NULL, &root, NULL},
	{0, 'v', "--verbose", 0, NULL, &root, NULL},
	{-1}};
	return (error(parse_argv(argc, argv, &root, NULL)));
}

int	main(const int argc, t_str argv[])
{
	if (/*parse_args(argc, argv)*/args(argc, argv))
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
