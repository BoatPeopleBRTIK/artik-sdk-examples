
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <unistd.h>
#include <inttypes.h>

#include <artik_lwm2m.h>
#include <artik_module.h>
#include <artik_platform.h>
#include <artik_log.h>
#include "artik_lwm2m_test_common.h"

#define HELP_COMMAND "help"
#define HELP_DESC    "Type '"HELP_COMMAND" [COMMAND]' for more details."
#define UNKNOWN_CMD_MSG "Unknown command. Type '"HELP_COMMAND"' for help."


void prv_init_command(command *cmd, char *buffer)
{
	cmd->buffer = buffer;
	cmd->offset = 0;
}

bool prv_isspace(char c)
{
	bool result;

	result = isspace(c&0xff);

	return result;
}

int prv_next_str(command *cmd)
{
	int offset = cmd->offset;
	char c;
	artik_error result;

	do {
		offset++;
		c = cmd->buffer[offset];
		if (c == 0) {
			result = E_LWM2M_ERROR;
			break;
		}

		if (prv_isspace(c) == false) {
			result = S_OK;
			cmd->offset = offset;
			break;
		}
	} while (true);

	return result;
}

int prv_next_space(command *cmd)
{
	int offset = cmd->offset;
	char c;
	artik_error result;

	do {
		offset++;
		c = cmd->buffer[offset];
		if (c == 0) {
			result = E_LWM2M_ERROR;
			break;
		}

		if (prv_isspace(c) == true) {
			result = S_OK;
			cmd->offset = offset;
			break;
		}
	} while (true);

	return result;
}

int prv_read_id(command *cmd, unsigned short *id)
{
	char *str;
	int value;

	str = cmd->buffer + cmd->offset;
	if (str[0] == 0)
		return E_LWM2M_ERROR;

	value = atoi(str);
	*id = value;

	prv_next_space(cmd);
	prv_next_str(cmd);

	return S_OK;
}

int prv_read_uri(command *cmd, char *uri)
{
	char *str;
	char c;
	int i;

	str = cmd->buffer + cmd->offset;
	for (i = 0;; i++) {
		c = str[i];
		if (c == 0 || prv_isspace(c) == true) {
			uri[i] = 0;
			break;
		}

		uri[i] = c;
	}

	if (i == 0) {
		return E_LWM2M_ERROR;
	} else {
		cmd->offset += i;
		prv_next_str(cmd);
		return S_OK;
	}
}

int prv_read_data(command *cmd, char *data)
{
	char *str;
	char c;
	int i;

	str = cmd->buffer + cmd->offset;
	for (i = 0;; i++) {
		c = str[i];
		if (c == 0 || prv_isspace(c) == true) {
			data[i] = 0;
			break;
		}

		data[i] = c;
	}

	if (i == 0) {
		return E_LWM2M_ERROR;
	} else {
		cmd->offset += i;
		return S_OK;
	}
}

static struct command_desc_t *prv_find_command(
		struct command_desc_t *commandArray, char *buffer, size_t length)
{
	int i;

	if (length == 0)
		return NULL;

	i = 0;
	while (commandArray[i].name != NULL
			&& (strlen(commandArray[i].name) != length
					|| strncmp(buffer,
							commandArray[i].name, length))) {
		i++;
	}

	if (commandArray[i].name == NULL)
		return NULL;
	else
		return &commandArray[i];
}

static void prv_displayHelp(struct command_desc_t *commandArray, char *buffer)
{
	struct command_desc_t *cmdP;
	int length;

	/* find end of first argument */
	length = 0;
	while (buffer[length] != 0 && !isspace(buffer[length] & 0xff))
		length++;

	cmdP = prv_find_command(commandArray, buffer, length);

	if (cmdP == NULL) {
		int i;

		fprintf(stdout, HELP_COMMAND"\t"HELP_DESC"\r\n");

		for (i = 0; commandArray[i].name != NULL; i++) {
			fprintf(stdout, "%s\t%s\r\n", commandArray[i].name,
					commandArray[i].shortDesc);
		}
	} else {
		fprintf(stdout, "%s\r\n",
				cmdP->longDesc ? cmdP->longDesc : cmdP->shortDesc);
	}
}

void handle_command(struct command_desc_t *commandArray, char *buffer)
{
	struct command_desc_t *cmdP;
	int length;

	/* find end of command name */
	length = 0;
	while (buffer[length] != 0 && !isspace(buffer[length] & 0xFF))
		length++;

	cmdP = prv_find_command(commandArray, buffer, length);
	if (cmdP != NULL) {
		while (buffer[length] != 0 && isspace(buffer[length] & 0xFF))
			length++;
		cmdP->callback(buffer + length, cmdP->user_data);
	} else {
		if (!strncmp(buffer, HELP_COMMAND, length)) {
			while (buffer[length] != 0 && isspace(buffer[length] & 0xFF))
				length++;
			prv_displayHelp(commandArray, buffer + length);
		} else {
			fprintf(stdout, UNKNOWN_CMD_MSG"\r\n");
		}
	}
}


static void print_indent(FILE *stream, int num)
{
	int i;

	for (i = 0; i < num; i++)
		fprintf(stream, "    ");
}

void output_buffer(FILE *stream, unsigned char *buffer, int length, int indent)
{
	int i;

	if (length == 0)
		fprintf(stream, "\n");

	i = 0;
	while (i < length) {
		unsigned char array[16];
		int j;

		print_indent(stream, indent);
		memcpy(array, buffer + i, 16);
		for (j = 0; j < 16 && i + j < length; j++) {
			fprintf(stream, "%02X ", array[j]);
			if (j % 4 == 3)
				fprintf(stream, " ");
		}
		if (length > 16) {
			while (j < 16) {
				fprintf(stream, "   ");
				if (j % 4 == 3)
					fprintf(stream, " ");
				j++;
			}
		}
		fprintf(stream, " ");
		for (j = 0; j < 16 && i + j < length; j++) {
			if (isprint(array[j]))
				fprintf(stream, "%c", array[j]);
			else
				fprintf(stream, ".");
		}
		fprintf(stream, "\n");
		i += 16;
	}
}
