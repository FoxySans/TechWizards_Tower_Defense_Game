#include "music.h"
#include <stdio.h>

// ─── Globális változók definíciója ──────────────────────────────
Mix_Music* music = NULL;
bool music_initialized = false;
int music_volume = 50;

// ─── Zene inicializálása ────────────────────────────────────────
void init_music(void)
{
    if (music_initialized) return;

    int flags = Mix_Init(MIX_INIT_MP3);
    if (!(flags & MIX_INIT_MP3)) {
        printf("Mix_Init figyelmeztetes: %s\n", Mix_GetError());
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        printf("SDL_mixer hiba: %s\n", Mix_GetError());
        return;
    }

    music = Mix_LoadMUS("assets/menu_music.mp3");
    if (!music) {
        printf("Betöltési hiba: %s\n", Mix_GetError());
        Mix_CloseAudio();
        Mix_Quit();
        return;
    }

    if (Mix_PlayMusic(music, -1) == -1) {
        printf("Lejátszási hiba: %s\n", Mix_GetError());
        Mix_FreeMusic(music);
        music = NULL;
        Mix_CloseAudio();
        Mix_Quit();
        return;
    }

    Mix_VolumeMusic(music_volume);
    music_initialized = true;
    printf("Zene sikeresen elindult! Hangerő: %d\n", music_volume);
}

// ─── Zene leállítása ─────────────────────────────────────────────
void stop_music(void)
{
    if (music) {
        Mix_HaltMusic();
        Mix_FreeMusic(music);
        music = NULL;
    }
    if (music_initialized) {
        Mix_CloseAudio();
        Mix_Quit();
        music_initialized = false;
    }
}

// ─── Hangerő beállítása ──────────────────────────────────────────
void set_music_volume(int volume)
{
    if (volume < 0) volume = 0;
    if (volume > 128) volume = 128;
    music_volume = volume;
    
    if (music_initialized) {
        Mix_VolumeMusic(volume);
    }
}

// ─── Hangerő lekérdezése ────────────────────────────────────────
int get_music_volume(void)
{
    return music_volume;
}