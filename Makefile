NAME = ft_ping

SRCS = src/main.c src/print.c src/timing.c src/socket.c src/print_utils.c src/alloc_less_argv_parser.c src/signal.c src/ping.c src/pong.c src/app.c

INCLUDES = -I src
OBJS = $(patsubst src/%.c, .objs/%.o, $(SRCS))
DEPS = $(patsubst src/%.c, .deps/%.d, $(SRCS))

.PHONY: all, ${NAME}, clean, fclean, re, test
.PRECIOUS: ${DEPS}

all: ${NAME}

test: all
	./${NAME} 0.0.0.0

clean:
	rm -rf .deps .objs

fclean: clean
	rm -f ${NAME}

re: fclean
	make ${NAME}

${NAME}: ${OBJS}
	gcc ${INCLUDES} $^ -o $@ -lm

norm:
	clear
	norminette ${SRCS}

-include ${DEPS}

.deps/%.d: src/%.c
	@mkdir -p .deps
	@gcc -M -MT $(patsubst src/%.c, .objs/%.o, $<) ${INCLUDES} $< > $@
	@printf "\t@mkdir -p .objs\n\tgcc ${INCLUDES} -c $< -o $(patsubst src/%.c, .objs/%.o, $<)\n\t" >> $@