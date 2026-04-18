#include "map.h"
#include <stdlib.h>

static int level1[15][20] = {
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

void map_load_hardcoded(Map* map)
{
    map->width  = 20;
    map->height = 15;
    map->spawn  = (Vec2){ 0, 2 };
    map->base   = (Vec2){ 15, 6 };

    for (int row = 0; row < map->height; row++)
        for (int col = 0; col < map->width; col++)
            map->tiles[col][row].type = (TileType)level1[row][col];
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