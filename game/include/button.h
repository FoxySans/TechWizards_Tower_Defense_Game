#ifndef BUTTON_H
#define BUTTON_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdbool.h>
#include <GL/gl.h> 
#include "scene.h" 

typedef struct {
    int x, y, w, h;
    char text[32];
    SDL_Color bg_color;
    SDL_Color hover_color;
    SDL_Color text_color;
    bool hovered;
    bool clicked;
    void (*on_click)(void* data);
    void* user_data;
} Button;

void button_init(Button* btn, int x, int y, int w, int h, const char* text);
void button_draw(Button* btn, TTF_Font* font, Scene* scene, SDL_Renderer* renderer);
void buttons_draw(Button** buttons, int count, TTF_Font* font, Scene* scene, SDL_Renderer* renderer);
void buttons_handle_event(Button** buttons, int count, SDL_Event* event);

#endif