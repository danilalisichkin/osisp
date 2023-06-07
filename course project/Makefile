CC = gcc
CFLAGS = -W -Wall -Werror -Wno-unused-parameter -Wno-unused-variable -Wredundant-decls -finput-charset=UTF-8 -std=c11 -pedantic -D_GNU_SOURCE
LDFLAGS = -lm

SRCS = main.c libs/my_alloc.c libs/my_btree.c libs/person.c libs/standard_btree.c libs/utilities.c libs/statistic.c
OBJS = $(SRCS:.c=.o)

make: main

all: clean main

main: $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	@if [ -e main ]; then rm -v $(OBJS) main; fi
	@if [ -e became.txt ]; then rm -v became.txt; fi

