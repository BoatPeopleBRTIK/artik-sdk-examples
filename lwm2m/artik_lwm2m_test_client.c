#include <stdio.h>
#include <unistd.h>
#include <ctype.h>

#include <artik_module.h>
#include <artik_platform.h>
#include <artik_loop.h>
#include <artik_lwm2m.h>
#include <artik_log.h>

#include "artik_lwm2m_test_common.h"

#define CLIENT_CERT "certs/client-cert.pem"
#define CLIENT_KEY "certs/client-key.pem"

#define UNUSED __attribute__((unused))

#define OBJ_COUNT 7
#define URI_MAX_LEN		32
#define DATA_MAX_LEN	1024
#define MAX_PACKET_SIZE 1024
#define DTLS_PORT_STR   "5684"

artik_loop_module *loop;
artik_lwm2m_module *lwm2m;

static int g_quit;

static void prv_update_client(char *buffer, void *user_data)
{
	artik_lwm2m_handle handle = (artik_lwm2m_handle) user_data;
	artik_error result;
	unsigned short server_id = -1;
	command cmd;

	prv_init_command(&cmd, buffer);

	result = prv_read_id(&cmd, &server_id);
	if (result != S_OK)
		goto syntax_error;

	result = lwm2m->client_update(handle, server_id);
	if (result == S_OK)
		err("client update failed (%s)", error_msg(result));
	else
		fprintf(stdout, "OK");

	return;

syntax_error:
	fprintf(stdout, "Syntax error !");
}

static void prv_change_obj(char *buffer, void *user_data)
{
	artik_lwm2m_handle handle = (artik_lwm2m_handle) user_data;
	artik_error result;
	char uri[URI_MAX_LEN];
	char data[DATA_MAX_LEN];
	command cmd;

	prv_init_command(&cmd, buffer);

	result = prv_read_uri(&cmd, uri);
	if (result != S_OK)
		goto syntax_error;

	result = prv_read_data(&cmd, data);

	if (result != S_OK)
		goto syntax_error;

	result = lwm2m->client_change_object(handle, uri, (unsigned char *)data,
			strlen(data));
	if (result != S_OK)
		err("client change object failed (%s)", error_msg(result));
	else
		fprintf(stdout, "OK");

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
				{ "update", "Trigger a registration update", NULL,
								prv_update_client, NULL },
				{ "change", "Change the value of resource.", NULL,
								prv_change_obj, NULL },

				{ "q", "Quit the client.", NULL, prv_quit, NULL },
				{ NULL, NULL, NULL, NULL, NULL } };


static int on_keyboard_received(int fd, enum watch_io io,
		UNUSED void *user_data)
{
	char buffer[MAX_PACKET_SIZE];

	if (!(fd == STDIN_FILENO)) {
		err("%s STDIN_FILENO failed\n", __func__);
	}

	if (!(io == WATCH_IO_IN || io == WATCH_IO_ERR || io == WATCH_IO_HUP
			|| io == WATCH_IO_NVAL)) {
		err("%s io failed\n", __func__);
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

artik_error test_lwm2m_default(bool dtls_enable)
{
	artik_error ret = S_OK;
	artik_lwm2m_handle client_h = NULL;
	artik_lwm2m_config config;

	int i = 0;
	int watch_id;
	int srv_port;

	fprintf(stdout, "TEST: %s starting\n", __func__);

	config.server_id = 123;
	config.server_ip = "127.0.0.1";
	config.server_port = "5683";
	config.local_port = "56830";
	config.name = "testlwm2mclient";
	config.lifetime = 3;
	config.keep_registration = true;
	config.storing = false;
	config.is_bootstrap = false;
	config.dtls_cert_path = NULL;
	config.dtls_key_path = NULL;

	srv_port = atoi(config.server_port);

	if(!dtls_enable) {
		ret = lwm2m->client_create(&client_h, &config);
	} else {
	/* for dtls mode */
		config.server_port = DTLS_PORT_STR;
		srv_port = atoi(config.server_port);
		ret = lwm2m->client_create(&client_h, &config);
	}
	if (ret != S_OK)
		goto exit;

	for (i = 0; commands[i].name != NULL; i++)
		commands[i].user_data = (void *) client_h;

	loop->add_fd_watch(STDIN_FILENO,
			(WATCH_IO_IN | WATCH_IO_ERR | WATCH_IO_HUP | WATCH_IO_NVAL),
			on_keyboard_received, client_h, &watch_id);

	printf(">");

	loop->run();

exit:
	fprintf(stdout, "TEST: %s %s\n", __func__,
			(ret == S_OK) ? "succeeded" : "failed");
	return ret;
}

int main(UNUSED int argc, UNUSED char *argv[])
{
	int opt;
	artik_error ret = S_OK;

	bool dtls_enable = false;

	while ((opt = getopt(argc, argv, "s:b:h:")) != -1) {
		switch (opt) {
		case 's':
			if (strcmp(optarg, "dtls") == 0) {
				dtls_enable = true;
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
		fprintf(stdout,
				"TEST: Loop module is not available, skipping test...\n");
		return -1;
	}

	if (!artik_is_module_available(ARTIK_MODULE_LWM2M)) {
		fprintf(stdout,
				"TEST: LWM2M module is not available, skipping test...\n");
		return -1;
	}

	loop = (artik_loop_module *) artik_request_api_module("loop");
	lwm2m = (artik_lwm2m_module *) artik_request_api_module("lwm2m");

	ret = test_lwm2m_default(dtls_enable);

	return (ret == S_OK) ? 0 : -1;
}
