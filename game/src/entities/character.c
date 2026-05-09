#include "entities/character.h"
#include "core/utils.h"
#include "obj/model.h"
#include <GL/gl.h>
#include <math.h>
#include <stdio.h>

#define EYE_HEIGHT      0.5f
#define WALL_HEIGHT     1.0f
#define SPRINT_MULT     2.5f
#define BOB_SPEED       6.0f
#define BOB_MAGNITUDE   0.04f
#define THIRD_PERSON_DIST   3.0f
#define THIRD_PERSON_HEIGHT 2.0f
#define THIRD_PERSON_PITCH -20.0f

void character_init(Character* c, float x, float y, float z)
{
    c->x = x;
    c->y = y;
    c->z = z;

    c->angle = 0.0f;
    c->pitch = 0.0f;

    c->speed_forward  = 0.0f;
    c->speed_side     = 0.0f;
    c->speed_vertical = 0.0f;
    c->is_sprinting   = false;

    c->bob_time   = 0.0f;
    c->bob_amount = 0.0f;

    c->third_person = false;

    c->model       = NULL;
    c->model_scale = 0.03f;

    c->hp     = 100;
    c->hp_max = 100;
}

void character_update(Character* c, Map* map, double dt)
{
    float sprint = c->is_sprinting ? SPRINT_MULT : 1.0f;
    float angle      = degree_to_radian(c->angle);
    float side_angle = degree_to_radian(c->angle + 90.0f);

    float new_x = c->x + cosf(angle)      * c->speed_forward * sprint * dt
                       + cosf(side_angle)  * c->speed_side    * sprint * dt;
    float new_y = c->y + sinf(angle)      * c->speed_forward * sprint * dt
                       + sinf(side_angle)  * c->speed_side    * sprint * dt;

    // X collision
    int col_x = (int)(new_x + (new_x > c->x ? PLAYER_RADIUS : -PLAYER_RADIUS));
    int row_x = (int)c->y;
    Tile* tx = map_get_tile(map, col_x, row_x);
    if (!(tx && tx->type == TILE_WALL && c->z < WALL_HEIGHT))
        c->x = new_x;

    // Y collision
    int col_y = (int)c->x;
    int row_y = (int)(new_y + (new_y > c->y ? PLAYER_RADIUS : -PLAYER_RADIUS));
    Tile* ty = map_get_tile(map, col_y, row_y);
    if (!(ty && ty->type == TILE_WALL && c->z < WALL_HEIGHT))
        c->y = new_y;

    // vertical movement
    c->z += c->speed_vertical * sprint * dt;

    // map border clamp
    if (c->x < 0.0f)          c->x = 0.0f;
    if (c->x > MAP_WIDTH - 1)  c->x = MAP_WIDTH - 1;
    if (c->y < 0.0f)          c->y = 0.0f;
    if (c->y > MAP_HEIGHT - 1) c->y = MAP_HEIGHT - 1;

    // vertical clamp based on tile below
    int col = (int)c->x;
    int row = (int)c->y;
    Tile* below = map_get_tile(map, col, row);
    if (below && below->type == TILE_WALL) {
        if (c->z < WALL_HEIGHT + EYE_HEIGHT)
            c->z = WALL_HEIGHT + EYE_HEIGHT;
    } else {
        if (c->z < EYE_HEIGHT)
            c->z = EYE_HEIGHT;
    }
    if (c->z > 10.0f) c->z = 10.0f;

    // walking bob
    bool is_moving = (c->speed_forward != 0.0f || c->speed_side != 0.0f);
    if (is_moving) {
        c->bob_time  += (float)dt * BOB_SPEED * sprint;
        c->bob_amount = sinf(c->bob_time) * BOB_MAGNITUDE;
    } else {
        // smoothly return to 0
        c->bob_amount *= 0.85f;
    }
}

void character_render(const Character* c)
{
    if (!c->third_person) return;
    if (!c->model)        return;

    bool is_moving = (c->speed_forward != 0.0f || c->speed_side != 0.0f);
    float bob = is_moving ? c->bob_amount : 0.0f;

    glPushMatrix();
        glTranslatef(c->x, c->y, c->z - EYE_HEIGHT + bob);
        glRotatef(c->angle + 180.0f, 0.0f, 0.0f, 1.0f);
        glRotatef(-90.0f, 0.0f, 0.0f, 1.0f);
        glScalef(c->model_scale, c->model_scale, c->model_scale);

        glBindTexture(GL_TEXTURE_2D, c->texture_id);
        draw_model(c->model);  
    glPopMatrix();
}

void character_set_view(const Character* c, Camera* cam)
{
    if (!c->third_person) {
        // FPS — camera at eye position
        cam->position.x = c->x;
        cam->position.y = c->y;
        cam->position.z = c->z + c->bob_amount;
        cam->rotation.z = c->angle;
        cam->rotation.x = c->pitch;
    } else {
        // third person — camera behind and above
        float angle = degree_to_radian(c->angle);
        cam->position.x = c->x - cosf(angle) * THIRD_PERSON_DIST;
        cam->position.y = c->y - sinf(angle) * THIRD_PERSON_DIST;
        cam->position.z = c->z + THIRD_PERSON_HEIGHT;
        cam->rotation.z = c->angle;
        cam->rotation.x = THIRD_PERSON_PITCH;
    }
}

void character_set_speed(Character* c, float forward, float side)
{
    c->speed_forward = forward;
    c->speed_side    = side;
}

void character_set_vertical(Character* c, float vertical)
{
    c->speed_vertical = vertical;
}

void character_set_sprint(Character* c, bool sprinting)
{
    c->is_sprinting = sprinting;
}

void character_rotate(Character* c, double dx, double dy, double sensy)
{
    c->angle -= (float)(dx * sensy);
    c->pitch -= (float)(dy * sensy);

    if (c->pitch >  90.0f) c->pitch =  90.0f;
    if (c->pitch < -90.0f) c->pitch = -90.0f;

    if (c->angle < 0.0f)   c->angle += 360.0f;
    if (c->angle > 360.0f) c->angle -= 360.0f;
}

void character_toggle_view(Character* c)
{
    c->third_person = !c->third_person;
}