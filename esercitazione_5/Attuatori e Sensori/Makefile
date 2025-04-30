# Makefile

CC=gcc
CFLAGS=-Wall -Iinclude
SRC=src
BIN=bin

all: server hub sensore attuatore

server: $(SRC)/server.c
	$(CC) $(CFLAGS) -o server $(SRC)/server.c

hub: $(SRC)/hub.c
	$(CC) $(CFLAGS) -o hub $(SRC)/hub.c

sensore: $(SRC)/sensore.c
	$(CC) $(CFLAGS) -o sensore $(SRC)/sensore.c

attuatore: $(SRC)/attuatore.c
	$(CC) $(CFLAGS) -o attuatore $(SRC)/attuatore.c

clean:
	rm -f server hub sensore attuatore
