CC = gcc

all: clean parent child1 child2

parent: parent.c
	$(CC) -o parent parent.c -lrt

child1: child1.c
	$(CC) -o child1 child1.c -lrt

child2: child2.c
	$(CC) -o child2 child2.c -lrt

clean:
	rm -f parent child1 child2 