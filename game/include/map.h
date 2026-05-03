#ifndef MAP_H
#define MAP_H

#define MAP_WIDTH  20
#define MAP_HEIGHT 15
#include <GL/gl.h>
#include "types.h"
#include <stdlib.h>
#include <stdio.h>

typedef enum {
    TILE_GRASS,      // építhető, ellenség nem jár itt
    TILE_PATH,       // ellenség útja, nem építhető
    TILE_BASE,        // az alap amit védeni kell
    TILE_WALL
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
void draw_floor(float x, float y, float z, float size);
void draw_cube(float x, float y, float z, float size);

#endif