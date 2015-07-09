#include "../include/dynamic_array.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

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
	if ((*dyn_array)) {
		return false;
	}
	
	*dyn_array = calloc(1,sizeof(DynamicArray_t));
	size_t new_capacity = 16;
	if (capacity > 16) {
		while (new_capacity <= capacity) {
			new_capacity <<= 1;	
		}
	}
	printf("capacity = %lu\n", new_capacity);
	(*dyn_array)->capacity = new_capacity;
	(*dyn_array)->array = calloc(new_capacity,data_type_size);
	return true;
}
bool dynamic_array_destroy (DynamicArray_t** dyn_array) {
	if (!(*dyn_array)) {
		return false;
	}
	free((*dyn_array)->array);
	free(*dyn_array);
	return true;
}
bool dynamic_array_push_back (DynamicArray_t* const dyn_array, void* object) {
	if (!dyn_array || !object) {
		return false;
	}
	dyn_array->size++;
	if (dyn_array->size == dyn_array->capacity) {
		if (!dynamic_array_increase_capacity(dyn_array)) {
			return false;
		}
	}
	memcpy(object,&dyn_array->array[dyn_array->size - 1],dyn_array->data_type_size);	
	return true;
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
	return dynamic_array_at(dyn_array,0);
}
void* dynamic_array_back (DynamicArray_t* const dyn_array) {
	return dynamic_array_at(dyn_array,dyn_array->size - 1);
}
bool dynamic_array_clear (DynamicArray_t* const dyn_array) {
	if (!dyn_array) return false;
	memset (dyn_array->array,0,dyn_array->data_type_size * dyn_array->size);
	dyn_array->size = 0;
	return true;
}
size_t dynamic_array_size (const DynamicArray_t* const dyn_array) {
	if (!dyn_array) {
		return 0;
	}
	return dyn_array->size;
}
void* dynamic_array_at (const DynamicArray_t* const dyn_array, size_t index) {
	if (index >= dyn_array->size || !dyn_array) {
		return NULL;
	}
	return &dyn_array->array[index * dyn_array->data_type_size];
}
bool dynamic_array_copy_at (const DynamicArray_t* const dyn_array, size_t index, 
								void** object) {
	if (index >= dyn_array->size || !dyn_array || !(*object)) {
		return false;
	}
	*object = calloc(1,dyn_array->data_type_size);
	memcpy(&dyn_array->array[index * dyn_array->data_type_size],*object,dyn_array->data_type_size);
	return true;
}
bool dynamic_array_insert (DynamicArray_t* const dyn_array, size_t index, 
								void **object) {
	if (index >= dyn_array->size || !dyn_array || !(*object)) {
		return false;
	}	
	memcpy(*object, &dyn_array->array[index * dyn_array->data_type_size],dyn_array->data_type_size);
	return true;								
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
