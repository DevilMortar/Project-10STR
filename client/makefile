flags = -lm $(shell pkg-config --cflags --libs sdl2) -lSDL2_ttf -lSDL2_image -lSDL2_mixer -Og
src = ./src
lib = ./lib
bin = ./bin
include = ./include

client: client.o source.o display.o
	gcc -o client client.o source.o display.o -lm $(flags)
	mv *.o $(lib)
	mv client $(bin)

client.o : $(src)/client.c display.o source.o
	gcc -g -Wall -c $(src)/client.c $(flags)

source.o : $(include)/header.h $(src)/source.c display.o
	gcc -g -Wall -c $(src)/source.c $(flags)

display.o : $(src)/display.c $(include)/header.h
	gcc -g -Wall -c $(src)/display.c $(flags)
	
clean :
	rm -f $(bin)/client $(lib)/*.o

