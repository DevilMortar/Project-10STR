server: server.o source.o source.c header.h
	gcc -o server server.o source.o -lm

server.o : server.c
	gcc -g -Wall -c server.c

source.o : header.h
	gcc -g -Wall -c source.c
	
clean :
	rm -f server *.o

