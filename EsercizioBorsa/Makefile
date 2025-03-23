all: server agente investitore



server:	server.o list.o
	gcc -Wall -o server server.o list.o

agente: agente.o list.o
	gcc -Wall -o agente agente.o list.o

investitore: investitore.o list.o
	gcc -Wall -o investitore investitore.o list.o


server.o: server.c list.h
	gcc -Wall -c server.c

agente.o: agente.c list.h
	gcc -Wall -c agente.c

investitore.o: investitore.c list.h
	gcc -Wall -c investitore.c

list.o: list.c list.h
	gcc -Wall -c list.c



clean:
	rm -f server agente investitore *o *~

