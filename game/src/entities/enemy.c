
#include "entities/enemy.h"
#include <GL/gl.h>
#include <math.h>
#include <limits.h>
#include <stdio.h>

#define ENEMY_Z 1.0f
#define EPSILON 0.05f
#define INF 999999

static void choose_next_tile(Enemy* e, const PathMap* pm);

typedef struct {
    int hp;
    float speed;
} EnemyStats;

static const EnemyStats ENEMY_STATS[] = {
    [ENEMY_BASIC] = { 100, 1.0f },
    [ENEMY_FAST]  = {  60, 3.0f },
    [ENEMY_TANK]  = { 300, 0.5f }
};

static int is_walkable(const Tile* t)
{
    return t && (t->type == TILE_PATH || t->type == TILE_BASE);
}

void enemy_build_pathmap(PathMap* pm, const Map* map)
{
    for (int y = 0; y < map->height; y++)
        for (int x = 0; x < map->width; x++)
            pm->dist[x][y] = INF;

    int qx[MAX_MAP_W * MAX_MAP_H];
    int qy[MAX_MAP_W * MAX_MAP_H];
    int qs = 0, qe = 0;

    int bx = map->base.x;
    int by = map->base.y;

    pm->dist[bx][by] = 0;
    qx[qe] = bx;
    qy[qe] = by;
    qe++;

    static const int dirs[4][2] = {
        { 1,0 }, { -1,0 }, { 0,1 }, { 0,-1 }
    };

    while (qs < qe) {
        int cx = qx[qs];
        int cy = qy[qs];
        qs++;

        for (int i = 0; i < 4; i++) {
            int nx = cx + dirs[i][0];
            int ny = cy + dirs[i][1];

            if (nx < 0 || ny < 0 ||
                nx >= map->width || ny >= map->height)
                continue;

            if (pm->dist[nx][ny] != INF)
                continue;

            const Tile* t = map_get_tile((Map*)map, nx, ny);
            if (!is_walkable(t))
                continue;

            pm->dist[nx][ny] = pm->dist[cx][cy] + 1;
            qx[qe] = nx;
            qy[qe] = ny;
            qe++;
        }
    }
}

void enemy_init(Enemy* e, const Map* map, const PathMap* pm, EnemyType type)
{
    e->col = map->spawn.x;
    e->row = map->spawn.y;

    e->x = e->col + 0.5f;
    e->y = e->row + 0.5f;
    e->z = ENEMY_Z;

    e->next_col = e->col;
    e->next_row = e->row;

    e->type = type;
    e->hp = ENEMY_STATS[type].hp;
    e->max_hp = e->hp;
    e->speed = ENEMY_STATS[type].speed;

    e->alive = true;
    e->reached_base = false;

    
    choose_next_tile(e, pm);
}

static void choose_next_tile(Enemy* e, const PathMap* pm)
{
    static const int dirs[4][2] = {
        { 1,0 }, { -1,0 }, { 0,1 }, { 0,-1 }
    };

    int best = pm->dist[e->col][e->row];
    e->next_col = e->col;
    e->next_row = e->row;

    for (int i = 0; i < 4; i++) {
        int nx = e->col + dirs[i][0];
        int ny = e->row + dirs[i][1];

        if (nx < 0 || ny < 0 ||
            nx >= MAX_MAP_W || ny >= MAX_MAP_H)
            continue;

        if (pm->dist[nx][ny] < best) {
            best = pm->dist[nx][ny];
            e->next_col = nx;
            e->next_row = ny;
        }
    }
}

void enemy_update(Enemy* e, const Map* map, const PathMap* pm, double dt)
{

printf(
    "enemy (%d,%d) next (%d,%d) dist=%d\n",
    e->col, e->row,
    e->next_col, e->next_row,
    pm->dist[e->col][e->row]
);

if (!e->alive || e->reached_base)
    return;

if (e->next_col == e->col && e->next_row == e->row) {
    choose_next_tile(e, pm);
    if (e->next_col == e->col && e->next_row == e->row)
        return;
}

float dx = (e->next_col + 0.5f) - e->x;
float dy = (e->next_row + 0.5f) - e->y;
float dist = sqrtf(dx * dx + dy * dy);

if (dist < EPSILON) {
    e->col = e->next_col;
    e->row = e->next_row;
    // snap float position exactly to tile center
    e->x = e->col + 0.5f;
    e->y = e->row + 0.5f;
    choose_next_tile(e, pm);

    // check if reached base
    if (e->col == (int)map->base.x && e->row == (int)map->base.y) {
        e->reached_base = true;
        e->alive = false;
    }
    return;
}

e->x += (dx / dist) * e->speed * dt;
e->y += (dy / dist) * e->speed * dt;
}

void enemy_damage(Enemy* e, int dmg)
{
    if (!e->alive) return;

    e->hp -= dmg;
    if (e->hp <= 0) {
        e->hp = 0;
        e->alive = false;
    }
}

void enemy_render(const Enemy* e, float cam_rot_z)
{
    if (!e->alive)
        return;

    glPushMatrix();
    glTranslatef(e->x, e->y, 0.0f);

    glColor3f(0.9f, 0.1f, 0.1f);
    float size = 0.2f;
    if (e->type == ENEMY_TANK) {
        glColor3f(0.0f, 0.9f, 0.0f);
        size = 0.28f;
    } else if (e->type == ENEMY_FAST) {
        glColor3f(0.0f, 0.0f, 0.9f);
        size = 0.16f;
    }

    glBegin(GL_QUADS);
        glVertex3f(-size, 0, 0);
        glVertex3f( size, 0, 0);
        glVertex3f( size, 0, 0.6f);
        glVertex3f(-size, 0, 0.6f);
    glEnd();

    // 2. HP BAR RENDERING (Billboarded to face camera)
    glPushMatrix();
        // Move to head height (above the enemy body)
        glTranslatef(0, 0, 0.75f); 
        
        // Rotate to face the camera based on Z-rotation
        glRotatef(cam_rot_z - 90.0f, 0, 0, 1);
        glRotatef(90.0f, 1.0f, 0.0f, 0.0f); 

        float bar_w = size * 2.0f;
        float bar_h = 0.06f;
        float health_perc = (float)e->hp / (float)ENEMY_STATS[e->type].hp;

        // DRAW RED BACKGROUND (The "container")
        glColor3f(0.3f, 0.0f, 0.0f); // Dark red
        glBegin(GL_QUADS);
            glVertex2f(-bar_w/2, 0);
            glVertex2f( bar_w/2, 0);
            glVertex2f( bar_w/2, bar_h);
            glVertex2f(-bar_w/2, bar_h);
        glEnd();

        // DRAW GREEN BAR (The actual health)
        // We add a tiny 0.001f offset on the Z-axis here to stop the flickering
        glTranslatef(0.0f, 0.0f, 0.001f); 
        
        glColor3f(0.0f, 1.0f, 0.0f); // Bright green
        glBegin(GL_QUADS);
            glVertex2f(-bar_w/2, 0);
            glVertex2f(-bar_w/2 + (bar_w * health_perc), 0);
            glVertex2f(-bar_w/2 + (bar_w * health_perc), bar_h);
            glVertex2f(-bar_w/2, bar_h);
        glEnd();
    glPopMatrix(); // End HP Bar transformations

    glPopMatrix(); // End Enemy transformations
}

