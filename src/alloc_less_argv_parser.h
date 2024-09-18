/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   alloc_less_argv_parser.h                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hmartzolf <hmartzol@student.42.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/01 00:00:00 by hmartzolf         #+#    #+#             */
/*   Updated: 2023/01/01 00:00:00 by hmartzolf        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ALLOC_LESS_ARGV_PARSER_H
# define ALLOC_LESS_ARGV_PARSER_H

/**
 * helper typedef to store the value of argv (or any array of strings)
 */
typedef const char *const				*t_csa;

/**
 * a node contains an error value if nothing match (positive, will be
 * transformed to negative in the parser's result, as a positive return
 * indicate what was the last arg parsed)
 * an alternative node if no choice match (only used if error is 0)
 * and an array of choices that can match at this node
 */
typedef struct s_arg_parser_node		t_arg_parser_node;

/**
 * a choice contains a priority (lowest to highest, unsigned)
 * an alias: one letter/number/char that can match in the same group of another
 * alias. ex : -ab will match alias 'a' and alias 'b' if no label '-ab' matches
 * a label: string that will match a full argument, '--' will not be added
 * automatically, so a label 'ab' will be parsed 'ab', not '-ab' or '--ab'
 * take_arg: a boolean that determine if the next arg will be consumed by a
 * callback and should be skipped by the parser
 * on_match: a callback that will be called on match only once and respecting
 * the priority order (if priority is 1 but parser is in priority 0, skipped)
 * next_if_match: nullable node pointer to override the current node only on
 * match (does not require priority)
 * finalizer_if_match: replace the finalizer callback of the parser by this
 * callback if the parsing matched and priority is valid)
 * *
 * an alias of '\0' and a label of NULL will result in a choice that always
 * match
 * a priority of -1 (max unsigned value) will result in a node that stops the
 * parsing of the current node (used to mark the end of the array of choices),
 * you can use the macro END_CHOICE instead of using a node with priority -1
 */
typedef struct s_arg_parser_choice		t_arg_parser_choice;

/**
 * internal state of the parser
 */
typedef struct s_arg_parser_instance	t_arg_parser_instance;

/**
 * callback used when a choice and priority is matched
 * will have the choice itself as it's first arg, then
 * the parsed arg or the next arg (if 'take_arg' is true),
 * and finally a void pointer to your data that you passed to the parser (state)
 */
typedef unsigned int					(*t_arg_parser_matching_choice_handler)(
	const t_arg_parser_choice *const choice, const char *const arg,
	void *data);

/**
 * the finalizer will have your data as the first arg, the return value of the
 * parser as the second (negative: your error, positive: at what argv the parser
 * stopped), then argc + argv
 */
typedef int								(*t_arg_parser_finalizer)(void *data,
	const int ret, const int argc, t_csa argv);

struct	s_arg_parser_node {
	const unsigned int				error;
	const t_arg_parser_node *const	next;
	t_arg_parser_choice				*choices;
};

struct	s_arg_parser_choice {
	const unsigned int						priority;
	const char								alias;
	const char *const						label;
	const int								take_arg;
	t_arg_parser_matching_choice_handler	on_match;
	const t_arg_parser_node					*next_if_match;
	const t_arg_parser_finalizer			finalizer_if_match;
};

/**
 * helper typedef to declare a static array of choices
 */

typedef t_arg_parser_choice				t_choice_array[];

struct	s_arg_parser_instance {
	const t_arg_parser_node			*node;
	int								argc;
	t_csa							argv;
	int								arg;
	int								alias;
	unsigned int					err;
	void							*data;
	t_arg_parser_finalizer			finalizer;
};

/**
 * state based parser without allocations
 * @param argc explicit (first arg of your main, usually)
 * @param argv explicit (second arg of your main, usually)
 * @param root a t_arg_parser_node (see it's doc)
 * @param data a pointer to your data that will be passed to the callbacks
 * @return if a finalizer is set (see choices), the return of the finalizer, or
 * the return of the parser (negative: error defined in nodes, positive: last
 * arg parsed)
 */

int	parse_argv(int argc, t_csa argv, const t_arg_parser_node *root, void *data);

#endif
