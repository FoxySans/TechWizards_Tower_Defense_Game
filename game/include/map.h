#ifndef MAP_H
#define MAP_H

#define MAP_WIDTH  20
#define MAP_HEIGHT 15

#include "types.h"

typedef enum {
    TILE_SPAWN, 
    TILE_GRASS,
    TILE_PATH,
    TILE_WALL,
    TILE_BASE,
    TILE_TOWER_RED,
    TILE_TOWER_BLUE
} TileType;

typedef struct {
    TileType type;
} Tile;

typedef struct {
    int   width, height;
    Tile  tiles[MAP_WIDTH][MAP_HEIGHT];  // [col][row] — max méret
    Vec2  spawn;          // honnan jönnek az ellenségek
    Vec2  base;           // hova kell megérkezni
} Map;

// Függvény deklarációk — implementáció: map.c
void map_load(Map *map);
Tile* map_get_tile(Map *map, int col, int row);
int   map_is_buildable(Map *map, int col, int row);
void map_upgrade_to_tower(Map* map, int col, int row, int tower_type);

#endif