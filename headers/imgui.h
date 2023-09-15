#ifndef _IMGUI_H_
#define _IMGUI_H_

#include "core.h"
#include "datatypes.h"
#include "font.h"
#include "la.h"
#include <SDL.h>
#include <assert.h>
#include <stdint.h>

// IF you have render logic from multiple sources, this would cause colisions
// Just define IMGUI_SRC_ID before including imgui.h
#ifdef IMGUI_SRC_ID
#define GEN_ID ((IMGUI_SRC_ID) + (__LINE__))
#else
#define GEN_ID (__LINE__)
#endif
typedef SDL_Rect Rect;

typedef struct {
    Vec2i mouse;
    int mousedown;

    int hotitem;
    int activeitem;
    int lastitem;
} uistate_t;

typedef enum {
    HORIZONTAL,
    VERTICAL,
} orient_t;

typedef struct {
    orient_t orient;
    Rect rect;
    int count;
    int i;
    int gap;
} layout_t;

typedef struct {
    layout_t *items;
    int count;
    int capacity;
} layout_stack_t;

static layout_stack_t default_stack = {0};

Rect layout_slot_loc(layout_t *l, const char *file_path, int line);
void layout_stack_push(layout_stack_t *stack, orient_t orient, Rect rect, int count, int gap);

#define layout_begin(orient, rect, count, gap) layout_stack_push(&default_stack, orient, rect, count, gap)
#define layout_end() layout_stack_pop(&default_stack)
#define layout_slot() layout_stack_slot(&default_stack)

#define layout_stack_slot(ls)                                                                                          \
    (assert((ls)->count > 0), layout_slot_loc(&(ls)->items[(ls)->count - 1], __FILE__, __LINE__))
#define layout_stack_pop(ls)                                                                                           \
    do {                                                                                                               \
        assert((ls)->count > 0);                                                                                       \
        (ls)->count -= 1;                                                                                              \
    } while (0)

#define DA_INIT_CAP 256
#define da_append(da, item)                                                                                            \
    do {                                                                                                               \
        if ((da)->count >= (da)->capacity) {                                                                           \
            (da)->capacity = (da)->capacity == 0 ? DA_INIT_CAP : (da)->capacity * 2;                                   \
            (da)->items = realloc((da)->items, (da)->capacity * sizeof(*(da)->items));                                 \
            assert((da)->items != NULL && "Buy more RAM lol");                                                         \
        }                                                                                                              \
                                                                                                                       \
        (da)->items[(da)->count++] = (item);                                                                           \
    } while (0)

void rect(Rect rect, uint32_t color);

int button(Rect bounds, const char *text, int id);
void label(Rect bounds, const char *text, uint32_t color);
void bglabel(Rect bounds, const char *text, uint32_t color);
void combobox(Rect bounds, char *text_list, int *active, int id);
void sodoku_type_combobox(Rect bounds, char *text_list, int *active, int id);
void solve_stragey(Rect bounds, char *text_list, int *active, int id);
void sodoku_ceil(const Rect *bounds, int *active);
void sodoku_board(sodoku_t *sodoku, Rect bounds);

void combobox(Rect bounds, char *text_list, int *active, int id);
void combox(Rect bounds, char *text, options_t *options, int id);

void imgui_init(SDL_Renderer *_renderer, uistate_t *_uistate, font_t *_font);
void imgui_begin();
void imgui_end();
#endif