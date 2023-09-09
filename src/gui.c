#include "utils.h"

#include <SDL.h>
#include <SDL_error.h>
#include <SDL_events.h>
#include <SDL_rect.h>
#include <SDL_render.h>
#include <SDL_video.h>
#include <stdbool.h>

int main2(void) {
  scc(SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO));

  int flags = SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE;
  SDL_Window *window = scp(SDL_CreateWindow("Hello SDL2", 0, 0, 640, 480, 0));
  SDL_Renderer *renderer = scp(SDL_CreateRenderer(
      window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC));

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

    SDL_Rect rect = {0, 0, 50, 50};
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 1);
    SDL_RenderDrawRect(renderer, &rect);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 1);

    SDL_RenderPresent(renderer);
  }

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();

  printf("QUit");

  return 0;
}