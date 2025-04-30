CC = gcc
CFLAGS = -Wall -pthread -Iinclude
SRC = src

all: server guida visitatore

server: $(SRC)/server.c
	$(CC) $(CFLAGS) -o server $(SRC)/server.c

guida: $(SRC)/guida.c
	$(CC) $(CFLAGS) -o guida $(SRC)/guida.c

visitatore: $(SRC)/visitatore.c
	$(CC) $(CFLAGS) -o visitatore $(SRC)/visitatore.c

clean:
	rm -f server guida visitatore
