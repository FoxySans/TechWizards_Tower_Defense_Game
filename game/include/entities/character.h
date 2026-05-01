#ifndef CHARACTER_H
#define CHARACTER_H

#include "obj/model.h"
#include "obj/draw.h"
#include "world/map.h"
#include "render/camera.h"
#include <GL/gl.h>
#include <stdbool.h>

#define EYE_HEIGHT 0.5f

typedef struct {
    // position
    float x, y, z;

    // facing
    float angle;         // horizontal — used by character.c
    float pitch;         // vertical

    // movement
    float speed_forward;
    float speed_side;
    float speed_vertical;
    bool  is_sprinting;

    // animation
    float bob_time;
    float bob_amount;

    // view
    bool  third_person;

    // model
    Model* model;        // pointer — matches c->model = NULL in character.c
    float  model_scale;
    GLuint texture_id;

    // stats
    int hp;
    int hp_max;

} Character;

void character_init(Character* c, float x, float y, float z);
void character_update(Character* c, Map* map, double dt);
void character_render(const Character* c);
void character_set_view(const Character* c, Camera* cam);
void character_rotate(Character* c, double dx, double dy, double sensy);
void character_set_speed(Character* c, float forward, float side);
void character_set_vertical(Character* c, float vertical);
void character_set_sprint(Character* c, bool sprinting);
void character_toggle_view(Character* c);

#endif