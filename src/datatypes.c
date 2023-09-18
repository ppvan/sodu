#include "datatypes.h"
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

vec_t *vec_new() {
    vec_t *v = malloc(sizeof(vec_t));
    memset(v, 0, sizeof(vec_t));
    v->data = malloc(VEC_DEFAULT_SIZE * sizeof(int));
    v->capacity = VEC_DEFAULT_SIZE;
    v->size = 0;
    return v;
}
vec_t *vec_reserve(vec_t *vec, int size) {
    vec->capacity = size;
    vec->size = size;
    vec->data = realloc(vec->data, vec->capacity * sizeof(int));

    return vec;
}

vec_t *vec_append(vec_t *vec, int data) {
    if (vec->size >= vec->capacity) {
        vec->capacity *= 2;

        vec->data = realloc(vec->data, vec->capacity * sizeof(int));
        assert(vec->data);
    }

    vec->data[vec->size++] = data;

    return vec;
}

void vec_free(vec_t *vec) {
    free(vec->data);
    free(vec);
}

void vec_print(vec_t *vec) {
    for (int i = 0; i < vec->size; i++) {
        printf("%d ", vec->data[i]);
    }

    printf("\n");
}