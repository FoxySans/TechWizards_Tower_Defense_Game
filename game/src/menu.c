#include "menu.h"
#include "scene.h"
#include <GL/gl.h>

void draw_text(Scene* scene, SDL_Renderer* renderer, const char* text, int x, int y, SDL_Color color)
{
    if (!scene->font) return;

    SDL_Surface* surf = TTF_RenderText_Blended(scene->font, text, color);
    if (!surf) return;

    // convert to RGBA to be safe
    SDL_Surface* converted = SDL_ConvertSurfaceFormat(surf, SDL_PIXELFORMAT_RGBA32, 0);
    SDL_FreeSurface(surf);
    if (!converted) return;

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, converted->w, converted->h, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, converted->pixels);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_TEXTURE_2D);
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_QUADS);
        glTexCoord2f(0, 0); glVertex2f(x, y);
        glTexCoord2f(1, 0); glVertex2f(x + converted->w, y);
        glTexCoord2f(1, 1); glVertex2f(x + converted->w, y + converted->h);
        glTexCoord2f(0, 1); glVertex2f(x, y + converted->h);
    glEnd();
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);

    glDeleteTextures(1, &texture);
    SDL_FreeSurface(converted);
}

void render_menu(Scene* scene, SDL_Renderer* renderer) {
    
    SDL_Color white = {255, 255, 255, 255};
    SDL_Color highlight = {255, 255, 0, 255};

    draw_text(scene, renderer, "MAIN MENU", 865, 200, white);
    draw_text(scene, renderer, "PLAY", 900, 450, (scene->selected_map == 0) ? highlight : white);
    draw_text(scene, renderer, "QUIT", 900, 550, (scene->selected_map == 1) ? highlight : white);
}

void render_map_select(Scene* scene, SDL_Renderer* renderer) {
    SDL_Color white = {255, 255, 255, 255};
    SDL_Color highlight = {0, 255, 255, 255}; // Cyan for selection

    draw_text(scene, renderer, "SELECT A MAP", 860, 50, white);

    // List of available maps requirement
    // Example: static list or iterate through your map files
    const char* maps[] = {"Level 1", "Level 2", "Level 3"};
    for (int i = 0; i < 3; i++) {
        SDL_Color c = (scene->selected_map == i) ? highlight : white;
        draw_text(scene, renderer, maps[i], 900, 200 + (i * 50), c);
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

