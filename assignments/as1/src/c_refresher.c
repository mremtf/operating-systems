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
unsigned int add_matrix (Matrix_t* mats, Matrix_t* new_matrix, unsigned int num_mats);
unsigned int find_matrix_given_name (Matrix_t* mats, unsigned int num_mats, const char* target);
void load_matrix (Matrix_t* m, unsigned int* data);

bool create_matrix (Matrix_t** new_matrix, const char* name, const unsigned int rows,
						const unsigned int cols) {
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
	unsigned int len = strlen(name) + 1; 
	if (len > 50) {
		return false;
	}
	strncpy((*new_matrix)->name,name,len);
	return true;

}

void destroy_matrix (Matrix_t** m) {
	free((*m)->data);
	//free(*m);
	//*m = NULL;
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

	memcpy(src,dest, sizeof(Matrix_t));	
	return equal_matrices (src,*dest);
}

bool bitwise_shift_matrix (Matrix_t* a, char direction, unsigned int shift) {
	if (!a) {
		return false;
	}

	if (direction == 'l') {
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

bool add_matrices (Matrix_t* a, Matrix_t* b, Matrix_t* c) {

	if (!a || !b) {
		return false;
	}

	if (a->rows != b->rows && a->cols != b->cols) {
		return false;
	}

	for (int i = 0; i < a->rows; ++i) {
		for (int j = 0; j < b->cols; ++j) {
			c->data[i * a->cols +j] = a->data[i * a->cols + j] + b->data[i * a->cols + j];
		}
	}
	return true;
}

void display_matrix (Matrix_t* m) {
	if (!m) {
		return;
	}

	printf("\nMatrix Contents (%s):\n", m->name);
	printf("DIM = (%u,%u)\n", m->rows, m->cols);
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

	

	/*read the wrote dimensions and name length*/
	unsigned int name_len = 0;
	unsigned int rows = 0;
	unsigned int cols = 0;
	
	if (read(fd,&name_len,sizeof(unsigned int)) != sizeof(unsigned int)) {
		printf("FAILURE 1\n");
		return false;
	}
	char name_buffer[50];
	if (read (fd,name_buffer,sizeof(char) * name_len) != sizeof(char) * name_len) {
		return false;	
	}

	if (read (fd,&rows, sizeof(unsigned int)) != sizeof(unsigned int)) {

		return false;
	}

	if (read(fd,&cols,sizeof(unsigned int)) != sizeof(unsigned int)) {

		return false;
	}

	unsigned int numberOfDataBytes = rows * cols * sizeof(unsigned int);
	unsigned int *data = calloc(rows * cols, sizeof(unsigned int));
	if (read(fd,data,numberOfDataBytes) != numberOfDataBytes) {
		return false;	
	}

	if (!create_matrix(m,name_buffer,rows,cols)) {
		return false;
	}

	load_matrix(*m,data);

	if (close(fd)) {
		return false;
	}

	return true;	
}


bool write_matrix (const char* matrix_output_filename, Matrix_t* m) {
	
	if (!matrix_output_filename || !m) {
		return false;
	}
	int fd = open (matrix_output_filename, O_CREAT | O_RDWR);
	if (fd < 0) {
		printf("FAILED TO CREATE/OPEN FILE FOR WRITING\n");
		return false;
	}
	/* Calculate the needed buffer for our matrix */
	unsigned int name_len = strlen(m->name) + 1;
	unsigned int numberOfBytes = sizeof(unsigned int) + (sizeof(unsigned int)  * 2) + name_len + sizeof(unsigned int) * m->rows * m->cols + 1;
	printf("bytes = %u\n", numberOfBytes);
	/* Allocate the output_buffer */
	unsigned char* output_buffer = calloc(numberOfBytes,sizeof(unsigned char));
	unsigned int offset = 0;
	memcpy(&output_buffer[offset], &name_len, sizeof(unsigned int));
	offset += sizeof(unsigned int);	
	memcpy(&output_buffer[offset], m->name,name_len);
	offset += name_len;
	memcpy(&output_buffer[offset],&m->rows,sizeof(unsigned int));
	offset += sizeof(unsigned int);
	memcpy(&output_buffer[offset],&m->cols,sizeof(unsigned int));
	offset += sizeof(unsigned int);
	memcpy (&output_buffer[offset],m->data,m->rows * m->cols * sizeof(unsigned int));
	offset += (m->rows * m->cols * sizeof(unsigned int));
	output_buffer[numberOfBytes - 1] = EOF;

	if (write(fd,output_buffer,numberOfBytes) != numberOfBytes) {
		printf("WRITE FAILED %s\n", m->name);
		return false;
	}
	
	if (close(fd)) {
		return false;
	}
	free(output_buffer);

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
			int idx = find_matrix_given_name(mats,num_mats,cmd->cmds[1]);
			if (idx >= 0) {
				display_matrix (&mats[idx]);
			}
	}
	else if (strncmp(cmd->cmds[0],"add",strlen("add") + 1) == 0
		&& cmd->num_cmds == 4) {
			printf("INSIDE ADD\n");
			int mat1_idx = find_matrix_given_name(mats,num_mats,cmd->cmds[1]);
			int mat2_idx = find_matrix_given_name(mats,num_mats,cmd->cmds[2]);
			if (mat1_idx >= 0 && mat2_idx >= 0) {
				Matrix_t* c = NULL;
				if( !create_matrix (&c,cmd->cmds[3], mats[mat1_idx].rows, 
						mats[mat1_idx].cols)) {
					return false;
				}
			
				add_matrix(mats,c, num_mats);

				if (! add_matrices(&mats[mat1_idx], &mats[mat2_idx],c) ) {
					return false;	
				}
			}
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
		printf("INSIDE READ\n");
		Matrix_t* new_matrix = NULL;
		if(! read_matrix(cmd->cmds[1],&new_matrix)) {
			return false;
		}	
		add_matrix(mats,new_matrix, num_mats);
		
	}
	else if (strncmp(cmd->cmds[0],"write",strlen("write") + 1) == 0
		&& cmd->num_cmds == 2) {
		printf("INSIDE WRITE\n");
		int mat1_idx = find_matrix_given_name(mats,num_mats,cmd->cmds[1]);
		if(! write_matrix(mats[mat1_idx].name,&mats[mat1_idx])) {
			return false;
		}
	
	}
	else if (strncmp(cmd->cmds[0], "create", strlen("create") + 1) == 0
		&& cmd->num_cmds == 4) {
		printf("INSIDE CREATE\n");
		Matrix_t* new_mat = NULL;
		const unsigned int rows = atoi(cmd->cmds[2]);
		const unsigned int cols = atoi(cmd->cmds[3]);

		create_matrix(&new_mat,cmd->cmds[1],rows, cols);
		add_matrix(mats,new_mat,num_mats);

		printf("Created Matrix (%s,%u,%u)\n", new_mat->name, new_mat->rows, new_mat->cols);
	}
	if (strncmp(cmd->cmds[0], "random", strlen("random") + 1) == 0
		&& cmd->num_cmds == 4) {
		int mat1_idx = find_matrix_given_name(mats,num_mats,cmd->cmds[1]);
		const unsigned int start_range = atoi(cmd->cmds[2]);
		const unsigned int end_range = atoi(cmd->cmds[3]);
		random_matrix(&mats[mat1_idx],start_range, end_range);

	}
	else {
		return false;
	}
	return true;
}

bool random_matrix(Matrix_t* m, unsigned int start_range, unsigned int end_range) {
	if (!m) {
	
	}
	for (unsigned int i = 0; i < m->rows; ++i) {
		for (unsigned int j = 0; j < m->cols; ++j) {
			m->data[i * m->cols + j] = rand() % end_range + start_range;
		}
	}
	return true;
}

/*Protected Functions in C*/

void load_matrix (Matrix_t* m, unsigned int* data) {
	memcpy(m->data,data,m->rows * m->cols * sizeof(unsigned int));
}

unsigned int add_matrix (Matrix_t* mats, Matrix_t* new_matrix, unsigned int num_mats) {
	static long int current_position = 0;
	const long int pos = current_position % num_mats;
	Matrix_t* temp = &mats[pos];
	if (temp->data) {
		destroy_matrix(&temp);
	}
	memcpy(&mats[pos],new_matrix,sizeof(Matrix_t));	
	current_position++;
	return pos;
}

unsigned int find_matrix_given_name (Matrix_t* mats, unsigned int num_mats, const char* target) {
	
	for (int i = 0; i < num_mats; ++i) {
		if (strncmp(mats[i].name,target,strlen(mats[i].name)) == 0) {
			return i;
		}
	}
	return -1;
}
