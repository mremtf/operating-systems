#include "../include/dynamic_array.h"

struct dynamic_array {
    size_t capacity;
    size_t size;
    size_t data_size;
    void *array;
    void (*destructor)(void *);
};

// Supports 64bit+ size_t! Capping at the largest power of two size_t can store.
// We'll run out of memory before we hit the cap. And rollover size_t (see below), unless we're storing bytes.
#define DYN_MAX_CAPACITY (1 << ((sizeof(size_t) << 3) - 1))

// Safety functions from when I was concerned size_t may overflow
// Taking them out because trying to cover all the edge cases was killing me
// and it would just slow down everything
// So, casual warning, this library will break if you're overflowing size_t...
// but size_t is the max addressable, so... you can't(?)
// No guarentee it'll work if you're using this to store, let's say 10EB+ of data
// We should probably transition to GMP if we need these kinds of numbers
/*
    #define SIZE_T_BYTE_COUNT (sizeof(size_t) << 3)
    size_t size_t_ffs(size_t A) {
    // can't be sure which ffs function will be appropriate
    // we we have to roll our own :/
    // (probably ffsll, but that's non-portable)
    // (and assembly won't be portable either)

    // 0 will return 1, so there's slight room for error
    size_t msb = 0;
    do {++msb;} while (A >>= 1);
    return msb;
    }

    #define MULTIPLY_MAY_OVERFLOW(A, B) ((size_t_ffs(A) + size_t_ffs(B)) > SIZE_T_BYTE_COUNT)
    // for multiplication any two operands will result in, at most,
    // the sum of the highest bits

    #define ADDITION_MAY_OVERFLOW(A, B) ((A) > (SIZE_MAX - (B)))
*/

// casts pointer and does arithmatic to get index of element
#define DYN_ARRAY_POSITION(dyn_array_ptr, idx) (((uint8_t*)dyn_array_ptr->array) + ((idx) * dyn_array_ptr->data_size))
// Gets the size (in bytes) of n dyn_array elements
#define DYN_SIZE_N_ELEMS(dyn_array_ptr, n) (dyn_array_ptr->data_size * (n))


/* private function prototypes */
typedef enum {CREATE_GAP, FILL_GAP, FILL_GAP_DESTRUCT} DYN_SHIFT_MODE;

bool dyn_shift(dynamic_array_t *dyn_array, size_t position, size_t count, DYN_SHIFT_MODE mode);

// Checks to see if the object can handle an increase in size (and optionally increases capacity)
bool dyn_request_size_increase(dynamic_array_t *dyn_array, size_t increment);


/* public function definitions */

dynamic_array_t *dynamic_array_initialize(size_t capacity, size_t data_type_size, void (*destruct_func)(void *)) {
    if (data_type_size) {
        // Check if req capacity is too high
        if (capacity <= DYN_MAX_CAPACITY) {
            // would have inf loop if requested size was between DYN_MAX_CAPACITY
            // and SIZE_MAX
            size_t actual_capacity = 16;
            while (capacity > actual_capacity) {actual_capacity <<= 1;}
            capacity = actual_capacity;
            // If data_type_size * capacity > MAX_SIZE, our malloc request size will rollover
            // and that's bad
            //if (!MULTIPLY_MAY_OVERFLOW(capacity, data_type_size)) {
            dynamic_array_t *dyn_array = (dynamic_array_t *) malloc(sizeof(dynamic_array_t));
            if (dyn_array) {
                // malloc worked, woo hoo (at least the first did)
                // TODO: Stack unwinding with gotos?
                // (nvm, not enough resources to bother with that)
                dyn_array->capacity = capacity;
                dyn_array->size = 0;
                dyn_array->data_size = data_type_size;
                dyn_array->destructor = destruct_func;

                dyn_array->array = malloc(data_type_size * capacity);
                if (dyn_array->array) {
                    // other malloc worked, yay!
                    // we're done?
                    return dyn_array;
                } else {
                    free(dyn_array);
                }
            }
        }
    }
    return NULL;
}

void dynamic_array_destroy(dynamic_array_t *dyn_array) {
    // switch to a call to shift and free?
    if (dyn_array) {
        if (dyn_array->destructor) {
            for (uint8_t *array_itr = (uint8_t *)dyn_array->array;
                    dyn_array->size;
                    --(dyn_array->size), array_itr += dyn_array->data_size) {
                dyn_array->destructor(array_itr);
            }
        }
        free(dyn_array->array);
        // Bad/dangerous assumption, don't make it
        //free(dyn_array);
    }
}




void *dynamic_array_front(const dynamic_array_t *const dyn_array) {
    if (dyn_array && dyn_array->size) {
        // If array is null, well, this is ok, because it's null
        // but if array is broken, well, we can't help that
        // nor can we detect that, so I guess it's not an error
        return dyn_array->array;
    }
    return NULL;
}

bool dynamic_array_push_front(dynamic_array_t *const dyn_array, void *object) {
    //dyn_shift(dynamic_array_t* dyn_array, size_t position, size_t count, DYN_SHIFT_MODE mode)
    // make sure to check pointer FIRST, shift does stuff to the structure that we don't want to have to undo
    if (object && dyn_shift(dyn_array, 0, 1, CREATE_GAP)) {
        // got auth to insert data, size was incremented and such for us
        memcpy(dyn_array->array, object, dyn_array->data_size);
        return true;
    }
    return false;
}

void dynamic_array_pop_front(dynamic_array_t *const dyn_array) {
    // can this really ever fail? (other than NULL)
    // No, so it's void.
    dyn_shift(dyn_array, 0, 1, FILL_GAP_DESTRUCT);
}

bool dynamic_array_extract_front(dynamic_array_t *const dyn_array, void *object) {
    // making them allocate room, makes us play nice with other things
    // as opposed to forcing them to do it our way like we know best
    if (object && dyn_array && dyn_array->size) {
        memcpy(object, dyn_array->array, dyn_array->data_size);
        // FILL_GAP can't have an error unless front doesn't exist,
        // which we're suppressing (for now at least)
        dyn_shift(dyn_array, 0, 1, FILL_GAP);
        return true;
    }
    return false;
}




void *dynamic_array_back(dynamic_array_t *const dyn_array) {
    if (dyn_array && dyn_array->size) {
        return DYN_ARRAY_POSITION(dyn_array, dyn_array->size - 1);
    }
    return NULL;
}

bool dynamic_array_push_back(dynamic_array_t *const dyn_array, void *object) {
    if (object && dyn_array) {
        // create gap
        //dyn_shift(dynamic_array_t* dyn_array, size_t position, size_t count, DYN_SHIFT_MODE mode)
        if (dyn_shift(dyn_array, dyn_array->size, 1, CREATE_GAP)) {
            // memcpy
            memcpy(DYN_ARRAY_POSITION(dyn_array, dyn_array->size),
                   object,
                   dyn_array->data_size);
            return true;
        }
    }
    return false;
}

void dynamic_array_pop_back(dynamic_array_t *const dyn_array) {
    if (dyn_array && dyn_array->size) {
        // MUST assert the size for the pop_back
        // if size is zero, it will rollover (rollunder?)
        // and then the size and count check will roll it back to zero during the check
        // and that will cause it to pass that check
        // and then things will probably break.
        // (a memmove with src = dest, I believe)
        // (that might not cause a disaster, but the size decrement will rollover the size variable)
        // TODO: need to assert size on gap filling in dyn_shift
        dyn_shift(dyn_array, dyn_array->size - 1, 1, FILL_GAP_DESTRUCT);
    }
}

bool dynamic_array_extract_back(dynamic_array_t *const dyn_array, void *object) {
    if (object && dyn_array && dyn_array->size) {
        memcpy(object, DYN_ARRAY_POSITION(dyn_array, dyn_array->size - 1), dyn_array->data_size);
        // FILL_GAP can't have an error unless front doesn't exist,
        // which we're suppressing (for now at least)
        dyn_shift(dyn_array, dyn_array->size - 1, 1, FILL_GAP);
        return true;
    }
    return false;
}




void *dynamic_array_at(const dynamic_array_t *const dyn_array, size_t index) {
    if (dyn_array && index < dyn_array->size) {
        return DYN_ARRAY_POSITION(dyn_array, index);
    }
    return NULL;
}

bool dynamic_array_insert(dynamic_array_t *const dyn_array, size_t index,
                          void *object) {
    // putting object at INDEX
    // so we shift a gap at INDEX
    if (object && dyn_shift(dyn_array, index, 1, CREATE_GAP)) {
        memcpy(DYN_ARRAY_POSITION(dyn_array, index),
               object,
               dyn_array->data_size);
        return true;
    }
    return false;
}

void dynamic_array_erase(dynamic_array_t *const dyn_array, size_t index) {
    // Going to treat it like the pops and say it can't fail
    // Maybe this isn't the best idea. Decision for when it's actually implemented
    dyn_shift(dyn_array, index, 1, FILL_GAP_DESTRUCT);
}

bool dynamic_array_extract(const dynamic_array_t *const dyn_array, size_t index,
                           void *object) {
    if (dyn_array && object && dyn_array->size > index) {
        memcpy(object,
               DYN_ARRAY_POSITION(dyn_array, index),
               dyn_array->data_size);
        dyn_shift(dyn_array, index, 1, FILL_GAP);
        return true;
    }
    return false;
}




void dynamic_array_clear(dynamic_array_t *const dyn_array) {
    if (dyn_array && dyn_array->size) {
        if (dyn_array->destructor) {
            for (uint8_t *array_itr = dyn_array->array;
                    dyn_array->size;
                    --(dyn_array->size), array_itr += dyn_array->data_size) {
                dyn_array->destructor(array_itr);
            }
        }
        // duplicate if we didn't have a destructor
        // but faster than more complex logic
        dyn_array->size = 0;
    }
}

bool dynamic_array_empty(dynamic_array_t *const dyn_array) {
    return dyn_array_size(dyn_array);
}

size_t dynamic_array_size(const dynamic_array_t *const dyn_array) {
    if (dyn_array) {
        return dyn_array->size;
    }
    return 0; // hmmmmm...

}




/* Private Function Definitions */
bool dyn_shift(dynamic_array_t *dyn_array, size_t position, size_t count, DYN_SHIFT_MODE mode) {
    // Shifts contents. Mode flag controls what happens and how (duh?)
    // So, if you erase idx 2, you're filling the gap at position two
    // [A][X][B][C][D][E]
    //       <---- 1
    // [A][B][C][D][E][?]

    // inserting between idx 2 and 3 (between B and C) means you're moving everything from 3 down to make room
    // (can't use traditional insert lingo (new space is following idx) because push_front can't say position -1)
    // [A][B][C][D][E][?]
    //       \--F
    //        -----> 1
    // [A][B][?][C][D][E]
    // (and the insert, not done by this function)
    // [A][B][F][C][D][E]

    // the count parameter controls how many gaps are created/filled, and grows to the LEFT.
    // Zero will result in an error (until we have error codes(?))

    // !!! WARNING !!!
    // This function will handle capacity increases and will change the size to reflect the changes made
    // it WILL NOT destruct objects, because we don't know when/if that should happen without adding another flag
    // (better to do that here and have a flag?)
    // This function will check ranges and sizes, it should be safe to pass parameters to this without checking them first.
    // This enables us to have one function that handles error checking and will be more maintainable, hopefully

    if (dyn_array && count && ((position + count) <= size)) {
        // dyn good, count ok, pos+count isn't crazy
        switch (mode) {
            case FILL_GAP_DESTRUCT:
                if (dyn_array->destructor) {
                    uint8_t *dest_pos = DYN_ARRAY_POSITION(dyn_array, position);
                    for (size_t total = count; total; --total, dest_pos += dyn_array->data_size) {
                        dyn_array->destructor(dest_pos);
                    }
                }
            case FILL_GAP:
                // shrinking in size
                // nice and simple (?)

                // pointer arithmatic on void pointers is illegal nowadays :C
                // GCC allows it for compatability, other provide it for GCC compatability. Way to implement a standard.
                // It should be cast to some sort of byte pointer, which is a pain. Hooray for macros
                if (position + count < size) {
                    // there's a actual gap, not just a hole to make at the end
                    memmove(DYN_ARRAY_POSITION(dyn_array, position),
                            DYN_ARRAY_POSITION(dyn_array, position + count),
                            DYN_SIZE_N_ELEMS(dyn_array, dyn_array->size - (position + count)));
                }
                // decrease the size and return
                dyn_array->size -= count;
                return true;
            case CREATE_GAP:
                // may or may not need to increase capacity.
                // We'll ask the capacity function if we can do it.
                // If we can, do it. If not... Too bad for the user.
                if (dyn_request_size_increase(dyn_array, count)) {
                    if (position != dyn_array->size) {
                        // wasn't a gap at the end, we need to move data
                        memmove(DYN_ARRAY_POSITION(dyn_array, position + count),
                                DYN_ARRAY_POSITION(dyn_array, position),
                                DYN_SIZE_N_ELEMS(dyn_array, dyn_array->size - position));
                    }
                    dyn_array->size += count;
                    return true;
                }
                return false;
            default:
                // ...what?
        }
    }
    return false;
}

bool dyn_request_size_increase(dynamic_array_t *dyn_array, size_t increment) {
    // check to see if the size can be increased by the increment
    // and increase capacity if need be
    // average case will be perfectly fine, single increment
    if (dyn_array) {
        //if (!ADDITION_MAY_OVERFLOW(dyn_array->size, increment)) {
        // increment is ok, but is the capacity?
        if (capacity >= (dyn_array->size + increment)) {
            // capacity is ok!
            return true;
        }
        // have to reallocate, is that even possible?
        size_t capacity_increase = 0, needed_size = dyn_array->size + increment;
        while ((dyn_array->capacity << capacity_increase) != DYN_MAX_CAPACITY && dyn_array->capacity < needed_size) {++capacity_increase;}
        // if capacity_increase is 0, we can't increase capacity because we are at max
        if (capacity_increase) {
            size_t new_capacity = dyn_array->capacity << capacity_increase;
            // we can theoretically hold this, check if we can allocate that
            //if (!MULTIPLY_MAY_OVERFLOW(new_capacity, dyn_array->data_size)) {
            // we won't overflow, so we can at least REQUEST this change
            void *new_array = realloc(dyn_array->array, new_capacity * dyn_array->data_size);
            if (new_array) {
                // success! Wasn't that easy?
                dyn_array->array = new_array;
                dyn_array->capacity = new_capacity;
                return true;
            }
        }
    }
    return false;
}