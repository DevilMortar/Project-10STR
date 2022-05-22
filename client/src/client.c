#include <stdio.h>
#include <stdlib.h> /* pour exit */
#include <unistd.h> /* pour close */
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>     /* pour memset */
#include <netinet/in.h> /* pour struct sockaddr_in */
#include <arpa/inet.h>  /* pour htons et inet_aton */
#include "../include/header.h"

#define LG_MESSAGE 256

int main(int argc, char *argv[])
{
   system("clear");
   checkArguments(argc, argv);
   int port = atoi(argv[2]);

   int descripteurSocket;
   struct sockaddr_in pointDeRencontreDistant;
   socklen_t longueurAdresse;
   char messageEnvoi[LG_MESSAGE]; /* le message de la couche Application ! */
   char messageRecu[LG_MESSAGE];  /* le message de la couche Application ! */
   int ecrits, lus;               /* nb d'octets ecrits et lus */
   int retour;

   // Crée un socket de communication
   descripteurSocket = socket(PF_INET, SOCK_STREAM, 0); /* 0 indique que l'on utilisera le protocole par défaut associé à SOCK_STREAM soit TCP */
   if (descripteurSocket < 0)                           /* échec ? */
   {
      perror("socket"); // Affiche le message d'erreur
      exit(-1);         // On sort en indiquant un code erreur
   }

   // Obtient la longueur en octets de la structure sockaddr_in
   longueurAdresse = sizeof(pointDeRencontreDistant);
   memset(&pointDeRencontreDistant, 0x00, longueurAdresse);
   pointDeRencontreDistant.sin_family = PF_INET;
   pointDeRencontreDistant.sin_port = htons(port);
   inet_aton("127.0.0.1", &pointDeRencontreDistant.sin_addr);

   // Débute la connexion vers le processus serveur distant
   if ((connect(descripteurSocket, (struct sockaddr *)&pointDeRencontreDistant, longueurAdresse)) == -1)
   {
      perror("connect");        // Affiche le message d'erreur
      close(descripteurSocket); // On ferme la ressource avant de quitter
      exit(-2);                 // On sort en indiquant un code erreur
   }

   /* ---------------- Initialisation du poll --------------- */
   POLL *poll_struct = malloc(sizeof(POLL));
   poll_struct->poll_set = malloc(2 * sizeof(struct pollfd));
   poll_struct->poll_set[0].fd = STDIN_FILENO;
   poll_struct->poll_set[0].events = POLLOUT;
   poll_struct->poll_set[1].fd = descripteurSocket;
   poll_struct->poll_set[1].events = POLLIN | POLLPRI | POLLHUP | POLLOUT;
   poll_struct->numfds = 2;

   /* ---------------- Initialisation de la structure display --------------- */
   DISPLAY *display = malloc(sizeof(DISPLAY));
   initDisplay(display);
   display->socket = descripteurSocket;

   /* ---------------- Création des bouttons --------------- */
   BUTTON *buttonList = NULL;
   void (*users)(DISPLAY *);
   users = askForUserList;
   void (*mg)(DISPLAY *);
   mg = switchToMg;
   void (*clear)(DISPLAY *);
   clear = clearTampon;
   buttonList = createButton("/users", 0, 0, 1, 1, buttonList, users);
   buttonList = createButton("/mg", 0, 0, 1, 1, buttonList, mg);
   buttonList = createButton("/clear", 0, 0, 1, 1, buttonList, clear);

   /* ---------------- Paramètres --------------- */
   bool needToUpdate = false;
   bool checkVersion = false;

   /* ---------------- Main Loop --------------- */

   while (display->running)
   {
      // Update
      needToUpdate = false;
      // Initialise à 0 les messages
      memset(messageEnvoi, 0x00, LG_MESSAGE * sizeof(char));
      memset(messageRecu, 0x00, LG_MESSAGE * sizeof(char));

      SDL_Event event;
      while (SDL_PollEvent(&event))
      {
         switch (event.type)
         {
         case SDL_QUIT:
            display->running = 0;
            break;

         case SDL_KEYDOWN:
            // On récupère le caractère tapé
            if (handleInput(display, event))
            {
               // Si le caractère tapé est un retour chariot, on envoie le message
               sendMessage(display);
            }
            needToUpdate = true; // On update la fenêtre
            break;
         case SDL_MOUSEBUTTONDOWN:
            // On vérifie si on a cliqué sur un bouton ou un user
            needToUpdate = needToUpdate | checkClickButton(buttonList, event, display);
            needToUpdate = needToUpdate | checkClickUser(display, event);
            break;
         default:
            // On vérifie si le curseur est dans la zone du bouton ou d'un user
            needToUpdate = needToUpdate | checkHoverButton(buttonList, event);
            needToUpdate = needToUpdate | checkHoverUser(display, event);
            break;
         }
      }

      // On écoute le socket
      poll(poll_struct->poll_set, poll_struct->numfds, -1);
      for (int fd_index = 0; fd_index < poll_struct->numfds; fd_index++)
      {
         if (poll_struct->poll_set[fd_index].revents & POLLIN || poll_struct->poll_set[fd_index].revents & POLLPRI)
         {
            // On lit le message
            lus = recv(poll_struct->poll_set[fd_index].fd, messageRecu, LG_MESSAGE, 0);
            if (lus < 0)
            {
               exit(-3);
            }
            else if (lus == 0)
            {
               printf("Le processus distant a été fermé !\n");
               exit(0);
            }
            else
            {
               // On affiche le message
               printf("%s\n", messageRecu);
               // On traite le message
               handleMessage(messageRecu, messageEnvoi, poll_struct->poll_set[fd_index].fd, display);
               needToUpdate = true;
            }
         }
      }

      if (!checkVersion)
      {
         // On vérifie si le client est à jour
         strcpy(messageEnvoi, "/version ");
         strcat(messageEnvoi, VERSION);
         send(descripteurSocket, messageEnvoi, LG_MESSAGE, 0);
         checkVersion = true;
         needToUpdate = true;
      }

      if (needToUpdate)
      {
         // On update la fenêtre
         SDL_RenderClear(display->renderer);
         displayBackground(display);
         if (display->filterActive)
         {
            displayTamponFiltered(display);
         }
         else
         {
            displayTampon(display);
         }
         displayInputField(display);
         displayUserList(display);
         displayUserName(display);
         if (display->logged)
         {
            displayButtons(display, buttonList);
         }
         SDL_RenderPresent(display->renderer);
      }
   }
   // On libère la mémoire
   freeUserList(display->users);
   SDL_Quit();
   return 0;
}