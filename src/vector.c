#include <cstl/vector.h>
#include <stdlib.h>
#include <string.h>

#define INITIAL_CAPACITY 4U

static void *element_address(const vector *vector, const size_t index) {
    unsigned char *bytes = vector->data;
    return bytes + index * vector->element_size;
}

static const void *element_address_const(const vector *vector, const size_t index) {
    const unsigned char *bytes = vector->data;
    return bytes + index * vector->element_size;
}

static bool multiplication_overflows(const size_t left, const size_t right) {
    return right > 0 && left > SIZE_MAX / right;
}

vector_status vector_init(vector *vector, const size_t element_size) {
    if (vector == NULL || element_size == 0) { return VECTOR_NULL_ARGUMENT; }

    vector->data = NULL;
    vector->size = 0;
    vector->capacity = 0;
    vector->element_size = element_size;

    return VECTOR_OK;
}

void vector_destroy(vector *vector) {
    if (vector == NULL) { return; }

    free(vector->data);

    vector->data = NULL;
    vector->size = 0;
    vector->capacity = 0;
    vector->element_size = 0;
}

size_t vector_size(const vector *vector) { return vector != NULL ? vector->size : 0; }

size_t vector_capacity(const vector *vector) { return vector != NULL ? vector->capacity : 0; }

void *vector_data(const vector *vector) { return vector != NULL ? vector->data : NULL; }

bool vector_empty(const vector *vector) { return vector == NULL || vector->size == 0; }

void *vector_at(const vector *vector, const size_t index) {
    if (vector == NULL || index >= vector->size) { return NULL; }

    return element_address(vector, index);
}

const void *vector_at_const(const vector *vector, const size_t index) {
    if (vector == NULL || index >= vector->size) { return NULL; }

    return element_address_const(vector, index);
}

void *vector_front(const vector *vector) {
    if (vector == NULL || vector->size == 0) { return NULL; }

    return element_address(vector, 0);
}

void *vector_back(const vector *vector) {
    if (vector == NULL || vector->size == 0) { return NULL; }

    return element_address(vector, vector->size - 1);
}

vector_status vector_reserve(vector *vector, const size_t capacity) {
    if (vector == NULL || vector->element_size == 0) { return VECTOR_NULL_ARGUMENT; }

    if (capacity <= vector->capacity) { return VECTOR_OK; }

    if (multiplication_overflows(capacity, vector->element_size)) { return VECTOR_OVERFLOW; }

    const size_t bytes = capacity * vector->element_size;
    void *data = realloc(vector->data, bytes);

    if (data == NULL) { return VECTOR_OUT_OF_MEMORY; }

    vector->data = data;
    vector->capacity = capacity;

    return VECTOR_OK;
}

vector_status vector_push_back(vector *vector, const void *element) {
    if (vector == NULL || element == NULL || vector->element_size == 0) { return VECTOR_NULL_ARGUMENT; }

    if (vector->size == vector->capacity) {
        size_t capacity;

        if (vector->capacity == 0) {
            capacity = INITIAL_CAPACITY;
        } else {
            if (vector->capacity > SIZE_MAX / 2) { return VECTOR_OVERFLOW; }
            capacity = vector->capacity * 2;
        }

        const vector_status status = vector_reserve(vector, capacity);

        if (status != VECTOR_OK) { return status; }
    }

    memcpy(element_address(vector, vector->size), element, vector->element_size);

    vector->size++;

    return VECTOR_OK;
}

vector_status vector_pop_back(vector *vector) {
    if (vector == NULL) { return VECTOR_NULL_ARGUMENT; }

    if (vector->size == 0) { return VECTOR_OUT_OF_RANGE; }

    vector->size--;

    return VECTOR_OK;
}

void vector_clear(vector *vector) {
    if (vector != NULL) { vector->size = 0; }
}

vector_status vector_insert(vector *vector, const size_t index, const void *element) {
    if (vector == NULL || element == NULL || vector->element_size == 0) { return VECTOR_NULL_ARGUMENT; }

    if (index >= vector->size) { return VECTOR_OUT_OF_RANGE; }

    if (vector->size == SIZE_MAX) { return VECTOR_OVERFLOW; }

    void *copy = malloc(vector->element_size);

    if (copy == NULL) { return VECTOR_OUT_OF_MEMORY; }

    memcpy(copy, element, vector->element_size);

    if (vector->size == vector->capacity) {
        size_t capacity;

        if (vector->capacity == 0) {
            capacity = INITIAL_CAPACITY;
        } else {
            if (vector->capacity > SIZE_MAX / 2) {
                free(copy);
                return VECTOR_OVERFLOW;
            }

            capacity = vector->capacity * 2;
        }

        const vector_status status = vector_reserve(vector, capacity);

        if (status != VECTOR_OK) {
            free(copy);
            return status;
        }
    }

    unsigned char *data = vector->data;

    memmove(
        data + (index + 1) * vector->element_size,
        data + index * vector->element_size,
        (vector->size - index) * vector->element_size
    );

    memcpy(
        data + index * vector->element_size,
        copy,
        vector->element_size
    );

    vector->size++;

    free(copy);
    return VECTOR_OK;
}

vector_status vector_erase(vector *vector, const size_t first, const size_t last) {
    if (vector == NULL || vector->element_size == 0) { return VECTOR_NULL_ARGUMENT; }

    if (last < first || last > vector->size) { return VECTOR_OUT_OF_RANGE; }

    if (first == last) { return VECTOR_OK; }

    const size_t remove_count = last - first;
    const size_t tail_count = vector->size - last;

    unsigned char *data = vector->data;

    memmove(
        data + first * vector->element_size,
        data + last * vector->element_size,
        tail_count * vector->element_size
    );

    vector->size -= remove_count;

    return VECTOR_OK;
}
