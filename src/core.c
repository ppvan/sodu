#include "core.h"
#include "datatypes.h"
#include "solver.h"
#include "utils.h"
#include <assert.h>
#include <errno.h>
#include <kissat.h>
#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

// Map (n,n,n) to 1..n^3
// row,col,val should be in [1,n]
static inline int index_3d_to_1d(int size, int i, int j, int v) {
    int idx = ((i - 1) * size + (j - 1)) * size + v;
    return idx;
}

int sodoku_auxnext(sodoku_t *s) { return s->aux_index++; }

static inline void index_1d_to_3d(int size, int idx, int *i, int *j, int *v) {
    *i = 1;
    *j = 1;
    *v = 1;
    idx--;
    *v = idx % size + 1;
    idx /= size; // Remove v from the index
    if (idx == 0)
        return;

    *j = idx % size + 1;
    idx /= size; // Remove j from the index
    if (idx == 0)
        return;

    *i = idx + 1;
}
/** Naive version, focus on correctness.
 */
bool sodoku_solve_internal(sodoku_t *s, solver_amo_t amo_func) {
    int size = s->size;
    int sr = sq_number_sqrt(size);
    vec_t *vec = vec_new();
    vec = vec_reserve(vec, size);
    counter_t aux = size * size * size;
    // counter_t counter = 0;

    // apply_defined_values(s);
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            if (SKU_AT(s, i, j) == 0)
                continue;
            // cnf require indx from 1 -> i + 1, j + 1
            int idx = index_3d_to_1d(size, i + 1, j + 1, SKU_AT(s, i, j));
            solver_add(s->solver, idx);
            solver_add(s->solver, 0);
        }
    }
    //  unique_row(s);
    for (int i = 1; i <= size; i++) {
        for (int v = 1; v <= size; v++) {
            for (int j = 1; j <= size; j++) {
                int idx = index_3d_to_1d(size, i, j, v);
                vec->data[j - 1] = idx;
            }

            solver_alo(s->solver, vec);
            amo_func(s->solver, vec, &aux);
        }
    }

    // unique_col(s);
    for (int j = 1; j <= size; j++) {
        for (int v = 1; v <= size; v++) {
            for (int i = 1; i <= size; i++) {
                int idx = index_3d_to_1d(size, i, j, v);
                vec->data[i - 1] = idx;
            }

            solver_alo(s->solver, vec);
            amo_func(s->solver, vec, &aux);
        }
    }
    // unique_ceil(s);
    for (int i = 1; i <= size; i++) {
        for (int j = 1; j <= size; j++) {
            for (int v = 1; v <= size; v++) {
                int idx = index_3d_to_1d(size, i, j, v);
                vec->data[v - 1] = idx;
            }
            solver_alo(s->solver, vec);
            amo_func(s->solver, vec, &aux);
        }
    }
    // unique_box(s);
    int cnt = 0;
    for (int sub_i = 1; sub_i < size; sub_i += sr) {
        for (int sub_j = 1; sub_j < size; sub_j += sr) {
            for (int v = 1; v <= size; v++) {

                for (int i = sub_i; i < sub_i + sr; i++) {
                    for (int j = sub_j; j < sub_j + sr; j++) {
                        int idx = index_3d_to_1d(size, i, j, v);
                        vec->data[cnt++] = idx;
                    }
                }

                solver_alo(s->solver, vec);
                amo_func(s->solver, vec, &aux);
                cnt = 0;
            }
        }
    }
    vec_free(vec);

    if (!solver_solve(s->solver)) {
        return false;
    }
    // extract_proof(s);
    int i, j, v;
    for (int idx = 1; idx <= size * size * size; idx++) {
        if (solver_value(s->solver, idx)) {
            index_1d_to_3d(size, idx, &i, &j, &v);
            SKU_AT(s, i - 1, j - 1) = v;
        }
    }

    return true;
}

sodoku_t *sodoku_init(int size) {
    sodoku_t *s = malloc(sizeof(sodoku_t));
    assert(s && "Can't malloc s");
    memset(s, 0, sizeof(sodoku_t));

    s->size = size;
    s->aux_index = size * size * size + 1;
    // init data
    s->data = malloc(size * size * sizeof(int));
    assert(s->data && "Can't malloc s->data");
    memset(s->data, 0, size * size * sizeof(int));
    // init solver
    s->solver = solver_new();
    return s;
}

void sodoku_free(sodoku_t *s) {
    free(s->data);
    solver_free(s->solver);
    free(s);
}

sodoku_t *sodoku_load(const char *filename) {
    FILE *fin = fopen(filename, "r");
    if (!fin) {
        fprintf(stderr, "Can't open: %s, errno: %d\n", filename, errno);
        exit(EXIT_FAILURE);
    }

    int size = 0;
    fscanf(fin, "%d", &size);
    sodoku_t *s = sodoku_init(size);
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; j++) {
            fscanf(fin, "%d", &SKU_AT(s, i, j));
        }
    }

    fclose(fin);

    return s;
}

bool sodoku_solve(sodoku_t *s, strategy_t strategy) {
    bool result = false;
    switch (strategy) {

    case BINOMIAL: {
        result = sodoku_solve_internal(s, solver_amo);
        break;
    }

    case SEQUENTIAL: {
        result = sodoku_solve_internal(s, solver_amo_seq);
        break;
    }

    case PRODUCT: {
        result = sodoku_solve_internal(s, solver_amo_product);
        break;
    }
    }

    return result;
}
bool sodoku_is_solution(sodoku_t *s) {
    int size = s->size;
    int sr = sq_number_sqrt(size);
    bool valid = true;
    bool *flags = malloc((size + 1) * sizeof(bool));
    assert(flags && "Can't malloc flags");

    // rows
    for (int i = 0; i < size; i++) {
        memset(flags, 0, (size + 1) * sizeof(bool));

        for (int j = 0; j < size; j++) {
            assert(SKU_AT(s, i, j) >= 0 && SKU_AT(s, i, j) <= size);

            if (SKU_AT(s, i, j) == 0) {
                valid = false;
                goto end;
            }

            if (flags[SKU_AT(s, i, j)]) { // duplicate
                valid = false;
                goto end;
            }
            flags[SKU_AT(s, i, j)] = true;
        }
    }

    // columns
    for (int i = 0; i < size; i++) {
        memset(flags, 0, (size + 1) * sizeof(bool));

        for (int j = 0; j < size; j++) {
            assert(SKU_AT(s, j, i) >= 0 && SKU_AT(s, j, i) <= size);

            if (SKU_AT(s, j, i) == 0) {
                valid = false;
                goto end;
            }

            if (flags[SKU_AT(s, j, i)]) {
                valid = false;
                goto end;
            }
            flags[SKU_AT(s, j, i)] = true;
        }
    }

    // sub-box
    for (int sub_i = 0; sub_i < size; sub_i += sr) {
        for (int sub_j = 0; sub_j < size; sub_j += sr) {

            memset(flags, 0, (size + 1) * sizeof(bool));
            // iterate in sub-box
            for (int i = sub_i; i < sub_i + sr; i++) {
                for (int j = sub_j; j < sub_j + sr; j++) {

                    assert(SKU_AT(s, i, j) >= 0 && SKU_AT(s, i, j) <= size);

                    if (SKU_AT(s, i, j) == 0) {
                        valid = false;
                        goto end;
                    }

                    if (flags[SKU_AT(s, i, j)]) {
                        valid = false;
                        goto end;
                    }
                    flags[SKU_AT(s, i, j)] = true;
                }
            }
        }
    }

end : { free(flags); }

    return valid;
}

void sodoku_print(sodoku_t *s, const char *name) {
    printf("%s {\n", name);
    printf("size: %d\n", s->size);
    for (int i = 0; i < s->size; ++i) {
        printf("    "); // 4 spaces
        for (int j = 0; j < s->size; j++) {
            printf("%4d ", SKU_AT(s, i, j));
        }
        printf("\n");
    }
    printf("}\n");
}

sodoku_t *sodoku_load_str(int size, const char *str) {
    sodoku_t *s = sodoku_init(size);
    char *token = NULL;
    char *buf = strdup(str);

    token = strtok(buf, " ");
    for (int r = 0; r < size; r++) {
        for (int c = 0; c < size; c++) {
            SKU_AT(s, r, c) = atoi(token);
            token = strtok(NULL, " ");
        }
    }
    free(buf);

    return s;
}

static sodoku_t *generate_solution(int size) {
    // try and error to find this values.
    int max_retried = 10;
    int sr = sq_number_sqrt(size);
    vec_t *vec = vec_new();
    vec = vec_reserve(vec, size);
    sodoku_t *s = NULL;
    while (max_retried--) {
        s = sodoku_init(size);
        counter_t aux = size * size * size;

        // apply_defined_values(s);
        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                if (SKU_AT(s, i, j) == 0)
                    continue;
                // cnf require indx from 1 -> i + 1, j + 1
                int idx = index_3d_to_1d(size, i + 1, j + 1, SKU_AT(s, i, j));
                solver_add(s->solver, idx);
                solver_add(s->solver, 0);
            }
        }
        //  unique_row(s);
        for (int i = 1; i <= size; i++) {
            for (int v = 1; v <= size; v++) {
                for (int j = 1; j <= size; j++) {
                    int idx = index_3d_to_1d(size, i, j, v);
                    vec->data[j - 1] = idx;
                }

                solver_alo(s->solver, vec);
                solver_amo(s->solver, vec, &aux);
            }
        }

        // unique_col(s);
        for (int j = 1; j <= size; j++) {
            for (int v = 1; v <= size; v++) {
                for (int i = 1; i <= size; i++) {
                    int idx = index_3d_to_1d(size, i, j, v);
                    vec->data[i - 1] = idx;
                }

                solver_alo(s->solver, vec);
                solver_amo(s->solver, vec, &aux);
            }
        }
        // unique_ceil(s);
        for (int i = 1; i <= size; i++) {
            for (int j = 1; j <= size; j++) {
                for (int v = 1; v <= size; v++) {
                    int idx = index_3d_to_1d(size, i, j, v);
                    vec->data[v - 1] = idx;
                }
                solver_alo(s->solver, vec);
                solver_amo(s->solver, vec, &aux);
            }
        }
        // unique_box(s);
        int cnt = 0;
        for (int sub_i = 1; sub_i < size; sub_i += sr) {
            for (int sub_j = 1; sub_j < size; sub_j += sr) {
                for (int v = 1; v <= size; v++) {

                    for (int i = sub_i; i < sub_i + sr; i++) {
                        for (int j = sub_j; j < sub_j + sr; j++) {
                            int idx = index_3d_to_1d(size, i, j, v);
                            vec->data[cnt++] = idx;
                        }
                    }

                    solver_alo(s->solver, vec);
                    solver_amo(s->solver, vec, &aux);
                    cnt = 0;
                }
            }
        }

        for (int i = 1; i <= CLUES; i++) {
            int val = math_rand() * size * size * size;
            int sign = math_rand() > 0.5 ? 1 : -1;
            solver_add(s->solver, sign * val);
            solver_add(s->solver, 0);
        }
        if (!solver_solve(s->solver)) {
            sodoku_free(s);
        } else {
            int i, j, v;
            for (int idx = 1; idx <= size * size * size; idx++) {
                if (solver_value(s->solver, idx)) {
                    index_1d_to_3d(size, idx, &i, &j, &v);
                    SKU_AT(s, i - 1, j - 1) = v;
                }
            }
            // reset solver for solving step
            solver_reset(s->solver);
            break;
        }
    }

    vec_free(vec);
    assert(max_retried >= 0 && "No solution found.");

    return s;
}

static void dig_holes(sodoku_t *s) {
    int size = s->size;
    int remain_clues = size * size / 8 + size;
    int cur_clues = size * size;

    while (cur_clues >= remain_clues) {

        int i = math_rand() * size;
        int j = math_rand() * size;
        if (SKU_AT(s, i, j) > 0) {
            SKU_AT(s, i, j) = 0;
            cur_clues--;
        }
    }
}

sodoku_t *sodoku_generate(int size) {

    sodoku_t *s = generate_solution(size);
    dig_holes(s);

    return s;
}