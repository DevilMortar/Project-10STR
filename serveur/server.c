#include "header.h"

int main(int argc, char *argv[])
{
   /* ---------------- Arguments --------------- */
   char greating[LG_MESSAGE];
   memset(greating, 0x00, LG_MESSAGE * sizeof(char));
   checkArguments(argc, argv, greating);

   /* ---------------- Initialisation --------------- */
   int port = atoi(argv[2]);
   char * serverName = argv[4];
   int socketEcoute;
   struct sockaddr_in pointDeRencontreLocal;
   socklen_t longueurAdresse;
   int socketDialogue;
   struct sockaddr_in pointDeRencontreDistant;
   char messageEnvoi[LG_MESSAGE];
   char messageRecu[LG_MESSAGE];
   int ecrits, lus;               
   int retour;

   // Crée un socket de communication TCP
   socketEcoute = socket(PF_INET, SOCK_STREAM, 0);

   /* ---------------- Contact & Poll --------------- */
   Contact *contact = malloc(sizeof(Contact));
   contact->numfds = 1;
   int id = 0;
   contact->poll_set = malloc(sizeof(struct pollfd));
   contact->poll_set[0].fd = socketEcoute;
   contact->poll_set[0].events = POLLIN | POLLPRI | POLLHUP;

   /* ---------------- Socket --------------- */
   checkServer(socketEcoute);

   // On prépare l'adresse d'attachement locale
   longueurAdresse = sizeof(struct sockaddr_in);
   memset(&pointDeRencontreLocal, 0x00, longueurAdresse);
   pointDeRencontreLocal.sin_family = PF_INET;
   pointDeRencontreLocal.sin_addr.s_addr = htonl(INADDR_ANY); // toutes les interfaces locales disponibles
   pointDeRencontreLocal.sin_port = htons(port);

   // On demande l'attachement local de la socket
   if ((bind(socketEcoute, (struct sockaddr *)&pointDeRencontreLocal, longueurAdresse)) < 0)
   {
      perror("bind");
      exit(-2);
   }

   // On fixe la taille de la file d'attente à 5 (pour les demande de connexion non encore traitées)
   if (listen(socketEcoute, 5) < 0)
   {
      perror("listen");
      exit(-3);
   }

   // Lancement du serveur !
   printServerStatus(port, socketEcoute);

   // boucle d'attente de connexion : en théorie, un serveur attend indéfiniment !
   while (1)
   {
      memset(messageEnvoi, 0x00, LG_MESSAGE * sizeof(char));
      memset(messageRecu, 0x00, LG_MESSAGE * sizeof(char));
      poll(contact->poll_set, contact->numfds, -1);
      for (int fd_index = 0; fd_index < contact->numfds; fd_index++)
      {
         if (contact->poll_set[fd_index].revents & POLLIN || contact->poll_set[fd_index].revents & POLLPRI || contact->poll_set[fd_index].revents & POLLOUT)
         {
            if (contact->poll_set[fd_index].fd == socketEcoute)
            {
               socketDialogue = accept(socketEcoute, (struct sockaddr *)&pointDeRencontreDistant, &longueurAdresse);
               checkSocketDialogue(socketDialogue, socketEcoute);
               newUser(contact, socketDialogue, &id);
               printConnected(contact->first);
            }
            else
            {
               // On réceptionne les données du client (cf. protocole)
               lus = read(contact->poll_set[fd_index].fd, messageRecu, LG_MESSAGE * sizeof(char)); // ici appel bloquant
               User *tmp = contact->first;
               User *tmp2 = contact->first;
               switch (lus)
               {
               case -1: /* Erreur de réception */
                  perror("read");
                  close(contact->poll_set[fd_index].fd);
                  exit(-5);
               case 0: /* Déconnexion de l'utilisateur */
                  removeUser(contact, fd_index);
                  continue;
               default: /* Réception de n octets */
                  while (tmp != NULL && tmp->socketClient != contact->poll_set[fd_index].fd)
                  {
                     tmp = tmp->suiv;
                  }
                  if (strcmp(messageRecu, "\n\0") != 0) {
                     printf("\nMESSAGE | %s (%d) : %s\n", tmp->login, tmp->socketClient, messageRecu);
                     handleMessage(messageRecu, messageEnvoi, contact->poll_set[fd_index].fd, contact, fd_index, greating);
                  }
                  continue;
               }
            }
         }
      }
   }
   // On ferme la ressource avant de quitter
   close(socketEcoute);

   return 0;
}