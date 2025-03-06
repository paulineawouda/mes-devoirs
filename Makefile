CC = gcc
CFLAGS = -Wall -g

# Fichiers objets
OBJ = main.o fast_trigo.o test_func.o util.o

# Bibliothèque
LIBRARY = libmyfunctions.a

# Programme final
TARGET = main

# Compilation du programme principal
$(TARGET): $(OBJ) $(LIBRARY)
	$(CC) $(OBJ) -o $(TARGET) -L. -lmyfunctions -lm


# Compilation des fichiers objets
main.o: main.c fast_trigo.h test_func.h util.h
	$(CC) $(CFLAGS) -c main.c

fast_trigo.o: fast_trigo.c fast_trigo.h
	$(CC) $(CFLAGS) -c fast_trigo.c

test_func.o: test_func.c test_func.h fast_trigo.h
	$(CC) $(CFLAGS) -c test_func.c

util.o: util.c util.h
	$(CC) $(CFLAGS) -c util.c

# Création de la bibliothèque statique
$(LIBRARY): fast_trigo.o test_func.o util.o
	ar rcs $(LIBRARY) fast_trigo.o test_func.o util.o

# Nettoyage
clean:
	rm -f $(OBJ) $(LIBRARY) $(TARGET)
