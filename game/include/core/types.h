#ifndef TYPES_H
#define TYPES_H

#include <stdbool.h>

typedef struct { float x, y; } Vec2;

#define MAX_ENEMIES     64
#define MAX_TOWERS      32
#define MAX_PROJECTILES 128

// types.h
typedef struct {
    int x, y;
    float reload_timer;
    int type; // TILE_TOWER_RED or TILE_TOWER_BLUE
} ActiveTower;

// types.h
typedef struct {
    float x1, y1, z1; // Start (Tower top)
    float x2, y2, z2; // End (Enemy)
    float duration;   // How long it stays visible
    int type;         // 0 for Laser, 1 for Bullet
} ShotEffect;

#endif