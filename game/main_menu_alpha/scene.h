#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

typedef enum GamePhase{
PHASE_MENU,
PHASE_MAP_SELECT
} GamePhase;

typedef struct Scene
{
    TTF_Font* font;
    int selected_map;
} Scene;

