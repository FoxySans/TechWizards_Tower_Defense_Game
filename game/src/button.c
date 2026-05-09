#include "button.h"
#include "scene.h"  // Scene struktúrához
#include <GL/gl.h>

void button_init(Button* btn, int x, int y, int w, int h, const char* text) {
    btn->x = x;
    btn->y = y;
    btn->w = w;
    btn->h = h;
    btn->text = text;
    btn->bg_color = (SDL_Color){60, 60, 60, 255};
    btn->hover_color = (SDL_Color){100, 100, 100, 255};
    btn->text_color = (SDL_Color){255, 255, 255, 255};
    btn->hovered = false;
    btn->clicked = false;
    btn->on_click = NULL;
    btn->user_data = NULL;
}

static void draw_rect(int x, int y, int w, int h, SDL_Color color) {
    glDisable(GL_TEXTURE_2D);
    glColor4f(color.r / 255.0f, color.g / 255.0f, color.b / 255.0f, color.a / 255.0f);
    glBegin(GL_QUADS);
        glVertex2f(x, y);
        glVertex2f(x + w, y);
        glVertex2f(x + w, y + h);
        glVertex2f(x, y + h);
    glEnd();
}

static void draw_border(int x, int y, int w, int h, SDL_Color color, int thickness) {
    glDisable(GL_TEXTURE_2D);
    glColor4f(color.r / 255.0f, color.g / 255.0f, color.b / 255.0f, color.a / 255.0f);
    glLineWidth(thickness);
    glBegin(GL_LINE_LOOP);
        glVertex2f(x, y);
        glVertex2f(x + w, y);
        glVertex2f(x + w, y + h);
        glVertex2f(x, y + h);
    glEnd();
}

// draw_text deklarálása
extern void draw_text(Scene* scene, SDL_Renderer* renderer, const char* text, int x, int y, SDL_Color color);

void button_draw(Button* btn, TTF_Font* font, Scene* scene, SDL_Renderer* renderer) {
    SDL_Color bg = btn->hovered ? btn->hover_color : btn->bg_color;
    if (btn->clicked) {
        bg.r = (Uint8)(bg.r * 0.8);
        bg.g = (Uint8)(bg.g * 0.8);
        bg.b = (Uint8)(bg.b * 0.8);
    }
    draw_rect(btn->x, btn->y, btn->w, btn->h, bg);
    
    SDL_Color border_color = btn->hovered ? 
        (SDL_Color){200, 200, 200, 255} : 
        (SDL_Color){100, 100, 100, 255};
    draw_border(btn->x, btn->y, btn->w, btn->h, border_color, 2);
    
    if (font && btn->text) {
        int text_w, text_h;
        TTF_SizeText(font, btn->text, &text_w, &text_h);
        int text_x = btn->x + (btn->w - text_w) / 2;
        int text_y = btn->y + (btn->h - text_h) / 2;
        
        SDL_Color shadow = {0, 0, 0, 128};
        draw_text(scene, renderer, btn->text, text_x + 1, text_y + 1, shadow);
        draw_text(scene, renderer, btn->text, text_x, text_y, btn->text_color);
    }
}

void button_handle_event(Button* btn, SDL_Event* event) {
    if (!btn) return;
    
    int mx, my;
    SDL_GetMouseState(&mx, &my);
    
    btn->hovered = (mx >= btn->x && mx <= btn->x + btn->w &&
                    my >= btn->y && my <= btn->y + btn->h);
    
    if (event->type == SDL_MOUSEBUTTONDOWN && event->button.button == SDL_BUTTON_LEFT) {
        if (btn->hovered) {
            btn->clicked = true;
        }
    }
    else if (event->type == SDL_MOUSEBUTTONUP && event->button.button == SDL_BUTTON_LEFT) {
        if (btn->clicked && btn->hovered) {
            if (btn->on_click) {
                btn->on_click(btn->user_data);
            }
        }
        btn->clicked = false;
    }
}

void buttons_handle_event(Button* buttons[], int count, SDL_Event* event) {
    for (int i = 0; i < count; i++) {
        button_handle_event(buttons[i], event);
    }
}

void buttons_draw(Button* buttons[], int count, TTF_Font* font, Scene* scene, SDL_Renderer* renderer) {
    for (int i = 0; i < count; i++) {
        button_draw(buttons[i], font, scene, renderer);
    }
}