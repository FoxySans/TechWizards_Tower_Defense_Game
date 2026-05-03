#include "map.h"
#include <stdlib.h>

static int level1[MAP_HEIGHT][MAP_WIDTH] = {
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,2,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
};

static int level2[MAP_HEIGHT][MAP_WIDTH] = {
    {3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3},
    {1,1,1,1,1,1,1,1,3,3,3,3,3,3,3,3,3,3,3,3},
    {3,3,3,3,3,3,3,1,3,3,3,3,3,3,3,3,3,3,3,3},
    {3,3,3,3,3,3,3,1,1,1,1,1,1,1,3,3,3,3,3,3},
    {3,3,3,3,3,3,3,3,3,3,3,3,3,1,3,3,3,3,3,3},
    {3,3,3,1,1,1,1,1,1,1,1,1,1,1,3,3,3,3,3,3},
    {3,3,3,1,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3},
    {3,3,3,1,1,1,1,1,1,1,1,1,3,3,3,3,3,3,3,3},
    {3,3,3,3,3,3,3,3,3,3,3,1,3,3,3,3,3,3,3,3},
    {3,3,3,3,3,3,3,3,3,3,3,1,1,1,1,1,1,3,3,3},
    {3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,1,3,3,3},
    {3,3,3,1,1,1,1,1,1,1,1,1,1,1,1,1,1,3,3,3},
    {3,3,3,1,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3},
    {3,3,3,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,2,3},
    {3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3},
};

void map_load(Map* map)
{
    map->width  = MAP_WIDTH;
    map->height = MAP_HEIGHT;
    map->spawn  = (Vec2){ 4, 2 };
    map->base   = (Vec2){ 15, 6 };

    for (int row = 0; row < map->height; row++)
        for (int col = 0; col < map->width; col++)
            map->tiles[col][row].type = (TileType)level2[row][col];
}

Tile* map_get_tile(Map* map, int col, int row)
{
    if (col < 0 || col >= map->width)  return NULL;
    if (row < 0 || row >= map->height) return NULL;
    return &map->tiles[col][row];
}

int map_is_buildable(Map* map, int col, int row)
{
    Tile* t = map_get_tile(map, col, row);
    return t && t->type == TILE_GRASS;
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