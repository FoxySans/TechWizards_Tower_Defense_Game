#ifndef SCENE_H
#define SCENE_H

#include "camera.h"
#include "map.h"
#include <SDL2/SDL_ttf.h>

typedef enum GamePhase{
PHASE_MENU,
PHASE_MAP_SELECT,
PHASE_GAMEPLAY
} GamePhase;

typedef struct Scene {
    Map map;
    TTF_Font* font;
    int selected_map;
    GamePhase phase;
} Scene;

#include "menu.h"

void init_scene(Scene* scene, SDL_Renderer* renderer);
void update_scene(Scene* scene);
void render_scene(const Scene* scene);
void render_map(const Map* map);
void draw_floor(float x, float y, float z, float size);
void draw_cube(float x, float y, float z, float size);

#endif
