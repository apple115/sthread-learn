CC = gcc
CFLAGS = -Wall
PROGS = sthread

all:$(PROGS)

sthread: main.c scheduler.c list.h scheduler.h
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	$(RM) $(PROGS) $(wildcard *h.gch)
