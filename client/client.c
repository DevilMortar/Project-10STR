#include <stdio.h>
#include <stdlib.h> /* pour exit */
#include <unistd.h> /* pour close */
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>     /* pour memset */
#include <netinet/in.h> /* pour struct sockaddr_in */
#include <arpa/inet.h>  /* pour htons et inet_aton */
#include "header.h"

#define LG_MESSAGE 256

int main(int argc, char *argv[])
{
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

   printf("Socket créée avec succès ! (%d)\n", descripteurSocket);

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

   /* ---------------- Contact & Poll --------------- */
   POLL * poll_struct = malloc(sizeof(POLL));
   poll_struct->poll_set = malloc(2*sizeof(struct pollfd));
   poll_struct->poll_set[0].fd = 0;
   poll_struct->poll_set[0].events = POLLIN | POLLPRI | POLLHUP | POLLOUT;
   poll_struct->poll_set[1].fd = descripteurSocket;
   poll_struct->poll_set[1].events = POLLIN | POLLPRI | POLLHUP | POLLOUT;
   poll_struct->numfds = 2;

   printf("Connexion au serveur réussie avec succès !\n");

   while (1)
   {
      // Initialise à 0 les messages
      memset(messageEnvoi, 0x00, LG_MESSAGE * sizeof(char));
      memset(messageRecu, 0x00, LG_MESSAGE * sizeof(char));
      poll(poll_struct->poll_set, poll_struct->numfds, -1);
      for (int fd_index = 0; fd_index < poll_struct->numfds; fd_index++)
      {
         if (poll_struct->poll_set[fd_index].revents & POLLIN || poll_struct->poll_set[fd_index].revents & POLLPRI)
         {
            lus = recv(poll_struct->poll_set[fd_index].fd, messageRecu, LG_MESSAGE, 0);
            if (lus < 0)
            {
               perror("recv");
               exit(-3);
            }
            else if (lus == 0)
            {
               printf("Le processus distant a été fermé !\n");
               exit(0);
            }
            else
            {
               printf("Message reçu : %s\n", messageRecu);
            }
         }
         if (poll_struct->poll_set[fd_index].revents & POLLOUT)
         {
            printf("Message à envoyer : ");
            fgets(messageEnvoi, LG_MESSAGE, stdin);
            ecrits = send(descripteurSocket, messageEnvoi, strlen(messageEnvoi), 0);
            if (ecrits < 0)
            {
               perror("send");
               exit(-4);
            }
            else if (ecrits == 0)
            {
               printf("Le processus distant a été fermé !\n");
               exit(0);
            }
            else
            {
               printf("Message envoyé : %s\n", messageEnvoi);
            }
         }
      }
   }
   return 0;
}