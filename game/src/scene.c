#include "scene.h"
#include <GL/gl.h>
#include "enemy_manager.h"
#include <math.h>
void init_scene(Scene* scene)
{
    map_load(&scene->map);
    
//TESZT
    enemy_manager_init(&scene->map);

//TESZT

    spawn_enemy(ENEMY_BASIC, 2);
    spawn_enemy(ENEMY_FAST, 1);
    spawn_enemy(ENEMY_TANK, 3);
    spawn_enemy(ENEMY_BASIC, 2);
    spawn_enemy(ENEMY_FAST, 1);

}

void update_scene(Scene* scene)
{
    (void)scene;
    
    
    
    update_enemies(&scene->map, 0.016);


}

void render_scene(const Scene* scene, float cam_rot_z)
{
    render_map(&scene->map);
    render_enemies(cam_rot_z);
}

void render_map(const Map* map)
{
    float tile_size = 1.0f;

    for (int row = 0; row < map->height; row++) {
        for (int col = 0; col < map->width; col++) {
            Tile* t = map_get_tile((Map*)map, col, row);
            if (!t) continue;

            float x = col * tile_size;
            float y = row * tile_size;

            if (t->type == TILE_WALL) {
                glColor3f(0.4f, 0.4f, 0.4f); 
                draw_cube(x, y, 0.0f, tile_size);
            } 
            else if (t->type == TILE_TOWER_RED) {
                glColor3f(0.4f, 0.4f, 0.4f);
                draw_cube(x, y, 0.0f, tile_size);

                glColor3f(1.0f, 0.0f, 0.0f);
                draw_sphere(x + 0.5f, y + 0.5f, 1.2f, 0.4f, 10);
            }
            else if (t->type == TILE_TOWER_BLUE) {
                glColor3f(0.4f, 0.4f, 0.4f);
                draw_cube(x, y, 0.0f, tile_size);

                glColor3f(0.0f, 0.0f, 1.0f);
                draw_sphere(x + 0.5f, y + 0.5f, 1.2f, 0.4f, 10);
            }
            else {
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

void draw_sphere(float x, float y, float z, float radius, int subdivisions) {
    float lat, lon;
    float step = M_PI / subdivisions;

    for (lat = 0; lat < M_PI; lat += step) {
        glBegin(GL_QUAD_STRIP);
        for (lon = 0; lon <= 2 * M_PI + step; lon += step) {
            // Vertex 1
            float x1 = x + radius * sin(lat) * cos(lon);
            float y1 = y + radius * sin(lat) * sin(lon);
            float z1 = z + radius * cos(lat);
            glVertex3f(x1, y1, z1);

            // Vertex 2 (next latitude step)
            float x2 = x + radius * sin(lat + step) * cos(lon);
            float y2 = y + radius * sin(lat + step) * sin(lon);
            float z2 = z + radius * cos(lat + step);
            glVertex3f(x2, y2, z2);
        }
        glEnd();
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
    float z2 = z + (size/4);

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