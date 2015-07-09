#ifndef DYNAMIC_ARRAY_H__
#define DYNAMIC_ARRAY_H__

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

typedef struct dynamic_array dynamic_array_t;

bool dynamic_array_initialize (dynamic_array_t** dyn_array, size_t capacity, size_t data_type_size);
bool dynamic_array_destroy (dynamic_array_t** dyn_array);
bool dynamic_array_push_back (dynamic_array_t* const dyn_array, void* object);
bool dynamic_array_push_front (dynamic_array_t* const dyn_array, void* object);
void* dynamic_array_pop_front (dynamic_array_t* const dyn_array);
void* dynamic_array_pop_back (dynamic_array_t* const dyn_array);
bool dynamic_array_erase (dynamic_array_t* const dyn_array, size_t index);
void* dynamic_array_front (dynamic_array_t* const dyn_array);
void* dynamic_array_back (dynamic_array_t* const dyn_array);
bool dynamic_array_clear (dynamic_array_t* const dyn_array);
size_t dynamic_array_size (const dynamic_array_t* const dyn_array);
void* dynamic_array_at (const dynamic_array_t* const dyn_array, size_t index);
bool dynamic_array_copy_at (const dynamic_array_t* const dyn_array, size_t index,
                            void** object);
bool dynamic_array_insert (dynamic_array_t* const dyn_array, size_t index,
                           void** object);
#endif
