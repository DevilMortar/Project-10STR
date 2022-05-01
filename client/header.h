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

#define PORT IPPORT_USERRESERVED // = 5000
#define LG_MESSAGE 1024
#define VERSION "1.0.1"
#define color(param) printf("\033[%sm",param)

typedef struct User User;
struct User
{
   int socketClient;
   struct sockaddr_in pointDeRencontreDistant;
   User *suiv;
   char login[50];
};

typedef struct POLL POLL;
struct POLL
{
   struct pollfd *poll_set;
   int numfds;
};

void checkArguments(int argc, char * argv[]);
void handleMessage(char messageRecu[LG_MESSAGE], char messageEnvoi[LG_MESSAGE], int socketDialogue, int fd_index);
