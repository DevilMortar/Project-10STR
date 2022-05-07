#include <stdio.h>
#include <stdlib.h> /* pour exit */
#include <unistd.h> /* pour close */
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
#define goto_x_y(y,x){printf("\033[%u;%uH",y,x);}
#define color(param) printf("\033[%sm",param)
#define delete() printf("%c[2K\r\n", 27)

typedef struct DISPLAY DISPLAY;
struct DISPLAY
{
   SDL_Window *window;
   SDL_Renderer *renderer;
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



void checkArguments(int argc, char * argv[]);
void handleMessage(char messageRecu[LG_MESSAGE], char messageEnvoi[LG_MESSAGE], int socketDialogue, DISPLAY *display);
void codeError(char *strToken);
