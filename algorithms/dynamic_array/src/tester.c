#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/dynamic_array.h"

int main (void) {
	
	DynamicArray_t* dyn_array = NULL;
	dynamic_array_initialize(&dyn_array,10,sizeof(int));
	dynamic_array_destroy(&dyn_array);
	dyn_array = NULL;
	dynamic_array_initialize(&dyn_array,32,sizeof(int));
	dynamic_array_destroy(&dyn_array);


	return 0;
}
