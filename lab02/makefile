CFLAGS = -W -Wall -Wno-unused-parameter -Wno-unused-variable -std=c11 -pedantic
CC = gcc

make: parent child

all: clean parent child

parent: parent.c
	$(CC) $(CFLAGS) parent.cpp -o parent

child: child.c
	$(CC) $(CFLAGS) child.cpp -o child

clean:
	@if [ -e parent ]; then rm -v parent; fi
	@if [ -e child ]; then rm -v child; fi
