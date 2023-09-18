#ifndef _CORE_H_

#define _CORE_H_
#include "kissat.h"
#include "solver.h"
#include <stdbool.h>
#include <stddef.h>

// Desire clues to init the generator, got from:
// https://www.semanticscholar.org/paper/Sudoku-Puzzles-Generating%3A-From-Easy-to-Evil-Li/bf2d098e473ea34319b2f2cac8a91abf298dd11d
#define CLUES 11
#define BUF_SIZE 1024

typedef struct {
    int size;
    int *data;

    solver_t *solver;
    int aux_index;
} sodoku_t;

typedef enum {
    BINOMIAL,
    SEQ,
    PRODUCT,
} strategy_t;

#define SKU_AT(s, i, j) (s)->data[(i) * (s)->size + (j)]
// void index_1d_to_3d(int size, int idx, int *i, int *j, int *v);

sodoku_t *sodoku_init(int size);
void sodoku_free(sodoku_t *s);

/** Load sodoku board from file.
    Should be in format:
    <size>
    <cell> <cell> ...

    0 = blank cell.
*/
sodoku_t *sodoku_load(const char *filename);
sodoku_t *sodoku_load_str(int size, const char *str);
sodoku_t *sodoku_generate(int size);
int sodoku_auxnext(sodoku_t *s);

bool sodoku_solve(sodoku_t *s, strategy_t strategy);

/** Check if current state is a valid solution. */
bool sodoku_is_solution(sodoku_t *s);

#define SKU_PRINT(s) sodoku_print(s, #s);
void sodoku_print(sodoku_t *s, const char *name);

#endif