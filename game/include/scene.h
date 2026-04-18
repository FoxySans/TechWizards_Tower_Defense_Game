#ifndef SCENE_H
#define SCENE_H

#include "camera.h"
#include "map.h"

typedef struct Scene {
    Map map;
} Scene;

void init_scene(Scene* scene);
void update_scene(Scene* scene);
void render_scene(const Scene* scene);
void render_map(const Map* map);

#endif