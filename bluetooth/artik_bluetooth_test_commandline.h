#include <stdio.h>

#define COMMAND_END_LIST {NULL, NULL, NULL, NULL, NULL}

typedef void (*command_handler_t)(char *args, void *user_data);

typedef struct {
	char *name;
	char *short_desc;
	char *long_desc;
	command_handler_t call_back;
	void *user_data;
} command_desc_t;

void handle_command(command_desc_t *command_array, char *buffer);
char *get_end_of_arg(char *buffer);
char *get_next_arg(char *buffer, char **end);
int check_end_of_args(char *buffer);
