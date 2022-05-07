#include "header.h"

void initDisplay(DISPLAY *display)
{
   if (!IMG_Init(IMG_INIT_PNG))
      printf("\033[1;31mIMG INIT: %s\033[0m\n", IMG_GetError());
   if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
   {
      SDL_ExitWithError("SDL | Failed to initialize");
   }
   if (TTF_Init() != 0)
   {
      SDL_ExitWithError("SDL | TTF: Failed to initialize");
   }

   display->window = SDL_CreateWindow("10-STR", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, 0);

   if (display->window == NULL)
   {
      SDL_ExitWithError("SDL | Failed to create a window");
   }

   display->renderer = SDL_CreateRenderer(display->window, -1, SDL_RENDERER_ACCELERATED);
   if (display->renderer == NULL)
   {
      SDL_ExitWithError("SDL | Failed to create a renderer");
   }
}

void SDL_ExitWithError(const char *message)
{
   SDL_Log("ERREUR : %s > %s\n", message, SDL_GetError());
   SDL_Quit();
   exit(EXIT_FAILURE);
}

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

void handleMessage(char messageRecu[LG_MESSAGE], char messageEnvoi[LG_MESSAGE], int socketDialogue, DISPLAY *display)
{
   if (strcmp(messageRecu, "\n\0") != 0)
   {
      char printMessage[LG_MESSAGE];
      memset(printMessage, 0x00, LG_MESSAGE * sizeof(char));
      const char *separators = " \n";
      char *strToken = strtok(messageRecu, separators);
      // Récupération du mot clé

      if (strToken != NULL)
      {
         if (strcmp(strToken, "/mp") == 0)
         {
            strToken = strtok(NULL, separators);
            strcpy(printMessage, strToken);
            strcat(printMessage, " : ");
            while (strToken != NULL)
            {
               strToken = strtok(NULL, separators);
               if (strToken != NULL)
               {
                  strcat(printMessage, " ");
                  strcat(printMessage, strToken);
               }
            }
            strcat(printMessage, "\n");
            printf("%s", printMessage);
         }
         else if (strcmp(strToken, "/mg") == 0)
         {
            // color("08");
            // color("30");
            strToken = strtok(NULL, separators);
            strcpy(printMessage, strToken);
            strcat(printMessage, " : ");
            while (strToken != NULL)
            {
               strToken = strtok(NULL, separators);
               if (strToken != NULL)
               {
                  strcat(printMessage, " ");
                  strcat(printMessage, strToken);
               }
            }
            strcat(printMessage, "\n");
            printf("%s", printMessage);
         }
         else if (strcmp(strToken, "/users") == 0)
         {
            strcat(printMessage, "Liste des utilisateurs connectés: \n");
            while (strToken != NULL)
            {
               strToken = strtok(NULL, separators);
               if (strToken != NULL)
               {
                  strcat(printMessage, strToken);
                  strcat(printMessage, "\n");
               }
            }
            printf("%s", printMessage);
         }
         else if (strcmp(strToken, "/greating") == 0)
         {
            while (strToken != NULL)
            {
               strToken = strtok(NULL, separators);
               if (strToken != NULL)
               {
                  strcat(printMessage, strToken);
                  strcat(printMessage, " ");
               }
            }
            strcat(printMessage, "\n");
            printf("%s", printMessage);
         }
         else if (strcmp(strToken, "/login") == 0)
         {
            printf("Choisissez un nom d'utilisateur (20 caractères max, espace interdit): \n");

            char login[LG_MESSAGE];
            memset(login, 0x00, LG_MESSAGE * sizeof(char));
            fgets(login, LG_MESSAGE, stdin);
            strcpy(messageEnvoi, "/login ");
            strcat(messageEnvoi, login);
            send(socketDialogue, messageEnvoi, LG_MESSAGE, 0);
         }
         else if (strcmp(strToken, "/ret") == 0)
         {
            strToken = strtok(NULL, separators);
            codeError(strToken);
         }
      }
   }
   color("0");
}

void codeError(char *strToken)
{
   if (strcmp(strToken, "400") == 0)
   {
      printf("ERROR 400 | The request is not unable\n");
   }

   else if (strcmp(strToken, "404") == 0)
   {
      printf("ERROR 404 | User is not found\n");
   }

   else if (strcmp(strToken, "409") == 0)
   {
      printf("ERROR 409 | You can't choose this name\n");
   }

   else if (strcmp(strToken, "426") == 0)
   {
      printf("ERROR 426 | Version need to upgraded\n");
   }

   else if (strcmp(strToken, "501") == 0)
   {
      printf("ERROR 501 | Request is not implemented\n");
   }
}
