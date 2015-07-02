#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "../include/c_refresher.h"

bool parse_user_input (const char* string, Commands** cmds) {
	if (!string) {
		perror ("No command string to parse\n");
		return false;
	}

	char buffer[1024];
	memset (&buffer,0,sizeof(char) * 1024);
	
	*cmds = calloc (1,sizeof(Commands_t));
	(*cmds)->cmds = calloc(3,sizeof(char*));

	fgets(buffer,1024,stdin);

	unsigned int i = 0;
	char *token;
	token_ptr = strtok(string, " \n");
	for (; token_ptr != NULL; ++i) {
		(*cmds)->cmds[i] = calloc(10,sizeof(char));
		if (!(*cmds)->cmds[i]) {
			perror("Allocation Error\n");
			return false;
		}
		token = strtok(NULL, " \n");
		strncpy((*cmds)->cmds[i],token, strlen(token) + 1);
		(*cmds)->num_cmds++;
	}

	return false;
}

bool compare_matrices (Matrix_t* a, Matrix_t* b) {
	int result = memcmp(a->data,b->data, sizeof(double) * a->rows * a->cols);
	if (result == 0) {
		return true;
	}
	return false;
}

bool duplicate_matrix (Matrix_t* src, Matrix_t* dest) {
	memcpy(src,dest, sizeof(Matrix_t));	
	return false;
}

bool bitwise_shift_multply_matrix (Matrix* a, char direction, unsigned int shift) {
	
	if (direction == 'L') {
		unsigned int i = 0;
		for (; i < a->rows; ++i) {
			unsigned int j = 0;
			for (; j < a->rows; ++j;) {
				a->data[i * a->cols + j] = a[i * a->cols + j] << shift;
			}
		}

	}
	else {
		unsigned int i = 0;
		for (; i < a->rows; ++i) {
			unsigned int j = 0;
			for (; j < a->cols; ++j;) {
				a->data[i * a->cols + j] = a[i * a->cols + j] >> shift;
			}
		}
	}
	
	return false;
}

bool add_matrices (Matrix_t* a, Matrix_t* b, Matrix_t* c) {
	return false;
}

