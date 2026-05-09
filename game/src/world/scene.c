#include "world/scene.h"
#include <GL/gl.h>
#include "entities/enemy_manager.h"
#include <obj/load.h>
#include <obj/draw.h>
#include <stdlib.h>

void init_scene(Scene* scene)
{
    map_load(&scene->map);
    character_init(&scene->character, 0.5f, 1.5f, EYE_HEIGHT);
    enemy_manager_init(&scene->map);

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

void update_scene(Scene* scene, double dt)
{
    update_enemies(&scene->map, dt);
}

void render_scene(const Scene* scene, float cam_rot_z)
{
    render_map(&scene->map, scene);
    render_enemies(cam_rot_z);

    glColor3f(1.0f, 1.0f, 1.0f);
    character_render(&scene->character);
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

