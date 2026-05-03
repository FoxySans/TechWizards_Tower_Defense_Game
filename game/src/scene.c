#include "scene.h"
#include <GL/gl.h>

void init_scene(Scene* scene, SDL_Renderer* renderer)
{
    if (TTF_Init() == -1) {
        printf("SDL_ttf error: %s\n", TTF_GetError());
    }
    scene->font = TTF_OpenFont("assets/fonts/font.ttf", 24);
    if (!scene->font) {
        printf("Failed to load font: %s\n", TTF_GetError());
    }
    
    scene->phase = PHASE_MENU;
    scene->selected_map = 0;
    map_load(&scene->map);
}
void update_scene(Scene* scene)
{
    (void)scene;
}

void render_scene(const Scene* scene)
{
    render_map(&scene->map);
}