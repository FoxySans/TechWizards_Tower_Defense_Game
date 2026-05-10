#include "world/scene.h"
#include <GL/gl.h>
#include "entities/enemy_manager.h"
#include <obj/load.h>
#include <obj/draw.h>
#include <stdlib.h>
#include <stdio.h>

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

void render_shots(Scene* scene, double dt) {
    glDisable(GL_TEXTURE_2D);
    for (int i = 0; i < scene->shot_count; i++) {
        ShotEffect* s = &scene->shots[i];
        
        if (s->type == 0) { // RED TOWER: Laser
            glColor3f(1.0f, 0.0f, 0.0f); // Bright Red
            glLineWidth(3.0f);
            glBegin(GL_LINES);
                glVertex3f(s->x1, s->y1, s->z1);
                glVertex3f(s->x2, s->y2, s->z2);
            glEnd();
        } else { // BLUE TOWER: Rectangle Bullet
            glColor3f(0.0f, 0.8f, 1.0f); // Cyan
            glPushMatrix();
                glTranslatef(s->x2, s->y2, s->z2); // Draw at enemy position
                // Draw a small 0.1 sized rectangle
                draw_cube(-0.05f, -0.05f, 0, 0.1f); 
            glPopMatrix();
        }

        s->duration -= (float)dt;
        if (s->duration <= 0) {
            // Remove shot by swapping with last element
            scene->shots[i] = scene->shots[--scene->shot_count];
            i--;
        }
    }
    glEnable(GL_TEXTURE_2D);
    glColor3f(1, 1, 1); // Reset color
}

void update_towers(Scene* scene, double dt)
{
    for (int i = 0; i < scene->active_tower_count; i++) {
        ActiveTower* t = &scene->active_towers[i];
        t->reload_timer -= (float)dt;

        if (t->reload_timer <= 0) {
            // BLUE TOWER: Targets 3 enemies, Fast fire rate, Low damage
            if (t->type == TILE_TOWER_BLUE) {
                int hits = 0;
                for (int j = 0; j < 3; j++) {
                    // find_best_enemy uses path distance to prioritize targets
                    Enemy* target = find_best_enemy(t->x + 0.5f, t->y + 0.5f, 4.0f);
                    if (target) {
                        enemy_damage(target, 10); //
                        hits++;

                        // Spawn Blue Bullet effect
                        if (scene->shot_count < 128) {
                            ShotEffect* s = &scene->shots[scene->shot_count++];
                            s->type = 1; // 1 = Bullet
                            s->x1 = t->x + 0.5f; s->y1 = t->y + 0.5f; s->z1 = 0.8f;
                            s->x2 = target->x;   s->y2 = target->y;   s->z2 = 0.3f;
                            s->duration = 0.08f; // Short flash for fast bullets
                        }
                    }
                }
                if (hits > 0) t->reload_timer = 0.4f;
            } 
            // RED TOWER: Targets 1 enemy, Slow fire rate, High damage
            else if (t->type == TILE_TOWER_RED) {
                Enemy* target = find_best_enemy(t->x + 0.5f, t->y + 0.5f, 6.0f);
                if (target) {
                    enemy_damage(target, 50); //

                    // Spawn Red Laser effect
                    if (scene->shot_count < 128) {
                        ShotEffect* s = &scene->shots[scene->shot_count++];
                        s->type = 0; // 0 = Laser
                        s->x1 = t->x + 0.5f; s->y1 = t->y + 0.5f; s->z1 = 0.8f;
                        s->x2 = target->x;   s->y2 = target->y;   s->z2 = 0.3f;
                        s->duration = 0.15f; // Longer duration for the laser beam
                    }
                    t->reload_timer = 1.5f;
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

