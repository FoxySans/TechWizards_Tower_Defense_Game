#include "scene.h"
#include <GL/gl.h>

void init_scene(Scene* scene)
{
    map_load_hardcoded(&scene->map);
}

void update_scene(Scene* scene)
{
    (void)scene;
}

void render_scene(const Scene* scene)
{
    render_map(&scene->map);
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

            glBegin(GL_QUADS);
                glVertex3f(x,        y,        0.0f);
                glVertex3f(x +tile_size, y,        0.0f);
                glVertex3f(x + tile_size, y + tile_size, 0.0f);
                glVertex3f(x,        y + tile_size, 0.0f);
            glEnd();
        }
    }
}