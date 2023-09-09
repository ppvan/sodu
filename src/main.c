#include <SDL.h>
#include <SDL_error.h>
#include <SDL_events.h>
#include <SDL_rect.h>
#include <SDL_render.h>
#include <SDL_video.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "utils.h"
#include <kissat.h>

enum { UNSATISFIABLE = 10, SATISFIABLE = 20 };

int main(void) {

  kissat *solver = kissat_init();
#define SAT

#ifdef SAT
  int literals[] = {1, 2, 0, -1, 0, -2, 0};
#else
  int literals[] = {-1, 2, 3, 0, 1, -2, 0, -3, -2, 0};
#endif
  int len = sizeof(literals) / sizeof(int);

  kissat_reserve(solver, 3);
  for (int i = 0; i < len; i++) {
    kissat_add(solver, literals[i]);
  }
  int ans = kissat_solve(solver);

  if (ans == SATISFIABLE) {
    printf("SATISFIABLE: \n");
    for (int i = 0; i < len; i++) {
      int val = kissat_value(solver, i);
      printf("%d = %s\n", i, val ? "true" : "false");
    }

  } else {
    printf("UNSATISFIABLE\n");
  }

  double time = kissat_time(solver);
  printf("Process time: %.f\n", time);
  //   kissat_terminate(solver);

  printf("%s\n", kissat_version());
  return 0;
}
