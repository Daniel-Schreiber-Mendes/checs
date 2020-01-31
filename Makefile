

make: 	       				   entityManager.o componentManager.o systemManager.o componentTable.o
	gcc -shared -o libchecs.so entityManager.o componentManager.o systemManager.o componentTable.o -lchecl
	mv libchecs.so /usr/local/lib/
	cp *.h /usr/local/include/checs/

entityManager.o: entityManager.c
	gcc -c -fpic entityManager.c

systemManager.o: systemManager.c
	gcc -c -fpic systemManager.c

componentManager.o: componentManager.c
	gcc -c -fpic componentManager.c

componentTable.o: componentTable.c
	gcc -c -fpic componentTable.c