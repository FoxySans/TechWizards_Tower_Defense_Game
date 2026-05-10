#include "world/map.h"
#include <stdlib.h>
#include <stdio.h>
#include "entities/enemy_manager.h"

// Ez a függvény azért kell, hogy a scene.c hívása ne dobjon hibát
void map_load(Map* map) {
    map_load_from_file(map, "maps/level1.txt");
}

int map_load_from_file(Map* map, const char* filename) {
    FILE* f = fopen(filename, "r");
    if (!f) return 0;

    map->width = MAP_WIDTH;
    map->height = MAP_HEIGHT;

    for (int row = 0; row < MAP_HEIGHT; row++) {
        for (int col = 0; col < MAP_WIDTH; col++) {
            int val;
            if (fscanf(f, "%d", &val) != 1) { fclose(f); return 0; }
            map->tiles[col][row].type = (TileType)val;

            if (val == TILE_SPAWN) {
                map->spawn = (Vec2){ (float)col, (float)row };
                map->tiles[col][row].type = TILE_PATH;
            }
            if (val == TILE_BASE) {
                map->base = (Vec2){ (float)col, (float)row };
            }
        }
    }
    fclose(f);

    // Frissítjük az ellenségek útvonalát az új térkép alapján
    enemy_manager_init(map);
    return 1;
}

Tile* map_get_tile(Map* map, int col, int row) {
    if (col < 0 || col >= MAP_WIDTH || row < 0 || row >= MAP_HEIGHT) return NULL;
    return &map->tiles[col][row];
}

void map_upgrade_to_tower(Map* map, int col, int row, int tower_type) {
    Tile* t = map_get_tile(map, col, row);
    if (t && t->type == TILE_WALL) t->type = (TileType)tower_type;
}