/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   args.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hmartzolf <hmartzol@student.42.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/01 00:00:00 by hmartzolf         #+#    #+#             */
/*   Updated: 2023/01/01 00:00:00 by hmartzolf        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"

int			strmatch(t_str s1, t_str s2)
{
	if (s1 == NULL || s2 == NULL)
		return (0);
	int i = 0;
	while (s1[i] == s2[i] && s1[i] != '\0')
		++i;
	return (s1[i] == s2[i]);
}

void	usage(void)
{
	print_s("Usage\n  ");
	print_s(app()->app_name);
	print_s(" [options] <destination>\n\nOptions:\n"
		"  <destination>                      "
		"dns name or ip address\n"
		"  -h / -u / -? / --help / --usage    "
		"show this usage\n"
		"  -v / --verbose                     "
		"show extra packet information\n"
		"  -q / --quiet                       "
		"quiet output (only show statistics at the end)\n\n");
}

t_exit_code	parse_args(int argc, t_str argv[])
{
	int	get_flags;

	app()->app_name = argv[0];
	get_flags = 1;
	for (int i = 1; i < argc; ++i) {
		if (argv[i][0] == '-' && argv[i][1] != '\0' && get_flags) {
			if (argv[i][1] == '-') {
				if (argv[i][2] != '\0') { //--<arg> long arg format
					if (strmatch(&argv[i][2], "help") || strmatch(&argv[i][2], "usage")) {
						app()->flags = HELP;
						return OK;
					} else if (strmatch(&argv[i][2], "verbose"))
						app()->flags |= VERBOSE;
					else if (strmatch(&argv[i][2], "quiet"))
						app()->flags |= QUIET;
					else {
						print_e((t_str[4]){argv[0], ": usage error: invalid arg ", &argv[i][0], NULL});
						return app()->error = UNKNOWN_ARG;
					}
				} else //--, end of arg parsing
					get_flags = 0;
			} else { //-<args> short arg format
				for (int j = 1; argv[i][j] != '\0'; ++j)
					switch (argv[i][j]) {
						case 'h':
						case 'u':
						case '?':
							app()->flags = HELP;
						return OK;
						case 'v':
							app()->flags |= VERBOSE;
						break;
						case 'q':
							app()->flags |= QUIET;
						break;
						default:
							print_e((t_str[4]){argv[0], ": usage error: invalid arg(s) ", &argv[i][j], NULL});
						return app()->error = UNKNOWN_ARG;
					}
			}
		} else { //non - preceded arg, probably the address
			if (app()->target == NULL)
				app()->target = argv[i];
			else
				return app()->error = MULTIPLE_DESTINATION;
		}
	}
	if (app()->flags == HELP)
	{
		usage();
		return (-1);
	}
	return OK;
}