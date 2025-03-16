all: server lettore editore 

lettore: lettore.o list.o
	gcc -Wall -o $@ $^
	
editore: editore.o list.o
	gcc -Wall -o $@ $^

server:	server.o list.o 
	gcc -Wall -o $@ $^
	
%.o: %.c 
	gcc -Wall -c $< 

clean:
	rm -f client server *~ *.o

