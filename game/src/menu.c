#include "menu.h"

void draw_text(Scene* scene, SDL_Renderer* renderer, const char* text, int x, int y, SDL_Color color) {
    if (!scene->font) return;
    SDL_Surface* surf = TTF_RenderText_Blended(scene->font, text, color);
    SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surf);
    
    SDL_Rect dst = { x, y, surf->w, surf->h };
    SDL_RenderCopy(renderer, tex, NULL, &dst);
    
    SDL_FreeSurface(surf);
    SDL_DestroyTexture(tex);
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

