#include "font.h"
#include "la.h"
#include "utils.h"

#include <SDL.h>
#include <SDL2/SDL_image.h>
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

int main(void) {
    scc(SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO));
    int imgFlag = IMG_INIT_PNG;
    if (!(IMG_Init(IMG_INIT_PNG) & imgFlag)) {
        fprintf(stderr, "SDL_image: %s\n", IMG_GetError());
        exit(EXIT_FAILURE);
    }

    int flags = SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE;
    SDL_Window *window =
        scp(SDL_CreateWindow("Hello SDL2", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, flags));
    SDL_Renderer *renderer = scp(SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC));

    font_t *font = font_init(renderer, "./assets/octin");

    bool quit = false;
    while (!quit) {
        SDL_Event event = {0};
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
            case SDL_QUIT:
                quit = true;
                break;
            default:
                break;
            }
            // decide what to do with this event.
        }

        SDL_RenderClear(renderer);

        Vec2i vec = {100, 100};
        font_render(renderer, font, vec, "Hello world\nSDL2 is hard\t as fuck");

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}