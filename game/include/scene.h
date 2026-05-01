#ifndef SCENE_H
#define SCENE_H

#include "camera.h"
#include "map.h"
#include "enemy.h" 
#include "texture.h"
#include "obj/model.h"
#include "obj/load.h"

typedef struct Scene {
    Map map;
    Model character;
    GLuint texture_id;
} Scene;

void init_scene(Scene* scene);
void update_scene(Scene* scene);
void render_scene(const Scene* scene, float cam_rot_z);
void render_map(const Map* map);
void draw_floor(float x, float y, float z, float size);
void draw_cube(float x, float y, float z, float size);
void draw_sphere(float x, float y, float z, float radius, int subdivisions);

#endif