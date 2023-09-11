#ifndef _IMGUI_H_
#define _IMGUI_H_

#include "font.h"
#include "la.h"
#include <SDL.h>
#include <stdint.h>

// IF you have render logic from multiple sources, this would cause colisions
// Just define IMGUI_SRC_ID before including imgui.h
#ifdef IMGUI_SRC_ID
#define GEN_ID ((IMGUI_SRC_ID) + (__LINE__))
#else
#define GEN_ID (__LINE__)
#endif

typedef struct {
    Vec2i mouse;
    int mousedown;

    int hotitem;
    int activeitem;
    int lastitem;
} uistate_t;

typedef SDL_Rect Rect;

void draw_rect(const Rect *rect, uint32_t color);

int button(const Rect *bound, const char *text, int id);
void label(const Rect *bounds, const char *text, uint32_t color);

void imgui_init(SDL_Renderer *_renderer, uistate_t *_uistate, font_t *_font);
void imgui_begin();
void imgui_end();
#endif