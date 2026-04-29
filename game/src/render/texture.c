#ifndef GL_BGRA
#define GL_BGRA 0x80E1
#endif

#ifndef GL_BGR
#define GL_BGR 0x80E0
#endif
#include "render/texture.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

GLuint load_texture(const char* filename)
{
    SDL_Surface* surface = IMG_Load(filename);
    if (!surface) {
        printf("Failed to load texture %s: %s\n", filename, IMG_GetError());
        return 0;
    }

    // Determine the format
    GLint n_colors = surface->format->BytesPerPixel;
    GLenum texture_format;
    if (n_colors == 4) { // contains alpha
        if (surface->format->Rmask == 0x000000ff)
            texture_format = GL_RGBA;
        else
            texture_format = GL_BGRA;
    } else if (n_colors == 3) { // no alpha
        if (surface->format->Rmask == 0x000000ff)
            texture_format = GL_RGB;
        else
            texture_format = GL_BGR;
    } else {
        printf("Warning: The image is not truecolor.. this will probably break.\n");
        SDL_FreeSurface(surface);
        return 0;
    }

    GLuint texture_name;
    glGenTextures(1, &texture_name);
    glBindTexture(GL_TEXTURE_2D, texture_name);

    // Use the detected texture_format here
    glTexImage2D(GL_TEXTURE_2D, 0, n_colors, surface->w, surface->h, 0, 
                 texture_format, GL_UNSIGNED_BYTE, surface->pixels);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Clean up the surface memory!
    SDL_FreeSurface(surface);
    
    return texture_name;
}