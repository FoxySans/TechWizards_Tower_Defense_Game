#include "menu.h"

#include <GL/gl.h>

void draw_text(Scene* scene, SDL_Renderer* renderer, const char* text, int x, int y, SDL_Color color) {
    if (!scene->font) return;

    // 1. Create Surface from Text
    SDL_Surface* surf = TTF_RenderText_Blended(scene->font, text, color);
    if (!surf) return;

    // 2. Generate OpenGL Texture
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    // Set texture parameters for scaling
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // 3. Upload Surface data to OpenGL
    // Replace GL_BGRA with GL_RGBA[cite: 3, 7]
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surf->w, surf->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, surf->pixels);

    // 4. Draw the texture as a 2D Quad
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture);
    glBegin(GL_QUADS);
        glTexCoord2f(0, 0); glVertex2f(x, y);
        glTexCoord2f(1, 0); glVertex2f(x + surf->w, y);
        glTexCoord2f(1, 1); glVertex2f(x + surf->w, y + surf->h);
        glTexCoord2f(0, 1); glVertex2f(x, y + surf->h);
    glEnd();
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);

    glDeleteTextures(1, &texture);
    SDL_FreeSurface(surf);
}

void render_menu(Scene* scene, SDL_Renderer* renderer) {
    
    SDL_Color white = {255, 255, 255, 255};
    SDL_Color highlight = {255, 255, 0, 255}; // Yellow for selection

    // Title requirement
    draw_text(scene, renderer, "MAIN MENU", 300, 100, white);

    // Play/Quit buttons
    // Use selected_map as a toggle index (0 for Play, 1 for Quit)
    draw_text(scene, renderer, "PLAY", 350, 250, (scene->selected_map == 0) ? highlight : white);
    draw_text(scene, renderer, "QUIT", 350, 300, (scene->selected_map == 1) ? highlight : white);
}

void render_map_select(Scene* scene, SDL_Renderer* renderer) {
    SDL_Color white = {255, 255, 255, 255};
    SDL_Color highlight = {0, 255, 255, 255}; // Cyan for selection

    draw_text(scene, renderer, "SELECT A MAP", 280, 50, white);

    // List of available maps requirement
    // Example: static list or iterate through your map files
    const char* maps[] = {"Level 1", "Level 2", "Level 3"};
    for (int i = 0; i < 3; i++) {
        SDL_Color c = (scene->selected_map == i) ? highlight : white;
        draw_text(scene, renderer, maps[i], 320, 150 + (i * 50), c);
    }
}

void handle_menu_input(Scene* scene, SDL_Event* event, bool* is_running) {
    if (event->type != SDL_KEYDOWN) return;

    switch (event->key.keysym.sym) {
        case SDLK_UP:
            scene->selected_map--;
            // Wrap around or clamp logic
            if (scene->selected_map < 0) scene->selected_map = 1; 
            break;

        case SDLK_DOWN:
            scene->selected_map++;
            if (scene->selected_map > 1) scene->selected_map = 0;
            break;

        case SDLK_ESCAPE:
            // Requirement: Route back to menu
            scene->phase = PHASE_MENU;
            break;

        case SDLK_RETURN:
            if (scene->phase == PHASE_MENU) {
                if (scene->selected_map == 0) {
                    scene->phase = PHASE_MAP_SELECT;
                    scene->selected_map = 0; // Reset index for map list
                } else if (scene->selected_map == 1) {
                    // Technical note: Quit button sets is_running = false
                    *is_running = false;
                }
            } else if (scene->phase == PHASE_MAP_SELECT) {
                // Logic to load selected map and start game
                scene->phase = PHASE_GAMEPLAY; 
            }
            break;
    }
}

