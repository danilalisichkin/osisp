CFLAGS = -W -Wall -Wno-unused-parameter -Wno-unused-variable -std=c11 -pedantic
CC = gcc

make: main

all: clean main

main: main.c
	$(CC) $(CFLAGS) main.c -o main

clean:
	@if [ -e main ]; then rm -v main; fi
