#ifndef _CORE_H_

#define _CORE_H_
#include "kissat.h"
#include <stdbool.h>
#include <stddef.h>

typedef struct {
    double solve_time;
    int variables;
    int clauses;
} statistics_t;

typedef struct {
    int size;
    int *data;

    kissat *solver;
    statistics_t *stats;
} sodoku_t;

typedef enum {
    BINOMIAL,
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
sodoku_t *sodoku_generate(int size);

bool sodoku_solve(sodoku_t *s, strategy_t strategy);

/** Check if current state is a valid solution. */
bool sodoku_valid(sodoku_t *s);

#define SKU_PRINT(s) sodoku_print(s, #s);
void sodoku_print(sodoku_t *s, const char *name);

#endif