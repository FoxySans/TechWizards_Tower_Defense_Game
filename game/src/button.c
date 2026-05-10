#include "button.h"
#include "menu.h"
#include <string.h>

void button_init(Button* btn, int x, int y, int w, int h, const char* text) {
    btn->x = x;
    btn->y = y;
    btn->w = w;
    btn->h = h;
    strncpy(btn->text, text, 31);
    btn->text[31] = '\0';
    btn->bg_color = (SDL_Color){100, 100, 100, 255};
    btn->hover_color = (SDL_Color){150, 150, 150, 255};
    btn->text_color = (SDL_Color){255, 255, 255, 255};
    btn->hovered = false;
    btn->clicked = false;
    btn->on_click = NULL;
    btn->user_data = NULL;
}

void button_draw(Button* btn, TTF_Font* font, Scene* scene, SDL_Renderer* renderer) {
    (void)font;
    (void)scene;
    (void)renderer;
    
    if (btn->clicked) {
        glColor3f(0.5f, 0.5f, 0.5f);
    } else if (btn->hovered) {
        glColor3f(
            btn->hover_color.r / 255.0f,
            btn->hover_color.g / 255.0f,
            btn->hover_color.b / 255.0f
        );
    } else {
        glColor3f(
            btn->bg_color.r / 255.0f,
            btn->bg_color.g / 255.0f,
            btn->bg_color.b / 255.0f
        );
    }

    glBegin(GL_QUADS);
        glVertex2f(btn->x, btn->y);
        glVertex2f(btn->x + btn->w, btn->y);
        glVertex2f(btn->x + btn->w, btn->y + btn->h);
        glVertex2f(btn->x, btn->y + btn->h);
    glEnd();

    if (font && scene && renderer) {
        // Keressük meg a \n karaktert
        char* newline = strchr(btn->text, '\n');
        
        if (newline) {
            // Két soros szöveg
            char line1[32];
            char line2[32];
            
            int len1 = newline - btn->text;
            strncpy(line1, btn->text, len1);
            line1[len1] = '\0';
            
            strcpy(line2, newline + 1);
            
            int text_w1, text_h1, text_w2, text_h2;
            TTF_SizeText(font, line1, &text_w1, &text_h1);
            TTF_SizeText(font, line2, &text_w2, &text_h2);
            
            // Teljes magasság + 5px rés
            int total_h = text_h1 + text_h2 + 5;
            
            int text_x1 = btn->x + (btn->w - text_w1) / 2;
            int text_x2 = btn->x + (btn->w - text_w2) / 2;
            int text_y1 = btn->y + (btn->h - total_h) / 2;
            int text_y2 = text_y1 + text_h1 + 5;
            
            draw_text(scene, renderer, line1, text_x1, text_y1, btn->text_color);
            draw_text(scene, renderer, line2, text_x2, text_y2, btn->text_color);
        } else {
            // Egy soros szöveg (eredeti kód)
            int text_w, text_h;
            TTF_SizeText(font, btn->text, &text_w, &text_h);
            int text_x = btn->x + (btn->w - text_w) / 2;
            int text_y = btn->y + (btn->h - text_h) / 2;
            
            draw_text(scene, renderer, btn->text, text_x, text_y, btn->text_color);
        }
    }
}

void buttons_draw(Button** buttons, int count, TTF_Font* font, Scene* scene, SDL_Renderer* renderer) {
    for (int i = 0; i < count; i++) {
        button_draw(buttons[i], font, scene, renderer);
    }
}

void buttons_handle_event(Button** buttons, int count, SDL_Event* event) {
    if (event->type != SDL_MOUSEMOTION && 
        event->type != SDL_MOUSEBUTTONDOWN && 
        event->type != SDL_MOUSEBUTTONUP) {
        return;
    }

    int mx, my;
    SDL_GetMouseState(&mx, &my);

    for (int i = 0; i < count; i++) {
        Button* btn = buttons[i];
        btn->hovered = (mx >= btn->x && mx <= btn->x + btn->w &&
                        my >= btn->y && my <= btn->y + btn->h);

        if (event->type == SDL_MOUSEBUTTONDOWN && event->button.button == SDL_BUTTON_LEFT) {
            if (btn->hovered) {
                btn->clicked = true;
            }
        }

        if (event->type == SDL_MOUSEBUTTONUP && event->button.button == SDL_BUTTON_LEFT) {
            if (btn->clicked && btn->hovered) {
                if (btn->on_click) {
                    btn->on_click(btn->user_data);
                }
            }
            btn->clicked = false;
        }
    }
}