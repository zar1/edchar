CC = gcc
LDFLAGS = -lncurses
#DEBUG = -DDEBUG
CFLAGS = -g $(LDFLAGS) $(DEBUG)

all: edchar

edchar: edchar.c main.c
	$(CC) -o edchar edchar.c main.c $(CFLAGS)

clean:
	rm edchar
