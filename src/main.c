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

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

void render(SDL_Renderer *renderer, uistate_t *uistate);

int main(void) {
    scc(SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO));
    int imgFlag = IMG_INIT_PNG;
    if (!(IMG_Init(IMG_INIT_PNG) & imgFlag)) {
        fprintf(stderr, "SDL_image: %s\n", IMG_GetError());
        exit(EXIT_FAILURE);
    }

    int flags = SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE;
    SDL_Window *window = scp(SDL_CreateWindow("Hello SDL2", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                              SCREEN_WIDTH, SCREEN_HEIGHT, flags));
    SDL_Renderer *renderer = scp(SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC));

    font_t *font = font_init(renderer, "./assets/octin");
    (void)font;

    uistate_t uistate = {0};
    imgui_init(renderer, &uistate, font);

    bool quit = false;
    while (!quit) {
        SDL_Event event = {0};
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
            case SDL_QUIT:
                quit = true;
                break;

            case SDL_MOUSEMOTION: {

                uistate.mouse = (Vec2i){
                    .x = event.motion.x,
                    .y = event.motion.y,
                };
                break;
            }

            case SDL_MOUSEBUTTONDOWN: {
                if (event.button.button == SDL_BUTTON_LEFT) {
                    uistate.mousedown = 1;
                }
                break;
            }

            case SDL_MOUSEBUTTONUP: {
                if (event.button.button == SDL_BUTTON_LEFT) {
                    uistate.mousedown = 0;
                }
                break;
            }

            case SDL_KEYUP: {
                switch (event.key.keysym.sym) {
                case SDLK_ESCAPE:
                    quit = true;
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

        render(renderer, &uistate);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

void render(SDL_Renderer *renderer, uistate_t *uistate) {
    imgui_begin();

    Rect screen = {.x = 0, .y = 0, .w = SCREEN_WIDTH, .h = SCREEN_HEIGHT};
    draw_rect(&screen, 0x000000);

    Rect ceilbox = {.x = 0, .y = 0, .w = 60, .h = 60};
    int board_size = 9;
    int sqrt_size = sq_number_sqrt(board_size);
    int border = 1;
    int large_border = 3;
    for (int i = 0; i < board_size; i++) {
        for (int j = 0; j < board_size; j++) {

            ceilbox.x = i * ceilbox.w + i * border + i / sqrt_size * large_border;
            ceilbox.y = j * ceilbox.h + j * border + j / sqrt_size * large_border;
            button(&ceilbox, "10", i * board_size + j + 1);
        }
    }

    imgui_end();
}