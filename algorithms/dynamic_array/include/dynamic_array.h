#ifndef _DYNAMIC_ARRAY_
#define _DYNAMIC_ARRAY_

#include <stdbool.h>

typedef struct DynamicArray DynamicArray_t;

bool dynamic_array_initialize(DynamicArray_t** dyn_array, size_t capacity, size_t data_type_size);
bool dynamic_array_destroy (DynamicArray_t** dyn_array);
bool dynamic_array_push_back (DynamicArray_t* const dyn_array, void* object);
bool dynamic_array_push_front (DynamicArray_t* const dyn_array, void* object);
void* dynamic_array_pop_front (DynamicArray_t* const dyn_array);
void* dynamic_array_pop_back (DynamicArray_t* const dyn_array);
bool dynamic_array_erase (DynamicArray_t* const dyn_array, size_t index);
void* dynamic_array_front (DynamicArray_t* const dyn_array);
void* dynamic_array_back (DynamicArray_t* const dyn_array);
bool dynamic_array_clear (DynamicArray_t* const dyn_array);
size_t dynamic_array_size (const DynamicArray_t* const dyn_array);
void* dynamic_array_at (const DynamicArray_t* const dyn_array, size_t index);
bool dynamic_array_copy_at (const DynamicArray_t* const dyn_array, size_t index, 
								DynamicArray_t** dyn_array);
bool dynamic_array_insert (DyanmicArray_t* const dyn_array, size_t index, 
								DynamicArray_t **dyn_array);
#endif
