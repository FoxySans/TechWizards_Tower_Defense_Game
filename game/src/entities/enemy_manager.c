
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
static bool enemies_paused = false;
#define MAX_SPAWN_QUEUE 16

static SpawnEntry spawn_queue[MAX_SPAWN_QUEUE];
static int spawn_queue_count = 0;
static int spawn_index = 0;

static double spawn_timer = 0.0;
static double spawn_interval = 5.0;

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


    if (spawn_queue_count == 1) {
        spawn_timer = spawn_interval; 
    }
}

void update_enemies(const Map* map, double dt)
{
    if (enemies_paused) { 
        return;
    }
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
    
    
    
    if (are_enemies_paused())
        return;

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


void enemy_manager_damage_at(float target_x, float target_y)
{
    for (int i = 0; i < enemy_count; i++) {
        Enemy* e = &enemies[i];
        if (!e->alive) continue;

        // Kiszámoljuk a távolságot a célpont és az ellenség között
        float dx = e->x - target_x;
        float dy = e->y - target_y;
        float dist_sq = dx * dx + dy * dy;

        // Ha 0.5 egységen belül van (a sugár négyzete 0.25)
        if (dist_sq < 3.25f) {
            enemy_damage(e, 25); // 25 sebzés kattintásonként
        }
    }
}

void activate_all_enemies(void)
{
    for (int i = 0; i < enemy_count; i++) {
        if (enemies[i].alive) {
            enemies[i].active = true;
        }
    }
}



void damage_any_enemy(int amount)
{
    for (int i = 0; i < enemy_count; i++) {
        if (enemies[i].alive) {
            enemy_damage(&enemies[i], amount);
        }
    }
    
}



void toggle_enemies_pause(void)
{
    
    enemies_paused = !enemies_paused;
    //printf("Enemies paused: %s\n", enemies_paused ? "YES" : "NO");
}

bool are_enemies_paused(void)
{
    return enemies_paused;
}




