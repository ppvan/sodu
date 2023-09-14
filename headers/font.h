#ifndef _FONT_H_
#define _FONT_H_

#include "la.h"
#include <SDL.h>
#include <SDL2/SDL_render.h>

typedef struct {
    char id;
    int x, y;
    int w, h;
    int xoff, yoff;
    int xadv;
} ch_t;

typedef struct {
    int count;
    int lineheight;
    ch_t chars[256];
    SDL_Texture *tex;
    SDL_Renderer *renderer;
} font_t;

typedef enum { LEFT, RIGHT, CENTER } Align;

font_t *font_init(SDL_Renderer *renderer, const char *filename);
void render_text(font_t *font, SDL_Rect bounds, const char *text, Align align);

#endif