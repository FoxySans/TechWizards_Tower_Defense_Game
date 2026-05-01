#include "app.h"
#include <stdio.h>
#include <math.h>   
#include <SDL2/SDL_image.h>

void init_app(App* app, int width, int height)
{
    int error_code;
    int inited_loaders;
    app->is_building = false;
    app->build_timer = 0.0f;
    app->build_threshold = 1.5f;

    app->is_running = false;

    error_code = SDL_Init(SDL_INIT_EVERYTHING);
    if (error_code != 0) {
        printf("[ERROR] SDL initialization error: %s\n", SDL_GetError());
        return;
    }

    app->window = SDL_CreateWindow(
        "SpellPunk",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        width, height,
        SDL_WINDOW_OPENGL);
    if (app->window == NULL) {
        printf("[ERROR] Unable to create the application window!\n");
        return;
    }

    app->gl_context = SDL_GL_CreateContext(app->window);
    if (app->gl_context == NULL) {
        printf("[ERROR] Unable to create the OpenGL context!\n");
        return;
    }

    init_opengl();
    reshape(width, height);
    SDL_SetRelativeMouseMode(SDL_TRUE);
    init_camera(&(app->camera));
    init_scene(&(app->scene));

    app->uptime = (double)SDL_GetTicks() / 1000;
    app->is_running = true;
}

void init_opengl()
{
    glShadeModel(GL_SMOOTH);

    glEnable(GL_NORMALIZE);
    glEnable(GL_AUTO_NORMAL);

    glClearColor(0.0, 0.0, 0.0, 1.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glEnable(GL_DEPTH_TEST);

    glClearDepth(1.0);

    glLineWidth(5);
}

void reshape(GLsizei width, GLsizei height)
{
    int x, y, w, h;
    double ratio;

    ratio = (double)width / height;
    if (ratio > VIEWPORT_RATIO) {
        w = (int)((double)height * VIEWPORT_RATIO);
        h = height;
        x = (width - w) / 2;
        y = 0;
    }
    else {
        w = width;
        h = (int)((double)width / VIEWPORT_RATIO);
        x = 0;
        y = (height - h) / 2;
    }

    glViewport(x, y, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(
        -.08, .08,
        -.06, .06,
        .1, 1000
    );
}

void handle_app_events(App* app)
{
    SDL_Event event;
    static bool is_mouse_down = false;
    static int mouse_x = 0;
    static int mouse_y = 0;
    int x;
    int y;

    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_KEYDOWN:
            switch (event.key.keysym.scancode) {
            case SDL_SCANCODE_ESCAPE:
                app->is_running = false;
                break;
            case SDL_SCANCODE_W:
                character_set_speed(&app->scene.character, 1, app->scene.character.speed_side);
                break;
            case SDL_SCANCODE_S:
                character_set_speed(&app->scene.character, -1, app->scene.character.speed_side);
                break;
            case SDL_SCANCODE_A:
                character_set_speed(&app->scene.character, app->scene.character.speed_forward, 1);
                break;
            case SDL_SCANCODE_D:
                character_set_speed(&app->scene.character, app->scene.character.speed_forward, -1);
                break;
            case SDL_SCANCODE_1:
                app->selected_tower_type = TILE_TOWER_RED;
                printf("Selected: Red Tower\n");
                break;
            case SDL_SCANCODE_2:
                app->selected_tower_type = TILE_TOWER_BLUE;
                printf("Selected: Blue Tower\n");
                break;
            case SDL_SCANCODE_E:
                    app->is_building = true;
                    if (app->build_timer == app->build_threshold)
                    {
                        app->is_building = false;
                    }
                break;
            case SDL_SCANCODE_LSHIFT:
                character_set_sprint(&app->scene.character, true);
                break;
            case SDL_SCANCODE_SPACE:
                character_set_vertical(&app->scene.character, 1);
                break;
            case SDL_SCANCODE_LCTRL:
                character_set_vertical(&app->scene.character, -1);
                break;
            case SDL_SCANCODE_V:
            character_toggle_view(&app->scene.character);
            break;

            default:
                break;
            }
            break;
        case SDL_KEYUP:
            switch (event.key.keysym.scancode) {
            case SDL_SCANCODE_SPACE:
            case SDL_SCANCODE_LCTRL:
                character_set_vertical(&app->scene.character, 0);  // was set_camera_vertical_speed
                break;
            case SDL_SCANCODE_W:
            case SDL_SCANCODE_S:
                character_set_speed(&app->scene.character, 0, app->scene.character.speed_side);  // was set_camera_speed
                break;
            case SDL_SCANCODE_A:
            case SDL_SCANCODE_D:
                character_set_speed(&app->scene.character, app->scene.character.speed_forward, 0);  // was set_camera_side_speed
                break;
            case SDL_SCANCODE_LSHIFT:
                character_set_sprint(&app->scene.character, false);  // was set_camera_sprint
                break;
            default:
                break;
            }
            break;
        case SDL_MOUSEBUTTONDOWN:
            is_mouse_down = true;
            break;
        case SDL_MOUSEMOTION:
            //cursor sticking to the center of the screen
            character_rotate(&app->scene.character, event.motion.xrel, event.motion.yrel, 0.4);
            break;
        case SDL_MOUSEBUTTONUP:
            is_mouse_down = false;
            break;
        case SDL_QUIT:
            app->is_running = false;
            break;
        default:
            break;
        }
    }
}

void update_app(App* app)
{
    double current_time = (double)SDL_GetTicks() / 1000;
    double elapsed_time = current_time - app->uptime;
    app->uptime = current_time;

    update_camera(&(app->camera), &(app->scene.map), elapsed_time);

    if (app->is_building) {
        app->build_timer += elapsed_time;

        if (app->build_timer >= app->build_threshold) {
            // THE BUILD TRIGGER
            double angle = degree_to_radian(app->camera.rotation.z);
            float target_x = app->camera.position.x + cos(angle) * 2.0f;
            float target_y = app->camera.position.y + sin(angle) * 2.0f;
            
            map_upgrade_to_tower(&app->scene.map, (int)target_x, (int)target_y, app->selected_tower_type);
            
            // Reset after successful build so they have to press again
            app->is_building = false; 
            app->build_timer = 0.0f;
        }
    }

    character_update(&app->scene.character, &app->scene.map, elapsed_time);
    character_set_view(&app->scene.character, &app->camera);
    update_scene(&app->scene, elapsed_time);  // also fixed: was current_time, should be elapsed_time
}

void render_app(App* app)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);

    glPushMatrix();
    set_view(&(app->camera));
    render_scene(&(app->scene), app->camera.rotation.z);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_BLEND);

    glPopMatrix();

    // 2D Overlay Section
    draw_crosshair(app); 

    // --- NEW INDICATOR LOGIC ---
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glDisable(GL_DEPTH_TEST);

    // Set color based on selection
    if (app->selected_tower_type == TILE_TOWER_RED) {
        glColor3f(1.0f, 0.0f, 0.0f); // Red
    } else if (app->selected_tower_type == TILE_TOWER_BLUE) {
        glColor3f(0.0f, 0.0f, 1.0f); // Blue
    } else {
        glColor3f(0.5f, 0.5f, 0.5f); // Grey if none
    }

    // Draw the circle in the bottom-left corner
    // (-0.8, -0.8) moves it to the corner, 0.1 is the size
    draw_ui_circle(0.85f, -0.85f, 0.08f, 20);

    glEnable(GL_DEPTH_TEST);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    // ----------------------------

    SDL_GL_SwapWindow(app->window);
}

void draw_crosshair(App* app)
{
    // 1. Logic Math (Keep this at the top)
    double angle = degree_to_radian(app->camera.rotation.z);
    float look_dist = 2.0f;
    float target_x = app->camera.position.x + cos(angle) * look_dist;
    float target_y = app->camera.position.y + sin(angle) * look_dist;

    Tile* t = map_get_tile(&app->scene.map, (int)target_x, (int)target_y);
    bool buildable = (t && t->type == TILE_WALL);

    // 2. Enter 2D HUD Mode
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);
    
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glDisable(GL_DEPTH_TEST);

    // 3. Draw the Crosshair Lines
    if (buildable) {
        glColor3f(0.0f, 1.0f, 0.0f); // Green
    } else {
        glColor3f(1.0f, 1.0f, 1.0f); // White
    }

    glLineWidth(2.0f);
    float size = 0.025f;
    glBegin(GL_LINES);
        glVertex2f(size/1.5, 0.0f); glVertex2f(-size/1.5, 0.0f);
        glVertex2f(0.0f, -size);    glVertex2f(0.0f, size);
    glEnd();

    // 4. Draw the Progress Circle (CRITICAL: Must stay inside the 2D block)
    if ((app->is_building && buildable) && (app->selected_tower_type == TILE_TOWER_BLUE || app->selected_tower_type == TILE_TOWER_RED)) {
        float progress = app->build_timer / app->build_threshold;
        if (progress > 1.0f) progress = 1.0f;

        glLineWidth(3.0f);
        glColor3f(0.0f, 1.0f, 0.0f); 
        
        float radius = 0.05f;
        int segments = 32;
        int segments_to_draw = (int)(segments * progress);

        glBegin(GL_LINE_STRIP);
        for (int i = 0; i <= segments_to_draw; i++) {
            float theta = 2.0f * 3.1415926f * (float)i / (float)segments;
            // Center is 0,0 because of our glOrtho(-1, 1...) setup
            float x = radius * cosf(theta);
            float y = radius * sinf(theta);
            glVertex2f(x, y);
            if (app->is_building == false)
            {
                break;
            }
        }
        glEnd();
    }

    // 5. Exit 2D HUD Mode (Cleanup)
    glEnable(GL_DEPTH_TEST);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}

void draw_ui_circle(float x, float y, float radius, int segments) {
    glBegin(GL_TRIANGLE_FAN);
        glVertex2f(x, y); // Center of circle
        for (int i = 0; i <= segments; i++) {
            float theta = 2.0f * 3.1415926f * (float)i / (float)segments;
            glVertex2f(x + (radius * cosf(theta)), y + (radius * sinf(theta)));
        }
    glEnd();
}

void destroy_app(App* app)
{
    if (app->gl_context != NULL) {
        SDL_GL_DeleteContext(app->gl_context);
    }

    if (app->window != NULL) {
        SDL_DestroyWindow(app->window);
    }

    SDL_Quit();
}
