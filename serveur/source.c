#include "header.h"

void newUser(Contact *contact, int socketDialogue, int *id)
{
   // Ajout du contact dans la liste
   User *elem;
   char ids[20];
   sprintf(ids, "%d", *id);
   elem = malloc(sizeof(User));
   elem->socketClient = socketDialogue;
   elem->logged = false;
   strcpy(elem->login, "(Unlogged) Client");
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
   printf("\nCONNEXION | A new client is connected.\n");

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
   fprintf(stderr, "\nDECONNEXION | %s (%d) is disconnected.\n", next->login, next->socketClient);
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
   printf(" • CONTACT | > User online : ");
   if (liste == NULL)
   {
      printf("None");
   }
   while (tmp != NULL)
   {
      printf("\n           * %s (%d)", tmp->login, tmp->socketClient);
      tmp = tmp->suiv;
   }
   printf("\n");
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
   printf("SERVER STATUS | Port selected : %d\n", port);
   printf("SERVER STATUS | Socket created with success ! (%d)\n", socketEcoute);
   printf("SERVER STATUS | Socket is now listening ...\n");
   printf("SERVER STATUS | Socket binded with success !\n");
   printf("SERVER STATUS | Serveur status : ON \n\n");
   printf("VERSION | Server version : %s\n", VERSION);
}

void handleMessage(char messageRecu[LG_MESSAGE], char messageEnvoi[LG_MESSAGE], int socketDialogue, Contact *contact, int fd_index, char * greating)
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
            cmd_version(strToken, messageEnvoi, socketDialogue, contact, fd_index, greating);
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
      if (emetteur != NULL && destinataire != NULL && destinataire->logged == 1)
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
         write(destinataire->socketClient, messageEnvoi, strlen(messageEnvoi));
         printf(" • Message | Message has been sent to %s\n", destinataire->login);
      }
      else
      {
         cmd_ret(404, socketDialogue, messageEnvoi);
         printf(" • Message | Error : %s is not connected\n", strToken);
      }
   }
   else
   {
      cmd_ret(400, socketDialogue, messageEnvoi);
      printf(" • Message | Error : missing arguments\n");
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
            if (destinataire->socketClient != socketDialogue && destinataire->logged == 1)
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
         cmd_ret(200, socketDialogue, messageEnvoi);
         printf(" • Message | Message has been sent to all users\n");
      }
      else
      {
         cmd_ret(404, socketDialogue, messageEnvoi);
         printf(" • Message | Error : User not found\n");
      }
   }
   else
   {
      cmd_ret(400, socketDialogue, messageEnvoi);
      printf(" • Message | Error : Missing parameters\n");
   }
}

void cmd_version(char *strToken, char messageEnvoi[LG_MESSAGE], int socketDialogue, Contact *contact, int fd_index, char * greating)
{
   const char *separators = " \n";
   strToken = strtok(NULL, separators);
   if (strToken != NULL)
   {
      printf(" • VERSION | User version : %s\n", strToken);
      if (strcmp(strToken, VERSION) != 0)
      {
         printf("    • VERSION | Error 426 : Client need upgrade !\n");
         printf("    • VERSION | Client will be kicked from the server !\n");
         removeUser(contact, fd_index);
         cmd_ret(426, socketDialogue, messageEnvoi);
      }
      else
      {
         strcpy(messageEnvoi, "/greating ");
         strcat(messageEnvoi, greating);
         send(socketDialogue, messageEnvoi, strlen(messageEnvoi), 0);
         usleep(100000);
         strcpy(messageEnvoi, "/login");
         send(socketDialogue, messageEnvoi, strlen(messageEnvoi), 0);
      }
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
      if (tmp->logged == 1)
      {
         strcat(messageEnvoi, " ");
         strcat(messageEnvoi, tmp->login);
      }
      tmp = tmp->suiv;
   }
   write(socketDialogue, messageEnvoi, strlen(messageEnvoi));
   printf(" • USERS | %s\n", messageEnvoi);
}

void cmd_ret(int code, int socketDialogue, char messageEnvoi[LG_MESSAGE])
{
   char codes[10] = "";
   sprintf(codes, "%d", code);
   strcpy(messageEnvoi, "/ret ");
   strcat(messageEnvoi, codes);
   send(socketDialogue, messageEnvoi, strlen(messageEnvoi), 0);
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
         printf(" • LOGIN | Error 409 : Login '%s' already used !\n", strToken);
         cmd_ret(409, socketDialogue, messageEnvoi);
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
            printf(" • LOGIN | User %s logged in !\n", strToken);
            tmp->logged = 1;
            cmd_ret(200, socketDialogue, messageEnvoi);
         }
      }
   }
   else
   {
      printf(" • LOGIN | Error 409 : Login is invalid !\n");
      cmd_ret(409, socketDialogue, messageEnvoi);
   }
}