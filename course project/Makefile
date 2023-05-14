CC = gcc
CFLAGS = -W -Wall -Wno-unused-parameter -Wno-unused-variable -std=c11 -pedantic
LDFLAGS = -lm

SRCS = main.c libs/my_alloc.c libs/my_btree.c libs/person.c libs/standart_btree.c
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
