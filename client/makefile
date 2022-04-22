client: client.o source.o source.c header.h
	gcc -o client client.o source.o -lm

client.o : client.c
	gcc -g -Wall -c client.c

source.o : header.h
	gcc -g -Wall -c source.c
	
clean :
	rm -f client *.o

