#include <SDL.h>
#include <SDL_error.h>
#include <SDL_events.h>
#include <SDL_rect.h>
#include <SDL_render.h>
#include <SDL_video.h>
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "core.h"
#include "utils.h"
#include <kissat.h>

void run_test(const char *filename) {
    double test_time = 0;
    assert(filename && "Can't malloc filename.");
    FILE *file = fopen(filename, "r");
    int tests = 0;
    int size = 0;
    fscanf(file, "%d", &tests);
    fscanf(file, "%d", &size);

    for (int i = 0; i < tests; i++) {

        // load data.
        sodoku_t *s = sodoku_init(size);
        for (int r = 0; r < size; r++) {
            for (int c = 0; c < size; c++) {
                fscanf(file, "%d", &SKU_AT(s, r, c));
            }
        }

        bool sat = sodoku_solve(s, BINOMIAL);

        if (sat && sodoku_valid(s)) {
            // SKU_PRINT(s);
            test_time += s->stats->solve_time;
            printf("%s\tPASS: %.4fs\n", filename, s->stats->solve_time);
            printf("%d vars, %d clauses\n", s->stats->variables, s->stats->clauses);
        } else {
            printf("%s\tFAILED\n", filename);
            break;
        }
        sodoku_free(s);
    }

    printf("Total: %f\n", test_time);
}

int main(void) {
    run_test("data/test-9x9.txt");
    return 0;
}