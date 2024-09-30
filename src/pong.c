/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pong.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hmartzolf <hmartzol@student.42.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/01 00:00:00 by hmartzolf         #+#    #+#             */
/*   Updated: 2023/01/01 00:00:00 by hmartzolf        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"

void	verbose_unreachable_response(int code, t_strbuff buff)
{
	static const t_str	messages[NR_ICMP_UNREACH + 1] = {
	[ICMP_NET_UNREACH] = "Destination Net Unreachable\n",
	[ICMP_HOST_UNREACH] = "Destination Host Unreachable\n",
	[ICMP_PROT_UNREACH] = "Destination Protocol Unreachable\n",
	[ICMP_PORT_UNREACH] = "Destination Port Unreachable\n",
	[ICMP_FRAG_NEEDED] = "Frag needed\n",
	[ICMP_SR_FAILED] = "Source Route Failed\n",
	[ICMP_NET_UNKNOWN] = "Destination Net Unknown\n",
	[ICMP_HOST_UNKNOWN] = "Destination Host Unknown\n",
	[ICMP_HOST_ISOLATED] = "Source Host Isolated\n",
	[ICMP_NET_ANO] = "Destination Net Prohibited\n",
	[ICMP_HOST_ANO] = "Destination Host Prohibited\n",
	[ICMP_NET_UNR_TOS] = "Destination Net Unreachable for Type of Service\n",
	[ICMP_HOST_UNR_TOS] = "Destination Host Unreachable for Type of Service\n",
	[ICMP_PKT_FILTERED] = "Packet filtered\n",
	[ICMP_PREC_VIOLATION] = "Precedence Violation\n",
	[ICMP_PREC_CUTOFF] = "Precedence Cutoff\n"};

	if (code <= NR_ICMP_UNREACH)
		print(1, (t_str[]){messages[code], NULL});
	else
		print(1, (t_str[]){"Dest Unreachable, Bad Code: ",
			butoa(buff, code, 10), "\n", NULL});
}

//debug print some info on received packet in verbose mode as stated in man:
//'ICMP packets other than ECHO_RESPONSE that are received are listed.'
void	verbose_invalid_pong(int type, int code)
{
	t_strbuff	buff;

	if (type == ICMP_SOURCE_QUENCH)
		write(1, "Source Quench\n", 14);
	else if (type == ICMP_REDIRECT)
	{
		if (code == ICMP_REDIR_NET)
			write(1, "Redirect Network\n", 17);
		else if (code == ICMP_REDIR_HOST)
			write(1, "Redirect Host\n", 14);
		else if (code == ICMP_REDIR_NETTOS)
			write(1, "Redirect Type of Service and Network\n", 37);
		else
			print(1, (t_str[]){"Redirect, Bad Code: ", butoa(buff, code, 10),
				"\n", NULL});
	}
	else if (type == ICMP_DEST_UNREACH)
		verbose_unreachable_response(code, buff);
}

int	valid_pong(void *packet, ssize_t size, uint32_t source)
{
	struct icmphdr	*head;
	t_strbuff		buff;

	if (size < sizeof(struct icmphdr))
	{
		if (app()->flags & VERBOSE)
			print(2, (t_str[]){app()->app_name, ": packet too short (",
				butoa(buff, size, 10), " bytes) from ",
				inet_ntoa(*(struct in_addr *)&source), "\n", NULL});
		return (0);
	}
	head = packet;
	if (head->type == ICMP_ECHOREPLY && size >= PING_H_SZ
		&& head->un.echo.id == getpid())
		return (!checksum(packet, size) && check_fill(packet + PING_H_SZ,
				size - PING_H_SZ));
	if (head->type != ICMP_ECHOREPLY && app()->flags & VERBOSE)
	{
		print(1, (t_str[]){butoa(buff, size, 10), " bytes from ",
			inet_ntoa(*(struct in_addr *)&source), ": ", NULL});
		verbose_invalid_pong(head->type, head->code);
	}
	return (0);
}

int	handle_receive_error(ssize_t received)
{
	if (received == -1)
	{
		if (errno != EAGAIN && errno != EWOULDBLOCK)
		{
			print(2, (t_str[]){app()->app_name, ": socket error on receive: ",
				strerror(errno), "\n", NULL});
			app()->error = SOCKET_ERROR;
			return (1);
		}
		return (1);
	}
	if (received <= sizeof(struct iphdr))
	{
		print(2, (t_str[]){app()->app_name, ": malformed/truncated message\n",
			NULL});
		app()->error = INVALID_MESSAGE;
		return (1);
	}
	return (0);
}

t_exit_code	receive_pong(void)
{
	t_ping_msg		buffer;
	ssize_t			received;
	t_i64			delta;
	struct msghdr	msg_reader;

	msg_reader = (struct msghdr){.msg_iov = (struct iovec [1]){{
		.iov_base = &buffer, .iov_len = sizeof(buffer)}}, .msg_iovlen = 1};
	received = recvmsg(app()->sock, &msg_reader, MSG_DONTWAIT);
	if (handle_receive_error(received))
		return (app()->error);
	if (valid_pong(&buffer.raw[IP_H_SZ], received - (ssize_t)IP_H_SZ,
			buffer.header.saddr))
	{
		if (received >= IP_H_SZ + PING_H_SZ + TIME_SZ)
			delta = delta_time(*(t_time *)&buffer.raw[IP_H_SZ + PING_H_SZ],
					now());
		else
			delta = delta_time(app()->last_sent, now());
		insert_pong(delta);
		if ((app()->flags & QUIET) != QUIET)
			print_pong(received - IP_H_SZ, delta, &buffer);
		if (app()->deadline > 0 && app()->received >= app()->deadline)
			app()->running = 0;
	}
	return (OK);
}
