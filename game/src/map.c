#include "map.h"
#include <stdlib.h>


static int level1[MAP_HEIGHT][MAP_WIDTH] = {
    {0,2,1,1,1,2,2,2,2,2,2,2,1,1,1,1,1,1,1,1},
    {1,2,1,1,1,2,1,1,1,1,1,2,1,3,1,1,1,1,1,1},
    {1,2,2,2,2,2,1,2,2,2,2,2,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,2,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,1,3,1,1,1,1,2,2,2,1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,2,1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,3,1,2,1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,2,1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,2,1,1,1,3,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,2,1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,3,1,2,1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,2,1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,2,1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,2,1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,2,2,2,2,2,2,2,2,2,2,4},
};

static int level2[MAP_HEIGHT][MAP_WIDTH] = {
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {0,2,2,2,2,2,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,2,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,2,2,2,2,2,2,2,2,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,2,1,1,1,1,1,1,1},
    {1,1,1,2,2,2,2,2,2,2,2,2,2,1,1,1,1,1,1,1},
    {1,1,1,2,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,1,1,2,2,2,2,2,2,2,2,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1,2,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1,2,2,2,2,2,2,2,2,1,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,2,1,1},
    {1,1,1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1,1},
    {1,1,1,2,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,1,1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,4},
};

void map_load(Map* map)
{
    map->width  = MAP_WIDTH;
    map->height = MAP_HEIGHT;

    for (int row = 0; row < map->height; row++) {
        for (int col = 0; col < map->width; col++) {
            TileType type = (TileType)level1[row][col];
            map->tiles[col][row].type = type;

            if (type == TILE_SPAWN) {
                map->spawn = (Vec2){ col, row };
                // treat spawn as path for movement
                map->tiles[col][row].type = TILE_PATH;
            }
            if (type == TILE_BASE) {
                map->base = (Vec2){ col, row };
            }
        }
    }
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
    return t && t->type == TILE_WALL;
}

void map_upgrade_to_tower(Map* map, int col, int row, int tower_type) {
    Tile* t = map_get_tile(map, col, row);
    if (t && t->type == TILE_WALL) {
        switch (tower_type)
        {
        case TILE_TOWER_BLUE:
            t->type = TILE_TOWER_BLUE;
            break;
        case TILE_TOWER_RED:
            t->type = TILE_TOWER_RED;
            break;
        default:
            break;
        }
        
        
         
    }
}