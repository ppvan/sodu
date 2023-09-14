#include "core.h"
#include "font.h"
#include "imgui.h"
#include "la.h"
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

    // states
    sodoku_t *sodoku;
    char *sodoku_modes;
    int current_mode;

    char *solve_modes;
    int current_solve_mode;
    bool running;
} application;
void handle_event(application *app);
void render(application *app);
void app_generate(application *app);
void app_solve(application *app);
void format_time(char *str, double time);
void pad_right(char *str, int pad);

int main(void) {
    srand(time(0));
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
    sodoku_t *sodoku = sodoku_init(9);
    char *value = malloc(1024 * sizeof(char));
    imgui_init(renderer, &uistate, font);

    application app = {
        .window = window,
        .renderer = renderer,
        .font = font,
        .uistate = &uistate,
        .value = value,
        .sodoku = sodoku,
        .sodoku_modes = "9x9;16x16;25x25",
        .current_mode = 0,
        .solve_modes = "BINOMIAL;PRODUCT",
        .current_solve_mode = 0,
        .running = true,
    };
    while (app.running) {
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

    sodoku_type_combobox(layout_slot(), app->sodoku_modes, &app->current_mode, GEN_ID);
    solve_stragey(layout_slot(), app->solve_modes, &app->current_solve_mode, GEN_ID);

    memset(app->value, 0, 1024);
    format_time(app->value, app->sodoku->stats->solve_time);
    bglabel(layout_slot(), app->value, 0xDBB8D7);

    memset(app->value, 0, 1024);
    sprintf(app->value, "Clause: %d", app->sodoku->stats->clauses);
    bglabel(layout_slot(), app->value, 0xDBB8D7);

    memset(app->value, 0, 1024);
    sprintf(app->value, "Vars: %d", app->sodoku->stats->variables);
    bglabel(layout_slot(), app->value, 0xDBB8D7);

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

    app->sodoku = sodoku_generate(9);
}

void app_solve(application *app) {
    sodoku_solve(app->sodoku, BINOMIAL);
    ;
    ;
}

void format_time(char *str, double time) {
    sprintf(str, "Time: %.2f ms", time * 1000);
    ;
    ;
}

void pad_right(char *str, int pad) {
    int len = strlen(str);
    while (len < pad) {
        str[len] = ' ';
        len++;
    }
    str[len] = '\0';
}