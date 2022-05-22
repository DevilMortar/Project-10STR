#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

typedef struct USER USER; // Structure d'un utilisateur
struct USER
{
   char * login; // Login de l'utilisateur
   SDL_Rect rect; // Rectangle de clic de l'utilisateur
   USER * next; // Pointeur vers l'utilisateur suivant
   bool hover; // Indique si l'utilisateur est survolé
};

typedef struct DISPLAY DISPLAY; // Structure d'une fenêtre
struct DISPLAY
{
   SDL_Window *window; // Fenêtre
   SDL_Renderer *renderer; // Rendu
   bool running; // Indique si la fenêtre est ouverte
   char **tampon; // Tampon de texte
   int tampon_length; // Longueur du tampon
   int tampon_cursor; // Position du curseur dans le tampon
   char **private; // Tampon de messages privés
   char * privateLogin; // Login du destinataire du message privé
   int private_length; // Longueur du tampon de messages privés
   int private_cursor; // Position du curseur dans le tampon de messages privés
   USER * users; // Liste des utilisateurs
   char * login; // Login de l'utilisateur
   char * inputText; // Texte entré par l'utilisateur
   char * prefix; // Préfixe du message
   bool filterActive; // Indique si le filtre est actif
   char * filter; // Filtre
   bool logged; // Indique si l'utilisateur est connecté
   bool shift;  // Indique si le clavier est en majuscule
   int socket; // Socket de dialogue
};

typedef struct BUTTON BUTTON; // Structure d'un bouton
struct BUTTON
{
   SDL_Rect rect; // Rectangle de clic du bouton
   char * text; // Texte du bouton
   bool clicked; // Indique si le bouton est cliqué
   bool hover; // Indique si le bouton est survolé
   bool visible; // Indique si le bouton est visible
   bool enabled; // Indique si le bouton est activé
   void (*callback)(DISPLAY *); // Fonction de callback
   BUTTON * next; // Pointeur vers le bouton suivant
};

//Display
void initDisplay(DISPLAY *display); // Initialise la fenêtre
void SDL_ExitWithError(const char *message); // Affiche un message d'erreur et quitte le programme
SDL_Color * colorMessage(char * message); // Change la couleur du message
void displayTampon(DISPLAY *display); // Affiche le tampon
void displayTamponFiltered(DISPLAY *display); // Affiche le tampon filtré
SDL_Texture * renderWidgetText(char *message, SDL_Color* color, int fontSize, SDL_Renderer *renderer, SDL_Rect *dstrect); // Rendu d'un text
void displayBackground(DISPLAY *display); // Affiche le fond
void displayButtons(DISPLAY *display, BUTTON *buttonsList); // Affiche les boutons
void displayUserList(DISPLAY *display); // Affiche la liste des utilisateurs
void displayUserName(DISPLAY *display); // Affiche le nom de l'utilisateur
void displayInputField(DISPLAY *display); // Affiche le champ de saisie

//Button
BUTTON * createButton(char *text, bool cliked, bool hover, bool visible, bool enabled, BUTTON *buttonList, void (*callback)(DISPLAY *)); // Crée un bouton
bool checkHoverButton(BUTTON * buttonList, SDL_Event event); // Vérifie si un bouton est survolé
bool checkClickButton(BUTTON * buttonList, SDL_Event event, DISPLAY *display); // Vérifie si un bouton est cliqué
void askForUserList(DISPLAY *display); // Demande la liste des utilisateurs
void leaveChat(DISPLAY *display); // Quitte le chat
void switchToMg(DISPLAY *display); // Change d'onglet
bool checkHoverUser(DISPLAY *display, SDL_Event event); // Vérifie si un utilisateur est survolé
bool checkClickUser(DISPLAY *display, SDL_Event event); // Vérifie si un utilisateur est cliqué