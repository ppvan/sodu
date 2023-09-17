#include "core.h"
#include "datatypes.h"
#include "font.h"
#include "imgui.h"
#include "la.h"
#include "solver.h"
#include "utils.h"

#include <SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>
#include <SDL_error.h>
#include <SDL_events.h>
#include <SDL_image.h>
#include <SDL_rect.h>
#include <SDL_render.h>
#include <SDL_video.h>
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define SCREEN_WIDTH 1200
#define SCREEN_HEIGHT 800

#define STAT_PADING 16
typedef struct {
    SDL_Window *window;
    SDL_Renderer *renderer;
    font_t *font;
    uistate_t *uistate;
    // string holder;
    char *value;
    // appdata
    appdata data;

    // states
    sodoku_t *sodoku;
    options_t mode;
    options_t board_mode;
    options_t algrithm_mode;
    bool running;
} application;
void update(application *app);
void handle_event(application *app);
void render(application *app);
void app_generate(application *app);
void app_solve(application *app);

enum board_mode { BOARD9x9 = 0, BOARD16x16, BOARD25x25 };
enum algorithm_mode { AL_BINOMINAL = 0, AL_PRODUCT };

int main(void) {
    srand(time(0));
    options_t mode = options_new(2, "Demo", "Test");
    options_t board_mode = options_new(3, "9x9", "16x16", "25x25");
    options_t algorithm_mode = options_new(2, "BINOMINAL", "PRODUCT");
    appdata data = appdata_alloc();
    sodoku_t *sodoku = sodoku_generate(9);

    scc(SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO));
    int imgFlag = IMG_INIT_PNG;
    if (!(IMG_Init(IMG_INIT_PNG) & imgFlag)) {
        fprintf(stderr, "SDL_image: %s\n", IMG_GetError());
        exit(EXIT_FAILURE);
    }

    int flags = SDL_WINDOW_SHOWN;
    SDL_Window *window = scp(SDL_CreateWindow("Hello SDL2", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                              SCREEN_WIDTH, SCREEN_HEIGHT, flags));
    SDL_Renderer *renderer = scp(SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC));

    font_t *font = font_init(renderer, "./assets/octin");
    uistate_t uistate = {0};
    imgui_init(renderer, &uistate, font);

    application app = {
        .window = window,
        .renderer = renderer,
        .font = font,
        .uistate = &uistate,
        .data = data,
        .sodoku = sodoku,
        .mode = mode,
        .board_mode = board_mode,
        .algrithm_mode = algorithm_mode,
        .running = true,
    };

    while (app.running) {
        update(&app);
        handle_event(&app);
        render(&app);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

void handle_event(application *app) {
    SDL_Event event = {0};
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_QUIT:
            app->running = false;
            break;

        case SDL_MOUSEMOTION: {

            app->uistate->mouse = (Vec2i){
                .x = event.motion.x,
                .y = event.motion.y,
            };
            break;
        }

        case SDL_MOUSEBUTTONDOWN: {
            if (event.button.button == SDL_BUTTON_LEFT) {
                app->uistate->mousedown = 1;
            }
            break;
        }

        case SDL_MOUSEBUTTONUP: {
            if (event.button.button == SDL_BUTTON_LEFT) {
                app->uistate->mousedown = 0;
            }
            break;
        }

        case SDL_KEYUP: {
            switch (event.key.keysym.sym) {
            case SDLK_ESCAPE:
                app->running = false;
                break;
            default:
                break;
            }

            break;
        }
        case SDL_KEYDOWN: {
            break;
        }

        default:
            break;
        }
        // decide what to do with this event.
    }
}

void render(application *app) {

    imgui_begin();
    Rect bounds = {4, 4, SCREEN_HEIGHT - 8, SCREEN_HEIGHT - 8};
    layout_begin(HORIZONTAL, bounds, 1, 0);
    sodoku_board(app->sodoku, layout_slot());
    layout_end();

    Rect controls = {bounds.x + bounds.w + 2, 4, SCREEN_WIDTH - bounds.x - bounds.w - 6, SCREEN_HEIGHT - 10};
    layout_begin(VERTICAL, controls, 10, 3);

    if (combox(layout_slot(), "Board", &(app->board_mode), GEN_ID)) {
        app_generate(app);
    }
    combox(layout_slot(), "ALG", &(app->algrithm_mode), GEN_ID);
    bglabel(layout_slot(), app->data.solve_time, 0xDBB8D7);
    bglabel(layout_slot(), app->data.clauses, 0xDBB8D7);
    bglabel(layout_slot(), app->data.variables, 0xDBB8D7);

    // button(layout_slot(), "Controls", GEN_ID);
    // button(layout_slot(), "Controls", GEN_ID);

    bglabel(layout_slot(), "", 0xDBB8D7);
    bglabel(layout_slot(), "", 0xDBB8D7);

    button(layout_slot(), "Export", GEN_ID);
    if (button(layout_slot(), "Generate", GEN_ID)) {
        app_generate(app);
    }
    if (button(layout_slot(), "Solve", GEN_ID)) {
        app_solve(app);
    }
    layout_end();
    imgui_end();
}

void app_generate(application *app) {
    if (app->sodoku != NULL) {
        sodoku_free(app->sodoku);
        app->sodoku = NULL;
    }
    switch (app->board_mode.current) {
    case BOARD9x9:
        app->sodoku = sodoku_generate(9);
        break;
    case BOARD16x16:
        app->sodoku = sodoku_generate(16);
        break;
    case BOARD25x25:
        app->sodoku = sodoku_generate(25);
    }
}

void app_solve(application *app) {

    if (sodoku_is_solution(app->sodoku)) {
        return;
    } else {
        switch (app->algrithm_mode.current) {
        case AL_BINOMINAL: {
            sodoku_solve(app->sodoku, BINOMIAL);
            break;
        case AL_PRODUCT:
            break;

        default:
            assert(0 && "Unreachable");
            break;
        }
        }
    }
}

void update(application *app) {
    update_app_data(&app->data, app->sodoku);
    ;
}