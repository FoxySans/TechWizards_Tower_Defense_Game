
#ifndef ENEMY_MANAGER_H
#define ENEMY_MANAGER_H

#include "entities/enemy.h"





void enemy_manager_init(const Map* map);
void spawn_enemy(EnemyType type, int amount);
void update_enemies(const Map* map, double dt);
void render_enemies(float cam_rot_z);
Enemy* find_best_enemy(float tower_x, float tower_y, float range);



#endif
