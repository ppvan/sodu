#include <SDL.h>
#include <SDL_error.h>
#include <SDL_events.h>
#include <SDL_rect.h>
#include <SDL_render.h>
#include <SDL_video.h>
#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "core.h"
#include "solver.h"
#include "utils.h"
#include <kissat.h>
#include <string.h>

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

        bool sat = sodoku_solve(s, PRODUCT);

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

sodoku_t **generate_test(int size) {

    sodoku_t **sodoku_list = malloc(4 * size * sizeof(sodoku_t *));
    int index = 0;
    assert(sodoku_list && "Can't malloc sodoku_list");

    for (int i = 0; i < size; i++) {
        sodoku_list[index++] = sodoku_generate(9);
    }

    for (int i = 0; i < size; i++) {
        sodoku_list[index++] = sodoku_generate(16);
    }

    for (int i = 0; i < size; i++) {
        sodoku_list[index++] = sodoku_generate(25);
    }

    for (int i = 0; i < size; i++) {
        sodoku_list[index++] = sodoku_generate(36);
    }

    return sodoku_list;
}

void clean_test(sodoku_t **list, int size) {
    for (int i = 0; i < 4 * size; i++) {
        sodoku_free(list[i]);
    }

    free(list);
}

void write_test(sodoku_t **list, const char *filename) {
    (void)list;
    (void)filename;
    ;
    ;
}

void write_result(sodoku_t **list, const char *filename) {
    (void)list;
    (void)filename;
    ;
    ;
}

#define AL_MODE 3
int main(void) {
    // run_test("./data/test-9x9.txt");

    int size = 10;

    printf("Generate tests\n");
    sodoku_t **list = generate_test(size);
    printf("Done\n");
    // return 1;
    //
    double exe_times[AL_MODE] = {0};
    size_t clauses[AL_MODE] = {0};
    size_t vars[AL_MODE] = {0};
    char *al_names[] = {"BINOMINAL", "SEQUENTIAL", "PRODUCT"};
    int *backup = malloc(4000 * sizeof(int)); // backup the sodoku data

    for (int i = 0; i < size * 4; i++) {
        for (int j = 0; j < AL_MODE; j++) {
            solver_reset(list[i]->solver);
            memcpy(backup, list[i]->data, (list[i]->size) * (list[i]->size));

            sodoku_solve(list[i], j);
            assert(sodoku_is_solution(list[i]));

            clauses[j] += list[i]->solver->clauses;
            vars[j] += list[i]->solver->vars;
            exe_times[j] += list[i]->solver->time;

            memcpy(list[i]->data, backup, (list[i]->size) * (list[i]->size));
        }
    }

    for (int i = 0; i < AL_MODE; i++) {
        printf("%s: exe_times: %.4fs, clauses: %zu, vars: %zu\n", al_names[i], exe_times[i], clauses[i], vars[i]);
    }

    free(backup);
    clean_test(list, size);
    return 0;
}