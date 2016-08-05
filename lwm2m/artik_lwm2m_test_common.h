#include <artik_lwm2m.h>

typedef void (*command_handler_t)(char *args, void *user_data);

/** This struct is defined lwm2m command information  */
struct command_desc_t {
	char *name; /**< command name */
	char *shortDesc; /**< command short description */
	char *longDesc; /**< command detail description */
	command_handler_t callback; /**< command handler */
	void *user_data; /**< user data */
};

typedef struct _buffer {
	char * buffer;
	int offset;
} command;

void print_status(FILE *stream, uint8_t status);
void output_data(FILE *stream, artik_lwm2m_media_type_t format, uint8_t *data,
		int dataLength, int indent);
void handle_command(struct command_desc_t *commandArray, char *buffer);
void prv_init_command(command *cmd, char * buffer);
bool prv_isspace(char c);
void prv_print_error(uint8_t status);
int prv_next_str(command *cmd);
int prv_next_space(command *cmd);
int prv_read_id(command *cmd, uint16_t *id);
int prv_read_uri(command *cmd, char *uri);
int prv_read_data(command *cmd, char * data);
