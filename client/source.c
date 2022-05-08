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
   display->prefix = malloc(sizeof(char) * LG_MESSAGE);
   display->login = malloc(sizeof(char) * LG_MESSAGE);
   display->users = NULL;
   strcpy(display->prefix, "/login");
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
            strToken = strtok(NULL, separators);
            strcpy(printMessage, strToken);
            strcat(printMessage, " to you : ");
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
         else if (strcmp(strToken, "/mg") == 0)
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
            addInTampon(display, printMessage);
         }
         else if (strcmp(strToken, "/users") == 0)
         {
            strcat(printMessage, "Users connected :\n");
            display->users = freeUserList(display->users);
            while (strToken != NULL)
            {
               strToken = strtok(NULL, separators);
               if (strToken != NULL)
               {
                  strcat(printMessage, strToken);
                  USER *new = malloc(sizeof(USER));
                  new->login = malloc(sizeof(char) * LG_MESSAGE);
                  strcpy(new->login, strToken);
                  new->next = display->users;
                  new->hover = 0;
                  display->users = new;
                  strcat(printMessage, "\n");
               }
            }
            printf("%s", printMessage);
            addInTampon(display, printMessage);
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
            printf("Choose a login (max 20 characters, no spaces): \n");
            addInTampon(display, "Choose a login (max 20 characters, no spaces): \n");
         }
         else if (strcmp(strToken, "/ret") == 0)
         {
            strToken = strtok(NULL, separators);
            codeError(strToken, display);
         }
      }
   }
   color("0");
}

void codeError(char *strToken, DISPLAY *display)
{
   if (strcmp(strToken, "400") == 0)
   {
      addInTampon(display, "ERROR 400 | The request is not unable\n");
   }

   else if (strcmp(strToken, "404") == 0)
   {
      addInTampon(display, "ERROR 404 | User is not found\n");
   }

   else if (strcmp(strToken, "409") == 0)
   {
      addInTampon(display, "ERROR 409 | You can't choose this name\n");
   }

   else if (strcmp(strToken, "426") == 0)
   {
      addInTampon(display, "ERROR 426 | Version need to upgraded\n");
   }

   else if (strcmp(strToken, "501") == 0)
   {
      addInTampon(display, "ERROR 501 | Request is not implemented\n");
   }
   else if (strcmp(strToken, "200") == 0)
   {
      if (display->logged == 0)
      {
         display->logged = 1;
         printf("Login successful\n");
         clearTampon(display);
         strcpy(display->prefix, "/mg");
      }
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

void clearTampon(DISPLAY *display)
{
   for (int i = 0; i < TAMPON_SIZE; i++)
   {
      memset(display->tampon[i], 0x00, LG_MESSAGE * sizeof(char));
   }
   display->tampon_length = 0;
}

void displayTampon(DISPLAY *display)
{
   SDL_Rect position = {5, 0, 0, 0};
   int max = 0;
   int index = display->tampon_cursor;
   if (display->tampon_length - index > TAMPON_CURSOR_SIZE)
   {
      max = index + TAMPON_CURSOR_SIZE;
   }
   else
   {
      max = display->tampon_length;
   }
   int y = HEADER_HEIGHT + 10;
   for (int i = max - 1; i >= index; i--)
   {
      SDL_Color *color;
      if (strstr(display->tampon[i], "ERROR") != NULL)
      {
         color = malloc(sizeof(SDL_Color));
         color->r = 255;
         color->g = 0;
         color->b = 0;
         color->a = 255;
      }
      else if (strstr(display->tampon[i], "->") != NULL && strstr(display->tampon[i], ":") != NULL)
      {
         color = malloc(sizeof(SDL_Color));
         color->r = 0;
         color->g = 0;
         color->b = 255;
         color->a = 255;
      }
      else
      {
         color = NULL;
      }
      position.y = y;
      SDL_Texture *text = renderWidgetText(display->tampon[i], color, TEXT_SIZE, display->renderer, &position);
      SDL_RenderCopy(display->renderer, text, NULL, &position);
      SDL_DestroyTexture(text);
      y += TEXT_SIZE;
   }
}

SDL_Texture *renderWidgetText(char *message, SDL_Color *color, int fontSize, SDL_Renderer *renderer, SDL_Rect *dstrect)
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
   return texture;
}

void displayInterface(DISPLAY *display, BUTTON *buttonList)
{
   SDL_RenderClear(display->renderer);
   // Render the background
   SDL_Rect header = {0, 0, WINDOW_WIDTH, HEADER_HEIGHT};
   SDL_Rect body = {0, HEADER_HEIGHT, WINDOW_WIDTH, BODY_HEIGHT};
   SDL_Rect footer = {0, WINDOW_HEIGHT - FOOTER_HEIGHT, WINDOW_WIDTH, FOOTER_HEIGHT};
   SDL_Rect list = {WINDOW_WIDTH - LIST_WIDTH, HEADER_HEIGHT, LIST_WIDTH, BODY_HEIGHT};
   SDL_Rect inputField = {10, WINDOW_HEIGHT - FOOTER_HEIGHT + 10, WINDOW_WIDTH - 20, TEXT_SIZE + 10};
   SDL_Color black = {0, 0, 0, 255};
   SDL_SetRenderDrawColor(display->renderer, 80, 80, 80, 255);
   SDL_RenderFillRect(display->renderer, &header);
   SDL_SetRenderDrawColor(display->renderer, 190, 190, 190, 255);
   SDL_RenderFillRect(display->renderer, &body);
   SDL_SetRenderDrawColor(display->renderer, 100, 100, 100, 255);
   SDL_RenderFillRect(display->renderer, &footer);
   SDL_SetRenderDrawColor(display->renderer, 255, 255, 255, 255);
   SDL_RenderFillRect(display->renderer, &inputField);
   SDL_SetRenderDrawColor(display->renderer, 0, 0, 0, 255);
   SDL_RenderFillRect(display->renderer, &list);
   // Render the text
   inputField.y += 6;
   inputField.x += 10;
   SDL_Texture *texture = renderWidgetText(display->prefix, &black, TEXT_SIZE, display->renderer, &inputField);
   SDL_RenderCopy(display->renderer, texture, NULL, &inputField);
   inputField.x += inputField.w + 10;
   SDL_Texture *inputText = NULL;
   if (strlen(display->inputText) > 0)
      inputText = renderWidgetText(display->inputText, &black, TEXT_SIZE, display->renderer, &inputField);
   SDL_RenderCopy(display->renderer, inputText, NULL, &inputField);
   if (display->logged == 1)
   {
      // Display connected as ...
      SDL_Rect connectedAs = {10, HEADER_HEIGHT - 10 - TEXT_SIZE, 0, 0};
      char *connectedAsText = malloc(LG_MESSAGE * sizeof(char));
      sprintf(connectedAsText, "Connected as %s", display->login);
      texture = renderWidgetText(connectedAsText, &black, TEXT_SIZE, display->renderer, &connectedAs);
      SDL_RenderCopy(display->renderer, texture, NULL, &connectedAs);
      // Display the list
      SDL_Rect listItem = {WINDOW_WIDTH - LIST_WIDTH + 10, HEADER_HEIGHT + 10, LIST_WIDTH - 20, TEXT_SIZE};
      USER *user = display->users;
      SDL_Color color = {255, 255, 255, 255};
      while (user != NULL)
      {
         if (user->hover) {
            color.r = 255;
            color.g = 255;
            color.b = 255;
            color.a = 255;
         }
         else {
            color.r = 200;
            color.g = 200;
            color.b = 200;
            color.a = 255;
         }
         texture = renderWidgetText(user->login, &color, TEXT_SIZE, display->renderer, &listItem);
         SDL_RenderCopy(display->renderer, texture, NULL, &listItem);
         user->rect = listItem;
         listItem.y += TEXT_SIZE + 10;
         user = user->next;
      }
      // Display the buttons
      BUTTON *tmp = buttonList;
      SDL_Rect button = {10, WINDOW_HEIGHT - 20, WINDOW_WIDTH - 20, TEXT_SIZE + 10};
      while (tmp != NULL)
      {
         if (tmp->visible)
         {

            texture = renderWidgetText(tmp->text, &black, TEXT_SIZE, display->renderer, &button);
            SDL_Rect buttonFrame = {button.x - 5, button.y - 5, button.w + 10, button.h + 10};
            tmp->rect = buttonFrame;
            if (tmp->clicked)
            {
               SDL_SetRenderDrawColor(display->renderer, 150, 150, 150, 225);
            }
            else if (tmp->hover)
            {
               SDL_SetRenderDrawColor(display->renderer, 255, 255, 255, 225);
            }
            else
            {
               SDL_SetRenderDrawColor(display->renderer, 200, 200, 200, 255);
            }
            SDL_RenderFillRect(display->renderer, &buttonFrame);
            SDL_RenderCopy(display->renderer, texture, NULL, &button);
            button.x += buttonFrame.w + 10;
         }
         tmp = tmp->next;
      }
   }
   displayTampon(display);
   SDL_DestroyTexture(texture);
   SDL_DestroyTexture(inputText);
}

bool handleInput(DISPLAY *display, SDL_Event event)
{
   char key[2];
   if (event.key.keysym.sym == SDLK_CAPSLOCK)
   {
      display->shift = !display->shift;
   }
   else if (event.key.keysym.sym >= SDLK_SPACE && event.key.keysym.sym <= SDLK_z)
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
            // printf("%d", key[0]);
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

   else if (event.key.keysym.sym == SDLK_BACKSPACE)
   {
      display->inputText[strlen(display->inputText) - 1] = '\0';
   }
   else if (event.key.keysym.sym == SDLK_UP)
   {
      if (display->tampon_cursor < display->tampon_length - TAMPON_CURSOR_SIZE)
         display->tampon_cursor++;
   }
   else if (event.key.keysym.sym == SDLK_DOWN)
   {
      if (display->tampon_cursor > 0)
         display->tampon_cursor--;
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
   char messageEnvoi[LG_MESSAGE] = "";
   strcpy(messageEnvoi, "/users");
   write(display->socket, messageEnvoi, LG_MESSAGE);
}

void checkHoverUser(DISPLAY *display, SDL_Event event) {
   if (display->users != NULL) {
      USER *tmp = display->users;
      SDL_Point mouse = {event.button.x, event.button.y};
      while (tmp != NULL) {
         if(SDL_PointInRect(&mouse, &tmp->rect)) {
            tmp->hover = true;
         } else {
            tmp->hover = false;
         }
         tmp = tmp->next;
      }
   }
}

void checkClickUser(DISPLAY *display, SDL_Event event) {
   if (display->users != NULL) {
      USER *tmp = display->users;
      SDL_Point mouse = {event.button.x, event.button.y};
      while (tmp != NULL) {
         if(SDL_PointInRect(&mouse, &tmp->rect)) {
            strcpy(display->prefix, "/mp ");
            strcat(display->prefix, tmp->login);
            strcat(display->inputText, "");
         }
         tmp = tmp->next;
      }
   }
}

void checkOverButton(BUTTON *buttonList, SDL_Event event)
{
   BUTTON *tmp = buttonList;
   while (tmp != NULL)
   {
      if (tmp->visible && tmp->enabled)
      {
         SDL_Point point = {event.motion.x, event.motion.y};
         if (SDL_PointInRect(&point, &tmp->rect))
         {
            tmp->hover = true;
         }
         else
         {
            tmp->hover = false;
         }
      }
      tmp->clicked = false;
      tmp = tmp->next;
   }
}

void checkClickButton(BUTTON *buttonList, SDL_Event event, DISPLAY *display)
{
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
         else
         {
            tmp->clicked = false;
         }
      }
      tmp = tmp->next;
   }
}

void sendMessage(DISPLAY *display)
{
   char messageEnvoi[LG_MESSAGE] = "";
   strcpy(messageEnvoi, display->prefix);
   strcat(messageEnvoi, " ");
   strcat(messageEnvoi, display->inputText);
   char printMessage[LG_MESSAGE] = "";
   if (strcmp(display->prefix, "/mg") == 0)
   {
      strcpy(printMessage, display->login);
      strcat(printMessage, " : ");
      strcat(printMessage, display->inputText);
      addInTampon(display, printMessage);
   }
   else if (strstr(display->prefix, "/mp") != NULL)
   {
      strcpy(printMessage, display->login);
      char * tmp = malloc(sizeof(char) * LG_MESSAGE);
      strcpy(tmp, display->prefix);
      char * strToken = strtok(tmp, " ");
      strToken = strtok(NULL, " ");
      strcat(printMessage, " -> ");
      strcat(printMessage, strToken);
      strcat(printMessage, " : ");
      strcat(printMessage, display->inputText);
      addInTampon(display, printMessage);
   }
   else if (strcmp(display->prefix, "/login") == 0)
   {
      strcpy(display->login, display->inputText);
   }
   send(display->socket, messageEnvoi, LG_MESSAGE, 0);
   strcpy(display->inputText, "");
}

USER *freeUserList(USER *userList)
{
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
