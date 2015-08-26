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

    bool dynamic_array_pop_front(dynamic_array_t *const dyn_array);
        1. NORMAL, size = 1
        2. NORMAL, size > 1
        3. FAIL, size = 0
        4. NORMAL, with destructor
        5. FAIL, NULL pointer

    bool dynamic_array_extract_front(dynamic_array_t *const dyn_array, void *object);
        1. NORMAL, size = 1
        2. NORMAL, size > 1
        3. NORMAL, with destructor, assert not destructed
        4. FAIL, size = 0
        5. FAIL, null array
        6. FAIL, null object


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


    bool dynamic_array_pop_back(dynamic_array_t *const dyn_array);
        1. NORMAL, size = 1
        2. NORMAL, size > 1
        3. NORMAL(?), size = 0
        4. NORMAL, with destructor
        5. FAIL, NULL pointer


    bool dynamic_array_extract_back(dynamic_array_t *const dyn_array, void *object);
        1. NORMAL, size = 1
        2. NORMAL, size > 1
        3. NORMAL, with destructor, assert not destructed
        4. FAIL, size = 0
        5. FAIL, null array
        6. FAIL, null object


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
        4. NORMAL (FAIL?), idx = size
        5. FAIL, empty, idx = 1 (just to make sure)
        6. FAIL, out of bounds
        7. FAIL, null object
        8. FAIL, null array


    bool dynamic_array_erase(dynamic_array_t *const dyn_array, size_t index);
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
        3. NORMAL, arbitrary
        4. NORMAL, with destructor, assert not destructed
        5. FAIL, empty, idx = 0
        6. FAIL, empty, idx = 1
        7. FAIL, idx = size
        8. FAIL, idx > size
        9. FAIL, null ptr
        10.FAIL, null array


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

    const void *dynamic_array_export(const dynamic_array_t *const dyn_array);
        SEE FRONT

    dynamic_array_t *dynamic_array_import(const void *const data, const size_t count, const size_t data_type_size, void (*destruct_func)(void *));
        SEE INIT (capacity -> size)
        10. FAIL, void data
*/

// Shamelessly stolen from
// http://pixelscommander.com/wp-content/uploads/2014/12/P10.pdf
// modded a bit so it dies when false
// Kinda funny it gets mad about abort() being void

bool die() {
    // so we can break on die and still have a useable stack to debug on
    // and we still have a core dump if it wasn't being debugged
    abort();
    return false;
}

#define assert(e) ((e) ? (true) : \
                   (printf("%s,%d: assertion '%s' failed\n",__FILE__, __LINE__, #e), \
                    fflush(stdout), die()))

#define DATA_BLOCK_SIZE 100
uint8_t DATA_BLOCKS[6][DATA_BLOCK_SIZE];
int destruct_counter = 0;

void block_destructor(void *block) {
    memset(block, 0xFF, 100);
    // right, probably can't check if the destructor ran if the memory gets freed
    ++destruct_counter;
}

void block_destructor_mini(void *block) {
    memset(block, 0xAA, 1);
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

// INSERT, EXTRACT, ERASE
void run_basic_tests_c();

// EXTRACT, IMPORT, bulk moving...?
void run_basic_tests_d();

void run_tests() {
    init_data_blocks();

    // CONSTRUCTOR, DESTRUCTOR, AT, FRONT, PUSH_FRONT, BACK, SIZE, CLEAR, EMPTY
    run_basic_tests_a();

    // PUSH_BACK, POP_FRONT, POP_BACK, EXTRACT_FRONT, EXTRACT_BACK
    run_basic_tests_b();

    // INSERT, EXTRACT, ERASE
    run_basic_tests_c();

    // EXTRACT, IMPORT, bulk moving...?
    run_basic_tests_d();

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

    dyn_a = dynamic_array_initialize(0, DATA_BLOCK_SIZE, NULL);
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
    assert(memcmp(dyn_a->array, DATA_BLOCKS[0], DATA_BLOCK_SIZE) == 0);

    // 1 EMPTY
    assert(dynamic_array_empty(dyn_a) == false);
    // 1 SIZE
    assert(dynamic_array_size(dyn_a) == 1);

    // SIZE and EMPTY TESTS COMPLETE

    // 1 FRONT
    assert(dynamic_array_front(dyn_a) == dyn_a->array);
    assert(memcmp(dynamic_array_front(dyn_a), DATA_BLOCKS[0], DATA_BLOCK_SIZE) == 0);

    // FRONT TESTS COMPLETE

    // 1 PUSH_FRONT
    assert(dynamic_array_push_front(dyn_a, DATA_BLOCKS[1]));
    assert(dynamic_array_size(dyn_a) == 2);
    assert(dynamic_array_front(dyn_a) == dyn_a->array);
    assert(memcmp(dyn_a->array, DATA_BLOCKS[1], DATA_BLOCK_SIZE) == 0);

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

    dyn_b = dynamic_array_initialize(0, DATA_BLOCK_SIZE, &block_destructor);
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
    assert(memcmp(dynamic_array_at(dyn_a, 0), DATA_BLOCKS[2], DATA_BLOCK_SIZE) == 0);
    // 2 AT
    // 1 BACK
    assert(dynamic_array_at(dyn_a, 2) == dynamic_array_back(dyn_a));
    assert(memcmp(dynamic_array_at(dyn_a, 2), DATA_BLOCKS[0], DATA_BLOCK_SIZE) == 0);

    // BACK TESTS COMPLETE

    // 3 AT
    assert(dynamic_array_at(dyn_a, 1));
    assert(memcmp(dynamic_array_at(dyn_a, 1), DATA_BLOCKS[1], DATA_BLOCK_SIZE) == 0);

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

    dyn_a = dynamic_array_initialize(0, DATA_BLOCK_SIZE, NULL);
    assert(dyn_a);
    while (dyn_a->size != 16) {
        assert(dynamic_array_push_front(dyn_a, DATA_BLOCKS[0]));
    }
    // 3 PUSH_FRONT
    assert(dyn_a->size == 16);
    assert(dyn_a->capacity == 16);
    assert(dynamic_array_push_front(dyn_a, DATA_BLOCKS[1]));
    assert(memcmp(dynamic_array_front(dyn_a), DATA_BLOCKS[1], DATA_BLOCK_SIZE) == 0);
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
    assert(memcmp(dynamic_array_front(dyn_a), DATA_BLOCKS[0], DATA_BLOCK_SIZE) == 0);

    // PUSH_FRONT TESTS COMPLETE

    dynamic_array_destroy(dyn_a);
    dynamic_array_destroy(dyn_b);
}

void run_basic_tests_b() {
    destruct_counter = 0;
    dynamic_array_t *dyn_a, *dyn_b;

    dyn_a = dynamic_array_initialize(0, DATA_BLOCK_SIZE, NULL);
    assert(dyn_a);
    dyn_b = dynamic_array_initialize(0, DATA_BLOCK_SIZE, &block_destructor);
    assert(dyn_b);

    // 2 POP_FRONT
    assert(dynamic_array_push_front(dyn_a, DATA_BLOCKS[0]));
    assert(dynamic_array_push_front(dyn_a, DATA_BLOCKS[1]));
    assert(dyn_a->size == 2);

    assert(dynamic_array_pop_front(dyn_a));
    assert(dyn_a->size == 1);
    assert(memcmp(dyn_a->array, DATA_BLOCKS[0], DATA_BLOCK_SIZE) == 0);

    // 1 POP_FRONT
    assert(dynamic_array_pop_front(dyn_a));
    assert(dyn_a->size == 0);

    // 3 POP_FRONT
    assert(dynamic_array_pop_front(dyn_a) == false);
    assert(dyn_a->size == 0);

    // 5 POP_FRONT
    assert(dynamic_array_pop_front(NULL) == false);

    // 4 POP_FRONT
    assert(dynamic_array_push_front(dyn_b, DATA_BLOCKS[0]));
    assert(dynamic_array_pop_front(dyn_b));
    assert(dyn_b->size == 0);
    assert(destruct_counter == 1);
    destruct_counter = 0;

    // POP_FRONT TESTS COMPLETE

    // 2 PUSH_BACK
    assert(dynamic_array_push_back(dyn_a, DATA_BLOCKS[2]));
    assert(dyn_a->size == 1);
    assert(memcmp(dynamic_array_front(dyn_a), DATA_BLOCKS[2], DATA_BLOCK_SIZE) == 0);

    // 1 PUSH_BACK
    assert(dynamic_array_push_back(dyn_a, DATA_BLOCKS[3]));
    assert(dyn_a->size == 2);
    assert(memcmp(dynamic_array_front(dyn_a), DATA_BLOCKS[2], DATA_BLOCK_SIZE) == 0);
    assert(memcmp(dynamic_array_back(dyn_a), DATA_BLOCKS[3], DATA_BLOCK_SIZE) == 0);

    while (dynamic_array_size(dyn_a) != 16) {
        assert(dynamic_array_push_back(dyn_a, DATA_BLOCKS[3]));
    }
    assert(dyn_a->capacity == 16);

    // 5 PUSH_BACK
    assert(dynamic_array_push_back(dyn_a, NULL) == false);
    assert(dyn_a->size == 16);
    assert(dyn_a->capacity == 16);

    // 3 PUSH_BACK
    assert(dynamic_array_push_back(dyn_a, DATA_BLOCKS[4]));
    assert(dyn_a->size == 17);
    assert(dyn_a->capacity == 32);
    assert(memcmp(dynamic_array_back(dyn_a), DATA_BLOCKS[4], DATA_BLOCK_SIZE) == 0);

    // 4 PUSH_BACK
    assert(dynamic_array_push_back(NULL, DATA_BLOCKS[0]) == false);
    assert(dyn_a->size == 17);

    // 6 PUSH_BACK
    while (dyn_a->size != DYN_MAX_CAPACITY) {
        assert(dynamic_array_push_back(dyn_a, DATA_BLOCKS[4]));
    }
    assert(memcmp(dynamic_array_back(dyn_a), DATA_BLOCKS[4], DATA_BLOCK_SIZE) == 0);
    assert(dyn_a->capacity == DYN_MAX_CAPACITY);
    assert(dynamic_array_push_back(dyn_a, DATA_BLOCKS[5]) == false);
    assert(dyn_a->size == DYN_MAX_CAPACITY);
    assert(dyn_a->capacity == DYN_MAX_CAPACITY);
    assert(memcmp(dynamic_array_back(dyn_a), DATA_BLOCKS[4], DATA_BLOCK_SIZE) == 0);

    // PUSH_BACK TESTS COMPLETE

    dynamic_array_clear(dyn_a);
    assert(dynamic_array_push_front(dyn_a, DATA_BLOCKS[0]));
    assert(dynamic_array_push_front(dyn_b, DATA_BLOCKS[0]));
    assert(dynamic_array_push_front(dyn_a, DATA_BLOCKS[1]));
    assert(dynamic_array_push_front(dyn_b, DATA_BLOCKS[1]));

    // 2 POP_BACK
    assert(dynamic_array_pop_back(dyn_a));
    assert(dyn_a->size == 1);
    assert(memcmp(dynamic_array_back(dyn_a), DATA_BLOCKS[1], DATA_BLOCK_SIZE) == 0);

    // 1 POP_BACK
    assert(dynamic_array_pop_back(dyn_a));
    assert(dyn_a->size == 0);

    // 3 POP_BACK
    assert(dynamic_array_pop_back(dyn_a) == false);
    assert(dyn_a->size == 0);

    // 4 POP_BACK
    assert(dynamic_array_pop_back(dyn_b));
    assert(dyn_b->size == 1);
    assert(memcmp(dynamic_array_back(dyn_b), DATA_BLOCKS[1], DATA_BLOCK_SIZE) == 0);
    assert(destruct_counter == 1);
    destruct_counter = 0;
    assert(dynamic_array_pop_back(dyn_b));
    assert(dyn_b->size == 0);
    assert(destruct_counter == 1);
    destruct_counter = 0;

    // 5 POP_BACK
    assert(dynamic_array_pop_back(NULL) == false);

    // POP_BACK TESTS COMPLETE

    uint8_t extraction_point[DATA_BLOCK_SIZE];

    assert(dynamic_array_push_front(dyn_a, DATA_BLOCKS[0]));
    assert(dynamic_array_push_front(dyn_b, DATA_BLOCKS[0]));
    assert(dynamic_array_push_front(dyn_a, DATA_BLOCKS[1]));
    assert(dynamic_array_push_front(dyn_b, DATA_BLOCKS[1]));

    // 2 EXTRACT_FRONT
    assert(dynamic_array_extract_front(dyn_a, extraction_point));
    assert(dyn_a->size == 1);
    assert(memcmp(extraction_point, DATA_BLOCKS[1], DATA_BLOCK_SIZE) == 0);

    // 1 EXTRACT_FRONT
    assert(dynamic_array_extract_front(dyn_a, extraction_point));
    assert(dyn_a->size == 0);
    assert(memcmp(extraction_point, DATA_BLOCKS[0], DATA_BLOCK_SIZE) == 0);

    // 3 EXTRACT_FRONT
    assert(dynamic_array_extract_front(dyn_b, extraction_point));
    assert(dyn_b->size == 1);
    assert(memcmp(extraction_point, DATA_BLOCKS[1], DATA_BLOCK_SIZE) == 0);
    assert(destruct_counter == 0);

    // 4 EXTRACT_FRONT
    assert(dynamic_array_extract_front(dyn_a, extraction_point) == false);
    assert(memcmp(extraction_point, DATA_BLOCKS[1], DATA_BLOCK_SIZE) == 0);
    assert(dyn_a->size == 0);

    // 5 EXTRACT_FRONT
    assert(dynamic_array_extract_front(NULL, extraction_point) == false);
    assert(memcmp(extraction_point, DATA_BLOCKS[1], DATA_BLOCK_SIZE) == 0);

    // 6 EXTRACT_FRONT
    assert(dynamic_array_extract_front(dyn_a, NULL) == false);
    assert(dyn_a->size == 0);

    dynamic_array_clear(dyn_b);
    destruct_counter = 0;

    // EXTRACT_FRONT TESTS COMPLETE

    assert(dynamic_array_push_back(dyn_a, DATA_BLOCKS[0]));
    assert(dynamic_array_push_back(dyn_b, DATA_BLOCKS[0]));
    assert(dynamic_array_push_back(dyn_a, DATA_BLOCKS[1]));
    assert(dynamic_array_push_back(dyn_b, DATA_BLOCKS[1]));

    // 2 EXTRACT_BACK
    assert(dynamic_array_extract_back(dyn_a, extraction_point));
    assert(dyn_a->size == 1);
    assert(memcmp(extraction_point, DATA_BLOCKS[1], DATA_BLOCK_SIZE) == 0);

    // 1 EXTRACT_BACK
    assert(dynamic_array_extract_back(dyn_a, extraction_point));
    assert(dyn_a->size == 0);
    assert(memcmp(extraction_point, DATA_BLOCKS[0], DATA_BLOCK_SIZE) == 0);

    // 3 EXTRACT_BACK
    assert(dynamic_array_extract_back(dyn_b, extraction_point));
    assert(dyn_b->size == 1);
    assert(memcmp(extraction_point, DATA_BLOCKS[1], DATA_BLOCK_SIZE) == 0);
    assert(destruct_counter == 0);

    // 4 EXTRACT_BACK
    assert(dynamic_array_extract_back(dyn_a, extraction_point) == false);
    assert(memcmp(extraction_point, DATA_BLOCKS[1], DATA_BLOCK_SIZE) == 0);
    assert(dyn_a->size == 0);

    // 5 EXTRACT_BACK
    assert(dynamic_array_extract_back(NULL, extraction_point) == false);
    assert(memcmp(extraction_point, DATA_BLOCKS[1], DATA_BLOCK_SIZE) == 0);

    // 6 EXTRACT_BACK
    assert(dynamic_array_extract_back(dyn_a, NULL) == false);
    assert(dyn_a->size == 0);

    dynamic_array_destroy(dyn_a);
    dynamic_array_destroy(dyn_b);
    destruct_counter = 0;

    // EXTRACT_BACK TESTS COMPLETE

}

// INSERT, EXTRACT, ERASE
void run_basic_tests_c() {
    dynamic_array_t *dyn_a = NULL, *dyn_b = NULL;
    uint8_t extraction_point[DATA_BLOCK_SIZE];
    destruct_counter = 0;

    dyn_a = dynamic_array_initialize(0, DATA_BLOCK_SIZE, NULL);
    assert(dyn_a);
    dyn_b = dynamic_array_initialize(0, DATA_BLOCK_SIZE, &block_destructor);
    assert(dyn_b);

    // 4 INSERT
    assert(dynamic_array_insert(dyn_a, 0, DATA_BLOCKS[0]));
    assert(dyn_a->size == 1);
    assert(memcmp(dyn_a->array, DATA_BLOCKS[0], DATA_BLOCK_SIZE) == 0);

    // 1 INSERT
    assert(dynamic_array_insert(dyn_a, 0, DATA_BLOCKS[1]));
    assert(dyn_a->size == 2);
    assert(memcmp(dynamic_array_front(dyn_a), DATA_BLOCKS[1], DATA_BLOCK_SIZE) == 0);
    assert(memcmp(dynamic_array_at(dyn_a, 1), DATA_BLOCKS[0], DATA_BLOCK_SIZE) == 0);

    // 2 INSERT
    assert(dynamic_array_insert(dyn_a, 1, DATA_BLOCKS[2]));
    assert(dyn_a->size == 3);
    assert(memcmp(dynamic_array_front(dyn_a), DATA_BLOCKS[1], DATA_BLOCK_SIZE) == 0);
    assert(memcmp(dynamic_array_at(dyn_a, 1), DATA_BLOCKS[2], DATA_BLOCK_SIZE) == 0);
    assert(memcmp(dynamic_array_at(dyn_a, 2), DATA_BLOCKS[0], DATA_BLOCK_SIZE) == 0);

    // 3 INSERT
    assert(dynamic_array_insert(dyn_a, 1, DATA_BLOCKS[3]));
    assert(dyn_a->size == 4);
    assert(memcmp(dynamic_array_front(dyn_a), DATA_BLOCKS[1], DATA_BLOCK_SIZE) == 0);
    assert(memcmp(dynamic_array_at(dyn_a, 1), DATA_BLOCKS[3], DATA_BLOCK_SIZE) == 0);
    assert(memcmp(dynamic_array_at(dyn_a, 2), DATA_BLOCKS[2], DATA_BLOCK_SIZE) == 0);
    assert(memcmp(dynamic_array_at(dyn_a, 3), DATA_BLOCKS[0], DATA_BLOCK_SIZE) == 0);

    // 5 INSERT
    assert(dynamic_array_insert(dyn_b, 1, DATA_BLOCKS[0]) == false);
    assert(dyn_b->size == 0);

    // 6 INSERT
    assert(dynamic_array_insert(dyn_a, 5, DATA_BLOCKS[5]) == false);
    assert(dyn_a->size == 4);
    assert(memcmp(dynamic_array_front(dyn_a), DATA_BLOCKS[1], DATA_BLOCK_SIZE) == 0);
    assert(memcmp(dynamic_array_at(dyn_a, 1), DATA_BLOCKS[3], DATA_BLOCK_SIZE) == 0);
    assert(memcmp(dynamic_array_at(dyn_a, 2), DATA_BLOCKS[2], DATA_BLOCK_SIZE) == 0);
    assert(memcmp(dynamic_array_at(dyn_a, 3), DATA_BLOCKS[0], DATA_BLOCK_SIZE) == 0);

    // 7 INSERT
    assert(dynamic_array_insert(dyn_a, 0, NULL) == false);
    assert(dyn_a->size == 4);
    assert(memcmp(dynamic_array_front(dyn_a), DATA_BLOCKS[1], DATA_BLOCK_SIZE) == 0);
    assert(memcmp(dynamic_array_at(dyn_a, 1), DATA_BLOCKS[3], DATA_BLOCK_SIZE) == 0);
    assert(memcmp(dynamic_array_at(dyn_a, 2), DATA_BLOCKS[2], DATA_BLOCK_SIZE) == 0);
    assert(memcmp(dynamic_array_at(dyn_a, 3), DATA_BLOCKS[0], DATA_BLOCK_SIZE) == 0);

    // 8 INSERT
    assert(dynamic_array_insert(NULL, 0, DATA_BLOCKS[0]) == false);

    // INSERT TESTS COMPLETE

    dynamic_array_clear(dyn_a);
    dynamic_array_clear(dyn_b);
    destruct_counter = 0;

    dynamic_array_push_back(dyn_a, DATA_BLOCKS[0]);
    dynamic_array_push_back(dyn_a, DATA_BLOCKS[1]);
    dynamic_array_push_back(dyn_a, DATA_BLOCKS[2]);
    dynamic_array_push_back(dyn_a, DATA_BLOCKS[3]);
    dynamic_array_push_back(dyn_b, DATA_BLOCKS[0]);
    dynamic_array_push_back(dyn_b, DATA_BLOCKS[1]);
    dynamic_array_push_back(dyn_b, DATA_BLOCKS[2]);
    dynamic_array_push_back(dyn_b, DATA_BLOCKS[3]);

    // 1 ERASE
    assert(dynamic_array_erase(dyn_a, 0));
    assert(memcmp(dynamic_array_at(dyn_a, 0), DATA_BLOCKS[1], DATA_BLOCK_SIZE) == 0);
    assert(memcmp(dynamic_array_at(dyn_a, 1), DATA_BLOCKS[2], DATA_BLOCK_SIZE) == 0);
    assert(memcmp(dynamic_array_at(dyn_a, 2), DATA_BLOCKS[3], DATA_BLOCK_SIZE) == 0);
    assert(dyn_a->size == 3);

    // 3 ERASE
    assert(dynamic_array_erase(dyn_a, 1));
    assert(memcmp(dynamic_array_at(dyn_a, 0), DATA_BLOCKS[1], DATA_BLOCK_SIZE) == 0);
    assert(memcmp(dynamic_array_at(dyn_a, 1), DATA_BLOCKS[3], DATA_BLOCK_SIZE) == 0);
    assert(dyn_a->size == 2);

    // 2 ERASE
    assert(dynamic_array_erase(dyn_a, 1));
    assert(memcmp(dynamic_array_at(dyn_a, 0), DATA_BLOCKS[1], DATA_BLOCK_SIZE) == 0);
    assert(dyn_a->size == 1);

    assert(dynamic_array_pop_back(dyn_a));
    // 4 ERASE
    assert(dynamic_array_erase(dyn_b, 2));
    assert(destruct_counter == 1);
    destruct_counter = 0;
    assert(dyn_b->size == 3);
    assert(memcmp(dynamic_array_at(dyn_b, 0), DATA_BLOCKS[0], DATA_BLOCK_SIZE) == 0);
    assert(memcmp(dynamic_array_at(dyn_b, 1), DATA_BLOCKS[1], DATA_BLOCK_SIZE) == 0);
    assert(memcmp(dynamic_array_at(dyn_b, 2), DATA_BLOCKS[3], DATA_BLOCK_SIZE) == 0);

    // 5 ERASE
    assert(dynamic_array_erase(dyn_a, 0) == false);
    assert(dyn_a->size == 0);

    // 6 ERASE
    assert(dynamic_array_erase(dyn_a, 1) == false);
    assert(dyn_a->size == 0);

    // 7 ERASE
    assert(dynamic_array_erase(dyn_b, 3) == false);
    assert(destruct_counter == 0);
    assert(memcmp(dynamic_array_at(dyn_b, 0), DATA_BLOCKS[0], DATA_BLOCK_SIZE) == 0);
    assert(memcmp(dynamic_array_at(dyn_b, 1), DATA_BLOCKS[1], DATA_BLOCK_SIZE) == 0);
    assert(memcmp(dynamic_array_at(dyn_b, 2), DATA_BLOCKS[3], DATA_BLOCK_SIZE) == 0);
    assert(dyn_b->size == 3);

    // 8 ERASE
    assert(dynamic_array_erase(dyn_b, 4) == false);
    assert(destruct_counter == 0);
    assert(memcmp(dynamic_array_at(dyn_b, 0), DATA_BLOCKS[0], DATA_BLOCK_SIZE) == 0);
    assert(memcmp(dynamic_array_at(dyn_b, 1), DATA_BLOCKS[1], DATA_BLOCK_SIZE) == 0);
    assert(memcmp(dynamic_array_at(dyn_b, 2), DATA_BLOCKS[3], DATA_BLOCK_SIZE) == 0);
    assert(dyn_b->size == 3);

    // 9 ERASE
    assert(dynamic_array_erase(NULL, 0) == false);

    // ERASE TESTS COMPLETE

    dynamic_array_clear(dyn_a);
    dynamic_array_clear(dyn_b);
    destruct_counter = 0;

    dynamic_array_push_back(dyn_a, DATA_BLOCKS[0]);
    dynamic_array_push_back(dyn_a, DATA_BLOCKS[1]);
    dynamic_array_push_back(dyn_a, DATA_BLOCKS[2]);
    dynamic_array_push_back(dyn_a, DATA_BLOCKS[3]);
    dynamic_array_push_back(dyn_b, DATA_BLOCKS[0]);
    dynamic_array_push_back(dyn_b, DATA_BLOCKS[1]);
    dynamic_array_push_back(dyn_b, DATA_BLOCKS[2]);
    dynamic_array_push_back(dyn_b, DATA_BLOCKS[3]);

    // 1 EXTRACT
    assert(dynamic_array_extract(dyn_a, 0, extraction_point));
    assert(memcmp(extraction_point, DATA_BLOCKS[0], DATA_BLOCK_SIZE) == 0);
    assert(memcmp(dynamic_array_at(dyn_a, 0), DATA_BLOCKS[1], DATA_BLOCK_SIZE) == 0);
    assert(memcmp(dynamic_array_at(dyn_a, 1), DATA_BLOCKS[2], DATA_BLOCK_SIZE) == 0);
    assert(memcmp(dynamic_array_at(dyn_a, 2), DATA_BLOCKS[3], DATA_BLOCK_SIZE) == 0);
    assert(dyn_a->size == 3);

    // 3 EXTRACT
    assert(dynamic_array_extract(dyn_a, 1, extraction_point));
    assert(memcmp(extraction_point, DATA_BLOCKS[2], DATA_BLOCK_SIZE) == 0);
    assert(memcmp(dynamic_array_at(dyn_a, 0), DATA_BLOCKS[1], DATA_BLOCK_SIZE) == 0);
    assert(memcmp(dynamic_array_at(dyn_a, 1), DATA_BLOCKS[3], DATA_BLOCK_SIZE) == 0);
    assert(dyn_a->size == 2);

    // 2 EXTRACT
    assert(dynamic_array_extract(dyn_a, 1, extraction_point));
    assert(memcmp(extraction_point, DATA_BLOCKS[3], DATA_BLOCK_SIZE) == 0);
    assert(memcmp(dynamic_array_at(dyn_a, 0), DATA_BLOCKS[1], DATA_BLOCK_SIZE) == 0);
    assert(dyn_a->size == 1);

    // 7 EXTRACT
    assert(dynamic_array_extract(dyn_a, 1, extraction_point) == false);
    assert(memcmp(extraction_point, DATA_BLOCKS[3], DATA_BLOCK_SIZE) == 0);
    assert(memcmp(dynamic_array_at(dyn_a, 0), DATA_BLOCKS[1], DATA_BLOCK_SIZE) == 0);
    assert(dyn_a->size == 1);

    // 8 EXTRACT
    assert(dynamic_array_extract(dyn_a, 2, extraction_point) == false);
    assert(memcmp(extraction_point, DATA_BLOCKS[3], DATA_BLOCK_SIZE) == 0);
    assert(memcmp(dynamic_array_at(dyn_a, 0), DATA_BLOCKS[1], DATA_BLOCK_SIZE) == 0);
    assert(dyn_a->size == 1);

    // 4 EXTRACT
    assert(dynamic_array_extract(dyn_b, 1, extraction_point));
    assert(memcmp(extraction_point, DATA_BLOCKS[1], DATA_BLOCK_SIZE) == 0);
    assert(memcmp(dynamic_array_at(dyn_b, 0), DATA_BLOCKS[0], DATA_BLOCK_SIZE) == 0);
    assert(memcmp(dynamic_array_at(dyn_b, 1), DATA_BLOCKS[2], DATA_BLOCK_SIZE) == 0);
    assert(memcmp(dynamic_array_at(dyn_b, 2), DATA_BLOCKS[3], DATA_BLOCK_SIZE) == 0);
    assert(dyn_b->size == 3);
    assert(destruct_counter == 0);

    dynamic_array_clear(dyn_a);

    // 5 EXTRACT
    assert(dynamic_array_extract(dyn_a, 0, extraction_point) == false);
    assert(dyn_a->size == 0);

    // 6 EXTRACT
    assert(dynamic_array_extract(dyn_a, 1, extraction_point) == false);
    assert(dyn_a->size == 0);

    // 9 EXTRACT
    assert(dynamic_array_extract(dyn_b, 0, NULL) == false);
    assert(dyn_b->size == 3);
    assert(destruct_counter == 0);

    // 10 EXTRACT
    assert(dynamic_array_extract(NULL, 0, extraction_point) == false);

    // EXTRACT TESTS COMPLETE

    dynamic_array_destroy(dyn_a);
    dynamic_array_destroy(dyn_b);
    destruct_counter = 0;

}

// EXTRACT, IMPORT, bulk moving...?
void run_basic_tests_d() {
    // by now you've seen the tests. I don't want to make more...
    // but I want to add features. UGH. Just copying tests from earlier and repurposing them
    // because export is just front and import is more or less init

    dynamic_array_t *dyn_a = NULL;

    // 1 IMPORT & 2 EXPORT
    dyn_a = dynamic_array_import(DATA_BLOCKS[0], 0, 1, NULL);
    assert(dyn_a);
    assert(dyn_a->size == 0);
    assert(dyn_a->capacity == 16);
    assert(dyn_a->data_size == 1);
    assert(dyn_a->destructor == NULL);
    assert(dyn_a->array);
    assert(dynamic_array_export(dyn_a) == NULL);

    dynamic_array_destroy(dyn_a);

    // 2 IMPORT & EXPORT 1
    dyn_a = dynamic_array_import(DATA_BLOCKS[1], 15, 1, &block_destructor_mini);
    assert(dyn_a);
    assert(dyn_a->size == 15);
    assert(dyn_a->capacity == 16);
    assert(dyn_a->data_size == 1);
    assert(dyn_a->destructor == &block_destructor_mini);
    assert(dyn_a->array);
    assert(memcmp(dyn_a->array, DATA_BLOCKS[1], 15) == 0);
    assert(dynamic_array_export(dyn_a) == dyn_a->array);

    dynamic_array_destroy(dyn_a);

    // 3 IMPORT
    dyn_a = dynamic_array_import(DATA_BLOCKS[2], 16, 1, &block_destructor_mini);
    assert(dyn_a);
    assert(dyn_a->size == 16);
    assert(dyn_a->capacity == 16);
    assert(dyn_a->data_size == 1);
    assert(dyn_a->destructor == &block_destructor_mini);
    assert(dyn_a->array);
    assert(memcmp(dyn_a->array, DATA_BLOCKS[2], 16) == 0);

    dynamic_array_destroy(dyn_a);

    // 4 IMPORT
    dyn_a = dynamic_array_import(DATA_BLOCKS[3], 17, 1, &block_destructor_mini);
    assert(dyn_a);
    assert(dyn_a->size == 17);
    assert(dyn_a->capacity == 32);
    assert(dyn_a->data_size == 1);
    assert(dyn_a->destructor == &block_destructor_mini);
    assert(dyn_a->array);
    assert(memcmp(dyn_a->array, DATA_BLOCKS[3], 17) == 0);

    dynamic_array_destroy(dyn_a);

    // 5 IMPORT
    dyn_a = dynamic_array_import(DATA_BLOCKS[4], DYN_MAX_CAPACITY - 1, 1, &block_destructor_mini);

    assert(dyn_a);
    assert(dyn_a->size == DYN_MAX_CAPACITY - 1);
    assert(dyn_a->capacity == DYN_MAX_CAPACITY);
    assert(dyn_a->data_size == 1);
    assert(dyn_a->destructor == &block_destructor_mini);
    assert(dyn_a->array);
    assert(memcmp(dyn_a->array, DATA_BLOCKS[4], DYN_MAX_CAPACITY - 1) == 0);

    dynamic_array_destroy(dyn_a);

    // 6 IMPORT
    dyn_a = dynamic_array_import(DATA_BLOCKS[5], DYN_MAX_CAPACITY, 1, &block_destructor_mini);

    assert(dyn_a);
    assert(dyn_a->size == DYN_MAX_CAPACITY);
    assert(dyn_a->capacity == DYN_MAX_CAPACITY);
    assert(dyn_a->data_size == 1);
    assert(dyn_a->destructor == &block_destructor_mini);
    assert(dyn_a->array);
    assert(memcmp(dyn_a->array, DATA_BLOCKS[5], DYN_MAX_CAPACITY) == 0);

    dynamic_array_destroy(dyn_a);

    // 8 IMPORT
    dyn_a = dynamic_array_import(DATA_BLOCKS[0], DYN_MAX_CAPACITY + 1, 1, &block_destructor_mini);
    assert(!dyn_a);

    // 9 IMPORT
    dyn_a = dynamic_array_import(DATA_BLOCKS[0], 16, 0, &block_destructor_mini);
    assert(!dyn_a);

    // 3 EXPORT
    assert(dynamic_array_export(NULL) == NULL);
}