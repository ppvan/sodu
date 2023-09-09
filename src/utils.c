#include "utils.h"
#include <SDL.h>
#include <stdio.h>
#include <stdlib.h>

void scc(int code) {
  if (code < 0) {
    fprintf(stderr, "Error: %s\n", SDL_GetError());
    exit(EXIT_FAILURE);
  }
}

void *scp(void *handler) {
  if (handler == NULL) {
    fprintf(stderr, "Error: %s\n", SDL_GetError());
    exit(EXIT_FAILURE);
  }

  return handler;
}

int sq_number_sqrt(int num) {
  int res = (int)sqrt(num);
  assert(res * res == num);

  return res;
}