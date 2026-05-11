#ifndef MENU_H
#define MENU_H

#include <SDL2/SDL.h>
#include "scene.h" 

void render_menu(Scene* scene, SDL_Renderer* renderer);
void render_map_select(Scene* scene, SDL_Renderer* renderer);
void handle_menu_input(Scene* scene, SDL_Event* event, bool* is_running);
void render_options(Scene* scene, SDL_Renderer* renderer);
void draw_text(Scene* scene, SDL_Renderer* renderer, const char* text, int x, int y, SDL_Color color);
void stop_music(void);
void update_menu_bg(float dt);

#endif