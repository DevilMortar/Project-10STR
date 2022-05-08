#include "header.h"

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

void displayTamponFiltered(DISPLAY *display)
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
        if (strstr(display->tampon[i], display->filter) != NULL && strstr(display->tampon[i], "->") != NULL && strstr(display->tampon[i], ":") != NULL)
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
            else
            {
                color = malloc(sizeof(SDL_Color));
                color->r = 0;
                color->g = 0;
                color->b = 255;
                color->a = 255;
            }
            position.y = y;
            SDL_Texture *text = renderWidgetText(display->tampon[i], color, TEXT_SIZE, display->renderer, &position);
            SDL_RenderCopy(display->renderer, text, NULL, &position);
            SDL_DestroyTexture(text);
            y += TEXT_SIZE;
        }
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

void displayBackground(DISPLAY *display)
{
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
}

void displayButtons(DISPLAY *display, BUTTON *buttonsList)
{
    // Display the buttons
    BUTTON *tmp = buttonsList;
    SDL_Rect button = {10, WINDOW_HEIGHT - 20, WINDOW_WIDTH - 20, TEXT_SIZE + 10};
    SDL_Color black = {0, 0, 0, 255};
    while (tmp != NULL)
    {
        if (tmp->visible)
        {

            SDL_Texture *texture = renderWidgetText(tmp->text, &black, TEXT_SIZE, display->renderer, &button);
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
            SDL_DestroyTexture(texture);
        }
        tmp = tmp->next;
    }
}

void displayUserList(DISPLAY *display)
{
    // Display the user list
    SDL_Rect listItem = {WINDOW_WIDTH - LIST_WIDTH + 10, HEADER_HEIGHT + 10, LIST_WIDTH - 20, TEXT_SIZE};
    USER *user = display->users;
    SDL_Color color = {255, 255, 255, 255};
    while (user != NULL)
    {
        if (strstr(display->filter, user->login) != NULL)
        {
            color.r = 255;
            color.g = 0;
            color.b = 0;
            color.a = 255;
        }
        else if (user->hover)
        {
            color.r = 255;
            color.g = 255;
            color.b = 255;
            color.a = 255;
        }
        else
        {
            color.r = 200;
            color.g = 200;
            color.b = 200;
            color.a = 255;
        }
        SDL_Texture *texture = renderWidgetText(user->login, &color, TEXT_SIZE, display->renderer, &listItem);
        SDL_RenderCopy(display->renderer, texture, NULL, &listItem);
        user->rect = listItem;
        listItem.y += TEXT_SIZE + 10;
        user = user->next;
    }
}

void displayUserName(DISPLAY *display)
{
    // Display connected as ...
    SDL_Rect connectedAs = {10, HEADER_HEIGHT - 10 - TEXT_SIZE, 0, 0};
    SDL_Color black = {0, 0, 0, 255};
    char *connectedAsText = malloc(LG_MESSAGE * sizeof(char));
    sprintf(connectedAsText, "Connected as %s", display->login);
    SDL_Texture *texture = renderWidgetText(connectedAsText, &black, TEXT_SIZE, display->renderer, &connectedAs);
    SDL_RenderCopy(display->renderer, texture, NULL, &connectedAs);
    SDL_DestroyTexture(texture);
    free(connectedAsText);
}

void displayInputField(DISPLAY *display)
{
    // Display the input field
    SDL_Rect inputField = {10, WINDOW_HEIGHT - FOOTER_HEIGHT + 10, WINDOW_WIDTH - 20, TEXT_SIZE + 10};
    SDL_Color black = {0, 0, 0, 255};
    inputField.y += 6;
    inputField.x += 10;
    SDL_Texture *texture = renderWidgetText(display->prefix, &black, TEXT_SIZE, display->renderer, &inputField);
    SDL_RenderCopy(display->renderer, texture, NULL, &inputField);
    inputField.x += inputField.w + 10;
    SDL_Texture *inputText = NULL;
    if (strlen(display->inputText) > 0)
        inputText = renderWidgetText(display->inputText, &black, TEXT_SIZE, display->renderer, &inputField);
    SDL_RenderCopy(display->renderer, inputText, NULL, &inputField);
    SDL_DestroyTexture(texture);
    SDL_DestroyTexture(inputText);
}