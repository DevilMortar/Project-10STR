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
#include <stdbool.h>

#define PORT IPPORT_USERRESERVED // = 5000
#define LG_MESSAGE 1024
#define VERSION "0.1c"

typedef struct User User;
struct User
{
   int socketClient;
   char login[50];
   bool logged;
   User *suiv;
};

typedef struct Contact Contact;
struct Contact
{
   User * first;
   struct pollfd *poll_set;
   int numfds;
};

void newUser(Contact *contact, int socketDialogue, int *id);
void removeUser(Contact *contact, int socketDialogue);
void printConnected(User *liste);
void checkServer(int socketEcoute);
void checkArguments(int argc, char * argv[], char * greating);
void printServerStatus(int port, int socketEcoute);
void checkSocketDialogue(int socketDialogue, int socketEcoute);
void handleMessage(char messageRecu[LG_MESSAGE], char messageEnvoi[LG_MESSAGE], int socketDialogue, Contact * contact, int fd_index, char * greating);

// CMD
void cmd_mp(char *strToken, char messageEnvoi[LG_MESSAGE], int socketDialogue, User *liste);
void cmd_version(char *strToken, char messageEnvoi[LG_MESSAGE], int socketDialogue, Contact * contact, int fd_index, char * greating);
void cmd_ret(int code, int socketDialogue, char messageEnvoi[LG_MESSAGE]);
void cmd_mg(char *strToken, char messageEnvoi[LG_MESSAGE], int socketDialogue, User *liste);
void cmd_users(int socketDialogue, char messageEnvoi[LG_MESSAGE], User *liste);
void cmd_login(char *strToken, char messageEnvoi[LG_MESSAGE], int socketDialogue, Contact *contact, int fd_index);