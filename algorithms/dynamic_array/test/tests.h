#define DYN_MAX_CAPACITY 64

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../src/dynamic_array.c"

/*
    dynamic_array_t *dynamic_array_initialize(size_t capacity, size_t data_type_size, void (*destruct_func)(void *));
        1. NORMAL, capacity 0, assert 16
        2. NORMAL, capacity 15, assert 16
        3. NORMAL, capacity 16, assert 16
        4. NORMAL, capacity 17, assert 32
        5. NORMAL, capacity DYN_MAX_CAPACITY - 1, assert DYN_MAX_CAPACITY
        6. NORMAL, capacity DYN_MAX_CAPACITY, assert DYN_MAX_CAPACITY
        7. NORMAL, NULL function pointer
        8. FAIL, capacity > DYN_MAX_CAPACITY
        9. FAIL, data_size == 0


    void dynamic_array_destroy(dynamic_array_t *const dyn_array);
        1. NORMAL, empty
        2. NORMAL, with contents, NULL destructor
        3. NORMAL, with contents, using destructor
        4. FAIL, NULL pointer


    void *dynamic_array_front(const dynamic_array_t *const dyn_array);
        1. NORMAL, has contents
        2. FAIL, empty.
        3. FAIL, NULL pointer

    bool dynamic_array_push_front(dynamic_array_t *const dyn_array, void *object);
        1. NORMAL, has contents
        2. NORMAL, empty
        3. NORMAL, at capacity boundary
        4. FAIL, NULL array
        5. FAIL, NULL object
        6. FAIL, at max capacity

    void dynamic_array_pop_front(dynamic_array_t *const dyn_array);
        1. NORMAL, size = 1
        2. NORMAL, size > 1
        3. NORMAL(?), size = 0
        4. NORMAL, with destructor
        5. FAIL, NULL pointer

    bool dynamic_array_extract_front(dynamic_array_t *const dyn_array, void *object);
        1. NORMAL, size = 1
        2. Normal, size > 1
        3. FAIL, size = 0
        4. FAIL, null array
        5. FAIL, null object


    void *dynamic_array_back(dynamic_array_t *const dyn_array);
        1. NORMAL, has contents
        2. FAIL, empty.
        3. FAIL, NULL pointer
        4. NORMAL, assert front == back on size = 1


    bool dynamic_array_push_back(dynamic_array_t *const dyn_array, void *object);
        1. NORMAL, has contents
        2. NORMAL, empty
        3. NORMAL, at capacity boundary
        4. FAIL, NULL array
        5. FAIL, NULL object
        6. FAIL, at max capacity


    void dynamic_array_pop_back(dynamic_array_t *const dyn_array);
        1. NORMAL, size = 1
        2. NORMAL, size > 1
        3. NORMAL(?), size = 0
        4. NORMAL, with destructor
        5. FAIL, NULL pointer


    bool dynamic_array_extract_back(dynamic_array_t *const dyn_array, void *object);
        1. NORMAL, size = 1
        2. Normal, size > 1
        3. FAIL, size = 0
        4. FAIL, null array
        5. FAIL, null object


    void *dynamic_array_at(const dynamic_array_t *const dyn_array, size_t index);
        1. NORMAL, front
        2. NORMAL, back
        3. NORMAl, arbitrary
        4. FAIL, idx = size
        5. FAIL, idx > size
        6. FAIL, empty, idx = 0
        7. FAIL, empty, idx = 1
        8. FAIl, null pointer


    bool dynamic_array_insert(dynamic_array_t *const dyn_array, size_t index, void *object);
        1. NORMAL, front
        2. NORMAL, back
        3. NORMAL, arbitrary
        4. NORMAL, empty, idx = 0
        5. FAIL, empty, idx = 1 (just to make sure)
        6. FAIL, out of bounds
        7. FAIL, null object
        8. FAIL, null array


    void dynamic_array_erase(dynamic_array_t *const dyn_array, size_t index);
        1. NORMAL, front
        2. NORMAL, back
        3. NORMAL, arbitrary
        4. NORMAL, with destructor
        5. FAIL, empty, idx = 0
        6. FAIL, empty, idx = 1
        7. FAIL, idx = size
        8. FAIL, idx > size
        9. FAIL, null ptr


    bool dynamic_array_extract(dynamic_array_t *const dyn_array, size_t index, void *object);
        1. NORMAL, front
        2. NORMAL, back
        3. NORMAL, arbitrary (assert not destructed)
        4. NORMAL, size = 1
        5. FAIL, size = 0, idx = 0
        6. FAIL, size = 0, idx = 1
        7. FAIL, null array
        8. FAIL, null object


    void dynamic_array_clear(dynamic_array_t *const dyn_array);
        1. NORMAL, contents
        2. NORMAL, contents with destructor
        3. NORMAL, empty
        4. FAIL, null ptr

    bool dynamic_array_empty(dynamic_array_t *const dyn_array);
        1. NORMAL, contents
        2. NORMAL, empty
        3. FAIL, null ptr

    size_t dynamic_array_size(const dynamic_array_t *const dyn_array);
        1. NORMAL, contents
        2. NORMAL, empty
        3. FAIL, null ptr
*/

// Shamelessly stolen from
// http://pixelscommander.com/wp-content/uploads/2014/12/P10.pdf
// modded a bit so it dies when false
// Kinda funny it gets mad about abort() being void

bool die(){
    // so we can break on die and still have a useable stack to debug on
    // and we still have a core dump if it wasn't being debugged
    abort();
    return false;
}

#define assert(e) ((e) ? (true) : \
                   (printf("%s,%d: assertion '%s' failed\n",__FILE__, __LINE__, #e), \
                    fflush(stdout), die()))

uint8_t DATA_BLOCKS[6][100];
int destruct_counter = 0;

void block_destructor(void *block) {
    memset(block, 0xFF, 100);
    // right, probably can't check if the destructor ran if the memory gets freed
    ++destruct_counter;
}

void init_data_blocks() {
    memset(DATA_BLOCKS[0], 0x11, 100);
    memset(DATA_BLOCKS[1], 0x22, 100);
    memset(DATA_BLOCKS[2], 0x33, 100);
    memset(DATA_BLOCKS[3], 0x44, 100);
    memset(DATA_BLOCKS[4], 0x55, 100);
    memset(DATA_BLOCKS[5], 0xFF, 100);
}

// CONSTRUCTOR, DESTRUCTOR, AT, FRONT, PUSH_FRONT, BACK, SIZE, CLEAR, EMPTY
void run_basic_tests_a();

// PUSH_BACK, POP_FRONT, POP_BACK, EXTRACT_FRONT, EXTRACT_BACK
void run_basic_tests_b();

void run_tests() {
    init_data_blocks();

    // CONSTRUCTOR, DESTRUCTOR, AT, FRONT, PUSH_FRONT, BACK, SIZE, CLEAR, EMPTY
    run_basic_tests_a();

    // PUSH_BACK, POP_FRONT, POP_BACK, EXTRACT_FRONT, EXTRACT_BACK
    run_basic_tests_b();

    puts("TESTS COMPLETE");
}

void run_basic_tests_a() {
    dynamic_array_t *dyn_a = NULL, *dyn_b = NULL;

    // CONSTRUCTOR / DESTRUCTOR
    // 1 & 7 CONSTRUCTOR
    dyn_a = dynamic_array_initialize(0, 4, NULL);
    assert(dyn_a);
    assert(dyn_a->size == 0);
    assert(dyn_a->capacity == 16);
    assert(dyn_a->data_size == 4);
    assert(dyn_a->destructor == NULL);
    assert(dyn_a->array);

    // 1 & 4 DESTRUCTOR
    dynamic_array_destroy(dyn_a);
    dynamic_array_destroy(NULL);

    // 2 CONSTRUCTOR
    dyn_a = dynamic_array_initialize(15, 5, &block_destructor);
    assert(dyn_a);
    assert(dyn_a->size == 0);
    assert(dyn_a->capacity == 16);
    assert(dyn_a->data_size == 5);
    assert(dyn_a->destructor == &block_destructor);
    assert(dyn_a->array);

    dynamic_array_destroy(dyn_a);

    // 3 CONSTRUCTOR
    dyn_a = dynamic_array_initialize(16, 8, &block_destructor);
    assert(dyn_a);
    assert(dyn_a->size == 0);
    assert(dyn_a->capacity == 16);
    assert(dyn_a->data_size == 8);
    assert(dyn_a->destructor == &block_destructor);
    assert(dyn_a->array);

    dynamic_array_destroy(dyn_a);

    // 4 CONSTRUCTOR
    dyn_a = dynamic_array_initialize(17, 4, &block_destructor);
    assert(dyn_a);
    assert(dyn_a->size == 0);
    assert(dyn_a->capacity == 32);
    assert(dyn_a->data_size == 4);
    assert(dyn_a->destructor == &block_destructor);
    assert(dyn_a->array);

    dynamic_array_destroy(dyn_a);

    // 5 CONSTRUCTOR
    dyn_a = dynamic_array_initialize(DYN_MAX_CAPACITY - 1, 4, &block_destructor);

    assert(dyn_a);
    assert(dyn_a->size == 0);
    assert(dyn_a->capacity == DYN_MAX_CAPACITY);
    assert(dyn_a->data_size == 4);
    assert(dyn_a->destructor == &block_destructor);
    assert(dyn_a->array);

    dynamic_array_destroy(dyn_a);

    // 6 CONSTRUCTOR
    dyn_a = dynamic_array_initialize(DYN_MAX_CAPACITY, 4, &block_destructor);

    assert(dyn_a);
    assert(dyn_a->size == 0);
    assert(dyn_a->capacity == DYN_MAX_CAPACITY);
    assert(dyn_a->data_size == 4);
    assert(dyn_a->destructor == &block_destructor);
    assert(dyn_a->array);

    dynamic_array_destroy(dyn_a);

    // 8 CONSTRUCTOR
    dyn_a = dynamic_array_initialize(DYN_MAX_CAPACITY + 1, 4, &block_destructor);
    assert(!dyn_a);

    // 9 CONSTRUCTOR
    dyn_a = dynamic_array_initialize(16, 0, &block_destructor);
    assert(!dyn_a);

    // CONSTRUCTOR TESTS COMPLETE


    // PUSH_FRONT, FRONT, SIZE, CLEAR, DESTRUCTOR
    // so basic operations can be used

    dyn_a = dynamic_array_initialize(0, sizeof(DATA_BLOCKS[0]), NULL);
    assert(dyn_a);

    // 2 & 3 FRONT
    assert(dynamic_array_front(dyn_a) == NULL);
    assert(dynamic_array_front(NULL) == NULL);
    // 2 SIZE
    assert(dynamic_array_size(dyn_a) == 0);
    // 3 SIZE
    assert(dynamic_array_size(NULL) == 0);
    // 2 EMPTY
    assert(dynamic_array_empty(dyn_a));
    // 3 EMPTY
    assert(dynamic_array_empty(NULL));

    // 2 PUSH_FRONT
    assert(dynamic_array_push_front(dyn_a, DATA_BLOCKS[0]));
    assert(dyn_a->size == 1);
    assert(memcmp(dyn_a->array, DATA_BLOCKS[0], sizeof(DATA_BLOCKS[0])) == 0);

    // 1 EMPTY
    assert(dynamic_array_empty(dyn_a) == false);
    // 1 SIZE
    assert(dynamic_array_size(dyn_a) == 1);

    // SIZE and EMPTY TESTS COMPLETE

    // 1 FRONT
    assert(dynamic_array_front(dyn_a) == dyn_a->array);
    assert(memcmp(dynamic_array_front(dyn_a), DATA_BLOCKS[0], sizeof(DATA_BLOCKS[0])) == 0);

    // FRONT TESTS COMPLETE

    // 1 PUSH_FRONT
    assert(dynamic_array_push_front(dyn_a, DATA_BLOCKS[1]));
    assert(dynamic_array_size(dyn_a) == 2);
    assert(dynamic_array_front(dyn_a) == dyn_a->array);
    assert(memcmp(dyn_a->array, DATA_BLOCKS[1], sizeof(DATA_BLOCKS[1])) == 0);

    // 4 & 5 PUSH_FRONT
    assert(dynamic_array_push_front(NULL, DATA_BLOCKS[1]) == false);
    assert(dynamic_array_push_front(dyn_a, NULL) == false);
    assert(dyn_a->size == 2);

    // 1 CLEAR
    dynamic_array_clear(dyn_a);
    assert(dyn_a->size == 0);
    // 3 CLEAR
    dynamic_array_clear(dyn_a);
    assert(dyn_a->size == 0);
    // 4 CLEAR
    dynamic_array_clear(NULL);


    // 6 AT
    assert(dynamic_array_at(dyn_a, 0) == NULL);
    // 7 AT
    assert(dynamic_array_at(dyn_a, 1) == NULL);

    dyn_b = dynamic_array_initialize(0, sizeof(DATA_BLOCKS[0]), &block_destructor);
    assert(dyn_b);
    // 2 BACK
    assert(dynamic_array_back(dyn_b) == NULL);
    // 3 BACK
    assert(dynamic_array_back(NULL) == NULL);
    assert(dynamic_array_push_front(dyn_b, DATA_BLOCKS[0]));
    // 4 BACK
    assert(dynamic_array_front(dyn_b) == dynamic_array_back(dyn_b));
    assert(dynamic_array_push_front(dyn_b, DATA_BLOCKS[1]));

    // 2 CLEAR
    dynamic_array_clear(dyn_b);
    assert(destruct_counter == 2);
    destruct_counter = 0;

    // CLEAR TESTS COMPLETE

    assert(dynamic_array_push_front(dyn_b, DATA_BLOCKS[0]));
    assert(dynamic_array_push_front(dyn_b, DATA_BLOCKS[1]));
    assert(dynamic_array_push_front(dyn_a, DATA_BLOCKS[0]));
    assert(dynamic_array_push_front(dyn_a, DATA_BLOCKS[1]));
    assert(dynamic_array_push_front(dyn_a, DATA_BLOCKS[2]));

    // 1 AT
    assert(dynamic_array_at(dyn_a, 0) == dynamic_array_front(dyn_a));
    assert(memcmp(dynamic_array_at(dyn_a, 0), DATA_BLOCKS[2], sizeof(DATA_BLOCKS[0])) == 0);
    // 2 AT
    // 1 BACK
    assert(dynamic_array_at(dyn_a, 2) == dynamic_array_back(dyn_a));
    assert(memcmp(dynamic_array_at(dyn_a, 2), DATA_BLOCKS[0], sizeof(DATA_BLOCKS[0])) == 0);

    // BACK TESTS COMPLETE

    // 3 AT
    assert(dynamic_array_at(dyn_a, 1));
    assert(memcmp(dynamic_array_at(dyn_a, 1), DATA_BLOCKS[1], sizeof(DATA_BLOCKS[0])) == 0);

    // 4 AT
    assert(dynamic_array_at(dyn_a, 3) == NULL);
    // 5 AT
    assert(dynamic_array_at(dyn_a, 4) == NULL);
    // 8 AT
    assert(dynamic_array_at(NULL, 0) == NULL);

    // AT TESTS COMPLETE

    // 2 DESTRUCTOR
    dynamic_array_destroy(dyn_a);

    // 3 DESTRUCTOR
    dynamic_array_destroy(dyn_b);
    assert(destruct_counter == 2);
    destruct_counter = 0;

    // DESTRUCTOR TESTS COMPLETE

    dyn_a = dynamic_array_initialize(0, sizeof(DATA_BLOCKS[0]), NULL);
    assert(dyn_a);
    while (dyn_a->size != 16) {
        assert(dynamic_array_push_front(dyn_a, DATA_BLOCKS[0]));
    }
    // 3 PUSH_FRONT
    assert(dyn_a->size == 16);
    assert(dyn_a->capacity == 16);
    assert(dynamic_array_push_front(dyn_a, DATA_BLOCKS[1]));
    assert(memcmp(dynamic_array_front(dyn_a), DATA_BLOCKS[1], sizeof(DATA_BLOCKS[0])) == 0);
    assert(dyn_a->size == 17);
    assert(dyn_a->capacity == 32);

    while (dyn_a->size != DYN_MAX_CAPACITY) {
        assert(dynamic_array_push_front(dyn_a, DATA_BLOCKS[0]));
    }
    // 6 PUSH_FRONT
    assert(dyn_a->size == DYN_MAX_CAPACITY);
    assert(dyn_a->capacity == DYN_MAX_CAPACITY);
    assert(dynamic_array_push_front(dyn_a, DATA_BLOCKS[1]) == false);
    assert(dyn_a->size == DYN_MAX_CAPACITY);
    assert(dyn_a->capacity == DYN_MAX_CAPACITY);
    assert(memcmp(dynamic_array_front(dyn_a), DATA_BLOCKS[0], sizeof(DATA_BLOCKS[0])) == 0);

    // PUSH_FRONT TESTS COMPLETE

    dynamic_array_destroy(dyn_a);
    dynamic_array_destroy(dyn_b);
}

void run_basic_tests_b() {
    destruct_counter = 0;
    dynamic_array_t *dyn_a, *dyn_b;

    dyn_a = dynamic_array_initialize(0, sizeof(DATA_BLOCKS[0]), NULL);
    assert(dyn_a);
    dyn_b = dynamic_array_initialize(0, sizeof(DATA_BLOCKS[0]), &block_destructor);
    assert(dyn_b);

    // 2 POP_FRONT
    assert(dynamic_array_push_front(dyn_a, DATA_BLOCKS[0]));
    assert(dynamic_array_push_front(dyn_a, DATA_BLOCKS[1]));
    assert(dyn_a->size == 2);

    dynamic_array_pop_front(dyn_a);
    assert(dyn_a->size == 1);
    assert(memcmp(dyn_a->array, DATA_BLOCKS[0], sizeof(DATA_BLOCKS[0])) == 0);

    // 1 POP_FRONT
    dynamic_array_pop_front(dyn_a);
    assert(dyn_a->size == 0);

    // 3 POP_FRONT
    dynamic_array_pop_front(dyn_a);
    assert(dyn_a->size == 0);

    // 5 POP_FRONT
    dynamic_array_pop_front(NULL);

    // 4 POP_FRONT
    assert(dynamic_array_push_front(dyn_b, DATA_BLOCKS[0]));
    dynamic_array_pop_front(dyn_b);
    assert(dyn_b->size == 0);
    assert(destruct_counter == 1);
    destruct_counter = 0;

    // POP_FRONT TESTS COMPLETE


}