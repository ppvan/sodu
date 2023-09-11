#include "imgui.h"
#include "font.h"
#include "la.h"
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_surface.h>
#include <assert.h>
#include <immintrin.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>

uistate_t *uistate;
SDL_Renderer *renderer;
font_t *font;
typedef SDL_Rect Rect;

void imgui_init(SDL_Renderer *_renderer, uistate_t *_uistate, font_t *_font) {
    assert(_renderer && "Can't init imgui with null _renderer.");
    assert(_uistate && "Can't init imgui with null _uistate.");

    renderer = _renderer;
    uistate = _uistate;
    font = _font;
}

void imgui_begin() {
    uistate->hotitem = 0;
    uistate->activeitem = 0;
}
void imgui_end() {
    if (uistate->mousedown == 0) {
        uistate->activeitem = 0;
    } else if (uistate->activeitem == 0) {
        uistate->activeitem = -1;
    }

    SDL_RenderPresent(renderer);
}

void draw_rect(const Rect *rect, uint32_t color) {

    uint8_t red = (color >> 16) & 0xff;
    uint8_t green = (color >> 8) & 0xff;
    uint8_t blue = (color >> 0) & 0xff;
    // uint8_t alpha = color & 0xFF;

    SDL_SetRenderDrawColor(renderer, red, green, blue, 255);
    SDL_RenderFillRect(renderer, rect);
}

bool point_in_rect(Vec2i pos, const Rect *rect) {
    return rect->x <= pos.x && pos.x <= rect->x + rect->w && rect->y <= pos.y && pos.y <= rect->y + rect->h;
}

void label(const Rect *bounds, const char *text, uint32_t color) {

    Vec2i pos = {
        .x = bounds->x + bounds->w / 2,
        .y = bounds->y + bounds->h / 2,
    };
    render_text_center(renderer, font, pos, text);
}

int button(const Rect *bounds, const char *text, int id) {

    int result = 0;

    if (point_in_rect(uistate->mouse, bounds)) {
        uistate->hotitem = id;
        if (uistate->activeitem == 0 && uistate->mousedown) {
            uistate->activeitem = id;
        }
    };

    // render btn
    if (uistate->hotitem == id) {
        if (uistate->activeitem == id) {
            // Button is both 'hot' and 'active'
            Rect pressed = {.x = bounds->x + 1, .y = bounds->y + 1, .w = bounds->w, .h = bounds->h};
            draw_rect(&pressed, 0xe67e22);
        } else {
            Rect hover = {.x = bounds->x, .y = bounds->y, .w = bounds->w, .h = bounds->h};
            draw_rect(&hover, 0xd35400);
        }
    } else {
        // button normal
        draw_rect(bounds, 0xe67e22);
    }

    label(bounds, text, 0xffffff);

    return result;
}