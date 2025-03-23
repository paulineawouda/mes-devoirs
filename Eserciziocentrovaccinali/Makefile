
all: server fornitore centro

server: server.o list.o
	gcc -Wall -o $@ $^
	
fornitore: fornitore.o list.o
	gcc -Wall -o $@ $^
	
centro: centro.o list.o
	gcc -Wall -o $@ $^
	
%.o: %.c
	gcc -Wall -c $<
	
clean:
	rm -rf *.o *~ server fornitore centro

