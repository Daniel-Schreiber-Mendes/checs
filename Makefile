SRC = attributeManager.c entityManager.c system.c commandManager.c eventManager.c systemManager.c componentManager.c componentSet.c templateManager.c
OBJ = $(SRC:.c=.o)
LFLAGS = -shared -o
CFLAGS = -c -g -fpic -std=c99 -O0

make: $(OBJ)
	gcc $(LFLAGS) libchecs.so $(OBJ) -lchecl
	mv libchecs.so /usr/local/lib/
	cp *.h /usr/local/include/checs/

%.o: %.c
	gcc -c $(CFLAGS) $*.c