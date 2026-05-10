
#include "entities/enemy_manager.h"
#include <stdio.h>

static Enemy enemies[MAX_ENEMIES];
static int enemy_count = 0;
static PathMap pathmap;
static const Map* current_map;

typedef struct {
    EnemyType type;
    int remaining;
} SpawnEntry;

#define MAX_SPAWN_QUEUE 128

static SpawnEntry spawn_queue[MAX_SPAWN_QUEUE];
static int spawn_queue_count = 0;
static int spawn_index = 0;

static double spawn_timer = 0.0;
static double spawn_interval = 3.0;

void enemy_manager_init(const Map* map)
{
    enemy_count = 0;

    spawn_queue_count = 0;
    spawn_index = 0;
    spawn_timer = 0.0;

    enemy_build_pathmap(&pathmap, map);
}

void spawn_enemy(EnemyType type, int amount)
{
    if (spawn_queue_count >= MAX_SPAWN_QUEUE)
        return;

    spawn_queue[spawn_queue_count].type = type;
    spawn_queue[spawn_queue_count].remaining = amount;
    spawn_queue_count++;
}

void update_enemies(const Map* map, double dt)
{
    
    spawn_timer += dt;

    if (spawn_index < spawn_queue_count &&
        spawn_timer >= spawn_interval)
    {
        SpawnEntry* entry = &spawn_queue[spawn_index];

        if (entry->remaining > 0 && enemy_count < MAX_ENEMIES) {
            enemy_init(&enemies[enemy_count],
                       map,
                       &pathmap,
                       entry->type);
            enemy_count++;
            entry->remaining--;
            spawn_timer = 0.0;
        }

        if (entry->remaining <= 0) {
            spawn_index++;
        }
    }
    for (int i = 0; i < enemy_count; i++) {
        enemy_update(&enemies[i], map, &pathmap, dt);
    }
}

void render_enemies(float cam_rot_z)
{
    for (int i = 0; i < enemy_count; i++) {
        if (enemies[i].alive) {
            enemy_render(&enemies[i], cam_rot_z);
        }
    }
}

// enemy_manager.c
// Inside enemy_manager.c
Enemy* find_best_enemy(float tower_x, float tower_y, float range) {
    Enemy* best = NULL;
    int min_path_dist = 9999;

    for (int i = 0; i < enemy_count; i++) {
        if (!enemies[i].alive) continue;

        float dx = enemies[i].x - tower_x;
        float dy = enemies[i].y - tower_y;
        float dist_sq = dx*dx + dy*dy;

        if (dist_sq <= range * range) {
            // Target the one closest to the base (lowest pathmap distance)
            int d = pathmap.dist[enemies[i].col][enemies[i].row];
            if (d < min_path_dist) {
                min_path_dist = d;
                best = &enemies[i];
            }
        }
    }
    return best;
}
