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
	print(1, (t_str []){"Usage\n  ", name, " [options] <destination>\n\n"
		"Options:\n"
		"  <destination>                              "
		"dns name or ip address\n"
		"  -h / -u / -? / --help / --usage            "
		"show this usage\n"
		"  -v / --verbose                             "
		"show extra packet information, list unexpected packets\n"
		"  -q / --quiet                               "
		"quiet output (only show statistics at the end)\n"
		"  -s <s> / --packet-size <s> (default: 16)   "
		"specify the size of the payload (not including the 8 bytes of header)"
		"\n  -l <qty> / --preload <qty> (default: 0)    "
		"send that quantity of packets as fast as possible on startup\n"
		"  -i <sec> / --interval <sec> (default: 1)   "
		"wait this many seconds between each request (defaults to 1)\n"
		"  -c <sec> / --count <sec> (0: disable)      "
		"wait to get this many response before stopping\n"
		"  -t <r> / --ttl <r> (default: 64)           "
		"set the ttl value of a packet (maximum number of routers it can go"
		"through)\n\n", NULL});
	return (1);
}

unsigned int	matcher(const t_arg_parser_choice *const choice,
	const char *const arg, void *data)
{
	if (choice->alias == 'h' || choice->alias == 'u' || choice->alias == '?')
		return (usage(app()->app_name));
	else if (choice->alias == 'q')
		app()->flags |= QUIET;
	else if (choice->alias == 'v')
		app()->flags |= VERBOSE;
	else if (choice->alias == 's')
	{
		app()->pack_size = (unsigned int)atoi(arg);
		if (app()->pack_size > MAX_PAYLOAD_SZ)
			app()->pack_size = MAX_PAYLOAD_SZ;
	}
	else if (choice->alias == 'l')
		app()->preload = (unsigned int)atoi(arg);
	else if (choice->alias == 'i')
	{
		app()->interval = atof(arg);
		if (app()->interval < 0.01f)
			app()->interval = 0.01f;
	}
	else if (choice->alias == 'c')
		app()->deadline = (unsigned int)atoi(arg);
	else if (choice->alias == 't')
		app()->ttl = (unsigned int)atoi(arg);
	return (0);
}

t_exit_code	args(const int argc, t_csa argv)
{
	int							ret;
	static t_arg_parser_node	root = {0, NULL, NULL};

	app()->app_name = argv[0];
	if (argc < 2)
		return (usage(argv[0]));
	root.choices = (t_arg_parser_choice []){
	{0, 'h', "--help", 0, &matcher, &root, NULL},
	{0, 'u', "--usage", 0, &matcher, &root, NULL},
	{0, '?', NULL, 0, &matcher, &root, NULL},
	{0, 'q', "--quiet", 0, &matcher, &root, NULL},
	{0, 'v', "--verbose", 0, &matcher, &root, NULL},
	{0, 's', "--packet-size", 1, &matcher, &root, NULL},
	{0, 'l', "--preload", 1, &matcher, &root, NULL},
	{0, 'i', "--interval", 1, &matcher, &root, NULL},
	{0, 'c', "--count", 1, &matcher, &root, NULL},
	{0, 't', "--ttl", 1, &matcher, &root, NULL}, {-1}};
	ret = parse_argv(argc - 1, &argv[1], &root, NULL);
	if (ret < 0)
		return (1);
	if (ret + 1 < argc)
		app()->target = argv[ret + 1];
	if (ret + 2 < argc)
		return (error(MULTIPLE_DESTINATION));
	return (0);
}

void	preload(void)
{
	while (app()->preload > 0 && app()->error == OK)
	{
		send_ping();
		--app()->preload;
	}
}

int	main(const int argc, t_str argv[])
{
	if (args(argc, argv))
		return (app()->error);
	if (app()->target == NULL)
	{
		print(2, (t_str[]){app()->app_name,
			": usage error: missing destination option\n", NULL});
		return (MISSING_DESTINATION);
	}
	if (getuid() != 0)
	{
		print(2, (t_str[]){app()->app_name,
			": usage error: need to be run as root\n", NULL});
		return (NEED_ROOT);
	}
	if (resolve_sock() || connect_sock())
		return (app()->error);
	set_sig_handler();
	set_timer((int)app()->interval, (int)(app()->interval * 1000000) % 1000000);
	print_init();
	preload();
	while (app()->running && app()->error == OK)
		receive_pong();
	print_stats();
	close(app()->sock);
	return (app()->error);
}
