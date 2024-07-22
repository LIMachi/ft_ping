NAME = ft_ping

SRCS = src/main.c

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
	make

${NAME}: ${OBJS}
	gcc $^ -o $@

-include ${DEPS}

.deps/%.d: src/%.c
	@mkdir -p .deps
	@gcc -M -MT $(patsubst src/%.c, .objs/%.o, $<) ${INCLUDES} $< > $@
	@printf "\t@mkdir -p .objs\n\tgcc ${INCLUDES} -c $< -o $(patsubst src/%.c, .objs/%.o, $<)\n\t" >> $@