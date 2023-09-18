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
        // SKU_PRINT(s);

        bool sat = sodoku_solve(s, SEQ);

        // SKU_PRINT(s);
        if (sat && sodoku_is_solution(s)) {
            test_time += s->solver->time;
            DEBUG("PASS: %.4fs\n", s->solver->time);
            DEBUG("%d vars, %d clauses\n", s->solver->vars, s->solver->vars);
        } else {
            DEBUG("tests-%02d\tFAILED\n", i);
            DEBUG("SAT: %s\n", sat ? "true" : "false");
            break;
        }
        sodoku_free(s);
    }

    DEBUG("Total: %f\n", test_time);
}

int main(void) {
    run_test("./data/test-9x9.txt");
    ;
    ;
    return 0;
}