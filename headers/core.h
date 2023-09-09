
#include <stddef.h>
typedef struct {
  size_t size;
  int *data;

  double solve_time;
} sodoku_t;

typedef enum { BINOMIAL = 0, SOME } strategy_t;

#define SKU_AT(s, i, j) (s).data[(i) * (s).cols + (j)]

sodoku_t *sodoku_init(size_t size);
void *sodoku_free(sodoku_t *board);

/** Load sodoku board from file.
    Should be in format:
    <size>
    <cell> <cell> ...
    <cell> <cell> ...
    ...

    0 = blank cell.
*/
sodoku_t *sodoku_load(const char *filename);
sodoku_t *sodoku_generate(size_t size);

void sodoku_solve(sodoku_t *s, strategy_t strategy);

#define SKU_PRINT(m) mat_print(m, #m);
void sodoku_print(sodoku_t *s, const char *name);