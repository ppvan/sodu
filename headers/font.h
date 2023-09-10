#ifndef _FONT_H_
#define _FONT_H_

#include "la.h"
#include <SDL.h>

typedef struct {
    char id;
    int x, y;
    int w, h;
    int xoff, yoff;
    int xadv;
} ch_t;

typedef struct {
    int count;
    ch_t chars[256];
    SDL_Texture *tex;
} font_t;

font_t *font_init(SDL_Renderer *renderer, const char *filename);
void font_render(SDL_Renderer *renderer, font_t *font, Vec2i pos, char *str);

#endif