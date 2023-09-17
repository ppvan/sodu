#include "datatypes.h"
#include "core.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

options_t options_new(int count, ...) {
    options_t options = {0};
    options.size = count;
    options.current = 0;
    options.options = malloc(count * sizeof(char *));
    assert(options.options && "Can't malloc options->options");

    va_list args;
    va_start(args, count);
    for (int i = 0; i < count; i++) {
        options.options[i] = va_arg(args, char *);
    }
    va_end(args);

    return options;
}
void options_free(options_t options) {
    if (options.options != NULL) {
        free(options.options);
        options.options = NULL;
    }
}
char *option_current(options_t options) { return options.options[options.current]; }

appdata appdata_alloc() {
    appdata data = {0};
    data.solve_time = malloc(BUF_SIZE * sizeof(char));
    data.variables = malloc(BUF_SIZE * sizeof(char));
    data.clauses = malloc(BUF_SIZE * sizeof(char));

    assert(data.solve_time && "Can't malloc data->solve_time");
    assert(data.variables && "Can't malloc data->variables");
    assert(data.clauses && "Can't malloc data->clauses");

    return data;
}
void update_app_data(appdata *appdata, sodoku_t *s) {
    sprintf(appdata->solve_time, "Time: %3.2f", s->solver->time);
    sprintf(appdata->variables, "Vars: %zu", s->solver->vars);
    sprintf(appdata->clauses, "Clauses: %zu", s->solver->clauses);
}