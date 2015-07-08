#ifndef _C_REFRESHER_
#define _C_REFRESHER_

#include <stdbool.h>

#define MATRIX_NAME_LEN 25

/* Data Structures used in this program*/
typedef struct {
	unsigned int num_cmds;
	char** cmds;
}Commands_t;

typedef struct {
	char name[25];
	unsigned int rows;
	unsigned int cols;
	unsigned int *data;
}Matrix_t;


bool parse_user_input (const char* input, Commands_t** cmd);
void destroy_commands(Commands_t** cmd);
bool create_matrix (Matrix_t** new_matrix,const unsigned int rows,const unsigned int cols);
void destroy_matrix (Matrix_t** m); 
bool write_matrix (const char* matrix_output_filename, Matrix_t* m);
bool read_matrix (const char* matrix_input_filename, Matrix_t** m);
int sum_matrix (Matrix_t* m);
bool add_matrices (Matrix_t* a, Matrix_t* b, Matrix_t** c); 
bool bitwise_shift_matrix (Matrix_t* a, char direction, unsigned int shift);
bool duplicate_matrix (Matrix_t* src, Matrix_t** dest);
bool equal_matrices (Matrix_t* a, Matrix_t* b); 
void display_matrix (Matrix_t* m); 
bool run_commands (Commands_t* cmd, Matrix_t* mats,unsigned int num_mats);


#endif
