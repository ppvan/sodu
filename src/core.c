#include "core.h"
#include "utils.h"
#include <assert.h>
#include <kissat.h>
#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum { UNSATISFIABLE = 20, SATISFIABLE = 10 };
int index_3d_to_1d(int size, int i, int j, int k);
void index_1d_to_3d(int size, int idx, int *i, int *j, int *v);
bool sodoku_solve_binominal(sodoku_t *s);

bool sodoku_solve_binominal(sodoku_t *s) {
    int size = 9;

    kissat *solver = kissat_init();
    // kissat_set_option(solver, "verbose", 1);
    kissat_set_option(solver, "quiet", 1);
    kissat_reserve(solver, size * size * size);
    int lit = 0;

    // Each cell 1 number.
    for (int i = 1; i <= size; i++) {
        for (int j = 1; j <= size; j++) {
            // each ceil at least 1 number.
            // ceil(i, j) = v1 or v2 or v3 or .... or vn
            for (int v = 1; v <= size; v++) {
                int idx = index_3d_to_1d(size, i, j, v);
                kissat_add(solver, idx);
                // printf("%d ", idx);
            }
            kissat_add(solver, 0);
            //  printf("\n");

            // each ceil at most 1 number.
            // (ceil(i, j) = v1 => ceil(i, j) != v2)
            // and (ceil(i, j) = v1 => ceil(i, j) != v3)
            // and ...

            // <=> (ceil(i, j) != v1 or ceil(i, j) != v2)
            // and (ceil(i, j) != v1 or ceil(i, j) != v3)
            // ...
            for (int v = 1; v <= size; v++) {
                for (int other_v = 1; other_v <= size; other_v++) {
                    if (other_v == v)
                        continue;

                    int idx1 = index_3d_to_1d(size, i, j, v);
                    int idx2 = index_3d_to_1d(size, i, j, other_v);
                    kissat_add(solver, -idx1);
                    kissat_add(solver, -idx2);
                    kissat_add(solver, 0);

                    // printf("%d => !%d\n", idx1, idx2);
                }
            }
        }
    }

    // once in row
    for (int i = 1; i <= size; i++) {
        for (int v = 1; v <= size; v++) {
            // at least 1 in row.
            // ceil(i, j1, v) or ceil(i, j2, v) or ...
            for (int j = 1; j <= size; j++) {
                int idx = index_3d_to_1d(size, i, j, v);
                kissat_add(solver, idx);
                // printf("%d ", idx);
            }
            kissat_add(solver, 0);
            //  printf("\n");

            // at most 1 in row
            for (int j = 1; j <= size; j++) {
                for (int other_j = 1; other_j <= size; other_j++) {
                    if (other_j == j)
                        continue;

                    int idx1 = index_3d_to_1d(size, i, j, v);
                    int idx2 = index_3d_to_1d(size, i, other_j, v);
                    kissat_add(solver, -idx1);
                    kissat_add(solver, -idx2);
                    kissat_add(solver, 0);
                    // printf("%d => !%d\n", idx1, idx2);
                }
            }
        }
    }

    // once in col
    for (int j = 1; j <= size; j++) {
        for (int v = 1; v <= size; v++) {
            // at least 1 in col.
            for (int i = 1; i <= size; i++) {
                int idx = index_3d_to_1d(size, i, j, v);
                kissat_add(solver, idx);
                // printf("%d ", idx);
            }
            kissat_add(solver, 0);
            //  printf("\n");

            // at most 1 in col
            for (int i = 1; i <= size; i++) {
                for (int other_i = 1; other_i <= size; other_i++) {
                    if (other_i == i)
                        continue;

                    int idx1 = index_3d_to_1d(size, i, j, v);
                    int idx2 = index_3d_to_1d(size, other_i, j, v);
                    kissat_add(solver, -idx1);
                    kissat_add(solver, -idx2);

                    kissat_add(solver, 0);
                    // printf("%d => !%d\n", idx1, idx2);
                }
            }
        }
    }

    // once in sub-box:
    // sub-box have length = sqrt(size) = x
    // we have (size/x) = x sub part for each dimensions
    int sr = sq_number_sqrt(size);

    // iterate sub-boxs
    for (int sub_i = 1; sub_i < size; sub_i += sr) {
        for (int sub_j = 1; sub_j < size; sub_j += sr) {
            // at least 1 val in sub-box ceil.
            for (int v = 1; v <= size; v++) {

                // iterate in sub-box
                for (int i = sub_i; i <= sub_i + sr; i++) {
                    for (int j = sub_j; j <= sub_j + sr; j++) {
                        int idx = index_3d_to_1d(size, i, j, v);
                        kissat_add(solver, idx);
                        // printf("%d ", idx);
                    }
                }
                //  printf("\n");
                kissat_add(solver, 0);
            }

            // at most 1 val in sub-box
            for (int v = 1; v <= size; v++) {

                // iterate in sub-box
                for (int i = sub_i; i < sub_i + sr; i++) {
                    for (int j = sub_j; j < sub_j + sr; j++) {
                        // kissat_add(solver, -index_3d_to_1d(size, i, j, v));

                        for (int other_i = sub_i; other_i < sub_i + sr; other_i++) {
                            for (int other_j = sub_j; other_j < sub_j + sr; other_j++) {
                                if (other_i == i && other_j == j) {
                                    continue;
                                }

                                int idx1 = index_3d_to_1d(size, i, j, v);
                                int idx2 = index_3d_to_1d(size, other_i, other_j, v);
                                kissat_add(solver, -idx1);
                                kissat_add(solver, -idx2);
                                kissat_add(solver, 0);

                                // printf("%d => !%d\n", idx1, idx2);
                            }
                        }
                    }
                }
            }
        }
    }

    // init value.
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            if (SKU_AT(s, i, j) == 0)
                continue;
            // cnf require indx from 1 -> i + 1, j + 1
            int idx = index_3d_to_1d(size, i + 1, j + 1, SKU_AT(s, i, j));
            kissat_add(solver, idx);
            kissat_add(solver, 0);
        }
    }

    int ans = kissat_solve(solver);

    // extract proof.
    int i, j, v;
    if (ans == SATISFIABLE) {
        for (int idx = 1; idx <= size * size * size; idx++) {
            if (kissat_value(solver, idx) > 0) {
                index_1d_to_3d(size, idx, &i, &j, &v);
                SKU_AT(s, i - 1, j - 1) = v;
            } else {
                assert(idx && "That variable shouldn't be true.");
            }
        }
        kissat_release(solver);

        return true;
    } else {
        kissat_release(solver);
        return false;
    }
}

// Map (n,n,n) to 1..n^3
// row,col,val should be in [1,n]
int index_3d_to_1d(int size, int i, int j, int v) {
    i -= 1;
    j -= 1;
    v -= 1;

    return (i * size + j) * size + v + 1;
}

void index_1d_to_3d(int size, int idx, int *i, int *j, int *v) {
    *i = 1;
    *j = 1;
    *v = 1;
    // Decrement the index to match the 1-based indexing used in the mapping
    idx--;

    // Calculate the v (third dimension) coordinate
    *v = idx % size + 1;

    // Calculate the remaining coordinates (i and j)
    idx /= size; // Remove v from the index
    if (idx == 0)
        return;

    *j = idx % size + 1;
    idx /= size; // Remove j from the index
    if (idx == 0)
        return;

    *i = idx + 1;
}

sodoku_t *sodoku_init(int size) {
    sodoku_t *s = malloc(sizeof(sodoku_t));
    memset(s, 0, sizeof(sodoku_t));

    s->size = size;
    s->data = malloc(size * size * sizeof(int));
    memset(s->data, 0, size * sizeof(int));
    return s;
}

void sodoku_free(sodoku_t *s) {
    free(s->data);
    free(s);
}

sodoku_t *sodoku_load(const char *filename) {
    FILE *fin = fopen(filename, "r");
    assert(fin != NULL && "Can't open file");
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
sodoku_t *sodoku_generate(int size) {
    size = 9;
    int sub_size = 3;

    sodoku_t *s = sodoku_init(size);
    kissat *solver = kissat_init();
    int lit = 0;

    // Each cell 1 number.
    for (int i = 1; i <= size; i++) {
        for (int j = 1; j <= size; j++) {
            // each ceil at least 1 number.
            // ceil(i, j) = v1 or v2 or v3 or .... or vn
            for (int v = 1; v <= size; v++) {
                kissat_add(solver, index_3d_to_1d(size, i, j, v));
            }
            kissat_add(solver, 0);

            // each ceil at most 1 number.
            // (ceil(i, j) = v1 => ceil(i, j) != v2)
            // and (ceil(i, j) = v1 => ceil(i, j) != v3)
            // and ...

            // <=> (ceil(i, j) != v1 or ceil(i, j) != v2)
            // and (ceil(i, j) != v1 or ceil(i, j) != v3)
            // ...
            for (int v = 1; v <= size; v++) {
                for (int other_v = 1; other_v <= size; other_v++) {
                    if (other_v == v)
                        continue;

                    kissat_add(solver, -index_3d_to_1d(size, i, j, v));
                    kissat_add(solver, -index_3d_to_1d(size, i, j, other_v));
                    kissat_add(solver, 0);
                }
            }
        }
    }

    // once in row
    for (int i = 1; i <= size; i++) {
        for (int v = 1; v <= size; v++) {
            // at least 1 in row.
            // ceil(i, j1, v) or ceil(i, j2, v) or ...
            for (int j = 1; j <= size; j++) {
                kissat_add(solver, index_3d_to_1d(size, i, j, v));
            }
            kissat_add(solver, 0);

            // at most 1 in row
            for (int j = 1; j <= size; j++) {
                for (int other_j = 1; other_j <= size; other_j++) {
                    if (other_j == j)
                        continue;

                    kissat_add(solver, -index_3d_to_1d(size, i, j, v));
                    kissat_add(solver, -index_3d_to_1d(size, i, other_j, v));
                    kissat_add(solver, 0);
                }
            }
        }
    }

    // once in col
    for (int j = 1; j <= size; j++) {
        for (int v = 1; v <= size; v++) {
            // at least 1 in col.
            for (int i = 1; i <= size; i++) {
                kissat_add(solver, index_3d_to_1d(size, i, j, v));
            }
            kissat_add(solver, 0);

            // at most 1 in col
            for (int i = 1; i <= size; i++) {
                for (int other_i = 1; other_i <= size; other_i++) {
                    if (other_i == i)
                        continue;

                    kissat_add(solver, -index_3d_to_1d(size, i, j, v));
                    kissat_add(solver, -index_3d_to_1d(size, other_i, j, v));
                    kissat_add(solver, 0);
                }
            }
        }
    }

    // once in sub-box:
    // sub-box have length = sqrt(size) = x
    // we have (size/x) = x sub part for each dimensions
    int sr = sq_number_sqrt(size);

    // iterate sub-boxs
    for (int sub_i = 1; sub_i < size; sub_i += sr) {
        for (int sub_j = 1; sub_j < size; sub_j += sr) {
            // at least 1 val in sub-box ceil.
            for (int v = 1; v <= size; v++) {

                // iterate in sub-box
                for (int i = sub_i; i <= sub_i + sr; i++) {
                    for (int j = sub_j; j <= sub_j + sr; j++) {
                        kissat_add(solver, index_3d_to_1d(size, i, j, v));
                    }
                }
                kissat_add(solver, 0);
            }

            // at most 1 val in sub-box
            for (int v = 1; v <= size; v++) {

                // iterate in sub-box
                for (int i = sub_i; i <= sub_i + sr; i++) {
                    for (int j = sub_j; j <= sub_j + sr; j++) {
                        // kissat_add(solver, -index_3d_to_1d(size, i, j, v));

                        for (int other_i = sub_i; other_i <= sub_i + sr; other_i++) {
                            for (int other_j = sub_j; other_j <= sub_j + sr; other_j++) {
                                kissat_add(solver, -index_3d_to_1d(size, i, j, v));
                                kissat_add(solver, -index_3d_to_1d(size, other_i, other_j, v));
                                kissat_add(solver, 0);
                            }
                        }
                    }
                }
                kissat_add(solver, 0);
            }
        }
    }

    int ans = kissat_solve(solver);
    if (ans == SATISFIABLE) {
        printf("SATISFIABLE: \n");
    } else {
        printf("UNSATISFIABLE\n");
    }

    kissat_release(solver);

    return s;
}

bool sodoku_solve(sodoku_t *s, strategy_t strategy) {
    switch (strategy) {

    case BINOMIAL: {
        return sodoku_solve_binominal(s);
    }
    case SOME: {
        assert(0 && "Unimplemented.");
    } break;
    }

    return true;
}

/** Check if current state is a valid solution. */
bool sodoku_valid(sodoku_t *s) {
    int size = s->size;
    int sr = sq_number_sqrt(size);
    bool *flags = malloc(size * sizeof(bool));

    // rows
    for (int i = 0; i < size; i++) {
        memset(flags, 0, size * sizeof(bool));

        for (int j = 0; j < size; j++) {
            assert(SKU_AT(s, i, j) >= 1 && SKU_AT(s, i, j) <= size);
            if (flags[SKU_AT(s, i, j) - 1]) {
                return false;
            }
            flags[SKU_AT(s, i, j) - 1] = true;
        }
    }

    // columns
    for (int i = 0; i < size; i++) {
        memset(flags, 0, size * sizeof(bool));

        for (int j = 0; j < size; j++) {
            assert(SKU_AT(s, j, i) >= 1 && SKU_AT(s, j, i) <= size);
            if (flags[SKU_AT(s, j, i) - 1]) {
                return false;
            }
            flags[SKU_AT(s, j, i) - 1] = true;
        }
    }

    // sub-box
    for (int sub_i = 0; sub_i < size; sub_i += sr) {
        for (int sub_j = 0; sub_j < size; sub_j += sr) {

            memset(flags, 0, size * sizeof(bool));
            // iterate in sub-box
            for (int i = sub_i; i < sub_i + sr; i++) {
                for (int j = sub_j; j < sub_j + sr; j++) {

                    assert(SKU_AT(s, i, j) >= 1 && SKU_AT(s, i, j) <= size);
                    if (flags[SKU_AT(s, i, j) - 1]) {
                        return false;
                    }
                    flags[SKU_AT(s, i, j) - 1] = true;
                }
            }
        }
    }

    free(flags);

    return true;
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
