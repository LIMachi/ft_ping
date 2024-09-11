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

void	print_e(t_str parts[])
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
		write(2, parts[i], len);
		++i;
	}
	write(2, "\n", 1);
}

void	print_s(t_str s)
{
	size_t	len;

	if (s == NULL)
		return ;
	len = 0;
	while (s[len] != '\0')
		++len;
	write(1, s, len);
}

void	print_u(t_i64 val)
{
	if (val >= 10)
		print_u(val / 10);
	write(1, &"0123456789"[val % 10], 1);
}

void	print_x(t_i64 val)
{
	if (val >= 0x10)
		print_x(val >> 4);
	write(1, &"0123456789ABCDEF"[val & 0xF], 1);
}

void	print_u3(t_i64 val)
{
	char			buff[3];
	unsigned int	i;

	buff[0] = '0';
	buff[1] = '0';
	buff[2] = '0';
	i = 3;
	while (val > 0)
	{
		if (i > 0)
			buff[--i] = "0123456789"[val % 10];
		else
		{
			buff[2] = buff[1];
			buff[1] = buff[0];
			buff[0] = "0123456789"[val % 10];
		}
		val /= 10;
	}
	write(1, buff, 3);
}
