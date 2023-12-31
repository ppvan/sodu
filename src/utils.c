#include "utils.h"
#include <SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void scc(int code) {
    if (code < 0) {
        fprintf(stderr, "SDL Error: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }
}

void *scp(void *handler) {
    if (handler == NULL) {
        fprintf(stderr, "SDL Error: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    return handler;
}

int sq_number_sqrt(int num) {
    int res = (int)sqrt(num);
    assert(res * res == num);

    return res;
}

void debug(const char *file, int line, const char *format, ...) {
#ifdef DEBUG
    printf("%s:%d: ", file, line);
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
#endif
}

double math_rand() { return rand() * 1.0 / RAND_MAX; }