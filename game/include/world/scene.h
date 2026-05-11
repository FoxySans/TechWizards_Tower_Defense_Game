#ifndef SCENE_H
#define SCENE_H

#include "render/camera.h"
#include "world/map.h"
#include "entities/character.h"
#include "entities/enemy.h"
#include "render/texture.h"
#include <GL/gl.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

typedef enum {
    PHASE_MENU,
    PHASE_MAP_SELECT,
    PHASE_BUILD,
    PHASE_COMBAT,
    PHASE_PAUSED,
    PHASE_GAME_OVER,
    PHASE_VICTORY
} GamePhase;

typedef struct Scene {
    Map       map;
    Character character;
    GamePhase phase;
    GamePhase previous_phase;
    Model towers[2];
    GLuint texture_id;
    GLuint tower_textures[2];
    int       lives;
    int       gold;
    int       wave;
    float     wave_timer;
    int       enemies_to_spawn;
    int       spawned_count;
    TTF_Font* font;
    float brightness;
    bool show_tooltip;
} Scene;

void init_scene(Scene* scene);
void update_scene(Scene* scene, double dt);
void render_scene(const Scene* scene, float cam_rot_z);
void render_map(const Map* map, const Scene* scene);
void render_character(Character* character, GLuint texture_id);
void render_user_manual(const Scene* scene,int width, int height);
void draw_floor(float x, float y, float z, float size);
void draw_cube(float x, float y, float z, float size);
void draw_text(const Scene* scene, SDL_Renderer* renderer, const char* text, int x, int y, SDL_Color color);
void set_lighting(const Scene* scene, float level);
void start_combat(Scene* scene);

#endif