#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>


#include "../include/c_refresher.h"


#define MAX_CMD_COUNT 50

/*protected functions*/
unsigned int find_next_free_matrix (Matrix_t* mats, unsigned int num_mats);
unsigned int find_matrix_given_name (Matrix_t* mats, unsigned int num_mats, const char* target);


bool create_matrix (Matrix_t** new_matrix,const unsigned int rows,const unsigned int cols) {
	*new_matrix = calloc(1,sizeof(Matrix_t));
	if (!(*new_matrix)) {
		return false;
	}
	(*new_matrix)->data = calloc(rows * cols,sizeof(unsigned int));
	if (!(*new_matrix)->data) {
		return false;
	}
	(*new_matrix)->rows = rows;
	(*new_matrix)->cols = cols;
	return true;

}

void destroy_matrix (Matrix_t** m) {
	free((*m)->data);
	free(*m);
	*m = NULL;
}



bool parse_user_input (const char* input, Commands_t** cmd) {

	if (!input) {
		perror ("No command string to parse\n");
		return false;
	}

	char *string = strdup(input);
	
	*cmd = calloc (1,sizeof(Commands_t));
	(*cmd)->cmds = calloc(MAX_CMD_COUNT,sizeof(char*));

	unsigned int i = 0;
	char *token;
	token = strtok(string, " \n");
	for (; token != NULL && i < MAX_CMD_COUNT; ++i) {
		(*cmd)->cmds[i] = calloc(50,sizeof(char));
		if (!(*cmd)->cmds[i]) {
			perror("Allocation Error\n");
			return false;
		}	
		strncpy((*cmd)->cmds[i],token, strlen(token) + 1);
		(*cmd)->num_cmds++;
		token = strtok(NULL, " \n");
	}
	free(string);
	return true;
}

void destroy_commands(Commands_t** cmd) {
	for (int i = 0; i < MAX_CMD_COUNT; ++i) {
		free((*cmd)->cmds[i]);
	}	
	free((*cmd));
	*cmd = NULL;
}


bool equal_matrices (Matrix_t* a, Matrix_t* b) {
	if (!a || !b || !a->data || !b->data) {
		return false;	
	}

	int result = memcmp(a->data,b->data, sizeof(unsigned int) * a->rows * a->cols);
	if (result == 0) {
		return true;
	}
	return false;
}

bool duplicate_matrix (Matrix_t* src, Matrix_t** dest) {
	if (!src) {
		return false;
	}

	if (!create_matrix(dest,src->rows, src->cols)) {
		return false;
	}

	memcpy(src,dest, sizeof(Matrix_t));	
	return equal_matrices (src,*dest);
}

bool bitwise_shift_matrix (Matrix_t* a, char direction, unsigned int shift) {
	if (!a) {
		return false;
	}

	if (direction == 'L') {
		unsigned int i = 0;
		for (; i < a->rows; ++i) {
			unsigned int j = 0;
			for (; j < a->rows; ++j) {
				a->data[i * a->cols + j] = a->data[i * a->cols + j] << shift;
			}
		}

	}
	else {
		unsigned int i = 0;
		for (; i < a->rows; ++i) {
			unsigned int j = 0;
			for (; j < a->cols; ++j) {
				a->data[i * a->cols + j] = a->data[i * a->cols + j] >> shift;
			}
		}
	}
	
	return true;
}

bool add_matrices (Matrix_t* a, Matrix_t* b, Matrix_t** c) {

	if (!a || !b) {
		return false;
	}

	if (a->rows != b->rows && a->cols != b->cols) {
		return false;
	}
	
	if (!create_matrix(c,a->rows,a->cols)) {
		return false;
	}

	for (int i = 0; i < a->rows; ++i) {
		for (int j = 0; j < b->cols; ++j) {
			(*c)->data[i * a->cols +j] = a->data[i * a->cols + j] + b->data[i * a->cols + j];
		}
	}
	return true;
}

void display_matrix (Matrix_t* m) {
	if (!m) {
		return;
	}

	printf("\nMatrix Contents:\n");
	for (int i = 0; i < m->rows; ++i) {
		for (int j = 0; j < m->cols; ++j) {
			printf("%u ", m->data[i * m->cols + j]);
		}
		printf("\n");
	}
	printf("\n");

}

int sum_matrix (Matrix_t* m) {
	if (!m) {
		return -1;
	}

	const int size = m->rows * m->cols;
	int sum = 0;
	for (int i = 0; i < size; ++i) {
		sum += m->data[i];
	}
	return sum;
}


bool read_matrix (const char* matrix_input_filename, Matrix_t** m) {
	if (!matrix_input_filename) {
		return false;
	}

	int fd = open(matrix_input_filename,O_RDONLY);
	if (fd < 0) {
		return false;
	}

	

	/*read the wrote dimensions*/
	unsigned int rows = 0;
	unsigned int cols = 0;

	if (read (fd,&rows, sizeof(unsigned int)) != sizeof(unsigned int)) {
		return false;
	}

	if (read(fd,&cols,sizeof(unsigned int)) != sizeof(unsigned int)) {
		return false;
	}

	if (!create_matrix(m,rows,cols)) {
		return false;
	}

	if ( read(fd,*m,sizeof(Matrix_t)) != sizeof(Matrix_t)) {
		free(*m);
		return false;
	}

	if (close(fd)) {
		return false;
	}

	return true;	
}


bool write_matrix (const char* matrix_output_filename, Matrix_t* m) {
	
	if (!matrix_output_filename || !m) {
		return false;
	}
	int fd = open (matrix_output_filename,O_WRONLY | O_APPEND);
	if (fd < 0) {
		return false;
	}

	if (write(fd,m,sizeof(Matrix_t)) != sizeof(Matrix_t)) {
		return false;
	}
	
	if (close(fd)) {
		return false;
	}

	return true;
}

bool run_commands (Commands_t* cmd, Matrix_t* mats, unsigned int num_mats) {
	if (!cmd) {
		return false;
	}

	/*Parsing and calling of commands*/
	if (strncmp(cmd->cmds[0],"display",strlen("display") + 1) == 0
		&& cmd->num_cmds == 2) {
			/*find the requested matrix*/
			int idx = find_next_free_matrix(mats,num_mats);
			if (idx < 0) {
				return false;
			}
			display_matrix (&mats[idx]);
	}
	else if (strncmp(cmd->cmds[0],"add",strlen("add") + 1) == 0
		&& cmd->num_cmds == 3) {
		//	add();
	}
	else if (strncmp(cmd->cmds[0],"duplicate",strlen("duplicate") + 1) == 0
		&& cmd->num_cmds == 2) {
		//duplicate();
	}
	else if (strncmp(cmd->cmds[0],"equal",strlen("equal") + 1) == 0
		&& cmd->num_cmds == 2) {
		//equal();
	}
	else if (strncmp(cmd->cmds[0],"shift",strlen("shift") + 1) == 0
		&& cmd->num_cmds == 3) {
		//shift();
	}
	else if (strncmp(cmd->cmds[0],"read",strlen("read") + 1) == 0
		&& cmd->num_cmds == 2) {

		//read();
	}
	else if (strncmp(cmd->cmds[0],"write",strlen("write") + 1) == 0
		&& cmd->num_cmds == 2) {

		//write();
	
	}
	else {
		return false;
	}
	return true;
}

/*Protected Functions in C*/

unsigned int find_next_free_matrix (Matrix_t* mats, unsigned int num_mats) {
	
	for (int i = 0; i < num_mats; ++i) {
		if (!mats[i].data) {
			return i;
		}	
	}
	return -1;
}

unsigned int find_matrix_given_name (Matrix_t* mats, unsigned int num_mats, const char* target) {
	
	for (int i = 0; i < num_mats; ++i) {
		if (strncmp(mats[i].name,target,strlen(mats[i].name)) == 0) {
			return i;
		}
	}
	return -1;
}
