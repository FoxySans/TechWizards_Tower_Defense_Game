#include "slider.h"
#include <GL/gl.h>
#include <stdio.h>

struct Scene;
bool sfx_enabled = true;
Slider volume_slider = {0};

// ─── Csúszka inicializálása ─────────────────────────────────────
void init_volume_slider(void) {
    static bool initialized = false;
    if (initialized) return;
    
    volume_slider.x = 750;
    volume_slider.y = 320;
    volume_slider.w = 400;
    volume_slider.h = 20;
    volume_slider.min_val = 0;
    volume_slider.max_val = 128;
    volume_slider.current_val = music_volume;  // külső változó
    volume_slider.dragging = false;
    volume_slider.hovered = false;
    volume_slider.bar_color = (SDL_Color){80, 80, 80, 255};
    volume_slider.fill_color = (SDL_Color){50, 150, 50, 255};
    volume_slider.handle_color = (SDL_Color){200, 200, 200, 255};
    volume_slider.handle_hover = (SDL_Color){255, 255, 255, 255};
    
    initialized = true;
}

// ─── Segédfüggvények (static, csak ebben a fájlban) ─────────────
static int slider_get_handle_x(Slider* slider) {
    float ratio = (float)(slider->current_val - slider->min_val) / 
                  (float)(slider->max_val - slider->min_val);
    return slider->x + (int)(ratio * slider->w);
}

static void slider_set_value_from_x(Slider* slider, int mouse_x) {
    if (mouse_x < slider->x) {
        slider->current_val = slider->min_val;
    } else if (mouse_x > slider->x + slider->w) {
        slider->current_val = slider->max_val;
    } else {
        float ratio = (float)(mouse_x - slider->x) / (float)slider->w;
        slider->current_val = slider->min_val + (int)(ratio * (slider->max_val - slider->min_val));
    }
}

// ─── Keret rajzolása ────────────────────────────────────────────
static void draw_frame(int x, int y, int w, int h, int thickness, SDL_Color color) {
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    glColor4f(color.r / 255.0f, color.g / 255.0f, color.b / 255.0f, color.a / 255.0f);
    
    glBegin(GL_QUADS);
        glVertex2f(x - thickness, y - thickness);
        glVertex2f(x + w + thickness, y - thickness);
        glVertex2f(x + w + thickness, y);
        glVertex2f(x - thickness, y);
    glEnd();
    
    glBegin(GL_QUADS);
        glVertex2f(x - thickness, y + h);
        glVertex2f(x + w + thickness, y + h);
        glVertex2f(x + w + thickness, y + h + thickness);
        glVertex2f(x - thickness, y + h + thickness);
    glEnd();
    
    glBegin(GL_QUADS);
        glVertex2f(x - thickness, y);
        glVertex2f(x, y);
        glVertex2f(x, y + h);
        glVertex2f(x - thickness, y + h);
    glEnd();
    
    glBegin(GL_QUADS);
        glVertex2f(x + w, y);
        glVertex2f(x + w + thickness, y);
        glVertex2f(x + w + thickness, y + h);
        glVertex2f(x + w, y + h);
    glEnd();
    
    glDisable(GL_BLEND);
}

// ─── Csúszka rajzolása ──────────────────────────────────────────
void draw_slider(Scene* scene, SDL_Renderer* renderer, Slider* slider) {
    (void)renderer;
    
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // Teljes sáv (háttér)
    glColor4f(slider->bar_color.r / 255.0f, slider->bar_color.g / 255.0f, 
              slider->bar_color.b / 255.0f, slider->bar_color.a / 255.0f);
    glBegin(GL_QUADS);
        glVertex2f(slider->x, slider->y);
        glVertex2f(slider->x + slider->w, slider->y);
        glVertex2f(slider->x + slider->w, slider->y + slider->h);
        glVertex2f(slider->x, slider->y + slider->h);
    glEnd();
    
    // Kitöltött rész
    float fill_ratio = (float)(slider->current_val - slider->min_val) / 
                       (float)(slider->max_val - slider->min_val);
    int fill_w = (int)(slider->w * fill_ratio);
    
    glColor4f(slider->fill_color.r / 255.0f, slider->fill_color.g / 255.0f, 
              slider->fill_color.b / 255.0f, slider->fill_color.a / 255.0f);
    glBegin(GL_QUADS);
        glVertex2f(slider->x, slider->y);
        glVertex2f(slider->x + fill_w, slider->y);
        glVertex2f(slider->x + fill_w, slider->y + slider->h);
        glVertex2f(slider->x, slider->y + slider->h);
    glEnd();
    
    // Fogantyú
    int handle_x = slider_get_handle_x(slider);
    int handle_w = 20;
    int handle_h = 40;
    int handle_y = slider->y - (handle_h - slider->h) / 2;
    
    SDL_Color* hcolor = slider->hovered || slider->dragging ? &slider->handle_hover : &slider->handle_color;
    
    glColor4f(hcolor->r / 255.0f, hcolor->g / 255.0f, 
              hcolor->b / 255.0f, hcolor->a / 255.0f);
    glBegin(GL_QUADS);
        glVertex2f(handle_x - handle_w/2, handle_y);
        glVertex2f(handle_x + handle_w/2, handle_y);
        glVertex2f(handle_x + handle_w/2, handle_y + handle_h);
        glVertex2f(handle_x - handle_w/2, handle_y + handle_h);
    glEnd();
    
    // Fogantyú keret
    glColor4f(0.2f, 0.2f, 0.2f, 1.0f);
    glBegin(GL_LINE_LOOP);
        glVertex2f(handle_x - handle_w/2, handle_y);
        glVertex2f(handle_x + handle_w/2, handle_y);
        glVertex2f(handle_x + handle_w/2, handle_y + handle_h);
        glVertex2f(handle_x - handle_w/2, handle_y + handle_h);
    glEnd();
    
    glDisable(GL_BLEND);
    
    // Érték szöveg - forward declaration vagy include kell!
    extern void draw_text(Scene* scene, SDL_Renderer* renderer, const char* text, int x, int y, SDL_Color color);
    
    char val_str[16];
    sprintf(val_str, "%d%%", (slider->current_val * 100) / 128);
    SDL_Color white = {255, 255, 255, 255};
    draw_text(scene, renderer, val_str, slider->x + slider->w + 20, slider->y - 5, white);
}

// ─── Csúszka esemény kezelése ───────────────────────────────────
void slider_handle_event(Slider* slider, SDL_Event* event) {
    if (event->type == SDL_MOUSEMOTION) {
        int mx = event->motion.x;
        int my = event->motion.y;
        
        int handle_x = slider_get_handle_x(slider);
        int handle_w = 30;
        int handle_h = 50;
        int handle_y = slider->y - (handle_h - slider->h) / 2;
        
        slider->hovered = (mx >= handle_x - handle_w/2 && mx <= handle_x + handle_w/2 &&
                           my >= handle_y && my <= handle_y + handle_h);
        
        if (slider->dragging) {
            slider_set_value_from_x(slider, mx);
        }
    }
    else if (event->type == SDL_MOUSEBUTTONDOWN) {
        if (event->button.button == SDL_BUTTON_LEFT) {
            int mx = event->button.x;
            int my = event->button.y;
            
            if (mx >= slider->x && mx <= slider->x + slider->w &&
                my >= slider->y - 10 && my <= slider->y + slider->h + 10) {
                slider->dragging = true;
                slider_set_value_from_x(slider, mx);
            }
        }
    }
    else if (event->type == SDL_MOUSEBUTTONUP) {
        if (event->button.button == SDL_BUTTON_LEFT) {
            slider->dragging = false;
        }
    }
}

// ─── Háttér panel rajzolása ─────────────────────────────────────
void draw_panel(int x, int y, int w, int h, SDL_Color bg_color, SDL_Color border_color, int border_thickness) {
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    glColor4f(bg_color.r / 255.0f, bg_color.g / 255.0f, bg_color.b / 255.0f, bg_color.a / 255.0f);
    
    glBegin(GL_QUADS);
        glVertex2f(x, y);
        glVertex2f(x + w, y);
        glVertex2f(x + w, y + h);
        glVertex2f(x, y + h);
    glEnd();
    
    glDisable(GL_BLEND);
    
    draw_frame(x, y, w, h, border_thickness, border_color);
}

// ─── SFX toggle callback ────────────────────────────────────────
void on_sfx_toggle(void* data) {
    (void)data;
    sfx_enabled = !sfx_enabled;  // ← Állapot váltás
    
    printf("SFX %s!\n", sfx_enabled ? "BEKAPCSOLVA" : "KIKAPCSOLVA");
}