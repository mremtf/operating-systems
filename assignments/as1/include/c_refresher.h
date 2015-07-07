#ifndef _C_REFRESHER_
#define _C_REFRESHER_

/* Data Structures used in this program*/
typedef struct {
	unsigned int num_cmds;
	char** cmds;
}Commands_t;

typedef struct {
	unsigned int rows;
	unsigned int cols;
	double *data;
}Matrix_t;


bool parse_user_input (const char* string, Commands** cmds);
bool equal_matrices (Matrix_t* a, Matrix_t* b);
bool duplicate_matrix (Matrix_t* src, Matrix_t* dest); 
bool sum_matrices (Matrix_t* a, Matrix_t* b, Matrix_t* c);
bool bitwise_shift_multply_matrix (Matrix* a, char direction, unsigned int shift);
bool add_matrices (Matrix_t* a, Matrix_t* b, Matrix_t* c);
bool display_matrix (Matrix_t* m);
bool read_matrix(Matrix_t **m, const char* inputname);
bool write_matrix(Matrix_t*m, const char* outputname);
#endif
