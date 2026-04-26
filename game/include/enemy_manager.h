
#ifndef ENEMY_MANAGER_H
#define ENEMY_MANAGER_H

#include "enemy.h"





void enemy_manager_init(const Map* map);
void spawn_enemy(EnemyType type, int amount);
void update_enemies(const Map* map, double dt);
void render_enemies(void);



#endif
