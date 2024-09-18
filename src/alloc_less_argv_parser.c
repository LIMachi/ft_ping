/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   alloc_less_argv_parser.c                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hmartzolf <hmartzol@student.42.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/01 00:00:00 by hmartzolf         #+#    #+#             */
/*   Updated: 2023/01/01 00:00:00 by hmartzolf        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "alloc_less_argv_parser.h"

int	i_match(const t_arg_parser_instance *const instance,
	unsigned int choice, const char *const arg)
{
	int					i;
	const char *const	label = instance->node->choices[choice].label;

	if (instance->node->choices[choice].alias == '\0'
		&& instance->node->choices[choice].label == 0)
		return (1);
	if (arg == 0)
		return (0);
	if (instance->alias > 0)
		return (arg[0] == instance->node->choices[choice].alias);
	if (!label)
		return (0);
	i = 0;
	while (arg[i] != '\0' && label[i] != '\0' && arg[i] == label[i])
		++i;
	return (arg[i] == label[i]);
}

int	match(t_arg_parser_instance *const p,
	t_arg_parser_choice **found)
{
	unsigned int		c;
	const char *const	a = (char *)&p->argv[p->arg][p->alias];

	c = 0;
	while (p->node->choices[c].priority != (unsigned)-1 && !i_match(p, c, a))
		++c;
	if (p->node->choices[c].priority == (unsigned)-1 && p->alias == 0
		&& a[0] == '-' && a[1] != '-')
	{
		p->alias = 1;
		c = 0;
		while (p->node->choices[c].priority != (unsigned)-1
			&& !i_match(p, c, &a[1]))
			++c;
	}
	if (p->node->choices[c].priority == (unsigned)-1)
	{
		p->err = p->node->error;
		p->node = p->node->next;
		return (0);
	}
	*found = &p->node->choices[c];
	return (1);
}

const char	*take_next(t_arg_parser_instance *parser, int take_next)
{
	const char	*out;

	if (take_next)
	{
		if (parser->alias > 0
			&& parser->argv[parser->arg][parser->alias + 1] != '\0')
			out = &parser->argv[parser->arg][parser->alias + 1];
		else
			out = parser->argv[++parser->arg];
		parser->alias = 0;
		++parser->arg;
		return (out);
	}
	out = &parser->argv[parser->arg][parser->alias];
	if (parser->alias > 0
		&& parser->argv[parser->arg][parser->alias + 1] != '\0')
		++parser->alias;
	else
	{
		parser->alias = 0;
		++parser->arg;
	}
	return (out);
}

t_arg_parser_instance	internal_parse(t_arg_parser_instance p,
	unsigned int *priority, unsigned int *max_priority, unsigned int sub)
{
	t_arg_parser_choice			*m;
	const t_arg_parser_instance	t = p;

	while (p.arg < p.argc && p.node != 0 && p.err == 0)
	{
		if (!match(&p, &m))
			continue ;
		if (m->priority > *max_priority)
			*max_priority = m->priority;
		if (m->priority < sub && m->priority > *priority)
			p.err = internal_parse(p, priority, max_priority, m->priority).err;
		else
		{
			p.node = m->next_if_match;
			if (m->priority == *priority && m->finalizer_if_match)
				p.finalizer = m->finalizer_if_match;
			if (m->priority == *priority && m->on_match)
				p.err = m->on_match(m, take_next(&p, m->take_arg), p.data);
			else
				(void) take_next(&p, m->take_arg);
		}
	}
	if (++*priority > *max_priority || p.err != 0)
		return (p);
	return (t);
}

int	parse_argv(const int argc, t_csa argv,
	const t_arg_parser_node *root, void *data)
{
	t_arg_parser_instance		instance;
	unsigned int				max_priority;
	unsigned int				priority;
	int							err;

	max_priority = 0;
	priority = 0;
	instance = (t_arg_parser_instance){root, argc, argv, 0, 0, 0, data, 0};
	instance = internal_parse(instance, &priority, &max_priority, -1);
	if (instance.err > 0)
		err = -(int)instance.err;
	else
		err = instance.arg;
	if (instance.finalizer != 0)
		return (instance.finalizer(instance.data, err, argc, argv));
	return (err);
}
