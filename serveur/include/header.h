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
#define LG_MESSAGE 1024 // Longueur du message
#define VERSION "0.1c" // version du serveur

typedef struct User User;
struct User
{
   int socketClient; // Socket du client
   char login[50]; // Login du client
   bool logged; // Si le client est connecté
   User *suiv; // Pointeur sur le suivant
};

typedef struct Contact Contact;
struct Contact
{
   User * first; // Pointeur sur le premier utilisateur
   struct pollfd *poll_set; // Pointeur sur le tableau de descripteurs de fichiers surveillés
   int numfds; // Nombre de descripteurs de fichiers surveillés
};

void newUser(Contact *contact, int socketDialogue, int *id); // Ajoute un nouvel utilisateur
void removeUser(Contact *contact, int socketDialogue); // Supprime un utilisateur
void printConnected(User *liste); // Affiche la liste des utilisateurs connectés
void checkServer(int socketEcoute); // Vérifie si le serveur est connecté
void checkArguments(int argc, char * argv[], char * greating); // Vérifie les arguments
void printServerStatus(int port, int socketEcoute); // Affiche le statut du serveur
void checkSocketDialogue(int socketDialogue, int socketEcoute); // Vérifie si le socket dialogue est valide
void handleMessage(char messageRecu[LG_MESSAGE], char messageEnvoi[LG_MESSAGE], int socketDialogue, Contact * contact, int fd_index, char * greating); // Traite le message reçu

void cmd_mp(char *strToken, char messageEnvoi[LG_MESSAGE], int socketDialogue, User *liste); // Envoi un message privé
void cmd_version(char *strToken, char messageEnvoi[LG_MESSAGE], int socketDialogue, Contact * contact, int fd_index, char * greating); // Envoi la version du serveur
void cmd_ret(int code, int socketDialogue, char messageEnvoi[LG_MESSAGE]); // Envoi un code de retour
void cmd_mg(char *strToken, char messageEnvoi[LG_MESSAGE], int socketDialogue, User *liste); // Envoi un message global
void cmd_users(int socketDialogue, char messageEnvoi[LG_MESSAGE], User *liste); // Envoi la liste des utilisateurs connectés
void cmd_login(char *strToken, char messageEnvoi[LG_MESSAGE], int socketDialogue, Contact *contact, int fd_index); // Envoi le login