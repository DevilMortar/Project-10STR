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
#include "display.h"

#define PORT IPPORT_USERRESERVED // = 5000
#define LG_MESSAGE 1024 // Longueur d'un message
#define VERSION "0.1c" // Version du serveur
#define WINDOW_WIDTH 1200 // Largeur de la fenêtre
#define WINDOW_HEIGHT 800 // Hauteur de la fenêtre
#define HEADER_HEIGHT 80 // Hauteur du header
#define FOOTER_HEIGHT 100 // Hauteur du footer
#define LIST_WIDTH 220 // Largeur de la liste des utilisateurs
#define USER_HEIGHT 40 // Hauteur d'un utilisateur
#define BODY_HEIGHT (WINDOW_HEIGHT - HEADER_HEIGHT - FOOTER_HEIGHT) // Hauteur du corps de la fenêtre
#define TEXT_SIZE 20 // Taille du texte
#define TAMPON_SIZE 200 // Taille du tampon
#define TAMPON_CURSOR_SIZE 21 // Taille du curseur

typedef struct POLL POLL; // Structure de poll
struct POLL
{
   struct pollfd *poll_set; // Tableau de pollfd
   int numfds; // Nombre de pollfd
};

//Private
void getPrivate(DISPLAY * display, char * login); // Récupère les messages privés avec le login
void clearPrivate(DISPLAY *display); // Vide le tampon de messages privés



void checkArguments(int argc, char * argv[]); // Vérifie les arguments de lancement du client
void handleMessage(char messageRecu[LG_MESSAGE], char messageEnvoi[LG_MESSAGE], int socketDialogue, DISPLAY *display); // Traite les messages reçus
void codeError(char *strToken, DISPLAY *display); // Affichage d'un message d'erreur
void addInTampon(DISPLAY *display, char *message); // Ajoute un message dans le tampon
void clearTampon(DISPLAY *display); // Vide le tampon
bool handleInput(DISPLAY *display, SDL_Event event); // Traite les entrées clavier
void sendMessage(DISPLAY *display); // Envoie le message du tampon
USER * freeUserList(USER *userList); // Libère la liste des utilisateurs
