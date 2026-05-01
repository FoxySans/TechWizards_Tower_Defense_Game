#ifndef APP_H
#define APP_H

#include "render/camera.h"
#include "world/scene.h"
#include "entities/character.h"
#include <SDL2/SDL.h>
#include <GL/gl.h>
#include <stdio.h>
#include <stdbool.h>

#define VIEWPORT_RATIO (16 / 9.0)
#define VIEWPORT_ASPECT 50.0

typedef struct App
{
    SDL_Window* window;
    SDL_GLContext gl_context;
    bool is_running;
    double uptime;
    Camera camera;
    Scene scene;
    int selected_tower_type;
    bool is_building;
    float build_timer;
    float build_threshold;
} App;

void draw_crosshair(App* app);

/**
 * Initialize the application.
 */
void init_app(App* app, int width, int height);

/**
 * Initialize the OpenGL context.
 */
void init_opengl();

/**
 * Reshape the window.
 */
void reshape(GLsizei width, GLsizei height);

/**
 * Handle the events of the application.
 */
void handle_app_events(App* app);

/**
 * Update the application.
 */
void update_app(App* app);

void draw_ui_circle(float x, float y, float radius, int segments);

/**
 * Render the application.
 */
void render_app(App* app);

void render_build_indicator(App* app);

/**
 * Destroy the application.
 */
void destroy_app(App* app);

#endif /* APP_H */
