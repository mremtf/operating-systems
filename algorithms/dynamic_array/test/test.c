#define DYN_MAX_CAPACITY 64

#include "../src/dynamic_array.c"


/*

    dynamic_array_t *dynamic_array_initialize(size_t capacity, size_t data_type_size, void (*destruct_func)(void *));
    	1. NORMAL, capacity 0, assert 16, normal data_size
    	2. NORMAL, capacity 15, assert 16, normal data_size
    	3. NORMAL, capacity 16, assert 16
    	4. NORMAL, capacity 17, assert 32
    	5. NORMAL, capacity DYN_MAX_CAPACITY - 1, assert DYN_MAX_CAPACITY
    	5. NORMAL, capacity DYN_MAX_CAPACITY, assert DYN_MAX_CAPACITY
    	6. FAIL, capacity > DYN_MAX_CAPACITY assert FAILURE
    	7. FAIL, data_size == 0
    	8. NORMAL, NULL function pointer


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
		4. FAIL, NULL pointer

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
		4. FAIL, NULL pointer


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


    bool dynamic_array_insert(dynamic_array_t *const dyn_array, size_t index,
                          void *object);
		1. NORMAL, front
		2. NORMAL, back
		3. NORMAL, arbitrary
		4. NORMAL, empty, idx = 0
		5. FAIL, empty, idx = 1 (just to make sure)
		6. FAIL, out of bounds
		7. FAIL, null object
		8. FAIL, null array


    void dynamic_array_erase(dynamic_array_t *const dyn_array, size_t index);



    bool dynamic_array_extract(dynamic_array_t *const dyn_array, size_t index,
                           void *object);


    void dynamic_array_clear(dynamic_array_t *const dyn_array);


    bool dynamic_array_empty(dynamic_array_t *const dyn_array);


    size_t dynamic_array_size(const dynamic_array_t *const dyn_array);

*/

uint8_t DATA_BLOCKS[5][100];
int destruct_counter = 0;

void block_destructor(void *block){
	memset(block, 0xFF, 100);
	// right, probably can't check if the destructor ran if the memory gets freed
	++destruct_counter;
}

void init_data_blocks() {

    memset(DATA_BLOCKS[1], 0x11, 100);

    memset(DATA_BLOCKS[2], 0x22, 100);

    memset(DATA_BLOCKS[3], 0x33, 100);

    memset(DATA_BLOCKS[4], 0x44, 100);

    memset(DATA_BLOCKS[5], 0x55, 100);

}

int main() {

	dynamic_array_t dyn_a, dyn_b;

}