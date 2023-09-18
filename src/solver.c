#include "solver.h"
#include "datatypes.h"
#include "kissat.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

solver_t *solver_new() {
    solver_t *solver = malloc(sizeof(solver_t));
    memset(solver, 0, sizeof(solver_t));

    kissat *kissat = kissat_init();
    kissat = kissat_init();
    kissat_set_option(kissat, "quiet", 1);
    kissat_set_option(kissat, "sat", 1);
    // kissat_set_option(kissat, "seed", rand());

    solver->kissat = kissat;
    solver->usable = true;

    return solver;
}
void solver_free(solver_t *solver) {
    kissat_release(solver->kissat);
    free(solver);
}
void solver_reset(solver_t *solver) {
    kissat_release(solver->kissat);
    solver->kissat = kissat_init();
    kissat_set_option(solver->kissat, "quiet", 1);
    kissat_set_option(solver->kissat, "sat", 1);

    solver->time = 0;
    solver->clauses = 0;
    solver->vars = 0;
    solver->usable = true;
}
bool solver_solve(solver_t *solver) {

    if (!solver->usable) {
        solver_reset(solver);
    }

    int ans = kissat_solve(solver->kissat);
    solver->time = kissat_time(solver->kissat);
    if (ans == UNSATISFIABLE) {
        return false;
    }

    return true;
}
void solver_add(solver_t *solver, int lit) {
    kissat_add(solver->kissat, lit);

    if (lit == 0) {
        solver->clauses++;
    } else {
        int tmp = solver->vars;
        solver->vars = abs(lit) > tmp ? abs(lit) : tmp;
    }
}
bool solver_value(solver_t *solver, int lit) {
    int val = kissat_value(solver->kissat, lit);
    return val == lit;
}

void solver_alo(solver_t *solver, vec_t *literals) {
    for (int i = 0; i < literals->size; i++) {
        solver_add(solver, literals->data[i]);
    }

    solver_add(solver, 0);
}
void solver_amo(solver_t *solver, vec_t *literals, counter_t *aux_start) {
    (void)aux_start;

    for (int i = 0; i < literals->size; i++) {
        for (int j = i + 1; j < literals->size; j++) {
            solver_add(solver, -literals->data[i]);
            solver_add(solver, -literals->data[j]);
            solver_add(solver, 0);
        }
    }
}
void solver_amo_seq(solver_t *solver, vec_t *literals, counter_t *aux_start) { (void)aux_start; }
void solver_amo_product(solver_t *solver, vec_t *literals, counter_t *aux_start) { (void)aux_start; }