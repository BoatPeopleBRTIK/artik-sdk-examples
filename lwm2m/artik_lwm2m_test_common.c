
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

int prv_read_id(command *cmd, uint16_t *id)
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

void prv_print_error(uint8_t status)
{
	err("Error: ");
	print_status(stdout, status);
	err("\r\n");
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

void output_buffer(FILE *stream, uint8_t *buffer, int length, int indent)
{
	int i;

	if (length == 0)
		fprintf(stream, "\n");

	i = 0;
	while (i < length) {
		uint8_t array[16];
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

void output_tlv(FILE *stream, uint8_t *buffer, size_t buffer_len, int indent)
{
	lwm2m_tlv_type_t type;
	uint16_t id;
	size_t dataIndex;
	size_t dataLen;
	int length = 0;
	int result;

	while (0
			!= (result = lwm2m_decodeTLV((uint8_t *) buffer + length,
					buffer_len - length, &type, &id, &dataIndex, &dataLen))) {
		print_indent(stream, indent);
		fprintf(stream, "{\r\n");
		print_indent(stream, indent + 1);
		fprintf(stream, "ID: %d", id);

		fprintf(stream, " type: ");
		switch (type) {
		case LWM2M_TYPE_OBJECT_INSTANCE:
			fprintf(stream, "Object Instance");
			break;
		case LWM2M_TYPE_RESOURCE_INSTANCE:
			fprintf(stream, "Resource Instance");
			break;
		case LWM2M_TYPE_MULTIPLE_RESOURCE:
			fprintf(stream, "Multiple Instances");
			break;
		case LWM2M_TYPE_RESOURCE:
			fprintf(stream, "Resource");
			break;
		default:
			dbg("unknown (%d)", (int) type);
			break;
		}
		fprintf(stream, "\n");

		print_indent(stream, indent + 1);
		fprintf(stream, "{\n");
		if (type == LWM2M_TYPE_OBJECT_INSTANCE
				|| type == LWM2M_TYPE_MULTIPLE_RESOURCE) {
			output_tlv(stream, buffer + length + dataIndex, dataLen,
					indent + 1);
		} else {
			int64_t intValue;
			double floatValue;

			print_indent(stream, indent + 2);
			fprintf(stream, "data (%zu bytes):\r\n", dataLen);
			output_buffer(stream, (uint8_t *) buffer + length + dataIndex,
					dataLen, indent + 2);

			if (0
					< lwm2m_opaqueToInt(buffer + length + dataIndex, dataLen,
							&intValue)) {
				print_indent(stream, indent + 2);
				fprintf(stream, "data as Integer: %" PRId64 "\r\n", intValue);
			}
			if (0
					< lwm2m_opaqueToFloat(buffer + length + dataIndex, dataLen,
							&floatValue)) {
				print_indent(stream, indent + 2);
				fprintf(stream, "data as Float: %.16g\r\n", floatValue);
			}
		}
		print_indent(stream, indent + 1);
		fprintf(stream, "}\r\n");
		length += result;
		print_indent(stream, indent);
		fprintf(stream, "}\r\n");
	}
}

void output_data(FILE *stream, artik_lwm2m_media_type_t format, uint8_t *data,
		int dataLength, int indent)
{
	int i;

	if (data == NULL)
		return;

	print_indent(stream, indent);
	fprintf(stream, "%d bytes received of type ", dataLength);
	switch (format) {
	case LWM2M_CONTENT_TEXT:
		fprintf(stream, "text/plain:\r\n");
		output_buffer(stream, data, dataLength, indent);
		break;

	case LWM2M_CONTENT_OPAQUE:
		fprintf(stream, "application/octet-stream:\r\n");
		output_buffer(stream, data, dataLength, indent);
		break;

	case LWM2M_CONTENT_TLV:
		fprintf(stream, "application/vnd.oma.lwm2m+tlv:\r\n");
		output_tlv(stream, data, dataLength, indent);
		break;

	case LWM2M_CONTENT_JSON:
		fprintf(stream, "application/vnd.oma.lwm2m+json:\r\n");
		print_indent(stream, indent);
		for (i = 0; i < dataLength; i++)
			fprintf(stream, "%c", data[i]);
		fprintf(stream, "\n");
		break;

	default:
		fprintf(stream, "Unknown (%d):\r\n", format);
		output_buffer(stream, data, dataLength, indent);
		break;
	}
}









#define CODE_TO_STRING(X)  #X

static const char *prv_status_to_string(int status)
{
	switch (status) {
	case COAP_NO_ERROR:
		return CODE_TO_STRING(COAP_NO_ERROR);
	case COAP_IGNORE:
		return CODE_TO_STRING(COAP_IGNORE);
	case COAP_201_CREATED:
		return CODE_TO_STRING(COAP_201_CREATED);
	case COAP_202_DELETED:
		return CODE_TO_STRING(COAP_202_DELETED);
	case COAP_204_CHANGED:
		return CODE_TO_STRING(COAP_204_CHANGED);
	case COAP_205_CONTENT:
		return CODE_TO_STRING(COAP_205_CONTENT);
	case COAP_400_BAD_REQUEST:
		return CODE_TO_STRING(COAP_400_BAD_REQUEST);
	case COAP_401_UNAUTHORIZED:
		return CODE_TO_STRING(COAP_401_UNAUTHORIZED);
	case COAP_404_NOT_FOUND:
		return CODE_TO_STRING(COAP_404_NOT_FOUND);
	case COAP_405_METHOD_NOT_ALLOWED:
		return CODE_TO_STRING(COAP_405_METHOD_NOT_ALLOWED);
	case COAP_406_NOT_ACCEPTABLE:
		return CODE_TO_STRING(COAP_406_NOT_ACCEPTABLE);
	case COAP_500_INTERNAL_SERVER_ERROR:
		return CODE_TO_STRING(COAP_500_INTERNAL_SERVER_ERROR);
	case COAP_501_NOT_IMPLEMENTED:
		return CODE_TO_STRING(COAP_501_NOT_IMPLEMENTED);
	case COAP_503_SERVICE_UNAVAILABLE:
		return CODE_TO_STRING(COAP_503_SERVICE_UNAVAILABLE);
	default:
		return "";
	}
}
void print_status(FILE *stream, uint8_t status)
{
	fprintf(stream, "%d.%02d (%s)", (status & 0xE0) >> 5, status & 0x1F,
			prv_status_to_string(status));
}
