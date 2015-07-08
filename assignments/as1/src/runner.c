#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <math.h>

#include<readline/readline.h>

#include "../include/c_refresher.h"


int main (int argc, char **argv) {
	
	char *line = NULL;
	Commands_t* cmd;
	line = readline("> ");

	Matrix_t* mats[10];
	memset(&mats,0, sizeof(Matrix_t) * 10);

	while (strncmp(line,"exit", strlen("exit")  + 1) != 0) {
		
		if (!parse_user_input(line,&cmd)) {
			printf("ERROR\n\n");
		}
		
		run_commands(cmd,mats,10);

		free(line);
		destroy_commands(&cmd);
		line = readline("> ");
	}
	free(line);
	return 0;	
}
