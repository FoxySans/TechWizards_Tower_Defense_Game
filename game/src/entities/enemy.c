
#include "entities/enemy.h"
#include "entities/enemy_manager.h"
#include <GL/gl.h>
#include <math.h>
#include <limits.h>
#include <stdio.h>

#define ENEMY_Z 1.0f
#define EPSILON 0.05f
#define INF 999999
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

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


if (are_enemies_paused())
        return;


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
static void draw_sphere_wireframe(float radius, int slices, int stacks) {
    // Vízszintes körök
    for (int i = 0; i <= stacks; i++) {
        float phi = M_PI * (float)i / (float)stacks;
        float r = radius * sinf(phi);
        float z = radius * cosf(phi);

        glBegin(GL_LINE_LOOP);
        for (int j = 0; j <= slices; j++) {
            float theta = 2.0f * M_PI * (float)j / (float)slices;
            glVertex3f(cosf(theta) * r, sinf(theta) * r, z);
        }
        glEnd();
    }
    
    // Függőleges körök
    for (int i = 0; i < slices; i++) {
        float theta = 2.0f * M_PI * (float)i / (float)slices;
        glBegin(GL_LINE_STRIP);
        for (int j = 0; j <= stacks; j++) {
            float phi = M_PI * (float)j / (float)stacks;
            glVertex3f(radius * sinf(phi) * cosf(theta), radius * sinf(phi) * sinf(theta), radius * cosf(phi));
        }
        glEnd();
    }
}
void enemy_render(const Enemy* e, float cam_rot_z) {
    if (!e->alive) return;

    // Méretek beállítása típus szerint
    float size = 0.2f;
    if (e->type == ENEMY_FAST) size = 0.15f;
    if (e->type == ENEMY_TANK) size = 0.45f;

    // 1. GÖMB RAJZOLÁSA
    glPushMatrix();
    glTranslatef(e->x, e->y, size + 0.1f);

    switch (e->type) {
        case ENEMY_BASIC:
            glColor3f(1.0f, 0.2f, 0.2f); // Piros
            draw_sphere_wireframe(size, 12, 10);
            break;
        case ENEMY_FAST:
            glColor3f(0.2f, 0.6f, 1.0f); // Kék
            draw_sphere_wireframe(size, 10, 8);
            break;
        case ENEMY_TANK:
            glColor3f(0.2f, 1.0f, 0.2f); // Zöld
            draw_sphere_wireframe(size, 16, 14);
            break;
    }
    glPopMatrix();

    // 2. HP SÁV RAJZOLÁSA
    glPushMatrix();
    glTranslatef(e->x, e->y, (size * 2.0f) + 0.4f);
    
    // Billboard technika (szembefordítás)
    glRotatef(cam_rot_z - 90.0f, 0, 0, 1);
    glRotatef(90.0f, 1.0f, 0.0f, 0.0f);

    float bar_w = size * 2.5f; 
    float bar_h = 0.06f;
    
    // FONTOS: float kényszerítés, hogy mozogjon a csík!
    float health_perc = (float)e->hp / (float)ENEMY_STATS[e->type].hp;
    if (health_perc < 0.0f) health_perc = 0.0f;

    // Sötét háttér
    glColor3f(0.2f, 0.0f, 0.0f);
    glBegin(GL_QUADS);
        glVertex2f(-bar_w/2, 0);
        glVertex2f( bar_w/2, 0);
        glVertex2f( bar_w/2, bar_h);
        glVertex2f(-bar_w/2, bar_h);
    glEnd();

    // Zöld élet-csík
    glTranslatef(0, 0, 0.001f);
    glColor3f(0.0f, 1.0f, 0.3f);
    glBegin(GL_QUADS);
        glVertex2f(-bar_w/2, 0);
        glVertex2f(-bar_w/2 + (bar_w * health_perc), 0);
        glVertex2f(-bar_w/2 + (bar_w * health_perc), bar_h);
        glVertex2f(-bar_w/2, bar_h);
    glEnd();

    glPopMatrix();
}



