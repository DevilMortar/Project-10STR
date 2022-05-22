src = ./src
lib = ./lib
bin = ./bin
include = ./include

server: server.o source.o $(src)/source.c $(include)/header.h
	gcc -o server server.o source.o -lm
	mv server $(bin)
	mv *.o $(lib)

server.o : $(src)/server.c
	gcc -g -Wall -c $(src)/server.c

source.o : $(include)/header.h
	gcc -g -Wall -c $(src)/source.c
	
clean :
	rm -f $(bin)/server $(lib)/*.o


