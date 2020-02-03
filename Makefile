

LFLAGS = -shared -o
CFLAGS = -c -fpic -std=c99

make: 	       				   entityManager.o componentManager.o systemManager.o componentTable.o
	gcc $(LFLAGS) libchecs.so entityManager.o componentManager.o systemManager.o componentTable.o -lchecl
	mv libchecs.so /usr/local/lib/
	cp *.h /usr/local/include/checs/

entityManager.o: entityManager.c
	gcc $(CFLAGS) entityManager.c

systemManager.o: systemManager.c
	gcc $(CFLAGS) systemManager.c

componentManager.o: componentManager.c
	gcc $(CFLAGS) componentManager.c

componentTable.o: componentTable.c
	gcc $(CFLAGS) componentTable.c