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

   display->tampon = malloc(sizeof(char *) * TAMPON_SIZE);
   display->tampon_length = 0;
   display->tampon_cursor = 0;
   display->inputText = malloc(sizeof(char) * LG_MESSAGE);
   strcpy(display->inputText, "");
   display->running = 1;
   display->shift = 0;

   for (int i = 0; i < TAMPON_SIZE; i++)
   {
      display->tampon[i] = malloc(sizeof(char) * LG_MESSAGE);
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
            addInTampon(display, printMessage);
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

void addInTampon(DISPLAY *display, char message[LG_MESSAGE])
{
   if (display->tampon_length < TAMPON_SIZE)
   {
      display->tampon_length++;
   }
   for (int i = TAMPON_SIZE - 1; i > 0; i--)
   {
      strcpy(display->tampon[i], display->tampon[i - 1]);
   }
   strcpy(display->tampon[0], message);
}

void displayTampon(DISPLAY *display)
{
   SDL_Rect position = {0, 0, 0, 0};
   int max = 0;
   int index = display->tampon_cursor * TAMPON_CURSOR_SIZE;
   if (display->tampon_length - index > TAMPON_CURSOR_SIZE)
   {
      max = index + TAMPON_CURSOR_SIZE;
   }
   else
   {
      max = display->tampon_length;
   }
   int y = 0;
   for (int i = max - 1; i >= index; i--)
   {
      position.y = y;
      renderWidgetText(display->tampon[i], NULL, TEXT_SIZE, display->renderer, &position);
      y += TEXT_SIZE;
   }
}

void renderWidgetText(char *message, SDL_Color *color, int fontSize, SDL_Renderer *renderer, SDL_Rect *dstrect)
{
   // Color
   if (color == NULL)
   {
      color = malloc(sizeof(SDL_Color));
      color->r = 255;
      color->g = 255;
      color->b = 255;
   }
   color->a = 255;
   // Open the font
   TTF_Font *font = TTF_OpenFont("police.ttf", fontSize); // Open the font you want
   if (font == NULL)
   {
      SDL_ExitWithError("SDL || TTF_OpenFont");
      return NULL;
   }
   // Render the text to a surface
   SDL_Surface *surf = TTF_RenderText_Solid(font, message, *color);
   if (surf == NULL)
   {
      TTF_CloseFont(font);
      SDL_ExitWithError("SDL || TTF_RenderText");
      return NULL;
   }
   // Create a texture from the surface
   SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surf);
   if (texture == NULL)
   {
      SDL_ExitWithError("SDL || CreateTexture");
   }
   // Take the size
   dstrect->w = surf->w;
   dstrect->h = surf->h;
   // Clean up the surface and font
   SDL_FreeSurface(surf);
   TTF_CloseFont(font);
   // Return the texture
   SDL_RenderCopy(renderer, texture, NULL, dstrect);
   SDL_DestroyTexture(texture);
}

bool handleInput(DISPLAY *display, SDL_Event event)
{
   char key[2];
   if (event.key.keysym.sym == SDLK_CAPSLOCK)
   {
      display->shift = !display->shift;
      printf("%s", display->shift ? "true" : "false");
   }
   if (event.key.keysym.sym >= SDLK_SPACE && event.key.keysym.sym <= SDLK_z)
   {
      if (display->shift)
      {
         if (event.key.keysym.sym >= SDLK_a && event.key.keysym.sym <= SDLK_z)
         {
            key[0] = event.key.keysym.sym - 32;
         }
         else
         {
            switch (event.key.keysym.sym)
            {
            case SDLK_RIGHTPAREN:
               key[0] = '°';
               break;
            case SDLK_EQUALS:
               key[0] = '+';
               break;
            case SDLK_DOLLAR:
               key[0] = '£';
               break;
            case SDLK_ASTERISK:
               key[0] = 'µ';
               break;
            case SDLK_COLON:
               key[0] = '/';
               break;
            case SDLK_SEMICOLON:
               key[0] = '.';
               break;
            case SDLK_COMMA:
               key[0] = '?';
               break;
            case SDLK_LESS:
               key[0] = '>';
               break;
            default:
               key[0] = event.key.keysym.sym;
               break;
            }
         }
      }
      else
      {
         switch (event.key.keysym.sym)
         {
         case SDLK_1:
            key[0] = '&';
            break;
         case SDLK_2:
            key[0] = 233; //é
            printf("%c", key[0]);
            //printf("%d", key[0]);
            break;
         case SDLK_3:
            key[0] = '"';
            break;
         case SDLK_4:
            key[0] = '\'';
            break;
         case SDLK_5:
            key[0] = '(';
            break;
         case SDLK_6:
            key[0] = '-';
            break;
         case SDLK_7:
            key[0] = 'è';
            break;
         case SDLK_8:
            key[0] = '_';
            break;
         case SDLK_9:
            key[0] = 'ç';
            break;
         case SDLK_0:
            key[0] = 'à';
         default:
            key[0] = event.key.keysym.sym;
            break;
         }
      }
      key[1] = '\0';
      // Add the key to the message
      strcat(display->inputText, key);
   }

   if (event.key.keysym.sym == SDLK_BACKSPACE)
   {
      display->inputText[strlen(display->inputText) - 1] = '\0';
   }
   if (event.key.keysym.sym == SDLK_RETURN)
   {
      return true;
   }
   //printf("%s\n", display->inputText);
   return false;
}
