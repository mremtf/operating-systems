#include "../include/dynamic_array.h"

#include <stdlib.h>
#include <string.h>

struct DynamicArray {
	size_t capacity;
	size_t size;
	size_t data_type_size;
	void* array;
};

/*private function prototypes*/

bool dynamic_array_shift (DynamicArray_t* dyn_array);
bool dynamic_array_request_capacity (DynamicArray_t* dyn_array);
bool dynamic_array_increase_capacity (DynamicArray_t* dyn_array);


/*public function definitions*/
bool dynamic_array_initialize(DynamicArray_t** dyn_array, size_t capacity, size_t data_type_size) {
	return false;
}
bool dynamic_array_destroy (DynamicArray_t** dyn_array) {
	return false;
}
bool dynamic_array_push_back (DynamicArray_t* const dyn_array, void* object) {
	return false;
}
bool dynamic_array_push_front (DynamicArray_t* const dyn_array, void* object) {
	return false;
}
void* dynamic_array_pop_front (DynamicArray_t* const dyn_array) {
	return NULL;
}
void* dynamic_array_pop_back (DynamicArray_t* const dyn_array) {
	return NULL;
}
bool dynamic_array_erase (DynamicArray_t* const dyn_array, size_t index) {
	return false;
}
void* dynamic_array_front (DynamicArray_t* const dyn_array) {
	return NULL;
}
void* dynamic_array_back (DynamicArray_t* const dyn_array) {
	return NULL;
}
bool dynamic_array_clear (DynamicArray_t* const dyn_array) {
	return false;
}
size_t dynamic_array_size (const DynamicArray_t* const dyn_array) {
	return 0;
}
void* dynamic_array_at (const DynamicArray_t* const dyn_array, size_t index) {
	return NULL;
}
bool dynamic_array_copy_at (const DynamicArray_t* const dyn_array, size_t index, 
								void** object) {

	return false;
}
bool dynamic_array_insert (DynamicArray_t* const dyn_array, size_t index, 
								void **object) {
								
	return false;								
}

/* Private Function Definitions */
bool dynamic_array_shift (DynamicArray_t* dyn_array) {
	return false;
}
bool dynamic_array_request_capacity (DynamicArray_t* dyn_array) {
	return false;
}
bool dynamic_array_increase_capacity (DynamicArray_t* dyn_array) {
	return false;
}
