#include "../include/dynamic_array.h"

struct dynamic_array {
    bool capacity_lock;
    size_t capacity;
    size_t size;
    size_t data_size;
    void* array;
    void (*destructor)(void*);
};

// Value safety functions
// Supports 64bit+ size_t!
// (we'll probably have a capacity/memory/rollover problem WAY before this)
#define DYN_MAX_CAPACITY (1 << (sizeof(size_t) << 3))

size_t size_t_ffs(size_t A) {
    // can't be sure which ffs function will be appropriate
    // we we have to roll our own :/
    // (probably ffsll, but that's non-portable)
    // (and assembly won't be portable either)

    // 0 and 1 will return 1, so there's slight room for error
    size_t msb = 0;
    do {++msb;} while (A >>= 1);
    return msb;
}

bool multiply_may_overflow(size_t A, size_t B) {
    // for multiplication any two operands will result in, at most,
    // the sum of the highest bits
    return (size_t_ffs(A) + size_t_ffs(B)) > (sizeof(size_t) << 3);
}

bool addition_may_overflow(size_t A, size_t B) {
    // addition can take up one more bit than the highest MSB of the two
    return ((size_t_ffs(A) < (sizeof(size_t) << 3)) && (size_t_ffs(B) < (sizeof(size_t) << 3)));
}



/* private function prototypes */

bool dynamic_array_shift(dynamic_array_t* dyn_array);
bool dynamic_array_request_capacity(dynamic_array_t* dyn_array);
bool dynamic_array_increase_capacity(dynamic_array_t* dyn_array);



/* public function definitions */

bool dynamic_array_initialize(dynamic_array_t** dyn_array, size_t capacity, size_t data_type_size, void (*destruct_func)(void*)) {
    if (dyn_array) {
        // adding || *dyn_array will forbid non-null destination pointers
        // (to prevent mallocing over a potentially pre-existing object and leaking memeory)
        // but people never read the documentation, initialize their vairables, or use clang,
        // so forbiding it will just result in complaints

        // have it separate for eventual error enums
        if (data_type_size) {
            // Check if req capacity is too high ( ! for branching that punishes error path)
            if (!(capacity ^ DYN_MAX_CAPACITY)) {
                {
                    size_t actual_capacity = 16;
                    // would have inf loop if requested size was between DYN_MAX_CAPACITY
                    // and SIZE_MAX
                    while (capacity > actual_capacity) {actual_capacity <<= 1;}
                    capacity = actual_capacity;
                }
                // If data_type_size * capacity > MAX_SIZE, our malloc request size will rollover
                // and that's bad
                if (!multiply_may_overflow(capacity, data_type_size)) {
                    *dyn_array = (dynamic_array_t*) malloc(sizeof(dynamic_array_t));
                    if (*dyn_array) {
                        // malloc worked, woo hoo (at least the first did)
                        // TODO: Stack unwinding with gotos?
                        *dyn_array->capacity = capacity;
                        *dyn_array->size = 0;
                        *dyn_array->data_size = data_type_size;
                        *dyn_array->destructor = destruct_func;
                        *dyn_array->capacity_lock = false;

                        *dyn_array->array = malloc(data_type_size * capacity);
                        if (*dyn_array->array) {
                            // other malloc worked, yay!
                            // we're done?
                            return true;
                        } else {
                            free(*dyn_array);
                        }
                    }
                }
            }
        }
    }
    return false;
}

bool dynamic_array_destroy (dynamic_array_t** dyn_array) {
    if (dyn_array && *dyn_array) {
        if (*dyn_array->destructor) {
            // that -- part probably won't parse right
            for (size_t position = 0; *dyn_array->size; --*dyn_array->size, position += *dyn_array->data_size) {
                *dyn_array->destructor(*dyn_array->array + position);
            }
        }
        free(*dyn_array->array);
        free(*dyn_array);
        return true;
    }
    return false;
}




void* dynamic_array_front (const dynamic_array_t* const dyn_array) {
    if (dyn_array && dyn_array->size) {
    	// If array is null, well, this is ok, because it's null
    	// but if array is broken, well, we can't help that
    	// nor can we detect that, so I guess it's not an error
        return dyn_array->array;
    }
    return NULL;
}

bool dynamic_array_push_front (dynamic_array_t* const dyn_array, void* object) {
    return false;
}

void* dynamic_array_pop_front (dynamic_array_t* const dyn_array) {
    return NULL;
}

void* dynamic_array_extract_front (dynamic_array_t* const dyn_array) {
    return NULL;
}




void* dynamic_array_back (dynamic_array_t* const dyn_array) {
    return dynamic_array_at(dyn_array, dyn_array->size - 1);
}

bool dynamic_array_push_back (dynamic_array_t* const dyn_array, void* object) {
    if (!dyn_array || !object) {
        return false;
    }
    dyn_array->size++;
    if (dyn_array->size == dyn_array->capacity) {
        if (!dynamic_array_increase_capacity(dyn_array)) {
            return false;
        }
    }
    memcpy(object, &dyn_array->array[dyn_array->size - 1], dyn_array->data_type_size);
    return true;
}

void* dynamic_array_pop_back (dynamic_array_t* const dyn_array) {
    return NULL;
}

void* dynamic_array_extract_back (dynamic_array_t* const dyn_array) {
    return NULL;
}




void* dynamic_array_at (const dynamic_array_t* const dyn_array, size_t index) {
    if (index >= dyn_array->size || !dyn_array) {
        return NULL;
    }
    return &dyn_array->array[index * dyn_array->data_type_size];
}

bool dynamic_array_insert (dynamic_array_t* const dyn_array, size_t index,
                           void** object) {
    if (index >= dyn_array->size || !dyn_array || !(*object)) {
        return false;
    }
    memcpy(*object, &dyn_array->array[index * dyn_array->data_type_size], dyn_array->data_type_size);
    return true;
}

bool dynamic_array_erase (dynamic_array_t* const dyn_array, size_t index) {
    return false;
}

bool dynamic_array_extract (const dynamic_array_t* const dyn_array, size_t index,
                            void** object) {
    if (index >= dyn_array->size || !dyn_array || !(*object)) {
        return false;
    }
    *object = calloc(1, dyn_array->data_type_size);
    memcpy(&dyn_array->array[index * dyn_array->data_type_size], *object, dyn_array->data_type_size);
    return true;
}




bool dynamic_array_clear (dynamic_array_t* const dyn_array) {
    if (!dyn_array) return false;
    memset (dyn_array->array, 0, dyn_array->data_type_size * dyn_array->size);
    dyn_array->size = 0;
    return true;
}

bool dynamic_array_empty (dynamic_array_t* const dyn_array) {
    if (dyn_array){
    	return dyn_array->size;
    }
    return false; // hmmmmmm...
}

size_t dynamic_array_size (const dynamic_array_t* const dyn_array) {
    if (dyn_array) {
        return dyn_array->size;
    }
    return 0; // hmmm...
    
}




/* Private Function Definitions */
bool dynamic_array_shift (dynamic_array_t* dyn_array) {
    return false;
}
bool dynamic_array_request_capacity (dynamic_array_t* dyn_array) {
    return false;
}
bool dynamic_array_increase_capacity (dynamic_array_t* dyn_array) {
    return false;
}
