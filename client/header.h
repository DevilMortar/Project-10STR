#include <stdio.h>
#include <stdlib.h> /* pour exit */
#include <unistd.h> /* pour close */
#include <stdbool.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>     /* pour memset */
#include <netinet/in.h> /* pour struct sockaddr_in */
#include <arpa/inet.h>  /* pour htons et inet_aton */
#include <unistd.h>     /* pour sleep */
#include <poll.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#define PORT IPPORT_USERRESERVED // = 5000
#define LG_MESSAGE 1024
#define VERSION "1.0.1"
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define TEXT_SIZE 20
#define TAMPON_SIZE 100
#define TAMPON_CURSOR_SIZE 2
#define goto_x_y(y,x){printf("\033[%u;%uH",y,x);}
#define color(param) printf("\033[%sm",param)
#define delete() printf("%c[2K\r\n", 27)

typedef struct DISPLAY DISPLAY;
struct DISPLAY
{
   SDL_Window *window;
   SDL_Renderer *renderer;
   bool running;
   char **tampon;
   int tampon_length;
   int tampon_cursor;
   char * inputText;
   bool shift;
};
   

typedef struct POLL POLL;
struct POLL
{
   struct pollfd *poll_set;
   int numfds;
};

//Display
void initDisplay(DISPLAY *display);
void SDL_ExitWithError(const char *message);
void displayTampon(DISPLAY *display);
void renderWidgetText(char *message, SDL_Color* color, int fontSize, SDL_Renderer *renderer, SDL_Rect *dstrect);



void checkArguments(int argc, char * argv[]);
void handleMessage(char messageRecu[LG_MESSAGE], char messageEnvoi[LG_MESSAGE], int socketDialogue, DISPLAY *display);
void codeError(char *strToken);
void addInTampon(DISPLAY *display, char *message);
bool handleInput(DISPLAY *display, SDL_Event event);
