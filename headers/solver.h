
#ifndef _SOLVER_H_

#define _SOLVER_H_

#include "datatypes.h"
#include <kissat.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    kissat *kissat;
    bool usable;
    double time;
    size_t vars;
    size_t clauses;
    size_t index;
} solver_t;

typedef void (*solver_amo_t)(solver_t *solver, vec_t *literals, counter_t *aux_start);

enum { UNSATISFIABLE = 20, SATISFIABLE = 10 };

solver_t *solver_new();
void solver_free(solver_t *solver);
void solver_reset(solver_t *solver);
bool solver_solve(solver_t *solver);
void solver_add(solver_t *solver, int lit);
bool solver_value(solver_t *solver, int lit);

void solver_alo(solver_t *solver, vec_t *literals);
void solver_amo(solver_t *solver, vec_t *literals, counter_t *aux_start);
void solver_amo_seq(solver_t *solver, vec_t *literals, counter_t *aux_start);
void solver_amo_product(solver_t *solver, vec_t *literals, counter_t *aux_start);
#endif
