#include "scene.h"
#include <GL/gl.h>
#include "enemy_manager.h"
void init_scene(Scene* scene)
{
    map_load(&scene->map);
    
//TESZT
    enemy_manager_init(&scene->map);

//TESZT

    spawn_enemy(ENEMY_BASIC, 5);
    spawn_enemy(ENEMY_FAST, 3);
    spawn_enemy(ENEMY_TANK, 2);

}

void update_scene(Scene* scene)
{
    (void)scene;
    
    
    printf("update_scene called\n");
    update_enemies(&scene->map, 0.016);


}

void render_scene(const Scene* scene)
{
    render_map(&scene->map);
    render_enemies();
}

void render_map(const Map* map)
{
    float tile_size = 1.0f;

    for (int row = 0; row < map->height; row++) {
        for (int col = 0; col < map->width; col++) {

            Tile* t = map_get_tile((Map*)map, col, row);
            if (!t) continue;

            switch (t->type) {
                case TILE_GRASS: glColor3f(0.2f, 0.6f, 0.2f); break;
                case TILE_PATH:  glColor3f(0.7f, 0.6f, 0.3f); break;
                case TILE_BASE:  glColor3f(0.8f, 0.2f, 0.2f); break;
            }

            float x = col * tile_size;
            float y = row * tile_size;

           if (t->type == TILE_WALL) {
                draw_cube(x, y, 0.0f, tile_size);
            } else {
                switch (t->type) {
                    case TILE_GRASS: glColor3f(0.2f, 0.6f, 0.2f); break;
                    case TILE_PATH:  glColor3f(0.7f, 0.6f, 0.3f); break;
                    case TILE_BASE:  glColor3f(0.8f, 0.2f, 0.2f); break;
                    default:         glColor3f(0.5f, 0.5f, 0.5f); break;
                }
                draw_floor(x, y, 0.0f, tile_size);
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
    float z2 = z + size;

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