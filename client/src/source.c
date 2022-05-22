#include "../include/header.h"

void initDisplay(DISPLAY *display)
{
   // On initialise la fenêtre
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

   // On initialise la structure display
   display->tampon = malloc(sizeof(char *) * TAMPON_SIZE);
   display->tampon_length = 0;
   display->tampon_cursor = 0;
   display->private = malloc(sizeof(char *));
   display->private_length = 0;
   display->private_cursor = 0;
   display->privateLogin = malloc(sizeof(char *));
   display->inputText = malloc(sizeof(char) * LG_MESSAGE);
   strcpy(display->inputText, "");
   display->running = 1;
   display->shift = 0;
   display->prefix = malloc(sizeof(char) * LG_MESSAGE);
   strcpy(display->prefix, "/login");
   display->login = malloc(sizeof(char) * LG_MESSAGE);
   strcpy(display->login, "");
   display->filter = malloc(sizeof(char) * LG_MESSAGE);
   strcpy(display->filter, "");
   display->filterActive = 0;
   display->users = NULL;
   display->logged = 0;

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
            // Réception d'un message privé
            // On récupère le login de l'expéditeur
            strToken = strtok(NULL, separators);
            strcpy(printMessage, strToken);
            strcat(printMessage, " -> ");
            strcat(printMessage, display->login);
            strcat(printMessage, " : ");
            // On récupère le message
            while (strToken != NULL)
            {
               strToken = strtok(NULL, separators);
               if (strToken != NULL)
               {
                  strcat(printMessage, " ");
                  strcat(printMessage, strToken);
               }
            } 
            // On ajoute le message au tampon
            strcat(printMessage, "\n");
            printf("%s", printMessage);
            addInTampon(display, printMessage);
            getPrivate(display, display->privateLogin);
         }
         else if (strcmp(strToken, "/mg") == 0)
         {
            // Réception d'un message global
            // On récupère l'emetteur
            strToken = strtok(NULL, separators);
            strcpy(printMessage, strToken);
            strcat(printMessage, " : ");
            // On récupère le message
            while (strToken != NULL)
            {
               strToken = strtok(NULL, separators);
               if (strToken != NULL)
               {
                  strcat(printMessage, " ");
                  strcat(printMessage, strToken);
               }
            }
            // On ajoute le message au tampon
            strcat(printMessage, "\n");
            printf("%s", printMessage);
            addInTampon(display, printMessage);
         }
         else if (strcmp(strToken, "/users") == 0)
         {
            // Réception de la liste des utilisateurs
            strcat(printMessage, "STATUS | User connected : {\n");
            display->users = freeUserList(display->users);
            while (strToken != NULL)
            {
               strToken = strtok(NULL, separators);
               if (strToken != NULL)
               {
                  strcat(printMessage, " ");
                  strcat(printMessage, strToken);
                  USER *new = malloc(sizeof(USER));
                  new->login = malloc(sizeof(char) * LG_MESSAGE);
                  SDL_Rect rect = {0, 0, 0, 0};
                  new->rect = rect;
                  strcpy(new->login, strToken);
                  new->next = display->users;
                  new->hover = 0;
                  display->users = new;
               }
            }
            // On ajoute le message au tampon
            strcat(printMessage, " }\n");
            addInTampon(display, printMessage);
         }
         else if (strcmp(strToken, "/greating") == 0)
         {
            // Réception de la bienvenue
            while (strToken != NULL)
            {
               strToken = strtok(NULL, separators);
               if (strToken != NULL)
               {
                  strcat(printMessage, strToken);
                  strcat(printMessage, " ");
               }
            }
            // On ajoute le message au tampon
            addInTampon(display, printMessage);
         }
         else if (strcmp(strToken, "/login") == 0)
         {
            // Réception de la demande de login
            strcpy(display->prefix, "/login");
            addInTampon(display, "Choose a login (max 20 characters, no spaces): \n");
         }
         else if (strcmp(strToken, "/ret") == 0)
         {
            // Réception du retour serveur
            strToken = strtok(NULL, separators);
            codeError(strToken, display);
         }
      }
   }
}

void codeError(char *strToken, DISPLAY *display)
{
   // On ajoute l'erreur au tampon
   if (strcmp(strToken, "400") == 0)
   {
      addInTampon(display, "ERROR 400 | The request is not unable");
   }

   else if (strcmp(strToken, "404") == 0)
   {
      addInTampon(display, "ERROR 404 | User is not found");
      getPrivate(display, display->privateLogin);
      askForUserList(display);
      display->tampon_cursor = 0;
   }

   else if (strcmp(strToken, "409") == 0)
   {
      addInTampon(display, "ERROR 409 | You can't choose this name");
      strcpy(display->prefix, "/login");
      addInTampon(display, "Choose a login (max 20 characters, no spaces): \n");
   }

   else if (strcmp(strToken, "426") == 0)
   {
      addInTampon(display, "ERROR 426 | Version need to upgraded");
      printf("ERROR 426 | Version need to upgraded\n");
      display->logged = 0;
      display->running = 0;
   }

   else if (strcmp(strToken, "501") == 0)
   {
      addInTampon(display, "ERROR 501 | Request is not implemented");
   }
   else if (strcmp(strToken, "200") == 0)
   {
      if (display->logged == 0)
      {
         display->logged = 1;
         clearTampon(display);
         strcpy(display->prefix, "/mg");
         askForUserList(display);
      }
   }
}

void getPrivate(DISPLAY *display, char *login)
{
   // On récupère les message privés d'un utilisateur
   strcpy(display->privateLogin, login);
   // On libère le tampon privé
   for (int i = display->private_length - 1; i >= 0; i--)
   {
      free(display->private[i]);
   }
   display->private_length = 0;
   // On récupère les messages privés de l'utilisateur demandé
   for (int i = display->tampon_length - 1; i >= 0; i--)
   {
      if (strstr(display->tampon[i], login) != NULL && strstr(display->tampon[i], " -> ") != NULL || strcmp(display->tampon[i], "ERROR 404 | User is not found") == 0)
      {
         display->private_length++;
         display->private = realloc(display->private, sizeof(char *) * display->private_length);
         for (int j = display->private_length - 1; j > 0; j--)
         {
            display->private[j] = display->private[j - 1];
         }
         display->private[0] = malloc(sizeof(char) * LG_MESSAGE);
         strcpy(display->private[0], display -> tampon[i]);
      }
   }
}

void addInTampon(DISPLAY *display, char message[LG_MESSAGE])
{
   // On ajoute un message au tampon
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

void clearTampon(DISPLAY *display)
{
   // On vide le tampon
   for (int i = 0; i < TAMPON_SIZE; i++)
   {
      memset(display->tampon[i], 0x00, LG_MESSAGE * sizeof(char));
   }
   display->tampon_length = 0;
   clearPrivate(display);
}

void clearPrivate(DISPLAY *display)
{
   // On vide le tampon des messages privés
   for (int i = 0; i < display->private_length; i++)
   {
      free(display->private[i]);
   }
   display->private_length = 0;
}

bool handleInput(DISPLAY *display, SDL_Event event)
{
   // Gestion des entrées clavier
   char key[2];
   // Activation du Caps Lock
   if (event.key.keysym.sym == SDLK_CAPSLOCK)
   {
      display->shift = !display->shift;
   }
   // Si la touche est un caractère
   else if (event.key.keysym.sym >= SDLK_SPACE && event.key.keysym.sym <= SDLK_z)
   {
      // Si le mode majuscule est activé
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
      // Si le mode majuscule n'est pas activé
      else
      {
         switch (event.key.keysym.sym)
         {
         case SDLK_1:
            key[0] = '&';
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
         case SDLK_8:
            key[0] = '_';
            break;
         default:
            key[0] = event.key.keysym.sym;
            break;
         }
      }
      key[1] = '\0';
      // Ajout du caractère à la zone de texte
      strcat(display->inputText, key);
   }

   else if (event.key.keysym.sym == SDLK_BACKSPACE)
   {
      // Suppression du dernier caractère de la zone de texte
      display->inputText[strlen(display->inputText) - 1] = '\0';
   }
   else if (event.key.keysym.sym == SDLK_UP)
   {
      if (!display->filterActive)
      {
         // On défile vers le haut dans le tampon
         if (display->tampon_cursor < display->tampon_length - TAMPON_CURSOR_SIZE)
            display->tampon_cursor++;
      }
      else
      {
         // On défile vers le haut dans le tampon filtré
         if (display->private_cursor < display->private_length - TAMPON_CURSOR_SIZE)
            display->private_cursor++;
      }
   }
   else if (event.key.keysym.sym == SDLK_DOWN)
   {
      if (!display->filterActive)
      {
         // On défile vers le bas dans le tampon
         if (display->tampon_cursor > 0)
            display->tampon_cursor--;
      }
      else
      {
         // On défile vers le bas dans le tampon filtré
         if (display->private_cursor > 0)
            display->private_cursor--;
      }  
   }
   else if (event.key.keysym.sym == SDLK_RETURN)
   {
      if (strlen(display->inputText) > 0)
      {
         return true;
      }
   }
   // printf("%s\n", display->inputText);
   return false;
}

BUTTON *createButton(char *text, bool cliked, bool hover, bool visible, bool enabled, BUTTON *buttonList, void (*callback)(DISPLAY *))
{
   // Création d'un bouton
   BUTTON *button = malloc(sizeof(BUTTON));
   button->text = text;
   button->clicked = cliked;
   button->hover = hover;
   button->visible = visible;
   button->enabled = enabled;
   button->next = buttonList;
   button->callback = callback;
   return button;
}

void askForUserList(DISPLAY *display)
{
   // On demande au serveur la liste des utilisateurs
   char messageEnvoi[LG_MESSAGE] = "";
   strcpy(messageEnvoi, "/users");
   write(display->socket, messageEnvoi, LG_MESSAGE);
   // On passe en mode mg
   switchToMg(display);
}

void switchToMg(DISPLAY *display)
{
   // On passe en mode mg
   strcpy(display->inputText, "");
   strcpy(display->prefix, "/mg");
   display->filterActive = false;
   strcpy(display->filter, "");
   display->tampon_cursor = 0;
}

bool checkHoverUser(DISPLAY *display, SDL_Event event)
{
   // On vérifie si le curseur est sur un utilisateur
   bool action = false;
   if (display->users != NULL)
   {
      USER *tmp = display->users;
      SDL_Point mouse = {event.button.x, event.button.y};
      while (tmp != NULL)
      {
         if (SDL_PointInRect(&mouse, &tmp->rect))
         {
            if (!tmp->hover)
            {
               tmp->hover = true;
               action = true;
            }
         }
         else if (tmp->hover)
         {
            tmp->hover = false;
            action = true;
         }
         tmp = tmp->next;
      }
   }
   return action;
}

bool checkClickUser(DISPLAY *display, SDL_Event event)
{
   // On vérifie si un utilisateur a été cliqué
   bool action = false;
   if (display->users != NULL)
   {
      USER *tmp = display->users;
      SDL_Point mouse = {event.button.x, event.button.y};
      while (tmp != NULL)
      {
         if (SDL_PointInRect(&mouse, &tmp->rect))
         {
            action = true;
            if (strcmp(display->filter, tmp->login) != 0)
            {
               // Si l'utilisateur n'est pas déjà sélectionné on passera en mode mp
               strcpy(display->prefix, "/mp ");
               strcat(display->prefix, tmp->login);
               strcat(display->inputText, "");
               display->filterActive = true;
               display->private_cursor = 0;
               strcpy(display->filter, tmp->login);
               getPrivate(display, tmp->login);
            }
            else
            {
               // Si l'utilisateur est déjà sélectionné on passera en mode mg
               switchToMg(display);
            }
         }
         tmp = tmp->next;
      }
   }
   return action;
}

bool checkHoverButton(BUTTON *buttonList, SDL_Event event)
{
   // On vérifie si le curseur est sur un bouton
   bool action = false;
   BUTTON *tmp = buttonList;
   while (tmp != NULL)
   {
      if (tmp->visible && tmp->enabled)
      {
         SDL_Point point = {event.motion.x, event.motion.y};
         if (SDL_PointInRect(&point, &tmp->rect))
         {
            if (!tmp->hover)
            {
               tmp->hover = true;
               action = true;
            }
         }
         else if (tmp->hover)
         {
            tmp->hover = false;
            action = true;
         }
      }
      tmp->clicked = false;
      tmp = tmp->next;
   }
   return action;
}

bool checkClickButton(BUTTON *buttonList, SDL_Event event, DISPLAY *display)
{
   // On vérifie si un bouton a été cliqué
   bool action = false;
   BUTTON *tmp = buttonList;
   while (tmp != NULL)
   {
      if (tmp->visible && tmp->enabled)
      {
         SDL_Point point = {event.motion.x, event.motion.y};
         if (SDL_PointInRect(&point, &tmp->rect))
         {
            tmp->clicked = true;
            tmp->callback(display);
         }
         else if (tmp->clicked)
         {
            tmp->clicked = false;
            action = true;
         }
      }
      tmp = tmp->next;
   }
   return action;
}

void sendMessage(DISPLAY *display)
{
   // On construit le message à envoyer
   char messageEnvoi[LG_MESSAGE] = "";
   strcpy(messageEnvoi, display->prefix);
   strcat(messageEnvoi, " ");
   strcat(messageEnvoi, display->inputText);
   char printMessage[LG_MESSAGE] = "";
   if (strcmp(display->prefix, "/mg") == 0)
   {
      // On affiche le message comme un message global
      strcpy(printMessage, display->login);
      strcat(printMessage, " : ");
      strcat(printMessage, display->inputText);
      addInTampon(display, printMessage);
   }
   else if (strstr(display->prefix, "/mp") != NULL)
   {
      // On affiche le message comme un message privé
      strcpy(printMessage, display->login);
      char *tmp = malloc(sizeof(char) * LG_MESSAGE);
      strcpy(tmp, display->prefix);
      char *strToken = strtok(tmp, " ");
      strToken = strtok(NULL, " ");
      strcat(printMessage, " -> ");
      strcat(printMessage, strToken);
      strcat(printMessage, " : ");
      strcat(printMessage, display->inputText);
      addInTampon(display, printMessage);
      getPrivate(display, strToken);
   }
   else if (strcmp(display->prefix, "/login") == 0)
   {
      // On affiche le message comme un message de login
      strcpy(display->login, display->inputText);
   }
   send(display->socket, messageEnvoi, LG_MESSAGE, 0);
   strcpy(display->inputText, "");
}

USER *freeUserList(USER *userList)
{
   // On libère la liste des utilisateurs
   if (userList != NULL)
   {
      USER *tmp = userList;
      USER *tmp2 = NULL;
      while (tmp != NULL)
      {
         tmp2 = tmp->next;
         free(tmp);
         tmp = tmp2;
      }
   }
   return NULL;
}
