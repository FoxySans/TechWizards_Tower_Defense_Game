#include "world/scene.h"
#include <GL/gl.h>
#include "entities/enemy_manager.h"
#include <obj/load.h>
#include <obj/draw.h>
#include <stdlib.h>

void init_scene(Scene* scene)
{
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);  
    glEnable(GL_LIGHT1);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    scene->brightness=1.5f;
    scene->show_tooltip=false;

    if (TTF_Init() == -1) {
    printf("TTF hiba: %s\n", TTF_GetError());
    }
    scene->font = TTF_OpenFont("assets/fonts/font.ttf", 24); 
    if (!scene->font) {
        printf("Nem sikerult betolteni a betutipust!\n");
    }

    map_load(&scene->map);
    character_init(&scene->character, 0.5f, 1.5f, EYE_HEIGHT);
    enemy_manager_init(&scene->map);

    scene->character.model = (Model*)malloc(sizeof(Model));
    
    glColor3f(1.0f, 1.0f, 1.0f);
    init_model(scene->character.model);
    load_model(scene->character.model, "assets/models/soldier.obj");
    scene->character.texture_id = load_texture("assets/textures/soldier_atlas.png");

    init_model(&scene->towers[0]);
    load_model(&scene->towers[0], "assets/models/tower.obj");
    scene->tower_textures[0] = load_texture("assets/textures/alap.jpg");

    init_model(&scene->towers[1]);
    load_model(&scene->towers[1], "assets/models/tower.obj");
    scene->tower_textures[1] = load_texture("assets/textures/alap.jpg");
}

void update_scene(Scene* scene, double dt)
{
    update_enemies(&scene->map, dt);
}

void render_scene(const Scene* scene)
{
    set_lighting(scene,scene->brightness);
    render_map(&scene->map, scene);
    glColor3f(1.0f, 1.0f, 1.0f);
    character_render(&scene->character);

    if (scene->show_tooltip)
    {
        render_user_manual(scene,800,700);
    }
}

void set_lighting(const Scene* scene,float level) {
    glEnable(GL_LIGHTING);

    // --- LIGHT 0
    float ambient0[] = { 0.2f * level, 0.2f * level, 0.2f * level, 1.0f };
    float diffuse0[] = { 0.8f * level, 0.8f * level, 0.8f * level, 1.0f };
    float pos0[] = { 0.0f, 0.0f, 10.0f, 1.0f };

    glEnable(GL_LIGHT0);
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient0);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse0);
    glLightfv(GL_LIGHT0, GL_POSITION, pos0);

    // --- LIGHT 1
    float diffuse1[] = { 1.0f, 0.2f, 0.2f, 1.0f }; 
    float pos1[] = { 10.0f, 10.0f, 2.0f, 1.0f };  

    glEnable(GL_LIGHT1);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse1);
    glLightfv(GL_LIGHT1, GL_POSITION, pos1);
    
    // Attenuáció 
    glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, 1.0f);
    glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, 0.1f);
    glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, 0.05f);
    float player_pos[] = { scene->character.x, scene->character.y, scene->character.z + 1.0f, 1.0f };
    glEnable(GL_LIGHT1);
    glLightfv(GL_LIGHT1, GL_POSITION, player_pos);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, (float[]){1.0f, 0.5f, 0.0f, 1.0f});
}


void render_map(const Map* map, const Scene* scene)
{
    float tile_size = 1.0f;

    for (int row = 0; row < map->height; row++) {
        for (int col = 0; col < map->width; col++) {
            Tile* t = map_get_tile((Map*)map, col, row);
            if (!t) continue;

            float x = col * tile_size;
            float y = row * tile_size;

            if (t->type == TILE_WALL) {
                draw_cube(x, y, 0.0f, tile_size);
            } 
           else if (t->type == TILE_TOWER_RED || t->type == TILE_TOWER_BLUE) {
                
                glColor3f(0.2f, 0.6f, 0.2f);  
                draw_cube(x, y, 0.0f, tile_size);

                int idx = (t->type == TILE_TOWER_RED) ? 0 : 1;
                glPushMatrix();
                    glTranslatef(x + 0.5f, y + 0.5f, 0.25f);
                    glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
                    glScalef(0.5f, 0.5f, 0.5f);
                    glEnable(GL_TEXTURE_2D);
                    glColor3f(1.0f, 1.0f, 1.0f);
                    glBindTexture(GL_TEXTURE_2D, scene->tower_textures[idx]);
                    draw_model(&scene->towers[idx]);
                    glDisable(GL_TEXTURE_2D);
                glPopMatrix();
            }
            else {
                switch (t->type) {
                    case TILE_GRASS: glColor3f(0.2f, 0.6f, 0.2f); break;
                    case TILE_PATH:  glColor3f(0.7f, 0.6f, 0.3f); break;
                    case TILE_BASE:  glColor3f(0.8f, 0.2f, 0.2f); break;
                    default:         glColor3f(0.5f, 0.5f, 0.5f); break;
                }
                glDepthMask(GL_FALSE);
                draw_floor(x, y, 0.0f, tile_size);
                glDepthMask(GL_TRUE);
            }
        }
    }
}

void draw_floor(float x, float y, float z, float size)
{
    glBegin(GL_QUADS);
        glNormal3f(0.0f, 0.0f, 1.0f);
        glVertex3f(x,        y,        z);
        glVertex3f(x + size, y,        z);
        glVertex3f(x + size, y + size, z);
        glVertex3f(x,        y + size, z);
    glEnd();
}
void draw_cube(float x, float y, float z, float size)
{
    float x2 = x + size;
    float y2 = y + size;
    float z2 = z + size/4;

    // ceiling (Teteje) 
    glBegin(GL_QUADS);
        glNormal3f(0.0f, 0.0f, 1.0f); 
        glVertex3f(x,  y,  z2);
        glVertex3f(x2, y,  z2);
        glVertex3f(x2, y2, z2);
        glVertex3f(x,  y2, z2);
    glEnd();

    // front (Eleje - Y irány) 
    glBegin(GL_QUADS);
        glNormal3f(0.0f, -1.0f, 0.0f); 
        glVertex3f(x,  y, z);
        glVertex3f(x2, y, z);
        glVertex3f(x2, y, z2);
        glVertex3f(x,  y, z2);
    glEnd();

    // back (Hátulja)
    glBegin(GL_QUADS);
        glNormal3f(0.0f, 1.0f, 0.0f); 
        glVertex3f(x,  y2, z);
        glVertex3f(x2, y2, z);
        glVertex3f(x2, y2, z2);
        glVertex3f(x,  y2, z2);
    glEnd();

    // left (Bal oldal) 
    glBegin(GL_QUADS);
        glNormal3f(-1.0f, 0.0f, 0.0f); 
        glVertex3f(x, y,  z);
        glVertex3f(x, y2, z);
        glVertex3f(x, y2, z2);
        glVertex3f(x, y,  z2);
    glEnd();

    // right (Jobb oldal) 
    glBegin(GL_QUADS);
        glNormal3f(1.0f, 0.0f, 0.0f);
        glVertex3f(x2, y,  z);
        glVertex3f(x2, y2, z);
        glVertex3f(x2, y2, z2);
        glVertex3f(x2, y,  z2);
    glEnd();
}

void draw_text(const Scene* scene, SDL_Renderer* renderer, const char* text, int x, int y, SDL_Color color)
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

void render_user_manual(const Scene* scene, int width, int height) {
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    
    // 2D nézet beállítása
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, width, height, 0, -1, 1);
    
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);

    // Félátlátszó háttér
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.0f, 0.0f, 0.0f, 0.8f);
    glBegin(GL_QUADS);
        glVertex2f(10, 10);
        glVertex2f(350, 10);
        glVertex2f(350, 280);
        glVertex2f(10, 280);
    glEnd();

    // Szövegek kiírása
    SDL_Color white = {255, 255, 255, 255};
    draw_text(scene, NULL, "--- Controls ---", 25, 30, white);
    draw_text(scene, NULL, "W/A/S/D - Movement", 25, 70, white);
    draw_text(scene, NULL, "SPACE   - Jump", 25, 100, white);
    draw_text(scene, NULL, "8 / 9   - Brightness", 25, 130, white);
    draw_text(scene, NULL, "V   - 3rd Person", 25, 160, white);
    draw_text(scene, NULL, "F1      - Tooltip", 25, 190, white);
    draw_text(scene, NULL, "Mouse    - Camera", 25, 220, white);
    draw_text(scene, NULL, "ESC    - Quit", 25, 250, white);
    
    // Visszaállás 3D-be
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glPopAttrib();
}