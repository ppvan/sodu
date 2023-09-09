#ifndef _kissat_h_INCLUDED
#define _kissat_h_INCLUDED

typedef struct kissat kissat;

// Default (partial) IPASIR interface.

const char *kissat_signature(void);
kissat *kissat_init(void);

/**Example usage for people like me.

c  simple_v3_c2.cnf
c
p cnf 3 2
1 -3 0
2 3 -1 0
will translate to

solver = kissat_init()
kissat_reserve(solver, 3);
kissat_add(solver, 1);
kissat_add(solver, -3);
kissat_add(solver, 0);
kissat_add(solver, 2);
kissat_add(solver, 3);
kissat_add(solver, -1);
kissat_add(solver, 0);

int sat = kissat_solve(solver);
10 => UNSAT
20 => SAT

int val = kissat_value(solver, index); // starts from 1
val = 0 -> false
val = 1 -> true
kissat_release(solver);
*/

void kissat_add(kissat *solver, int lit);
int kissat_solve(kissat *solver);
/** Get result after solved. */
int kissat_value(kissat *solver, int lit);

/** Free memory use by solver.*/
void kissat_release(kissat *solver);

void kissat_set_terminate(kissat *solver, void *state,
                          int (*terminate)(void *state));

// Additional API functions.

void kissat_terminate(kissat *solver);
/**
    Declare problem will have max_var
*/
void kissat_reserve(kissat *solver, int max_var);

const char *kissat_id(void);
const char *kissat_version(void);
const char *kissat_compiler(void);

const char **kissat_copyright(void);
void kissat_build(const char *line_prefix);
void kissat_banner(const char *line_prefix, const char *name_of_app);

int kissat_get_option(kissat *solver, const char *name);
int kissat_set_option(kissat *solver, const char *name, int new_value);

int kissat_has_configuration(const char *name);
int kissat_set_configuration(kissat *solver, const char *name);

void kissat_set_conflict_limit(kissat *solver, unsigned);
void kissat_set_decision_limit(kissat *solver, unsigned);

void kissat_print_statistics(kissat *solver);

/** Symbols i personally take from header files of kissat.
If you get link error, don't use it.*/
double kissat_time(struct kissat *);

#endif
