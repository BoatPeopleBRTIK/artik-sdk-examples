#include <stdio.h>
#include <unistd.h>
#include <ctype.h>

#include <artik_module.h>
#include <artik_platform.h>
#include <artik_loop.h>
#include <artik_lwm2m.h>
#include <artik_log.h>

#include "artik_lwm2m_test_common.h"

#define UNUSED __attribute__((unused))

#define MAX_PACKET_SIZE 1024
#define SERVER_CERT "certs/server-cert.pem"
#define SERVER_KEY "certs/server-key.pem"
#define STANDARD_PORT_STR LWM2M_STANDARD_PORT_STR
#define STANDARD_PORT     LWM2M_STANDARD_PORT
#define DTLS_PORT_STR     LWM2M_DTLS_PORT_STR
#define DTLS_PORT         LWM2M_DTLS_PORT
#define URI_MAX_LEN		32
#define DATA_MAX_LEN	1024


artik_loop_module *loop;
artik_lwm2m_module *lwm2m;
static int g_quit;


static void on_result_callback(UNUSED artik_lwm2m_handle handle,
		uint16_t client_id, const char *uri, int status,
		artik_lwm2m_media_type_t format, uint8_t *data, int data_length)
{
	fprintf(stdout, "\r\nClient #%d %s", client_id, uri);
	fprintf(stdout, " : ");
	print_status(stdout, status);
	fprintf(stdout, "\r\n");

	output_data(stdout, format, data, data_length, 1);

	fprintf(stdout, "\r\n> ");
	fflush(stdout);
}

static void on_notify_callback(UNUSED artik_lwm2m_handle handle,
		uint16_t client_id, const char *uri, int status,
		artik_lwm2m_media_type_t format, uint8_t *data, int data_length)
{
	fprintf(stdout, "\r\nNotify from client #%d %s", client_id, uri);
	fprintf(stdout, " : ");
	print_status(stdout, status);
	fprintf(stdout, "\r\n");

	fprintf(stdout, " number %d\r\n", status);

	output_data(stdout, format, data, data_length, 1);

	fprintf(stdout, "\r\n> ");
	fflush(stdout);
}


static void prv_output_clients(UNUSED char *buffer, void *user_data)
{
	artik_lwm2m_handle handle = (artik_lwm2m_handle) user_data;
	int i, count;
	artik_error result;
	artik_lwm2m_client_t client;

	result = lwm2m->server_get_client_count(handle, &count);
	if (result != S_OK)
		return;

	memset(&client, 0, sizeof(artik_lwm2m_client_t));
	for (i = 0; i < count; i++) {
		lwm2m->server_get_client(handle, i, &client);
		fprintf(stdout, "Client #%d\r\n", client.internalID);
		if (client.name != NULL)
			fprintf(stdout, "	name: %s\r\n", client.name);
	}
}

static void prv_read_client(char *buffer, void *user_data)
{
	artik_lwm2m_handle handle = (artik_lwm2m_handle) user_data;
	artik_error result;
	uint16_t client_id = -1;
	char uri[URI_MAX_LEN];
	command cmd;

	prv_init_command(&cmd, buffer);

	result = prv_read_id(&cmd, &client_id);
	if (result != S_OK)
		goto syntax_error;

	result = prv_read_uri(&cmd, uri);
	if (result != S_OK)
		goto syntax_error;

	result = lwm2m->server_read_client(handle, client_id, uri,
			on_result_callback);
	if (result == S_OK)
		fprintf(stdout, "OK");
	else
		prv_print_error(result);

	return;

syntax_error:
	fprintf(stdout, "Syntax error !");
}

static void prv_write_client(char *buffer, void *user_data)
{
	artik_lwm2m_handle handle = (artik_lwm2m_handle) user_data;
	artik_error result;
	uint16_t client_id = -1;
	char uri[URI_MAX_LEN];
	char data[DATA_MAX_LEN];
	command cmd;

	prv_init_command(&cmd, buffer);

	result = prv_read_id(&cmd, &client_id);
	if (result != S_OK)
		goto syntax_error;

	result = prv_read_uri(&cmd, uri);
	if (result != S_OK)
		goto syntax_error;

	result = prv_read_data(&cmd, data);
	if (result != S_OK)
		goto syntax_error;

	result = lwm2m->server_write_client(handle, client_id, uri,
			(uint8_t *) data, strlen(data), on_result_callback);
	if (result == S_OK)
		fprintf(stdout, "OK");
	else
		prv_print_error(result);

	return;

syntax_error:
	fprintf(stdout, "Syntax error !");
}

static void prv_exec_client(char *buffer, void *user_data)
{
	artik_lwm2m_handle handle = (artik_lwm2m_handle) user_data;
	artik_error result;
	uint16_t client_id = -1;
	char uri[URI_MAX_LEN];
	command cmd;

	prv_init_command(&cmd, buffer);

	result = prv_read_id(&cmd, &client_id);
	if (result != S_OK)
		goto syntax_error;

	result = prv_read_uri(&cmd, uri);
	if (result != S_OK)
		goto syntax_error;

	result = lwm2m->server_exec_client(handle, client_id, uri, NULL, 0,
			on_result_callback);
	if (result == S_OK)
		fprintf(stdout, "OK");
	else
		prv_print_error(result);

	return;

syntax_error:
	fprintf(stdout, "Syntax error !");
}

static void prv_create_client(char *buffer, void *user_data)
{
	artik_lwm2m_handle handle = (artik_lwm2m_handle) user_data;
	artik_error result;
	uint16_t client_id = -1;
	char uri[URI_MAX_LEN];
	char data[DATA_MAX_LEN];
	command cmd;

	prv_init_command(&cmd, buffer);

	result = prv_read_id(&cmd, &client_id);
	if (result != S_OK)
		goto syntax_error;

	result = prv_read_uri(&cmd, uri);
	if (result != S_OK)
		goto syntax_error;

	result = prv_read_data(&cmd, data);
	if (result != S_OK)
		goto syntax_error;

	result = lwm2m->server_create_client(handle, client_id, uri,
			(uint8_t *)data, strlen(data), on_result_callback);
	if (result == S_OK)
		fprintf(stdout, "OK");
	else
		prv_print_error(result);

	return;

syntax_error:
	fprintf(stdout, "Syntax error !");
}

static void prv_delete_client(char *buffer, void *user_data)
{
	artik_lwm2m_handle handle = (artik_lwm2m_handle) user_data;
	artik_error result;
	uint16_t client_id = -1;
	char uri[URI_MAX_LEN];
	command cmd;

	prv_init_command(&cmd, buffer);

	result = prv_read_id(&cmd, &client_id);
	if (result != S_OK)
		goto syntax_error;

	result = prv_read_uri(&cmd, uri);
	if (result != S_OK)
		goto syntax_error;

	result = lwm2m->server_delete_client(handle, client_id, uri,
			on_result_callback);
	if (result == LWM2M_SUCCESS)
		fprintf(stdout, "OK");
	else
		prv_print_error(result);

	return;

syntax_error:
	fprintf(stdout, "Syntax error !");
}

static void prv_observe_client(char *buffer, void *user_data)
{
	artik_lwm2m_handle handle = (artik_lwm2m_handle) user_data;
	artik_error result;
	uint16_t client_id = -1;
	char uri[URI_MAX_LEN];
	command cmd;

	prv_init_command(&cmd, buffer);

	result = prv_read_id(&cmd, &client_id);
	if (result != S_OK)
		goto syntax_error;

	result = prv_read_uri(&cmd, uri);
	if (result != S_OK)
		goto syntax_error;

	result = lwm2m->server_observe_client(handle, client_id, uri,
			on_notify_callback);
	if (result == LWM2M_SUCCESS)
		fprintf(stdout, "OK");
	else
		prv_print_error(result);

	return;

syntax_error:
	fprintf(stdout, "Syntax error !");
}

static void prv_cancel_client(char *buffer, void *user_data)
{
	artik_lwm2m_handle handle = (artik_lwm2m_handle) user_data;
	artik_error result;
	uint16_t client_id = -1;
	char uri[URI_MAX_LEN];
	command cmd;

	prv_init_command(&cmd, buffer);

	result = prv_read_id(&cmd, &client_id);
	if (result != S_OK)
		goto syntax_error;

	result = prv_read_uri(&cmd, uri);
	if (result != S_OK)
		goto syntax_error;

	result = lwm2m->server_observe_cancel_client(handle, client_id, uri,
			on_result_callback);
	if (result == LWM2M_SUCCESS)
		fprintf(stdout, "OK");
	else
		prv_print_error(result);

	return;

syntax_error:
	fprintf(stdout, "Syntax error !");
}

static void prv_quit(UNUSED char *buffer, UNUSED void *user_data)
{
	g_quit = 1;
	loop->quit();
}

struct command_desc_t commands[] = {
		{ "list", "List registered clients.", NULL, prv_output_clients,
		NULL },
				{ "read", "Read from a client.",
						" read CLIENT# URI\r\n"
								"   CLIENT#: client number as returned by command 'list'\r\n"
								"   URI: uri to read such as /3, /3//2, /3/0/2, /1024/11, /1024//1\r\n"
								"Result will be displayed asynchronously.",
						prv_read_client, NULL },
				{ "write", "Write to a client.",
						" write CLIENT# URI DATA\r\n"
								"   CLIENT#: client number as returned by command 'list'\r\n"
								"   URI: uri to write to such as /3, /3//2, /3/0/2, /1024/11, /1024//1\r\n"
								"   DATA: data to write\r\n"
								"Result will be displayed asynchronously.",
						prv_write_client, NULL },
				{ "exec", "Execute a client resource.",
						" exec CLIENT# URI\r\n"
								"   CLIENT#: client number as returned by command 'list'\r\n"
								"   URI: uri of the resource to execute such as /3/0/2\r\n"
								"Result will be displayed asynchronously.",
						prv_exec_client, NULL },
				{ "del", "Delete a client Object instance.",
						" del CLIENT# URI\r\n"
								"   CLIENT#: client number as returned by command 'list'\r\n"
								"   URI: uri of the instance to delete such as /1024/11\r\n"
								"Result will be displayed asynchronously.",
						prv_delete_client, NULL },
				{ "create", "create an Object instance.",
						" create CLIENT# URI DATA\r\n"
								"   CLIENT#: client number as returned by command 'list'\r\n"
								"   URI: uri to which create the Object Instance such as /1024, /1024/45 \r\n"
								"   DATA: data to initialize the new Object Instance (0-255 for object 1024) \r\n"
								"Result will be displayed asynchronously.",
						prv_create_client, NULL },
				{ "observe", "Observe from a client.",
						" observe CLIENT# URI\r\n"
								"   CLIENT#: client number as returned by command 'list'\r\n"
								"   URI: uri to observe such as /3, /3/0/2, /1024/11\r\n"
								"Result will be displayed asynchronously.",
						prv_observe_client, NULL },
				{ "cancel", "Cancel an observe.",
						" cancel CLIENT# URI\r\n"
								"   CLIENT#: client number as returned by command 'list'\r\n"
								"   URI: uri on which to cancel an observe such as /3, /3/0/2, /1024/11\r\n"
								"Result will be displayed asynchronously.",
						prv_cancel_client, NULL },

				{ "q", "Quit the server.", NULL, prv_quit, NULL },

				{ NULL, NULL, NULL, NULL, NULL } };

static void on_monitor_callback(UNUSED artik_lwm2m_handle handle,
		uint16_t client_id, UNUSED const char *uri, int status,
		UNUSED artik_lwm2m_media_type_t format,
		UNUSED uint8_t *data, UNUSED int data_length)
{

	switch (status) {
	case LWM2M_CREATED:
		fprintf(stdout, "\r\nNew client #%d registered.\r\n", client_id);
		break;

	case LWM2M_DELETED:
		fprintf(stdout, "\r\nClient #%d unregistered.\r\n", client_id);
		break;

	case LWM2M_CHANGED:
		fprintf(stdout, "\r\nClient #%d updated.\r\n", client_id);
		break;

	default:
		fprintf(stdout,
				"\r\nMonitor callback called with an unknown status: %d.\r\n",
				status);
		break;
	}

	fprintf(stdout, "\r\n> ");
	fflush(stdout);
}

static int on_keyboard_received(int fd, enum watch_io io,
		UNUSED void *user_data)
{
	char buffer[MAX_PACKET_SIZE];

	if (!(fd == STDIN_FILENO)) {
		dbg("%s STDIN_FILENO failed\n", __func__);
	}
	if (!(io == WATCH_IO_IN || io == WATCH_IO_ERR || io == WATCH_IO_HUP
			|| io == WATCH_IO_NVAL)) {
		dbg("%s io failed\n", __func__);
	}

	if (fgets(buffer, MAX_PACKET_SIZE, stdin) == NULL)
		return 1;

	handle_command(commands, buffer);
	fprintf(stdout, "\r\n");

	if (g_quit == 0) {
		fprintf(stdout, "> ");
		fflush(stdout);
	} else {
		fprintf(stdout, "\r\n");
	}

	return 1;
}

int main(int argc, char *argv[])
{
	artik_lwm2m_handle handle = NULL;
	int i;
	char *server_port = STANDARD_PORT_STR;
	int opt;
	bool dtls_enable = false;
	artik_error ret = S_OK;
	int watch_id;

	while ((opt = getopt(argc, argv, "s:b:h:")) != -1) {
		switch (opt) {
		case 's':
			if (strcmp(optarg, "dtls") == 0) {
				dtls_enable = true;
				server_port = DTLS_PORT_STR;
			} else if (strcmp(optarg, "nodtls") == 0) {
				dtls_enable = false;
			}
			break;
		default:
			printf("Usage: lwm2mserver\r\n");
			return 0;
		}
	}

	if (!artik_is_module_available(ARTIK_MODULE_LOOP)) {
		dbg("TEST: Loop module is not available, skipping test...\n");
		return -1;
	}

	if (!artik_is_module_available(ARTIK_MODULE_LWM2M)) {
		dbg("TEST: LWM2M module is not available, skipping test...\n");
		return -1;
	}

	loop = (artik_loop_module *) artik_request_api_module("loop");
	lwm2m = (artik_lwm2m_module *) artik_request_api_module("lwm2m");

	if (dtls_enable) {
		ret = lwm2m->server_create(&handle, server_port, SERVER_CERT,
		SERVER_KEY);
		if (ret != S_OK)
			goto exit;
	} else {
		ret = lwm2m->server_create(&handle, server_port, NULL, NULL);
		if (ret != S_OK)
			goto exit;
	}

	ret = lwm2m->server_set_monitoring_callback(handle, on_monitor_callback);
	if (ret != S_OK)
		goto exit;

	for (i = 0; commands[i].name != NULL; i++)
		commands[i].user_data = (void *) handle;

	loop->add_fd_watch(STDIN_FILENO,
			(WATCH_IO_IN | WATCH_IO_ERR | WATCH_IO_HUP | WATCH_IO_NVAL),
			on_keyboard_received, handle, &watch_id);

	printf(">");
	loop->run();

exit:
	if (lwm2m != NULL) {
		if (handle != NULL)
			lwm2m->server_destroy(handle);
	}

	return (ret == S_OK) ? 0 : -1;
}

