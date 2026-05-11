#ifndef SLIDER_H
#define SLIDER_H

#include <SDL2/SDL.h>
#include <stdbool.h>
#include "scene.h"

extern bool sfx_enabled;


// Slider struktúra
typedef struct {
    int x, y;
    int w, h;
    int min_val;
    int max_val;
    int current_val;
    bool dragging;
    bool hovered;
    SDL_Color bar_color;
    SDL_Color fill_color;
    SDL_Color handle_color;
    SDL_Color handle_hover;
} Slider;

extern Slider volume_slider;
// Függvénydeklarációk 
void init_volume_slider(void);
void draw_slider(Scene* scene, SDL_Renderer* renderer, Slider* slider);
void slider_handle_event(Slider* slider, SDL_Event* event);
void draw_panel(int x, int y, int w, int h, SDL_Color bg_color, SDL_Color border_color, int border_thickness);

// Callback
void on_sfx_toggle(void* data);

// Külső függvények 
extern int music_volume;
void set_music_volume(int volume);
int get_music_volume(void);

#endif