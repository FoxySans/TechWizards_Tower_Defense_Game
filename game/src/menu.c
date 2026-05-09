#include "menu.h"
#include "scene.h"
#include "button.h"
#include <GL/gl.h>

// ─── Callback függvények ──────────────────────────────────────────
static void on_play_click(void* data) {
    Scene* scene = (Scene*)data;
    scene->phase = PHASE_MAP_SELECT;
    scene->selected_map = 0;
}

static void on_quit_click(void* data) {
    bool* running = (bool*)data;
    *running = false;
}

// ─── Statikus gombok ──────────────────────────────────────────────
static Button play_btn;
static Button quit_btn;
static bool buttons_initialized = false;

static void init_menu_buttons(Scene* scene, bool* is_running) {
    if (buttons_initialized) return;
    
    button_init(&play_btn, 850, 400, 220, 60, "PLAY");
    play_btn.bg_color = (SDL_Color){50, 100, 200, 255};
    play_btn.hover_color = (SDL_Color){80, 150, 255, 255};
    play_btn.on_click = on_play_click;
    play_btn.user_data = scene;
    
    button_init(&quit_btn, 850, 500, 220, 60, "QUIT");
    quit_btn.bg_color = (SDL_Color){200, 50, 50, 255};
    quit_btn.hover_color = (SDL_Color){255, 80, 80, 255};
    quit_btn.on_click = on_quit_click;
    quit_btn.user_data = is_running;
    
    buttons_initialized = true;
}

// ─── draw_text (eredeti, változatlan) ─────────────────────────────
void draw_text(Scene* scene, SDL_Renderer* renderer, const char* text, int x, int y, SDL_Color color)
{
    (void)renderer; // Warning elnyomása
    
    if (!scene->font) return;

    SDL_Surface* surf = TTF_RenderText_Blended(scene->font, text, color);
    if (!surf) return;

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

// ─── render_menu ──────────────────────────────────────────────────
void render_menu(Scene* scene, SDL_Renderer* renderer) {
    init_menu_buttons(scene, NULL);
    
    // ─── BILLENTYŰZET NAVIGÁCIÓ SZINKRONIZÁLÁSA ─────────────────
    // Ha a fel/le gombokkal navigáltak, szinkronizáljuk a gomb hover állapotát
    play_btn.hovered = (scene->selected_map == 0);
    quit_btn.hovered = (scene->selected_map == 1);
    
    SDL_Color white = {255, 255, 255, 255};
    draw_text(scene, renderer, "MAIN MENU", 865, 200, white);
    
    // Gombok kirajzolása (a hovered állapot alapján színeznek)
    Button* buttons[] = {&play_btn, &quit_btn};
    buttons_draw(buttons, 2, scene->font, scene, renderer);
    
    // Kijelölés jelzése (kis nyíl vagy háromszög a kiválasztott gomb mellett)
    SDL_Color arrow_color = {255, 255, 0, 255};
    int arrow_y = (scene->selected_map == 0) ? 430 : 530;
    draw_text(scene, renderer, ">", 820, arrow_y, arrow_color);
}

// ─── render_map_select (eredeti, változatlan) ─────────────────────
void render_map_select(Scene* scene, SDL_Renderer* renderer) {
    SDL_Color white = {255, 255, 255, 255};
    SDL_Color highlight = {0, 255, 255, 255};

    draw_text(scene, renderer, "SELECT A MAP", 860, 50, white);

    const char* maps[] = {"Level 1", "Level 2", "Level 3"};
    for (int i = 0; i < 3; i++) {
        SDL_Color c = (scene->selected_map == i) ? highlight : white;
        draw_text(scene, renderer, maps[i], 900, 200 + (i * 50), c);
    }
}

// ─── handle_menu_input ────────────────────────────────────────────
void handle_menu_input(Scene* scene, SDL_Event* event, bool* is_running) {
    init_menu_buttons(scene, is_running);
    
    // ─── EGÉR ESEMÉNYEK (gomb kattintás) ─────────────────────────
    Button* buttons[] = {&play_btn, &quit_btn};
    buttons_handle_event(buttons, 2, event);
    
    // Ha az egérrel hover-eltünk, szinkronizáljuk a selected_map-et
    if (play_btn.hovered && !quit_btn.hovered) {
        scene->selected_map = 0;
    } else if (quit_btn.hovered && !play_btn.hovered) {
        scene->selected_map = 1;
    }
    
    // ─── BILLENTYŰZET ESEMÉNYEK (eredeti működés megőrzése) ─────
    if (event->type != SDL_KEYDOWN) return;

    switch (event->key.keysym.sym) {
        case SDLK_UP:
            scene->selected_map--;
            if (scene->selected_map < 0) scene->selected_map = 1; 
            break;

        case SDLK_DOWN:
            scene->selected_map++;
            if (scene->selected_map > 1) scene->selected_map = 0;
            break;

        case SDLK_ESCAPE:
            scene->phase = PHASE_MENU;
            break;

        case SDLK_RETURN:
            if (scene->phase == PHASE_MENU) {
                if (scene->selected_map == 0) {
                    scene->phase = PHASE_MAP_SELECT;
                    scene->selected_map = 0;
                } else if (scene->selected_map == 1) {
                    *is_running = false;
                }
            } else if (scene->phase == PHASE_MAP_SELECT) {
                scene->phase = PHASE_GAMEPLAY; 
            }
            break;
    }
}