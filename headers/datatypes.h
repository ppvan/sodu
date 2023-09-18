#ifndef _DATA_TYPES_H_
#define _DATA_TYPES_H_

#include <stdarg.h>
#include <stddef.h>

typedef struct {
    int current;
    int size;
    char **options;
} options_t;

#define VEC_DEFAULT_SIZE 16

typedef struct {
    int size;
    int capacity;
    int *data;
} vec_t;

typedef size_t counter_t;

options_t options_new(int count, ...);
void options_free(options_t options);
char *option_current(options_t options);

vec_t *vec_new();
vec_t *vec_reserve(vec_t *vec, int size);
vec_t *vec_append(vec_t *vec, int data);
void vec_free(vec_t *vec);
void vec_print(vec_t *vec);

#endif