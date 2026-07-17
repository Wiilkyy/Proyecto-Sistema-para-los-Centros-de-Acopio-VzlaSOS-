CC = gcc
CFLAGS = -Wall -Wextra -g
OBJ = main.o inventario.o
EXEC = sistema_acopio

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) $(CFLAGS) -o $(EXEC) $(OBJ)

main.o: main.c inventario.h
	$(CC) $(CFLAGS) -c main.c

inventario.o: inventario.c inventario.h
	$(CC) $(CFLAGS) -c inventario.c

clean:
	rm -f *.o $(EXEC)

run: all
	./$(EXEC)