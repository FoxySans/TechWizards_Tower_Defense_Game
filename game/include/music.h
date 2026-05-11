#ifndef MUSIC_H
#define MUSIC_H

#include <SDL2/SDL_mixer.h>
#include <stdbool.h>

// Külső deklarációk (NEM static!)
extern Mix_Music* music;
extern bool music_initialized;
extern int music_volume;

// Függvénydeklarációk
void init_music(void);
void stop_music(void);
void set_music_volume(int volume);
int get_music_volume(void);

#endif