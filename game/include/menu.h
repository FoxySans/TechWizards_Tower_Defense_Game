#ifndef MENU_H
#define MENU_H

#include "scene.h"

void render_menu(Scene *scene, SDL_Renderer* renderer);

void render_map_select(Scene *scene, SDL_Renderer* renderer);

void handle_menu_input(Scene* scene, SDL_Event* event, bool* is_running);

#endif
