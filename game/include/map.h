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
    TILE_BASE
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

#endif