#include "world/scene.h"
#include <GL/gl.h>
#include "entities/enemy_manager.h"
#include <obj/load.h>
#include <obj/draw.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

void init_scene(Scene* scene)
{
    map_load(&scene->map);
    character_init(&scene->character, 0.5f, 1.5f, EYE_HEIGHT);
    enemy_manager_init(&scene->map);

    scene->active_tower_count = 0;
    scene->shot_count = 0;

    // Allocate memory because character.model is a pointer
    scene->character.model = (Model*)malloc(sizeof(Model));
    
    // REMOVE & here: model is already a pointer
    init_model(scene->character.model);
    load_model(scene->character.model, "assets/models/scientist.obj");
    scene->character.texture_id = load_texture("assets/textures/texture.png");

    // KEEP & here: towers[0] is a struct in scene.h
    init_model(&scene->towers[0]);
    load_model(&scene->towers[0], "assets/models/tower.obj");
    scene->tower_textures[0] = load_texture("assets/textures/alap.jpg");

    init_model(&scene->towers[1]);
    load_model(&scene->towers[1], "assets/models/tower.obj");
    scene->tower_textures[1] = load_texture("assets/textures/alap.jpg");

    spawn_enemy(ENEMY_BASIC, 2); 
    spawn_enemy(ENEMY_FAST, 1);
    spawn_enemy(ENEMY_TANK, 3);
}

// Inside render_shots in scene.c
void render_shots(Scene* scene, double dt) {
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glDisable(GL_TEXTURE_2D);
    for (int i = 0; i < scene->shot_count; i++) {
        ShotEffect* s = &scene->shots[i];
        
        if (s->type == 0) { // RED TOWER: Laser
            glColor3f(1.0f, 0.0f, 0.0f);
            glLineWidth(6.0f);
            glBegin(GL_LINES);
                glVertex3f(s->x1, s->y1, s->z1);
                glVertex3f(s->x2, s->y2, s->z2);
            glEnd();
        } else { // BLUE TOWER: Traveling Bullet
            // Calculate progress (0.0 at start, 1.0 at end)
            float t = 1.0f - (s->duration / 0.2f); 
            if (t < 0) t = 0; if (t > 1) t = 1;

            // Interpolate position
            float curr_x = s->x1 + (s->x2 - s->x1) * t;
            float curr_y = s->y1 + (s->y2 - s->y1) * t;
            float curr_z = s->z1 + (s->z2 - s->z1) * t;

            glColor3f(0.0f, 0.8f, 1.0f);
            glPushMatrix();
                glTranslatef(curr_x, curr_y, curr_z);
                draw_cube(-0.05f, -0.05f, 0, 0.1f); 
            glPopMatrix();
        }

        s->duration -= (float)dt;
        if (s->duration <= 0) {
            scene->shots[i] = scene->shots[--scene->shot_count];
            i--;
        }
    }
    glEnable(GL_TEXTURE_2D);
    glPopAttrib();
}

void update_towers(Scene* scene, double dt)
{
    for (int i = 0; i < scene->active_tower_count; i++) {
        ActiveTower* t = &scene->active_towers[i];
        t->reload_timer -= (float)dt;

        if (t->reload_timer <= 0) {
            if (t->type == TILE_TOWER_BLUE) {
                int hits = 0;
                for (int j = 0; j < 3; j++) {
                    Enemy* target = find_best_enemy(t->x + 0.5f, t->y + 0.5f, 4.0f);
                    if (target) {
                        enemy_damage(target, 2);
                        hits++;

                        if (scene->shot_count < 128) {
                            ShotEffect* s = &scene->shots[scene->shot_count++];
                            s->type = 1;
                            s->x1 = t->x + 0.5f; s->y1 = t->y + 0.5f; s->z1 = 1.5f; 
                            s->x2 = target->x;   s->y2 = target->y;   s->z2 = 0.3f;
                            s->duration = 0.25f; // Slightly longer so we see it travel
                        }
                    }
                }
                if (hits > 0) t->reload_timer = 0.25f;
            } 
            else if (t->type == TILE_TOWER_RED) {
                Enemy* target = find_best_enemy(t->x + 0.5f, t->y + 0.5f, 6.0f);
                if (target) {
                    enemy_damage(target, 60);
                    if (scene->shot_count < 128) {
                        ShotEffect* s = &scene->shots[scene->shot_count++];
                        s->type = 0;
                        
                        s->x1 = t->x + 0.5f; s->y1 = t->y + 0.5f; s->z1 = 1.5f; 
                        s->x2 = target->x;   s->y2 = target->y;   s->z2 = 0.3f;
                        s->duration = 0.3f; 
                    }
                    t->reload_timer = 5.5f;
                }
            }
        }
    }
}

void update_scene(Scene* scene, double dt) {
    update_enemies(&scene->map, dt);
    update_towers(scene, dt); // Add this call
}

// scene.c - Add this function
void add_active_tower(Scene* scene, int col, int row, int type)
{
    if (scene->active_tower_count < MAX_TOWERS) {
        ActiveTower* t = &scene->active_towers[scene->active_tower_count++];
        t->x = col;
        t->y = row;
        t->type = type;
        t->reload_timer = 0.0f; // Ready to fire immediately
    }
}

void render_scene(const Scene* scene, float cam_rot_z)
{
    render_map(&scene->map, scene);
    render_enemies(cam_rot_z);
    glColor3f(1.0f, 1.0f, 1.0f);
    character_render(&scene->character);
    render_shots((Scene*)scene, 0.016f);
}

void render_map(const Map* map, const Scene* scene)
{
    float tile_size = 1.0f;

    for (int row = 0; row < map->height; row++) {
        for (int col = 0; col < map->width; col++) {
            Tile* t = map_get_tile((Map*)map, col, row);
            if (!t) continue;

            float x = col * tile_size;
            float y = row * tile_size;

            if (t->type == TILE_WALL) {
                draw_cube(x, y, 0.0f, tile_size);
            } 
           else if (t->type == TILE_TOWER_RED || t->type == TILE_TOWER_BLUE) {
                
                glColor3f(0.2f, 0.6f, 0.2f);  
                draw_cube(x, y, 0.0f, tile_size);

                if (t->type == TILE_TOWER_RED) {
                        glColor3f(1.0f, 0.0f, 0.0f);
                        draw_range_circle(x + 0.5f, y + 0.5f, 0.26f, 6.0f);
                    } else {
                        glColor3f(0.0f, 0.8f, 1.0f);
                        draw_range_circle(x + 0.5f, y + 0.5f, 0.26f, 4.0f);
                    }

                int idx = (t->type == TILE_TOWER_RED) ? 0 : 1;
                glPushMatrix();
                    glTranslatef(x + 0.5f, y + 0.5f, 0.25f);
                    glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
                    glScalef(0.5f, 0.5f, 0.5f);
                    glEnable(GL_TEXTURE_2D);
                    glColor3f(1.0f, 1.0f, 1.0f);
                    glBindTexture(GL_TEXTURE_2D, scene->tower_textures[idx]);
                    if (idx == 0){
                        glColor3f(0.5f, 0.0f, 0.0f);
                    }
                    else{
                        glColor3f(0.0f, 0.0f, 0.5f);
                    }
                    draw_model(&scene->towers[idx]);
                    glDisable(GL_TEXTURE_2D);
                    glLineWidth(2.0f);
                glPopMatrix();
            }
            else {
                switch (t->type) {
                    case TILE_GRASS: glColor3f(0.2f, 0.6f, 0.2f); break;
                    case TILE_PATH:  glColor3f(0.7f, 0.6f, 0.3f); break;
                    case TILE_BASE:  glColor3f(0.8f, 0.2f, 0.2f); break;
                    default:         glColor3f(0.5f, 0.5f, 0.5f); break;
                }
                glDepthMask(GL_FALSE);
                draw_floor(x, y, 0.0f, tile_size);
                glDepthMask(GL_TRUE);
            }
        }
    }
}

void draw_range_circle(float cx, float cy, float cz, float radius) {
    int segments = 64;
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < segments; i++) {
        float theta = 2.0f * 3.1415926f * (float)i / (float)segments;
        // x and y vary to create the circle, z stays at the floor/base height
        float x = cx + (radius * cosf(theta));
        float y = cy + (radius * sinf(theta));
        glVertex3f(x, y, cz); 
    }
    glEnd();
}

void draw_floor(float x, float y, float z, float size)
{
    glBegin(GL_QUADS);
        glVertex3f(x,        y,        z);
        glVertex3f(x + size, y,        z);
        glVertex3f(x + size, y + size, z);
        glVertex3f(x,        y + size, z);
    glEnd();
}
void draw_cube(float x, float y, float z, float size)
{
    float x2 = x + size;
    float y2 = y + size;
    float z2 = z + size/4;

    // floor
    glColor3f(0.4f, 0.4f, 0.4f);
    glBegin(GL_QUADS);
        glVertex3f(x,  y,  z);
        glVertex3f(x2, y,  z);
        glVertex3f(x2, y2, z);
        glVertex3f(x,  y2, z);
    glEnd();

    // ceiling
    glColor3f(0.5f, 0.5f, 0.5f);
    glBegin(GL_QUADS);
        glVertex3f(x,  y,  z2);
        glVertex3f(x2, y,  z2);
        glVertex3f(x2, y2, z2);
        glVertex3f(x,  y2, z2);
    glEnd();

    // front (y)
    glColor3f(0.35f, 0.35f, 0.35f);
    glBegin(GL_QUADS);
        glVertex3f(x,  y, z);
        glVertex3f(x2, y, z);
        glVertex3f(x2, y, z2);
        glVertex3f(x,  y, z2);
    glEnd();

    // back (y2)
    glBegin(GL_QUADS);
        glVertex3f(x,  y2, z);
        glVertex3f(x2, y2, z);
        glVertex3f(x2, y2, z2);
        glVertex3f(x,  y2, z2);
    glEnd();

    // left (x)
    glColor3f(0.3f, 0.3f, 0.3f);
    glBegin(GL_QUADS);
        glVertex3f(x, y,  z);
        glVertex3f(x, y2, z);
        glVertex3f(x, y2, z2);
        glVertex3f(x, y,  z2);
    glEnd();

    // right (x2)
    glBegin(GL_QUADS);
        glVertex3f(x2, y,  z);
        glVertex3f(x2, y2, z);
        glVertex3f(x2, y2, z2);
        glVertex3f(x2, y,  z2);
    glEnd();
}

