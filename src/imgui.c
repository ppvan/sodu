#include "imgui.h"
#include "core.h"
#include "datatypes.h"
#include "font.h"
#include "la.h"
#include "utils.h"
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_stdinc.h>
#include <SDL2/SDL_surface.h>
#include <assert.h>
#include <immintrin.h>
#include <limits.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

uistate_t *uistate;
SDL_Renderer *renderer;
font_t *font;
typedef SDL_Rect Rect;

void imgui_init(SDL_Renderer *_renderer, uistate_t *_uistate, font_t *_font) {
    assert(_renderer && "Can't init imgui with null _renderer.");
    assert(_uistate && "Can't init imgui with null _uistate.");
    assert(_uistate && "Can't init imgui with null _font.");

    renderer = _renderer;
    uistate = _uistate;
    font = _font;
}

void imgui_begin() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    scc(SDL_RenderClear(renderer));
    uistate->hotitem = 0;
}
void imgui_end() {

    if (uistate->mousedown == 0) {
        uistate->activeitem = 0;
    } else if (uistate->activeitem == 0) {
        uistate->activeitem = -1;
    }

    SDL_RenderPresent(renderer);
}

void rect(Rect rect, uint32_t color) {

    uint8_t red = (color >> 16) & 0xff;
    uint8_t green = (color >> 8) & 0xff;
    uint8_t blue = (color >> 0) & 0xff;
    // uint8_t alpha = color & 0xFF;

    scc(SDL_SetRenderDrawColor(renderer, red, green, blue, SDL_ALPHA_OPAQUE));
    scc(SDL_RenderFillRect(renderer, &rect));
}

bool point_in_rect(Vec2i pos, Rect rect) {
    return rect.x <= pos.x && pos.x <= rect.x + rect.w && rect.y <= pos.y && pos.y <= rect.y + rect.h;
}

void sodoku_board(sodoku_t *sodoku, Rect bounds) {

    int size = sodoku->size;
    int gap = 1;
    int sub_gap = 1;
    char ceil_data[100];
    int sr = sq_number_sqrt(size);

    // board background
    rect(bounds, 0x000000);
    layout_begin(VERTICAL, bounds, sr, gap);
    for (int i = 0; i < sr; i++) {
        layout_begin(HORIZONTAL, layout_slot(), sr, gap);
        for (int j = 0; j < sr; j++) {
            layout_begin(VERTICAL, layout_slot(), sr, sub_gap);

            // rect(layout_slot(), 0xff00ff);
            // layout_end();

            // continue;
            for (int s_i = 0; s_i < sr; s_i++) {
                // int id = GEN_ID + (i * sr + j + 1);
                layout_begin(HORIZONTAL, layout_slot(), sr, sub_gap);
                for (int s_j = 0; s_j < sr; s_j++) {
                    // int subid = id * GEN_ID + (s_i * sr + s_j + 1);
                    // button(layout_slot(), "10", subid);
                    Rect c = layout_slot();
                    // rect(c, 0xffffff);
                    // layout_end();

                    // continue;

                    if (SKU_AT(sodoku, sr * i + s_i, sr * j + s_j) == 0) {
                        rect(c, 0xB1B1E7);
                    } else {
                        rect(c, 0x858AE3);
                        sprintf(ceil_data, "%d", SKU_AT(sodoku, sr * i + s_i, sr * j + s_j));
                        label(c, ceil_data, 0x000000);
                    }
                }
                layout_end();
            }
            layout_end();
        }
        layout_end();
    }

    layout_end();
}

void label(Rect bounds, const char *text, uint32_t color) {

    Uint8 r, g, b;
    r = (color >> 16) & 0xff;
    g = (color >> 8) & 0xff;
    b = color & 0xff;

    // rect(bounds, color);
    scc(SDL_SetTextureColorMod(font->tex, r, g, b));
    render_text(font, bounds, text, CENTER);
    // scc(SDL_SetTextureColorMod(font->tex, r, g, b));
}

void bglabel(Rect bounds, const char *text, uint32_t color) {
    rect(bounds, color);
    scc(SDL_SetTextureColorMod(font->tex, 0, 0, 0));
    render_text(font, bounds, text, CENTER);
}

void combobox(Rect bounds, char *text_list, int *active, int id) {
    int size = 0;
    char *delim = ";";
    char *token;
    Rect content = {
        .x = bounds.x,
        .y = bounds.y,
        .w = bounds.w * 3 / 4,
        .h = bounds.h,
    };

    Rect index = {
        .x = bounds.x + bounds.w * 3 / 4 + 4,
        .y = bounds.y,
        .w = bounds.w / 4 - 4,
        .h = bounds.h,
    };

#define MAX_SIZE 32
    char **list = malloc(sizeof(char *) * MAX_SIZE);
    char *tmp = strdup(text_list);
    char *index_text = malloc(MAX_SIZE);
    token = strtok(tmp, delim);

    // Loop until strtok() returns NULL, which indicates that there are no more tokens.
    while (token != NULL) {
        list[size] = token;
        token = strtok(NULL, delim);
        size++;
        if (size > MAX_SIZE) {
            assert(0 && "Too many items.");
        }
    }

    if (point_in_rect(uistate->mouse, bounds)) {
        uistate->hotitem = id;
        if (uistate->activeitem == 0 && uistate->mousedown) {
            uistate->activeitem = id;
        }
    };

    if (uistate->hotitem == id) {
        if (uistate->activeitem == id) {
            // Button is both 'hot' and 'active'
            rect(content, 0xDBB8D7);
            rect(index, 0xDBB8D7);
        } else {
            rect(content, 0x8D7471);
            rect(index, 0x8D7471);
        }
    } else {
        // button normal
        rect(content, 0xDBB8D7);
        rect(index, 0xDBB8D7);
        // draw_rect(bounds, 0xff0000);
    }
    sprintf(index_text, "%d/%d", *active + 1, size);
    render_text(font, content, list[*active], CENTER);
    render_text(font, index, index_text, CENTER);

    // click
    if (uistate->hotitem == id && uistate->activeitem == id && uistate->mousedown == 0) {
        (*active)++;

        if (*active >= size) {
            *active = 0;
        }
    }

    free(index_text);
    free(tmp);
    free(list);
}

void sodoku_type_combobox(Rect bounds, char *text_list, int *active, int id) {
    int size = 0;
    char *delim = ";";
    char *token;
    Rect content = {
        .x = bounds.x,
        .y = bounds.y,
        .w = bounds.w * 1 / 2,
        .h = bounds.h,
    };

    Rect index = {
        .x = bounds.x + bounds.w * 1 / 2 + 4,
        .y = bounds.y,
        .w = bounds.w * 1 / 2 - 4,
        .h = bounds.h,
    };

#define MAX_SIZE 32
    char **list = malloc(sizeof(char *) * MAX_SIZE);
    char *tmp = strdup(text_list);
    token = strtok(tmp, delim);

    // Loop until strtok() returns NULL, which indicates that there are no more tokens.
    while (token != NULL) {
        list[size] = token;
        token = strtok(NULL, delim);
        size++;
        if (size > MAX_SIZE) {
            assert(0 && "Too many items.");
        }
    }

    if (point_in_rect(uistate->mouse, bounds)) {
        uistate->hotitem = id;
        if (uistate->activeitem == 0 && uistate->mousedown) {
            uistate->activeitem = id;
        }
    };

    if (uistate->hotitem == id) {
        if (uistate->activeitem == id) {
            // Button is both 'hot' and 'active'
            rect(content, 0xDBB8D7);
            rect(index, 0xDBB8D7);
        } else {
            rect(content, 0x8D7471);
            rect(index, 0x8D7471);
        }
    } else {
        // button normal
        rect(content, 0xDBB8D7);
        rect(index, 0xDBB8D7);
        // draw_rect(bounds, 0xff0000);
    }

    render_text(font, content, "SIZE", CENTER);
    render_text(font, index, list[*active], CENTER);

    // click
    if (uistate->hotitem == id && uistate->activeitem == id && uistate->mousedown == 0) {
        (*active)++;

        if (*active >= size) {
            *active = 0;
        }
    }
    free(tmp);
    free(list);
}

void solve_stragey(Rect bounds, char *text_list, int *active, int id) {
    int size = 0;
    char *delim = ";";
    char *token;
    Rect content = {
        .x = bounds.x,
        .y = bounds.y,
        .w = bounds.w * 1 / 2,
        .h = bounds.h,
    };

    Rect index = {
        .x = bounds.x + bounds.w * 1 / 2 + 4,
        .y = bounds.y,
        .w = bounds.w * 1 / 2 - 4,
        .h = bounds.h,
    };

#define MAX_SIZE 32
    char **list = malloc(sizeof(char *) * MAX_SIZE);
    char *tmp = strdup(text_list);
    token = strtok(tmp, delim);

    // Loop until strtok() returns NULL, which indicates that there are no more tokens.
    while (token != NULL) {
        list[size] = token;
        token = strtok(NULL, delim);
        size++;
        if (size > MAX_SIZE) {
            assert(0 && "Too many items.");
        }
    }

    if (point_in_rect(uistate->mouse, bounds)) {
        uistate->hotitem = id;
        if (uistate->activeitem == 0 && uistate->mousedown) {
            uistate->activeitem = id;
        }
    };

    if (uistate->hotitem == id) {
        if (uistate->activeitem == id) {
            // Button is both 'hot' and 'active'
            rect(content, 0xDBB8D7);
            rect(index, 0xDBB8D7);
        } else {
            rect(content, 0x8D7471);
            rect(index, 0x8D7471);
        }
    } else {
        // button normal
        rect(content, 0xDBB8D7);
        rect(index, 0xDBB8D7);
        // draw_rect(bounds, 0xff0000);
    }

    render_text(font, content, "Algorithm", CENTER);
    render_text(font, index, list[*active], CENTER);

    // click
    if (uistate->hotitem == id && uistate->activeitem == id && uistate->mousedown == 0) {
        (*active)++;

        if (*active >= size) {
            *active = 0;
        }
    }
    free(tmp);
    free(list);
}

bool combox(Rect bounds, char *text, options_t *options, int id) {

    Rect pre = {
        .x = bounds.x,
        .y = bounds.y,
        .w = bounds.w / 3 - 2,
        .h = bounds.h,
    };

    Rect after = {
        .x = bounds.x + bounds.w / 3 + 2,
        .y = bounds.y,
        .w = bounds.w * 2 / 3,
        .h = bounds.h,
    };

    if (point_in_rect(uistate->mouse, bounds)) {
        uistate->hotitem = id;
        if (uistate->activeitem == 0 && uistate->mousedown == 1) {
            uistate->activeitem = id;
        }
    };

    if (uistate->hotitem == id) {
        if (uistate->activeitem == id) {
            // Button is both 'hot' and 'active'
            rect(pre, 0x986C6A);
            rect(after, 0x986C6A);
        } else {
            rect(pre, 0x8D7471);
            rect(after, 0x8D7471);
        }

        label(pre, text, 0xffffff);
        label(after, option_current(*options), 0xffffff);
    } else {
        // button normal
        rect(pre, 0x986C6A);
        rect(after, 0x986C6A);
        // draw_rect(bounds, 0xff0000);
        label(pre, text, 0x000000);
        label(after, option_current(*options), 0x000000);
    }
    // render_text(font, bounds, text, CENTER);

    if (uistate->hotitem == id && uistate->activeitem == id && uistate->mousedown == 0) {
        options->current++;

        if (options->current >= options->size) {
            options->current = 0;
        }

        return true;
    }

    return false;
}

int button(Rect bounds, const char *text, int id) {

    if (point_in_rect(uistate->mouse, bounds)) {
        uistate->hotitem = id;
        if (uistate->activeitem == 0 && uistate->mousedown == 1) {
            uistate->activeitem = id;
        }
    };

    Rect border = {.x = bounds.x - 1, .y = bounds.y - 1, .w = bounds.w + 2, .h = bounds.h + 2};
    rect(border, 0x000000);
    // render btn
    if (uistate->hotitem == id) {
        if (uistate->activeitem == id) {
            // Button is both 'hot' and 'active'
            Rect pressed = {.x = bounds.x + 1, .y = bounds.y + 1, .w = bounds.w, .h = bounds.h};
            rect(pressed, 0x3C1A15);
        } else {
            Rect hover = {.x = bounds.x, .y = bounds.y, .w = bounds.w, .h = bounds.h};
            rect(hover, 0x2E1410);
        }
        label(bounds, text, 0xffffff);
    } else {
        // button normal
        rect(bounds, 0x986C6A);
        label(bounds, text, 0x000000);

        // draw_rect(bounds, 0xff0000);
    }
    // render_text(font, bounds, text, CENTER);

    // click
    if (uistate->hotitem == id && uistate->activeitem == id && uistate->mousedown == 0) {
        return 1;
    }

    return 0;
}

void layout_stack_push(layout_stack_t *stack, orient_t orient, Rect rect, int count, int gap) {
    layout_t l = {0};
    l.orient = orient;
    l.rect = rect;
    l.count = count;
    l.gap = gap;

    da_append(stack, l);
}

Rect layout_slot_loc(layout_t *l, const char *file_path, int line) {
    if (l->i >= l->count) {
        fprintf(stderr, "%s:%d: ERROR: Layout overflow\n", file_path, line);
        exit(1);
    }

    Rect r = {0};

    switch (l->orient) {
    case HORIZONTAL:
        r.w = (l->rect.w - l->gap * (l->count - 1)) / l->count;
        r.h = l->rect.h;
        r.x = l->rect.x + l->i * (r.w + l->gap);
        r.y = l->rect.y;
        break;

    case VERTICAL:
        r.w = l->rect.w;
        r.h = (l->rect.h - l->gap * (l->count - 1)) / l->count;
        r.x = l->rect.x;
        r.y = l->rect.y + l->i * (r.h + l->gap);
        break;

    default:
        assert(0 && "Unreachable");
    }

    l->i += 1;

    return r;
}