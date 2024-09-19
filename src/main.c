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

int	usage(const char *name)
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
	return (1);
}

unsigned int	matcher(const t_arg_parser_choice *const choice,
	const char *const arg, void *data)
{
	if (choice->alias == 'h')
		return (usage(app()->app_name));
	else if (choice->alias == 'q')
		app()->flags |= QUIET;
	else if (choice->alias == 'v')
		app()->flags |= VERBOSE;
	return (0);
}

t_exit_code	args(const int argc, t_csa argv)
{
	int							ret;
	static t_arg_parser_node	root = {0, NULL, NULL};

	app()->app_name = argv[0];
	if (argc < 2)
		return (usage(argv[0]));
	root.choices = (t_arg_parser_choice [4]){
	{0, 'h', "--help", 0, &matcher, &root, NULL},
	{0, 'q', "--quiet", 0, &matcher, &root, NULL},
	{0, 'v', "--verbose", 0, &matcher, &root, NULL}, {-1}};
	ret = parse_argv(argc - 1, &argv[1], &root, NULL);
	if (ret < 0)
		return (1);
	if (ret < argc)
		app()->target = argv[ret + 1];
	return (0);
}

int	main(const int argc, t_str argv[])
{
	if (args(argc, argv))
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
