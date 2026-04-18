#ifndef MAP_H
#define MAP_H

#include "types.h"

typedef enum {
    TILE_GRASS,      // építhető, ellenség nem jár itt
    TILE_PATH,       // ellenség útja, nem építhető
    TILE_BASE        // az alap amit védeni kell
} TileType;

typedef struct {
    TileType type;
} Tile;

typedef struct {
    int   width, height;
    Tile  tiles[20][15];  // [col][row] — max méret
    Vec2  spawn;          // honnan jönnek az ellenségek
    Vec2  base;           // hova kell megérkezni
} Map;

// Függvény deklarációk — implementáció: map.c
void map_load_hardcoded(Map *map);
Tile* map_get_tile(Map *map, int col, int row);
int   map_is_buildable(Map *map, int col, int row);

#endif