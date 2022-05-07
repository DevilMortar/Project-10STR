flags = -lm $(shell pkg-config --cflags --libs sdl2) -lSDL2_ttf -lSDL2_image -lSDL2_mixer -Og

client: client.o source.o source.c header.h
	gcc -o client client.o source.o -lm $(flags)

client.o : client.c
	gcc -g -Wall -c client.c $(flags)

source.o : header.h
	gcc -g -Wall -c source.c $(flags)
	
clean :
	rm -f client *.o

