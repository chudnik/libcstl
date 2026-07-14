#include "cstl/vector.h"

#include <assert.h>
#include <stdint.h>
#include <stdio.h>

static void test_init(void) {
    vector vector;

    assert(vector_init(&vector, sizeof(int)) == VECTOR_OK);
    assert(vector.data == NULL);
    assert(vector.size == 0);
    assert(vector.capacity == 0);
    assert(vector.element_size == sizeof(int));
    assert(vector_empty(&vector));

    vector_destroy(&vector);
}

static void test_invalid_init(void) {
    vector vector;

    assert(vector_init(NULL, sizeof(int)) == VECTOR_NULL_ARGUMENT);

    assert(vector_init(&vector, 0) == VECTOR_NULL_ARGUMENT);
}

static void test_push_back(void) {
    vector vector;
    assert(vector_init(&vector, sizeof(int)) == VECTOR_OK);

    const int value = 42;

    assert(vector_push_back(&vector, &value) == VECTOR_OK);
    assert(vector_size(&vector) == 1);
    assert(vector_capacity(&vector) >= 1);
    assert(!vector_empty(&vector));

    const int *stored = vector_at_const(&vector, 0);

    assert(stored != NULL);
    assert(*stored == 42);

    vector_destroy(&vector);
}

static void test_capacity_growth(void) {
    vector vector;
    assert(vector_init(&vector, sizeof(int)) == VECTOR_OK);

    for (int i = 0; i < 100; ++i) {
        assert(vector_push_back(&vector, &i) == VECTOR_OK);
    }

    assert(vector_size(&vector) == 100);
    assert(vector_capacity(&vector) >= 100);

    for (size_t i = 0; i < 100; ++i) {
        const int *value = vector_at_const(&vector, i);

        assert(value != NULL);
        assert(*value == (int)i);
    }

    vector_destroy(&vector);
}

static void test_at(void) {
    vector vector;
    assert(vector_init(&vector, sizeof(int)) == VECTOR_OK);

    constexpr int first = 10;
    constexpr int second = 20;

    assert(vector_push_back(&vector, &first) == VECTOR_OK);
    assert(vector_push_back(&vector, &second) == VECTOR_OK);

    int *element = vector_at(&vector, 1);

    assert(element != NULL);
    assert(*element == 20);

    *element = 30;

    assert(*(const int *)vector_at_const(&vector, 1) == 30);

    assert(vector_at(&vector, 2) == NULL);
    assert(vector_at_const(&vector, 100) == NULL);
    assert(vector_at(NULL, 0) == NULL);

    vector_destroy(&vector);
}

static void test_front_and_back(void) {
    vector vector;
    assert(vector_init(&vector, sizeof(int)) == VECTOR_OK);

    assert(vector_front(&vector) == NULL);
    assert(vector_back(&vector) == NULL);

    int values[] = {10, 20, 30};

    for (size_t i = 0; i < 3; ++i) {
        assert(vector_push_back(&vector, &values[i]) == VECTOR_OK);
    }

    assert(*(int *)vector_front(&vector) == 10);
    assert(*(int *)vector_back(&vector) == 30);

    vector_destroy(&vector);
}

static void test_reserve(void) {
    vector vector;
    assert(vector_init(&vector, sizeof(int)) == VECTOR_OK);

    assert(vector_reserve(&vector, 32) == VECTOR_OK);
    assert(vector_capacity(&vector) == 32);
    assert(vector_size(&vector) == 0);

    int value = 123;
    assert(vector_push_back(&vector, &value) == VECTOR_OK);

    assert(vector_reserve(&vector, 16) == VECTOR_OK);
    assert(vector_capacity(&vector) == 32);
    assert(*(int *)vector_at(&vector, 0) == 123);

    assert(vector_reserve(&vector, 64) == VECTOR_OK);
    assert(vector_capacity(&vector) == 64);
    assert(*(int *)vector_at(&vector, 0) == 123);

    vector_destroy(&vector);
}

static void test_reserve_overflow(void) {
    vector vector;
    assert(vector_init(&vector, sizeof(int)) == VECTOR_OK);

    assert(vector_reserve(&vector, SIZE_MAX) == VECTOR_OVERFLOW);

    assert(vector.data == NULL);
    assert(vector.size == 0);
    assert(vector.capacity == 0);

    vector_destroy(&vector);
}

static void test_pop_back(void) {
    vector vector;
    assert(vector_init(&vector, sizeof(int)) == VECTOR_OK);

    assert(vector_pop_back(&vector) == VECTOR_OUT_OF_RANGE);

    int first = 5;
    int second = 10;

    assert(vector_push_back(&vector, &first) == VECTOR_OK);
    assert(vector_push_back(&vector, &second) == VECTOR_OK);

    size_t cap = vector_capacity(&vector);

    assert(vector_pop_back(&vector) == VECTOR_OK);
    assert(vector_size(&vector) == 1);
    assert(vector_capacity(&vector) == cap);
    assert(*(int *)vector_back(&vector) == 5);

    assert(vector_pop_back(&vector) == VECTOR_OK);
    assert(vector_empty(&vector));

    vector_destroy(&vector);
}

static void test_clear(void) {
    vector vector;
    assert(vector_init(&vector, sizeof(int)) == VECTOR_OK);

    for (int i = 0; i < 10; ++i) {
        assert(vector_push_back(&vector, &i) == VECTOR_OK);
    }

    size_t cap = vector_capacity(&vector);

    vector_clear(&vector);

    assert(vector_size(&vector) == 0);
    assert(vector_capacity(&vector) == cap);
    assert(vector_empty(&vector));

    vector_clear(NULL);
    vector_destroy(&vector);
}

static void test_struct_elements(void) {
    typedef struct {
        int id;
        double value;
    } item;

    vector vector;
    assert(vector_init(&vector, sizeof(item)) == VECTOR_OK);

    item first = {.id = 1, .value = 3.14};
    item second = {.id = 2, .value = 2.71};

    assert(vector_push_back(&vector, &first) == VECTOR_OK);
    assert(vector_push_back(&vector, &second) == VECTOR_OK);

    const item *stored_first = vector_at_const(&vector, 0);
    const item *stored_second = vector_at_const(&vector, 1);

    assert(stored_first->id == 1);
    assert(stored_first->value == 3.14);
    assert(stored_second->id == 2);
    assert(stored_second->value == 2.71);

    vector_destroy(&vector);
}

static void test_null_arguments(void) {
    int value = 10;

    assert(vector_push_back(NULL, &value) == VECTOR_NULL_ARGUMENT);

    assert(vector_reserve(NULL, 10) == VECTOR_NULL_ARGUMENT);

    assert(vector_pop_back(NULL) == VECTOR_NULL_ARGUMENT);

    assert(vector_size(NULL) == 0);
    assert(vector_capacity(NULL) == 0);
    assert(vector_empty(NULL));

    vector_destroy(NULL);
}

static void test_destroy(void) {
    vector vector;
    assert(vector_init(&vector, sizeof(int)) == VECTOR_OK);

    int value = 10;
    assert(vector_push_back(&vector, &value) == VECTOR_OK);

    vector_destroy(&vector);

    assert(vector.data == NULL);
    assert(vector.size == 0);
    assert(vector.capacity == 0);
    assert(vector.element_size == 0);

    vector_destroy(&vector);
}

int main(void) {
    test_init();
    test_invalid_init();
    test_push_back();
    test_capacity_growth();
    test_at();
    test_front_and_back();
    test_reserve();
    test_reserve_overflow();
    test_pop_back();
    test_clear();
    test_struct_elements();
    test_null_arguments();
    test_destroy();

    printf("All vector tests passed\n");
    return 0;
}
