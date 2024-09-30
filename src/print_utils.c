/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hmartzolf <hmartzol@student.42.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/01 00:00:00 by hmartzolf         #+#    #+#             */
/*   Updated: 2023/01/01 00:00:00 by hmartzolf        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"

void	print(int fd, t_str parts[])
{
	size_t	len;
	int		i;

	if (parts == NULL)
		return ;
	i = 0;
	while (parts[i] != NULL)
	{
		len = 0;
		while (parts[i][len] != '\0')
			++len;
		write(fd, parts[i], len);
		++i;
	}
}

static size_t	base_len(t_i64 val, t_i64 base)
{
	size_t	len;

	len = 1;
	while (val >= base)
	{
		val /= base;
		++len;
	}
	return (len);
}

t_str	butoa(t_strbuff into, t_i64 val, t_i64 base)
{
	const char	char_set[17] = "0123456789ABCDEF";
	size_t		len;

	if (base < 2 || base > 16 || val < 0)
	{
		into[0] = 'e';
		into[1] = 'r';
		into[2] = 'r';
		into[3] = '\0';
		return (into);
	}
	len = base_len(val, base);
	into[len] = '\0';
	while (val >= base)
	{
		into[--len] = char_set[val % base];
		val /= base;
	}
	into[0] = char_set[val];
	return (into);
}

t_str	bu3(t_strbuff into, t_i64 val)
{
	unsigned int	i;

	into[0] = '0';
	into[1] = '0';
	into[2] = '0';
	into[3] = '\0';
	i = 3;
	while (val > 0)
	{
		if (i > 0)
			into[--i] = "0123456789"[val % 10];
		else
		{
			into[2] = into[1];
			into[1] = into[0];
			into[0] = "0123456789"[val % 10];
		}
		val /= 10;
	}
	return (into);
}
