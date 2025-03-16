all: server player



server:	server.o list.o
	gcc -Wall -o server server.o list.o

player: player.o
	gcc -Wall -o player player.o



server.o: server.c list.h
	gcc -Wall -c server.c

player.o: player.c list.h
	gcc -Wall -c player.c

list.o: list.c list.h
	gcc -Wall -c list.c



clean:
	rm -f server player *o *~

