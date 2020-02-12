

LFLAGS = -shared -o
CFLAGS = -c -g -fpic -std=c99

make: 	       				  entityManager.o componentManager.o systemManager.o sparseSet.o system.o commandManager.o
	gcc $(LFLAGS) libchecs.so entityManager.o componentManager.o systemManager.o sparseSet.o system.o commandManager.o -lchecl
	mv libchecs.so /usr/local/lib/
	cp *.h /usr/local/include/checs/

entityManager.o: entityManager.c
	gcc $(CFLAGS) entityManager.c

systemManager.o: systemManager.c
	gcc $(CFLAGS) systemManager.c

componentManager.o: componentManager.c
	gcc $(CFLAGS) componentManager.c

sparseSet.o: sparseSet.c
	gcc $(CFLAGS) sparseSet.c

system.o: system.c
	gcc $(CFLAGS) system.c

commandManager.o: commandManager.c
	gcc $(CFLAGS) commandManager.c