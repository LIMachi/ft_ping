#include "ft_ping.h"

void print_e(char *pre, char *err) {
	size_t len;

	if (pre == NULL)
		return;
	len = 0;
	while (pre[len] != '\0')
		++len;
	write(2, pre, len);
	if (err != NULL) {
		len = 0;
		while (err[len] != '\0')
			++len;
		write(2, err, len);
	}
	write(2, "\n", 1);
}

void print_s(char *str) {
	size_t len;

	if (str == NULL)
		return;
	len = 0;
	while (str[len] != '\0')
		++len;
	write(1, str, len);
}

void print_u(unsigned val) {
	if (val >= 10)
		print_u(val / 10);
	write(1, &"0123456789"[val % 10], 1);
}

//prints the 3 most significant digit of a number
void print_u3(unsigned val) {
	char buff[3] = { '0', '0', '0' };
	unsigned i = 3;

	while (val > 0) {
		if (i > 0)
			buff[--i] = "0123456789"[val % 10];
		else {
			buff[2] = buff[1];
			buff[1] = buff[0];
			buff[0] = "0123456789"[val % 10];
		}
		val /= 10;
	}
	write(1, buff, 3);
}

void print_stats(t_ping_stats *stats) {
	print_s("\n--- ");
	print_s("<ip/url>"); //FIXME
	print_s(" ping statistics ---\n");
	print_u(stats->sent);
	print_s(" packets transmitted, ");
	print_u(stats->received);
	print_s(" packets received, ");
	print_u((unsigned)((1. - (double)stats->received / (double)stats->sent) * 100.));
	print_s("% packet loss\n");
	if (stats->received > 0) {
		print_s("round-trip min/avg/max/stddev = ");
		print_u((unsigned)stats->min);
		print_s(".");
		print_u3((unsigned)(stats->min * 1000.));
		print_s("/");
		print_u((unsigned)stats->average);
		print_s(".");
		print_u3((unsigned)(stats->average * 1000.));
		print_s("/");
		print_u((unsigned)stats->max);
		print_s(".");
		print_u3((unsigned)(stats->max * 1000.));
		print_s("/");
		print_u((unsigned)stats->deviation);
		print_s(".");
		print_u3((unsigned)(stats->deviation * 1000.));
		print_s(" ms\n");
	}
}