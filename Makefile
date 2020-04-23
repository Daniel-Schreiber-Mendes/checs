SRC = entityManager.c componentManager.c systemManager.c sparseSet.c system.c commandManager.c eventManager.c
OBJ = $(SRC:.c=.o)
LFLAGS = -shared -o
CFLAGS = -c -g -fpic -std=c99 -O1

make: 	       				  entityManager.o componentManager.o systemManager.o sparseSet.o system.o commandManager.o eventManager.o
	gcc $(LFLAGS) libchecs.so entityManager.o componentManager.o systemManager.o sparseSet.o system.o commandManager.o eventManager.o -lchecl
	mv libchecs.so /usr/local/lib/
	cp *.h /usr/local/include/checs/

%.o: %.c
	gcc -c $(CFLAGS) $*.c