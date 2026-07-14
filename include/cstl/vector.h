#ifndef LIBCSTL_VECTOR_H
#define LIBCSTL_VECTOR_H

#include <stddef.h>

typedef enum {
    VECTOR_OK = 0,
    VECTOR_NULL_ARGUMENT,
    VECTOR_OUT_OF_MEMORY,
    VECTOR_OUT_OF_RANGE,
    VECTOR_OVERFLOW
} vector_status;

typedef struct {
    void *data;
    size_t size;
    size_t capacity;
    size_t element_size;
} vector;

vector_status vector_init(vector *vector, size_t element_size);

void vector_destroy(vector *vector);

size_t vector_size(const vector *vector);

size_t vector_capacity(const vector *vector);

bool vector_empty(const vector *vector);

void *vector_at(const vector *vector, size_t index);

const void *vector_at_const(const vector *vector, size_t index);

void *vector_front(const vector *vector);

void *vector_back(const vector *vector);

vector_status vector_reserve(vector *vector, size_t capacity);

vector_status vector_push_back(vector *vector, const void *element);

vector_status vector_pop_back(vector *vector);

void vector_clear(vector *vector);

vector_status vector_insert(vector *vector, size_t index, const void *element);

vector_status vector_erase(vector *vector, size_t first, size_t last);

#endif //LIBCSTL_VECTOR_H
