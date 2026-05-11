#include "menu.h"
#include "scene.h"

#include <GL/gl.h>

#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_image.h>

#include <string.h> 

// ─── DVD-szerű háttér minta struktúra ─────────────────────────────
typedef struct {
    float x, y;           
    float vx, vy;         
    float scale;          
    GLuint texture;       
    int tex_w, tex_h;     
    float alpha;          
    bool initialized;     
} BouncingPattern;

static BouncingPattern bg_pattern = {0};

// ─── Háttér minta inicializálása ──────────────────────────────────
static void init_bg_pattern(void) {
    if (bg_pattern.initialized) return;
    
    SDL_Surface* surface = IMG_Load("assets/minta.png");
    if (!surface) {
        printf("Háttér minta betöltési hiba: %s\n", IMG_GetError());
        return;
    }
    
    SDL_Surface* optimized = SDL_ConvertSurfaceFormat(surface, SDL_PIXELFORMAT_RGBA32, 0);
    SDL_FreeSurface(surface);
    if (!optimized) return;
    
    glGenTextures(1, &bg_pattern.texture);
    glBindTexture(GL_TEXTURE_2D, bg_pattern.texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, optimized->w, optimized->h, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, optimized->pixels);
    
    bg_pattern.tex_w = optimized->w;
    bg_pattern.tex_h = optimized->h;
    bg_pattern.x = 100.0f;
    bg_pattern.y = 100.0f;
    bg_pattern.vx = 2000.0f;   // sebesség X
    bg_pattern.vy = 2000.0f;   // sebesség Y
    bg_pattern.scale = 0.5f;  
    bg_pattern.alpha = 0.3f;  
    bg_pattern.initialized = true;
    
    SDL_FreeSurface(optimized);
    printf("Háttér minta betöltve: %dx%d\n", bg_pattern.tex_w, bg_pattern.tex_h);
}

// ─── Háttér minta frissítése (pattogás) ──────────────────────────
static void update_bg_pattern(float dt) {
    if (!bg_pattern.initialized) return;
    
    bg_pattern.x += bg_pattern.vx * dt;
    bg_pattern.y += bg_pattern.vy * dt;
    
    int screen_w = 1920;
    int screen_h = 1080;
    float w = bg_pattern.tex_w * bg_pattern.scale;
    float h = bg_pattern.tex_h * bg_pattern.scale;
    
    // Jobb szél visszapattanás
    if (bg_pattern.x + w > screen_w) {
        bg_pattern.x = screen_w - w;
        bg_pattern.vx = -bg_pattern.vx;
    }
    // Bal szél
    if (bg_pattern.x < 0) {
        bg_pattern.x = 0;
        bg_pattern.vx = -bg_pattern.vx;
    }
    // Alsó szél
    if (bg_pattern.y + h > screen_h) {
        bg_pattern.y = screen_h - h;
        bg_pattern.vy = -bg_pattern.vy;
    }
    // Felső szél
    if (bg_pattern.y < 0) {
        bg_pattern.y = 0;
        bg_pattern.vy = -bg_pattern.vy;
    }
}

// ─── Háttér minta kirajzolása ───────────────────────────────────
static void draw_bg_pattern(void) {
    if (!bg_pattern.initialized) return;
    
    float w = bg_pattern.tex_w * bg_pattern.scale;
    float h = bg_pattern.tex_h * bg_pattern.scale;
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, bg_pattern.texture);
    
    glColor4f(1.0f, 1.0f, 1.0f, bg_pattern.alpha);
    
    glBegin(GL_QUADS);
        glTexCoord2f(0, 0); glVertex2f(bg_pattern.x, bg_pattern.y);
        glTexCoord2f(1, 0); glVertex2f(bg_pattern.x + w, bg_pattern.y);
        glTexCoord2f(1, 1); glVertex2f(bg_pattern.x + w, bg_pattern.y + h);
        glTexCoord2f(0, 1); glVertex2f(bg_pattern.x, bg_pattern.y + h);
    glEnd();
    
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
}

// ─── Callback függvények ───────────────
static void on_play_click(void* data) {
    Scene* scene = (Scene*)data;
    scene->phase = PHASE_MAP_SELECT;
    scene->selected_map = 0;
}

static void on_quit_click(void* data) {
    bool* running = (bool*)data;
    *running = false;
}

static void on_map1_click(void* data) {
    Scene* scene = (Scene*)data;
    scene->selected_map = 0;
    stop_music();
    scene->phase = PHASE_GAMEPLAY;
}

static void on_map2_click(void* data) {
    Scene* scene = (Scene*)data;
    scene->selected_map = 1;
    stop_music();
    scene->phase = PHASE_GAMEPLAY;
}

static void on_map3_click(void* data) {
    Scene* scene = (Scene*)data;
    scene->selected_map = 2;
    stop_music();
    scene->phase = PHASE_GAMEPLAY;
}

static void on_back_click(void* data) {
    Scene* scene = (Scene*)data;
    scene->phase = PHASE_MENU;
    scene->selected_map = 0;
}

static void on_options_click(void* data) {  // ← ÚJ callback!
    Scene* scene = (Scene*)data;
    scene->phase = PHASE_OPTIONS;
    scene->selected_map = 0;
}
// ─── Statikus gombok ──────────────────────────────────────────────
static Button play_btn;
static Button quit_btn;
static Button options_btn;
static Button battle_pass_btn;
static Button map1_btn;
static Button map2_btn;
static Button map3_btn;
static Button back_btn;
static Button sfx_btn;
static Button options_back_btn;
static bool menu_buttons_initialized = false;
static bool map_buttons_initialized = false;
static bool options_buttons_initialized = false;
//----------------------------------------------

// ─── Gombok inicializálása ───────────────────────────────────────
// -----menü gombok
static void init_menu_buttons(Scene* scene, bool* is_running) {
    if (menu_buttons_initialized) return;
    
    button_init(&play_btn, 850, 400, 220, 60, "PLAY");
    play_btn.bg_color = (SDL_Color){50, 100, 200, 255};
    play_btn.hover_color = (SDL_Color){80, 150, 255, 255};
    play_btn.on_click = on_play_click;
    play_btn.user_data = scene;
    
    button_init(&options_btn, 850, 500, 220, 60, "OPTIONS");
    options_btn.bg_color = (SDL_Color){50, 150, 100, 255};
    options_btn.hover_color = (SDL_Color){80, 200, 130, 255};
    options_btn.on_click = on_options_click;
    options_btn.user_data = scene;

    button_init(&quit_btn, 850, 600, 220, 60, "QUIT");
    quit_btn.bg_color = (SDL_Color){200, 50, 50, 255};
    quit_btn.hover_color = (SDL_Color){255, 80, 80, 255};
    quit_btn.on_click = on_quit_click;
    quit_btn.user_data = is_running;

    button_init(&battle_pass_btn, 850, 700, 220, 100, "BATTLE PASS \n (COMING SOON)");
    battle_pass_btn.bg_color = (SDL_Color){255, 0, 0, 255};
    battle_pass_btn.hover_color = (SDL_Color){255, 80, 80, 255};
    battle_pass_btn.user_data = scene; 
    
    menu_buttons_initialized = true;
}

// ----- map selection gombok
static void init_map_buttons(Scene* scene) {
    if (map_buttons_initialized) return;
    
    button_init(&map1_btn, 850, 200, 220, 60, "EASY");
    map1_btn.bg_color = (SDL_Color){50, 150, 50, 255};
    map1_btn.hover_color = (SDL_Color){80, 200, 80, 255};
    map1_btn.on_click = on_map1_click;
    map1_btn.user_data = scene;
    
    button_init(&map2_btn, 850, 300, 220, 60, "MEDIUM");
    map2_btn.bg_color = (SDL_Color){50, 100, 150, 255};
    map2_btn.hover_color = (SDL_Color){80, 150, 200, 255};
    map2_btn.on_click = on_map2_click;
    map2_btn.user_data = scene;
    
    button_init(&map3_btn, 810, 400, 300, 150, "HARD \n (COMING SOON)");
    map3_btn.bg_color = (SDL_Color){150, 50, 150, 255};
    map3_btn.hover_color = (SDL_Color){200, 80, 200, 255};
    map3_btn.on_click = on_map3_click;
    map3_btn.user_data = scene;
    
    button_init(&back_btn, 850, 630, 220, 60, "BACK");
    back_btn.bg_color = (SDL_Color){100, 100, 100, 255};
    back_btn.hover_color = (SDL_Color){150, 150, 150, 255};
    back_btn.on_click = on_back_click;
    back_btn.user_data = scene;
    
    map_buttons_initialized = true;
}
// ----beállítások gombok -------------
static void init_options_buttons(Scene* scene) {
    if (options_buttons_initialized) return;
    
    button_init(&sfx_btn, 850, 520, 220, 60, "SFX: ON");
    sfx_btn.bg_color = (SDL_Color){50, 150, 50, 255};
    sfx_btn.hover_color = (SDL_Color){80, 200, 80, 255};
    sfx_btn.on_click = on_sfx_toggle;
    sfx_btn.user_data = scene;
    
    button_init(&options_back_btn, 850, 650, 220, 60, "BACK");
    options_back_btn.bg_color = (SDL_Color){150, 100, 100, 255};
    options_back_btn.hover_color = (SDL_Color){150, 150, 150, 255};
    options_back_btn.on_click = on_back_click;
    options_back_btn.user_data = scene;
    
    options_buttons_initialized = true;
}

// ─── draw_text ────────────────────────────────────────────────────
void draw_text(Scene* scene, SDL_Renderer* renderer, const char* text, int x, int y, SDL_Color color)
{
    (void)renderer;
    
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

void draw_text_scaled(Scene* scene, SDL_Renderer* renderer, const char* text, int x, int y, SDL_Color color, float scale)
{
    (void)renderer;
    
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

    // Skálázott méret
    int w = (int)(converted->w * scale);
    int h = (int)(converted->h * scale);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_TEXTURE_2D);
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_QUADS);
        glTexCoord2f(0, 0); glVertex2f(x, y);
        glTexCoord2f(1, 0); glVertex2f(x + w, y);
        glTexCoord2f(1, 1); glVertex2f(x + w, y + h);
        glTexCoord2f(0, 1); glVertex2f(x, y + h);
    glEnd();
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);

    glDeleteTextures(1, &texture);
    SDL_FreeSurface(converted);
}

// -----logo ----------------------------------------------------------
void draw_logo(const char* filename, int x, int y, int w, int h) {
    SDL_Surface* surface = IMG_Load(filename);
    if (!surface) {
        printf("Nem sikerült a logó betöltése: %s\n", IMG_GetError());
        return;
    }

    // Átalakítás RGBA formátumra az OpenGL-hez
    SDL_Surface* optimized = SDL_ConvertSurfaceFormat(surface, SDL_PIXELFORMAT_RGBA32, 0);
    SDL_FreeSurface(surface);

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, optimized->w, optimized->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, optimized->pixels);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_TEXTURE_2D);
    
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_QUADS);
        glTexCoord2f(0, 0); glVertex2f(x, y);
        glTexCoord2f(1, 0); glVertex2f(x + w, y);
        glTexCoord2f(1, 1); glVertex2f(x + w, y + h);
        glTexCoord2f(0, 1); glVertex2f(x, y + h);
    glEnd();

    glDisable(GL_TEXTURE_2D);
    glDeleteTextures(1, &texture);
    SDL_FreeSurface(optimized);
}

// ─── Publikus függvény: háttér minta update (hívni kell a game loopból) ─
void update_menu_bg(float dt) {
    init_bg_pattern();
    update_bg_pattern(dt);
}

// ---- render_menu ---------------------------------------------------------------
void render_menu(Scene* scene, SDL_Renderer* renderer) {
    init_menu_buttons(scene, NULL);
    init_music();
    
    init_bg_pattern();
    draw_bg_pattern();
    
    draw_logo("assets/logo.png", 885, 200, 150, 150);
    play_btn.hovered = (scene->selected_map == 0);
    options_btn.hovered = (scene->selected_map == 1);
    quit_btn.hovered = (scene->selected_map == 2);
    battle_pass_btn.hovered = (scene->selected_map == 3);
    
    SDL_Color white = {255, 255, 255, 255};

    draw_logo("assets/tw.png", 1730, 900, 100, 100);
    draw_text_scaled(scene, renderer, "@techwizards", 1700, 1030, white, 0.75f);

    Button* buttons[] = {&play_btn, &options_btn, &quit_btn, &battle_pass_btn};
    buttons_draw(buttons, 4, scene->font, scene, renderer);
    
    SDL_Color arrow_color = {255, 255, 0, 255};
    int arrow_y;
    switch (scene->selected_map)
    {
    case 0: arrow_y = 420; break;
    case 1: arrow_y = 520; break;
    case 2: arrow_y = 620; break;
    case 3: arrow_y = 730; break;
    }
    draw_text(scene, renderer, ">", 815, arrow_y, arrow_color);
}

// ─── render_map_select ────────────────────────────────────────────
void render_map_select(Scene* scene, SDL_Renderer* renderer) {
    init_map_buttons(scene);
    init_bg_pattern();
    draw_bg_pattern();
    
    map1_btn.hovered = (scene->selected_map == 0);
    map2_btn.hovered = (scene->selected_map == 1);
    map3_btn.hovered = (scene->selected_map == 2);
    back_btn.hovered = (scene->selected_map == 3);
    
    SDL_Color white = {255, 255, 255, 255};
    draw_text_scaled(scene, renderer, "SELECT A MAP", 750, 50, white, 2.0f);
    
    Button* buttons[] = {&map1_btn, &map2_btn, &map3_btn, &back_btn};
    buttons_draw(buttons, 4, scene->font, scene, renderer);

    draw_text_scaled(scene, renderer, "@techwizards", 1700, 1030, white, 0.75f);
    draw_logo("assets/logo.png", 885, 800, 150, 150);
    draw_logo("assets/tw.png", 1730, 900, 100, 100);
    
    SDL_Color arrow_color = {255, 255, 0, 255};
    int arrow_y, arrow_x;
    switch (scene->selected_map) {
        case 0: 
            arrow_y = 220; 
            arrow_x = 820; 
            break;
        case 1: 
            arrow_y = 320;  
            arrow_x = 820; 
            break;
        case 2: 
            arrow_y = 475;  
            arrow_x = 770; 
            break;
        case 3: 
            arrow_y = 650;  
            arrow_x = 820; 
            break;
        default: 
            arrow_y = 230;
            arrow_x = 820;
    }
    
    draw_text(scene, renderer, ">", arrow_x, arrow_y, arrow_color);
       
}

void render_options(Scene* scene, SDL_Renderer* renderer) {
    init_options_buttons(scene);
    init_volume_slider();

    // === KERETES PANEL ===
    SDL_Color panel_bg = {30, 30, 50, 220};
    SDL_Color panel_border = {255, 215, 0, 255};
    
    int panel_w = 900;
    int panel_h = 400;
    int panel_x = (1920 - panel_w) / 2;
    int panel_y = (1080 - panel_h) / 2;
    
    draw_panel(panel_x, panel_y, panel_w, panel_h, panel_bg, panel_border, 5);
    
    // === CÍM ===
    SDL_Color white = {255, 255, 255, 255};
    SDL_Color gold = {255, 215, 0, 255};
    
    draw_text_scaled(scene, renderer, "OPTIONS", panel_x + 300, panel_y + 30, gold, 2.0f);
    
    // === ZENE HANGERŐ CÍMKE ===
    // === ZENE HANGERŐ CÍMKE ÉS CSÚSZKA EGY SORBAN ===
    int label_x = panel_x + 50;
    int label_y = panel_y + 120;
    
    // "MUSIC VOLUME" szöveg
    draw_text_scaled(scene, renderer, "MUSIC VOLUME", label_x, label_y, white, 1.2f);
    int text_w, text_h;
    TTF_SizeText(scene->font, "MUSIC VOLUME", &text_w, &text_h);

    // === CSÚSZKA ===
    // Csúszka pozíció frissítése a szöveg mellé
    volume_slider.x = label_x + text_w + 80;  // 30px távolság a szövegtől
    volume_slider.y = label_y + (text_h - volume_slider.h) / 2;
    
    draw_slider(scene, renderer, &volume_slider);

    if (sfx_enabled) {
        // BEKapcsolva - ZÖLD
        strcpy(sfx_btn.text, "SFX: ON");
        sfx_btn.bg_color = (SDL_Color){50, 150, 50, 255};
        sfx_btn.hover_color = (SDL_Color){80, 200, 80, 255};
    } else {
        // KIKapcsolva - PIROS
        strcpy(sfx_btn.text, "SFX: OFF");
        sfx_btn.bg_color = (SDL_Color){200, 50, 50, 255};
        sfx_btn.hover_color = (SDL_Color){255, 80, 80, 255};
    }
    
    // === GOMBOK ÁLLAPOTA ===
    sfx_btn.hovered = (scene->selected_map == 0);
    options_back_btn.hovered = (scene->selected_map == 1);
    
    // === GOMBOK KIRAJZOLÁSA ===
    Button* buttons[] = {&sfx_btn, &options_back_btn};
    buttons_draw(buttons, 2, scene->font, scene, renderer);
    
    // === NYÍL ===
    SDL_Color arrow_color = {255, 255, 0, 255};
    int arrow_y;
    switch (scene->selected_map) {
        case 0: arrow_y = 540; break;
        case 1: arrow_y = 670; break;
        default: arrow_y = 520;
    }
    draw_text(scene, renderer, ">", 800, arrow_y, arrow_color);
    
    // === ALSÓ LOGO ÉS felirat ===
    draw_logo("assets/logo.png", 885, 800, 150, 150);
    draw_logo("assets/tw.png", 1730, 900, 100, 100);
    draw_text_scaled(scene, renderer, "@techwizards", 1700, 1030, white, 0.75f);
}

// ─── handle_menu_input ────────────────────────────────────────────
void handle_menu_input(Scene* scene, SDL_Event* event, bool* is_running) {
    init_menu_buttons(scene, is_running);
    init_map_buttons(scene);
    init_options_buttons(scene);
    init_volume_slider();

    if (scene->phase == PHASE_MENU) {
        Button* buttons[] = {&play_btn, &options_btn, &quit_btn, &battle_pass_btn};
        buttons_handle_event(buttons, 4, event);
        
        if (play_btn.hovered) {
            scene->selected_map = 0;
        } else if (options_btn.hovered) {
            scene->selected_map = 1;
        } else if (quit_btn.hovered) {
            scene->selected_map = 2;
        } else if (battle_pass_btn.hovered) {
            scene->selected_map = 3;
        }
    } else if (scene->phase == PHASE_MAP_SELECT) {
        Button* buttons[] = {&map1_btn, &map2_btn, &map3_btn, &back_btn};
        buttons_handle_event(buttons, 4, event);
        
        if (map1_btn.hovered) scene->selected_map = 0;
        else if (map2_btn.hovered) scene->selected_map = 1;
        else if (map3_btn.hovered) scene->selected_map = 2;
        else if (back_btn.hovered) scene->selected_map = 3;
    } else if (scene->phase == PHASE_OPTIONS) {
        // Csúszka esemény kezelése  ← ÚJ!
        slider_handle_event(&volume_slider, event);
        
        // Hangerő frissítése ha változott
        if (volume_slider.current_val != music_volume) {
            set_music_volume(volume_slider.current_val);
        }
        
        Button* buttons[] = {&sfx_btn, &options_back_btn};
        buttons_handle_event(buttons, 2, event);
        
        if (sfx_btn.hovered) scene->selected_map = 0;
        else if (options_back_btn.hovered) scene->selected_map = 1;
    }
    
    // Billentyűzet események
    if (event->type != SDL_KEYDOWN) return;

    switch (event->key.keysym.sym) {
        case SDLK_UP:
            scene->selected_map--;
            if (scene->selected_map < 0) {
                if (scene->phase == PHASE_MENU) scene->selected_map = 3;
                else if (scene->phase == PHASE_MAP_SELECT) scene->selected_map = 3;
                else if (scene->phase == PHASE_OPTIONS) scene->selected_map = 1;
            }
            break;

        case SDLK_DOWN:
            scene->selected_map++;
            if (scene->phase == PHASE_MENU) {
                if (scene->selected_map > 3) scene->selected_map = 0;
            } else if (scene->phase == PHASE_MAP_SELECT) {
                if (scene->selected_map > 3) scene->selected_map = 0;
            } else if (scene->phase == PHASE_OPTIONS) {
                if (scene->selected_map > 1) scene->selected_map = 0;
            }
            break;

        case SDLK_LEFT:
            if (scene->phase == PHASE_OPTIONS && scene->selected_map == 0) {
                // Csúszka balra  ← ÚJ!
                int new_vol = volume_slider.current_val - 10;
                if (new_vol < 0) new_vol = 0;
                set_music_volume(new_vol);
            }
            break;

        case SDLK_RIGHT:
            if (scene->phase == PHASE_OPTIONS && scene->selected_map == 0) {
                // Csúszka jobbra  ← ÚJ!
                int new_vol = volume_slider.current_val + 10;
                if (new_vol > 128) new_vol = 128;
                set_music_volume(new_vol);
            }
            break;

        case SDLK_ESCAPE:
            if (scene->phase == PHASE_MAP_SELECT || scene->phase == PHASE_OPTIONS) {
                scene->phase = PHASE_MENU;
                scene->selected_map = 0;
            } else {
                *is_running = false;
            }
            break;

        case SDLK_RETURN:
            if (scene->phase == PHASE_MENU) {
                if (scene->selected_map == 0) {
                    scene->phase = PHASE_MAP_SELECT;
                    scene->selected_map = 0;
                } else if (scene->selected_map == 1) {
                    scene->phase = PHASE_OPTIONS;
                    scene->selected_map = 0;
                } else if (scene->selected_map == 2) {
                    *is_running = false;
                }
            } else if (scene->phase == PHASE_MAP_SELECT) {
                if (scene->selected_map == 0) {
                    stop_music();
                    scene->phase = PHASE_GAMEPLAY;
                } else if (scene->selected_map == 1) {
                    stop_music();
                    scene->phase = PHASE_GAMEPLAY;
                } else if (scene->selected_map == 2) {
                    stop_music();
                    scene->phase = PHASE_GAMEPLAY;
                } else if (scene->selected_map == 3) {
                    scene->phase = PHASE_MENU;
                    scene->selected_map = 0;
                }
            } else if (scene->phase == PHASE_OPTIONS) {
                if (scene->selected_map == 0) {
                    on_sfx_toggle(scene);
                } else if (scene->selected_map == 1) {
                    scene->phase = PHASE_MENU;
                    scene->selected_map = 0;
                }
            }
            break;
    }
}