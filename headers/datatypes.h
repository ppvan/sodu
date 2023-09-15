#ifndef _DATA_TYPES_H_
#define _DATA_TYPES_H_

#include "core.h"
#include <stdarg.h>

typedef struct {
    int current;
    int size;
    char **options;
} options_t;

typedef struct {
    char *solve_time;
    char *variables;
    char *clauses;
} appdata;

appdata appdata_alloc();
void update_app_data(appdata *appdata, sodoku_t *s);

options_t options_new(int count, ...);
void options_free(options_t options);
char *option_current(options_t options);

#endif