
#ifndef ENEMY_MANAGER_H
#define ENEMY_MANAGER_H

#include "entities/enemy.h"
#include <stdbool.h>




void enemy_manager_init(const Map* map);
void spawn_enemy(EnemyType type, int amount);
void update_enemies(const Map* map, double dt);
void render_enemies(float cam_rot_z);
void activate_all_enemies(void);
void damage_any_enemy(int amount);
void toggle_enemies_active(void);
void enemy_manager_damage_at(float target_x, float target_y);
void toggle_enemies_pause(void);
bool are_enemies_paused(void);

#endif
