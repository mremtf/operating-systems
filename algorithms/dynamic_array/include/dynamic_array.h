#ifndef DYNAMIC_ARRAY_H__
#define DYNAMIC_ARRAY_H__

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>

typedef struct dynamic_array dynamic_array_t;



// initializes a dyn_array with the desired capacity (not size!), object size, and optional destructor (NULL to disable)
// Returns new object, NULL on error
dynamic_array_t * dynamic_array_initialize(size_t capacity, size_t data_type_size, void (*destruct_func)(void*));

// Deconstructs and frees dynamic array, applying optional destructor to remaining elements
void dynamic_array_destroy (dynamic_array_t* dyn_array);




// Prefer the X_back functions if you use a lot of push/pop operations

// Returns a pointer to the object at the front of the array
void* dynamic_array_front (const dynamic_array_t* const dyn_array);

// Copies the given object and places it at the front of the array, increasing container size by one
bool dynamic_array_push_front (dynamic_array_t* const dyn_array, void* object);

// Removes and optionally destructs the object at the front of the array, decreasing the container size by one
void dynamic_array_pop_front (dynamic_array_t* const dyn_array);

// Removes the object in the front of the array and places it in the desired location, decreasing container size
// Does not destruct since it was returned to the user
bool dynamic_array_extract_front (dynamic_array_t* const dyn_array, void* object);




// Returns a pointer to the object at the end of the array
void* dynamic_array_back (dynamic_array_t* const dyn_array);

// Copies the given object and places it at the back of the array, increasing container size by one
bool dynamic_array_push_back (dynamic_array_t* const dyn_array, void* object);

// Removes and optionally destructs the object at the back of the array
void dynamic_array_pop_back (dynamic_array_t* const dyn_array);

// Removes the object in the back of the array and places it in the desired location
// Does not destruct since it was returned to the user
bool dynamic_array_extract_back (dynamic_array_t* const dyn_array, void* object);



// Returns a pointer to the desired object in the array
// Pointer may be invalidated if the container increases in size
void* dynamic_array_at (const dynamic_array_t* const dyn_array, size_t index);

// Inserts the given object behind the given index in the array, increasing the container size by one
// (Given object will be at position index+1)
bool dynamic_array_insert (dynamic_array_t* const dyn_array, size_t index,
                           void* object);

// Removes and optionally destructs the object at the given index
void dynamic_array_erase (dynamic_array_t* const dyn_array, size_t index);

// Removes the object at the given index and places it at the desired location
// Does not destruct the object since it is returned to the user
bool dynamic_array_extract (dynamic_array_t* const dyn_array, size_t index,
                            void* object);



// Removes and optionally destructs all array elements
void dynamic_array_clear (dynamic_array_t* const dyn_array);

// Tests if array is empty
bool dynamic_array_empty (dynamic_array_t* const dyn_array);

// Returns size of array
size_t dynamic_array_size (const dynamic_array_t* const dyn_array);

#endif