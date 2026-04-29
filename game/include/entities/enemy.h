
#ifndef ENEMY_H
#define ENEMY_H

#include "world/map.h"
#include <stdbool.h>

#define MAX_MAP_W MAP_WIDTH
#define MAX_MAP_H MAP_HEIGHT


typedef enum {
    ENEMY_BASIC,
    ENEMY_FAST,
    ENEMY_TANK
} EnemyType;


typedef struct {
    int dist[MAX_MAP_W][MAX_MAP_H];
} PathMap;


typedef struct {
    float x, y, z;

    int col, row;
    int next_col, next_row;

    float speed;
    int hp;
    int max_hp;

    EnemyType type;
    int remaining;
    bool alive;
    bool reached_base;
} Enemy;

/* pathfinding */
void enemy_build_pathmap(PathMap* pm, const Map* map);


void enemy_init(Enemy* e, const Map* map, const PathMap* pm, EnemyType type);
void enemy_update(Enemy* enemy, const Map* map, const PathMap* pm, double delta_time);
void enemy_damage(Enemy* enemy, int damage);
void enemy_render(const Enemy* e, float cam_rot_z);

#endif
