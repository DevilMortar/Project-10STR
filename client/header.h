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
#define WINDOW_WIDTH 1000
#define WINDOW_HEIGHT 800
#define HEADER_HEIGHT 80
#define FOOTER_HEIGHT 100
#define LIST_WIDTH 200
#define BODY_HEIGHT (WINDOW_HEIGHT - HEADER_HEIGHT - FOOTER_HEIGHT)
#define TEXT_SIZE 20
#define TAMPON_SIZE 10
#define TAMPON_CURSOR_SIZE 5
#define goto_x_y(y,x){printf("\033[%u;%uH",y,x);}
#define color(param) printf("\033[%sm",param)
#define delete() printf("%c[2K\r\n", 27)

typedef struct USER USER;
struct USER
{
   char * login;
   SDL_Rect rect;
   USER * next;
   bool hover;
};

typedef struct DISPLAY DISPLAY;
struct DISPLAY
{
   SDL_Window *window;
   SDL_Renderer *renderer;
   bool running;
   char **tampon;
   int tampon_length;
   int tampon_cursor;
   USER * users;
   char * login;
   char * inputText;
   char * prefix;
   bool filterActive;
   char * filter;
   bool logged;
   bool shift;
   int socket;
};

typedef struct BUTTON BUTTON;
struct BUTTON
{
   SDL_Rect rect;
   char * text;
   bool clicked;
   bool hover;
   bool visible;
   bool enabled;
   void (*callback)(DISPLAY *);
   BUTTON * next;
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
void displayTamponFiltered(DISPLAY *display);
SDL_Texture * renderWidgetText(char *message, SDL_Color* color, int fontSize, SDL_Renderer *renderer, SDL_Rect *dstrect);
void displayBackground(DISPLAY *display);
void displayButtons(DISPLAY *display, BUTTON *buttonsList);
void displayUserList(DISPLAY *display);
void displayUserName(DISPLAY *display);
void displayInputField(DISPLAY *display);

//Button
BUTTON * createButton(char *text, bool cliked, bool hover, bool visible, bool enabled, BUTTON *buttonList, void (*callback)(DISPLAY *));
bool checkHoverButton(BUTTON * buttonList, SDL_Event event);
bool checkClickButton(BUTTON * buttonList, SDL_Event event, DISPLAY *display);
void askForUserList(DISPLAY *display);
void leaveChat(DISPLAY *display);
void switchToMg(DISPLAY *display);
bool checkHoverUser(DISPLAY *display, SDL_Event event);
bool checkClickUser(DISPLAY *display, SDL_Event event);



void checkArguments(int argc, char * argv[]);
void handleMessage(char messageRecu[LG_MESSAGE], char messageEnvoi[LG_MESSAGE], int socketDialogue, DISPLAY *display);
void codeError(char *strToken, DISPLAY *display);
void addInTampon(DISPLAY *display, char *message);
void clearTampon(DISPLAY *display);
bool handleInput(DISPLAY *display, SDL_Event event);
void sendMessage(DISPLAY *display);
USER * freeUserList(USER *userList);
