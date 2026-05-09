#ifndef BUTTON_H
#define BUTTON_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>  // TTF_Font-hoz
#include <stdbool.h>

typedef struct Scene Scene; // Előre deklarálás

typedef struct {
    int x, y;
    int w, h;
    const char* text;
    SDL_Color bg_color;
    SDL_Color hover_color;
    SDL_Color text_color;
    bool hovered;
    bool clicked;
    void (*on_click)(void*);
    void* user_data;
} Button;

void button_init(Button* btn, int x, int y, int w, int h, const char* text);
void button_draw(Button* btn, TTF_Font* font, Scene* scene, SDL_Renderer* renderer);
void button_handle_event(Button* btn, SDL_Event* event);
void buttons_handle_event(Button* buttons[], int count, SDL_Event* event);
void buttons_draw(Button* buttons[], int count, TTF_Font* font, Scene* scene, SDL_Renderer* renderer);

#endif