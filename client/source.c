#include "header.h"

void checkArguments(int argc, char *argv[])
{
   if (argc != 3)
   {
      perror("SERVER | Missing or too many arguments (4 required)");
      exit(-1);
   }
   if (strcmp(argv[1], "-p") != 0)
   {
      perror("SERVER | You need to use : ./server -p [port]");
      exit(-1);
   }

   system("clear");
}

void handleMessage(char messageRecu[LG_MESSAGE], char messageEnvoi[LG_MESSAGE], int socketDialogue, int fd_index) {
   if (strcmp(messageRecu, "\n\0") != 0) {
      char printMessage[LG_MESSAGE];
      memset(printMessage, 0x00, LG_MESSAGE * sizeof(char));
      const char *separators = " \n";
      char *strToken = strtok(messageRecu, separators);
      // Récupération du mot clé

      if (strToken != NULL) {
         if (strcmp(strToken,"/mp") == 0) {
            strToken = strtok(NULL, separators);
            strcpy(printMessage, strToken);
            strcat(printMessage, " : ");
            while (strToken != NULL) {
               strToken = strtok(NULL, separators);
               if (strToken != NULL) {
                  strcat(printMessage, " ");
                  strcat(printMessage, strToken);
               }
            }
            strcat(printMessage, "\n");
            printf("%s", printMessage);
         }
         else if (strcmp(strToken,"/mg") == 0) {
            //color("08");
            //color("30");
            strToken = strtok(NULL, separators);
            strcpy(printMessage, strToken);
            strcat(printMessage, " : ");
            while (strToken != NULL) {
               strToken = strtok(NULL, separators);
               if (strToken != NULL) {
                  strcat(printMessage, " ");
                  strcat(printMessage, strToken);
               }
            }
            strcat(printMessage, "\n");
            printf("%s", printMessage);
         }
      }  
   }
   color("0");
}

void codeError(char messageRecu[LG_MESSAGE])
{
   if(strcmp(messageRecu, "/400"))
   {
      printf("ERROR 400 | The request is not unable");
   }

   if(strcmp(messageRecu, "/404"))
   {
      printf("ERROR 404 | User is not found");
   }

   if(strcmp(messageRecu, "/409"))
   {
      printf("ERROR 409 | Name is already used");
   }

   if(strcmp(messageRecu, "/426"))
   {
      printf("ERROR 426 | Version need to upgraded");
   }

   if(strcmp(messageRecu, "/501"))
   {
      printf("ERROR 501 | Request is not implemented");
   }
}
