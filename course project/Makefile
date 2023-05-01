CC = gcc
CFLAGS = -W -Wall -Wno-unused-parameter -Wno-unused-variable -std=c11 -pedantic
LDFLAGS = -lm

SRCS = main.c my_alloc.c my_btree.c person.c standart_btree.c
OBJS = $(SRCS:.c=.o)

make: main

all: clean main

main: $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	@if [ -e main ]; then rm -v $(OBJS) main; fi
