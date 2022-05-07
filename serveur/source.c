#include "header.h"

void newUser(Contact *contact, struct sockaddr_in pointDeRencontreDistant, int socketDialogue, int *id)
{
   // Ajout du contact dans la liste
   User *elem;
   char ids[20];
   sprintf(ids, "%d", *id);
   elem = malloc(sizeof(User));
   elem->socketClient = socketDialogue;
   elem->pointDeRencontreDistant = pointDeRencontreDistant;
   strcpy(elem->login, "Client");
   strcat(elem->login, ids);
   if (contact->first != NULL)
   {
      elem->suiv = contact->first;
   }
   else
   {
      elem->suiv = NULL;
   }
   contact->first = elem;

   // On affiche des infos sur le client
   printf("CONNEXION | Connexion client de %s:%d (%d)\n", inet_ntoa(pointDeRencontreDistant.sin_addr), pointDeRencontreDistant.sin_port, socketDialogue);

   // Création du pollfd
   contact->numfds += 1;
   contact->poll_set = realloc(contact->poll_set, contact->numfds * sizeof(struct pollfd));
   contact->poll_set[contact->numfds - 1].fd = socketDialogue;
   contact->poll_set[contact->numfds - 1].events = POLLIN | POLLPRI | POLLHUP;
   *id += 1;
}

void removeUser(Contact *contact, int fd_index)
{
   User *previous = contact->first;
   User *next = contact->first;
   while (next != NULL && next->socketClient != contact->poll_set[fd_index].fd)
   {
      previous = next;
      next = next->suiv;
   }
   fprintf(stderr, "\nDECONNEXION | %s s'est déconnecté !\n", next->login);
   if (next == previous)
   {
      contact->first = next->suiv;
   }
   else
   {
      previous->suiv = next->suiv;
   }
   close(contact->poll_set[fd_index].fd);
   free(next);
   for (int i = fd_index; i < contact->numfds; i++)
   {
      contact->poll_set[i] = contact->poll_set[i + 1];
   }
   printConnected(contact->first);
   contact->numfds -= 1;
}

void printConnected(User *liste)
{
   User *tmp = liste;
   printf("CONTACT | > Utilisateurs en ligne : ");
   if (liste == NULL)
   {
      printf("Aucun");
   }
   while (tmp != NULL)
   {
      char ip[INET_ADDRSTRLEN];
      inet_ntop(AF_INET, &(tmp->pointDeRencontreDistant.sin_addr), ip, INET_ADDRSTRLEN);
      printf("\n           * %s [%s:%i] (%d)", tmp->login, ip, tmp->pointDeRencontreDistant.sin_port, tmp->socketClient);

      tmp = tmp->suiv;
   }
   printf("\n\n");
}

void checkServer(int socketEcoute)
{
   if (socketEcoute < 0) /* échec ? */
   {
      perror("socket"); // Affiche le message d'erreur
      exit(-1);         // On sort en indiquant un code erreur
   }
}

void checkSocketDialogue(int socketDialogue, int socketEcoute)
{
   if (socketDialogue < 0)
   {
      perror("accept");
      close(socketDialogue);
      close(socketEcoute);
      exit(-4);
   }
}

void checkArguments(int argc, char *argv[], char *greating)
{
   // ./main<p7port -n name -g Hello Machin
   if (argc < 7)
   {
      perror("SERVER | Missing arguments (4 required)");
      exit(-1);
   }
   if (strcmp(argv[1], "-p") != 0)
   {
      perror("SERVER | You need to use : ./server -p [port] -n [name] -g [greating]");
      exit(-1);
   }

   if (strcmp(argv[3], "-n") != 0)
   {
      perror("SERVER | You need to use : ./server -p [port] -n [name] -g [greating]");
      exit(-1);
   }

   if (strcmp(argv[5], "-g") != 0)
   {
      perror("SERVER | You need to use : ./server -p [port] -n [name] -g [greating]");
      exit(-1);
   }

   for (int i = 6; i < argc; i++)
   {
      strcat(greating, argv[i]);
      strcat(greating, " ");
   }
   system("clear");
}

void printServerStatus(int port, int socketEcoute)
{
   printf("SERVER STATUS | Port sélectionée : %d\n", port);
   printf("SERVER STATUS | Socket créée avec succès ! (%d)\n", socketEcoute);
   printf("SERVER STATUS | Socket placée en écoute passive ...\n");
   printf("SERVER STATUS | Socket attachée avec succès !\n");
   printf("SERVER STATUS | Serveur status : ON \n\n");
   printf("VERSION | Server version : %s\n\n", VERSION);
}

void handleMessage(char messageRecu[LG_MESSAGE], char messageEnvoi[LG_MESSAGE], int socketDialogue, Contact *contact, int fd_index)
{
   if (strcmp(messageRecu, "\n\0") != 0)
   {
      const char *separators = " \n";
      char *strToken = strtok(messageRecu, separators);
      // Récupération du mot clé

      if (strToken != NULL)
      {
         if (strcmp(strToken, "/mp") == 0)
         {
            cmd_mp(strToken, messageEnvoi, socketDialogue, contact->first);
         }

         else if (strcmp(strToken, "/mg") == 0)
         {
            cmd_mg(strToken, messageEnvoi, socketDialogue, contact->first);
         }

         else if (strcmp(strToken, "/version") == 0)
         {
            cmd_version(strToken, messageEnvoi, socketDialogue, contact, fd_index);
         }
         else if (strcmp(strToken, "/users") == 0)
         {
            cmd_users(socketDialogue, messageEnvoi, contact->first);
         }
         else if (strcmp(strToken, "/login") == 0)
         {
            cmd_login(strToken, messageEnvoi, socketDialogue, contact, fd_index);
         }
         else
         {
            cmd_ret(501, socketDialogue, messageEnvoi);
         }
      }
   }
}

void cmd_mp(char *strToken, char messageEnvoi[LG_MESSAGE], int socketDialogue, User *liste)
{
   const char *separators = " \n";
   strToken = strtok(NULL, separators);
   if (strToken != NULL)
   {
      User *destinataire = liste;
      while (destinataire != NULL && strcmp(strToken, destinataire->login) != 0)
      {
         destinataire = destinataire->suiv;
      }
      User *emetteur = liste;
      while (emetteur != NULL && socketDialogue != emetteur->socketClient)
      {
         emetteur = emetteur->suiv;
      }
      if (emetteur != NULL && destinataire != NULL)
      {
         cmd_ret(200, socketDialogue, messageEnvoi);
         strcpy(messageEnvoi, "/mp ");
         strcat(messageEnvoi, emetteur->login);
         strToken = strtok(NULL, separators);
         while (strToken != NULL)
         {
            strcat(messageEnvoi, " ");
            strcat(messageEnvoi, strToken);
            strToken = strtok(NULL, separators);
         }
         printf("---- %d -----\n", socketDialogue);
         write(destinataire->socketClient, messageEnvoi, strlen(messageEnvoi));
      }
      else
      {
         cmd_ret(404, socketDialogue, messageEnvoi);
      }
   }
   else
   {
      cmd_ret(400, socketDialogue, messageEnvoi);
   }
}

void cmd_mg(char *strToken, char messageEnvoi[LG_MESSAGE], int socketDialogue, User *liste)
{
   const char *separators = " \n";
   strToken = strtok(NULL, separators);
   if (strToken != NULL)
   {
      User *emetteur = liste;
      while (emetteur != NULL && socketDialogue != emetteur->socketClient)
      {
         emetteur = emetteur->suiv;
      }
      if (emetteur != NULL)
      {
         User *destinataire = liste;
         while (destinataire != NULL)
         {
            if (destinataire->socketClient != socketDialogue)
            {
               strcpy(messageEnvoi, "/mg ");
               strcat(messageEnvoi, emetteur->login);
               while (strToken != NULL)
               {
                  strcat(messageEnvoi, " ");
                  strcat(messageEnvoi, strToken);
                  strToken = strtok(NULL, separators);
               }
               write(destinataire->socketClient, messageEnvoi, strlen(messageEnvoi));
            }
            destinataire = destinataire->suiv;
         }
         printf("---- %d -----\n", socketDialogue);
         cmd_ret(200, socketDialogue, messageEnvoi);
      }
      else
      {
         cmd_ret(404, socketDialogue, messageEnvoi);
      }
   }
   else
   {
      cmd_ret(400, socketDialogue, messageEnvoi);
   }
}

void cmd_version(char *strToken, char messageEnvoi[LG_MESSAGE], int socketDialogue, Contact *contact, int fd_index)
{
   const char *separators = " \n";
   strToken = strtok(NULL, separators);
   if (strToken != NULL)
   {
      printf("VERSION | User version : %s\n", strToken);
      if (strcmp(strToken, VERSION) != 0)
      {
         printf("VERSION | Error 426 : Client need upgrade !\n");
         printf("VERSION | Client will be kicked from the server !\n");
         removeUser(contact, fd_index);
         cmd_ret(426, socketDialogue, messageEnvoi);
      }
      else
      {
         strcpy(messageEnvoi, "/greating Bienvenue sur le serveur ");
         send(socketDialogue, messageEnvoi, strlen(messageEnvoi), 0);
         usleep(10);
         strcpy(messageEnvoi, "/login");
         send(socketDialogue, messageEnvoi, strlen(messageEnvoi), 0);
      }
      printf("\n");
   }
   else
   {
      cmd_ret(400, socketDialogue, messageEnvoi);
   }
}

void cmd_users(int socketDialogue, char messageEnvoi[LG_MESSAGE], User *liste)
{
   User *tmp = liste;
   strcpy(messageEnvoi, "/users");
   while (tmp != NULL)
   {
      strcat(messageEnvoi, " ");
      strcat(messageEnvoi, tmp->login);
      tmp = tmp->suiv;
   }
   write(socketDialogue, messageEnvoi, strlen(messageEnvoi));
}

void cmd_ret(int code, int socketDialogue, char messageEnvoi[LG_MESSAGE])
{
   char codes[10] = "";
   sprintf(codes, "%d", code);
   strcpy(messageEnvoi, "/ret ");
   strcat(messageEnvoi, codes);
   write(socketDialogue, messageEnvoi, strlen(messageEnvoi));
}

void cmd_login(char *strToken, char messageEnvoi[LG_MESSAGE], int socketDialogue, Contact *contact, int fd_index)
{
   const char *separators = "\n";
   strToken = strtok(NULL, separators);
   // check if login contain spaces
   if (strToken != NULL && strlen(strToken) <= 20 && strchr(strToken, ' ') == NULL)
   {
      User *tmp = contact->first;
      while (tmp != NULL && strcmp(strToken, tmp->login) != 0)
      {
         tmp = tmp->suiv;
      }
      if (tmp != NULL)
      {
         printf("LOGIN | Error 409 : Login '%s' already used !\n", strToken);
         cmd_ret(409, socketDialogue, messageEnvoi);
         usleep(10);
         strcpy(messageEnvoi, "/login");
         send(socketDialogue, messageEnvoi, strlen(messageEnvoi), 0);
      }
      else
      {
         User *tmp = contact->first;
         while (tmp != NULL && tmp->socketClient != socketDialogue)
         {
            tmp = tmp->suiv;
         }
         if (tmp != NULL)
         {
            strcpy(tmp->login, strToken);
            printf("LOGIN | User %s logged in !\n", strToken);
            cmd_ret(200, socketDialogue, messageEnvoi);
         }
         else
         {
            printf("LOGIN | Error 404 : User not found !\n");
            cmd_ret(404, socketDialogue, messageEnvoi);
         }
      }
   }
   else
   {
      printf("LOGIN | Error 409 : Login is invalid !\n");
      cmd_ret(409, socketDialogue, messageEnvoi);
      usleep(10);
      strcpy(messageEnvoi, "/login");
      send(socketDialogue, messageEnvoi, strlen(messageEnvoi), 0);
   }
}