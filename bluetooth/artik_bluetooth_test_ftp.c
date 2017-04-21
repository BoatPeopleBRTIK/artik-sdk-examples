#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <gio/gio.h>
#include <stdbool.h>
#include <errno.h>
#include <signal.h>
#include <ctype.h>
#include <inttypes.h>

#include <artik_module.h>
#include <artik_loop.h>
#include <artik_bluetooth.h>
#include "artik_bluetooth_test_commandline.h"

#define MAX_BDADDR_LEN			17
#define MAX_PACKET_SIZE			1024
#define SCAN_TIME_MILLISECONDS	(20*1000)
#define BUFFER_LEN				128

static artik_loop_module *loop_main;

typedef void (*signal_fuc)(int);

void uninit(int signal)
{
	fprintf(stdout, "<FTP>: Process cancel\n");
	loop_main->quit();
}

static void prop_callback(artik_bt_event event, void *data, void *user_data)
{
	artik_bt_ftp_property *p = (artik_bt_ftp_property *)data;

	fprintf(stdout, "Name: %s\n", p->name);
	fprintf(stdout, "File Name: %s\n", p->file_name);
	fprintf(stdout, "Status: %s\t", p->status);
	fprintf(stdout, "Size: %"PRIu64"/%"PRIu64"\n", p->transfered, p->size);
}

static void prv_list(char *buffer, void *user_data)
{
	artik_error ret = S_OK;
	artik_bt_ftp_file *file_list;
	artik_bluetooth_module *bt = (artik_bluetooth_module *)
		artik_request_api_module("bluetooth");

	fprintf(stdout, "Start testing list file\n");
	ret = bt->ftp_list_folder(&file_list);
	if (ret != S_OK) {
		fprintf(stdout, "ftp list file failed !\n");
		return;
	}
	fprintf(stdout, "ftp list file succeeded !\n");
	while (file_list != NULL) {
		fprintf(stdout, "Type: %s\t", file_list->file_type);
		fprintf(stdout, "Permission: %s\t", file_list->file_permission);
		if (file_list->size < 10)
			fprintf(stdout, "Size: %"PRIu64"\t\t", file_list->size);
		else
			fprintf(stdout, "Size: %"PRIu64"\t", file_list->size);
		fprintf(stdout, "Name: %s\n", file_list->file_name);
		file_list = file_list->next_file;
	}
	artik_release_api_module(bt);
}

static void prv_get(char *buffer, void *user_data)
{
	artik_error ret = S_OK;
	char **argv = NULL;
	artik_bluetooth_module *bt = (artik_bluetooth_module *)
		artik_request_api_module("bluetooth");

	if (strlen(buffer) > 0) {
		char *arg = malloc(strlen(buffer));

		strncpy(arg, buffer, strlen(buffer) - 1);
		arg[strlen(buffer) - 1] = '\0';
		argv = g_strsplit(arg, " ", -1);
	}

	fprintf(stdout, "Start testing download file from %s to %s...\n",
		argv[0], argv[1]);
	ret = bt->ftp_get_file(argv[1], argv[0]);
	if (ret == E_BUSY)
		fprintf(stdout, "Transfer in progress\n");
	else if (ret != S_OK)
		fprintf(stdout, "ftp download file failed !\n");
	else
		fprintf(stdout, "ftp download file succeeded !\n");
	g_strfreev(argv);
	artik_release_api_module(bt);
}

static void prv_put(char *buffer, void *user_data)
{
	artik_error ret = S_OK;
	char **argv = NULL;
	artik_bluetooth_module *bt = (artik_bluetooth_module *)
		artik_request_api_module("bluetooth");

	if (strlen(buffer) > 0) {
		char *arg = malloc(strlen(buffer));

		strncpy(arg, buffer, strlen(buffer) - 1);
		arg[strlen(buffer) - 1] = '\0';
		argv = g_strsplit(arg, " ", -1);
	}

	fprintf(stdout, "Start testing upload file from %s to %s...\n",
		argv[0], argv[1]);
	ret = bt->ftp_put_file(argv[0], argv[1]);
	if (ret == E_BUSY)
		fprintf(stdout, "Transfer in progress\n");
	else if (ret != S_OK)
		fprintf(stdout, "ftp upload file failed !\n");
	else
		fprintf(stdout, "ftp upload file succeeded !\n");
	g_strfreev(argv);
	artik_release_api_module(bt);
}

static void prv_change(char *buffer, void *user_data)
{
	artik_error ret = S_OK;
	char *folder = NULL;
	artik_bluetooth_module *bt = (artik_bluetooth_module *)
		artik_request_api_module("bluetooth");

	folder = (char *)malloc(strlen(buffer));
	if (folder == NULL)
		goto quit;
	strncpy(folder, buffer, strlen(buffer));
	folder[strlen(buffer) - 1] = '\0';

	fprintf(stdout, "Start testing change folder to %s...\n", folder);
	ret = bt->ftp_change_folder(folder);
	if (ret != S_OK)
		fprintf(stdout, "ftp change folder failed !\n");
	else
		fprintf(stdout, "ftp change folder succeeded !\n");
	free(folder);
quit:
	artik_release_api_module(bt);
}

static void prv_create(char *buffer, void *user_data)
{
	artik_error ret = S_OK;
	char *folder = NULL;
	artik_bluetooth_module *bt = (artik_bluetooth_module *)
		artik_request_api_module("bluetooth");

	folder = (char *)malloc(strlen(buffer));
	if (folder == NULL)
		goto quit;
	strncpy(folder, buffer, strlen(buffer));
	folder[strlen(buffer) - 1] = '\0';

	fprintf(stdout, "Start testing create folder %s...\n", folder);
	ret = bt->ftp_create_folder(folder);
	if (ret != S_OK)
		fprintf(stdout, "ftp create folder failed !\n");
	else
		fprintf(stdout, "ftp create folder succeeded !\n");
	free(folder);
quit:
	artik_release_api_module(bt);
}

static void prv_quit(char *buffer, void *user_data)
{
	artik_error ret = S_OK;
	artik_bluetooth_module *bt = (artik_bluetooth_module *)
		artik_request_api_module("bluetooth");

	ret = bt->ftp_remove_session();
	if (ret != S_OK)
		fprintf(stdout, "<FTP>: Remove session failed !\n");
	else
		fprintf(stdout, "<FTP>: Remove session success!\n");
	ret = artik_release_api_module(bt);
	if (ret != S_OK)
		fprintf(stdout, "<FTP>: release bt module error!\n");
	loop_main->quit();
}

static void prv_resume(char *buffer, void *user_data)
{
	artik_bluetooth_module *bt = (artik_bluetooth_module *)
		artik_request_api_module("bluetooth");
	bt->ftp_resume_transfer();
	artik_release_api_module(bt);
}

static void prv_suspend(char *buffer, void *user_data)
{
	artik_bluetooth_module *bt = (artik_bluetooth_module *)
		artik_request_api_module("bluetooth");
	bt->ftp_suspend_transfer();
	artik_release_api_module(bt);
}

static void prv_delete(char *buffer, void *user_data)
{
	artik_error ret = S_OK;
	char *file = NULL;
	artik_bluetooth_module *bt = (artik_bluetooth_module *)
		artik_request_api_module("bluetooth");

	file = (char *)malloc(strlen(buffer));
	if (file == NULL)
		goto quit;
	strncpy(file, buffer, strlen(buffer));
	file[strlen(buffer) - 1] = '\0';

	fprintf(stdout, "Start testing delete file %s...\n", file);
	ret = bt->ftp_delete_file(file);
	if (ret != S_OK)
		fprintf(stdout, "ftp delete file failed !\n");
	else
		fprintf(stdout, "ftp delete file succeeded !\n");
	free(file);
quit:
	artik_release_api_module(bt);
}

command_desc_t commands[] = {
		{ "get", "Get file.", NULL, prv_get, NULL},
		{ "put", "Put file.", NULL, prv_put, NULL},
		{ "ls", "List files.", NULL, prv_list, NULL },
		{ "delete", "Delete file.", NULL, prv_delete, NULL},
		{ "cd", "Change folder.", NULL, prv_change, NULL},
		{ "mkdir", "Create folder.", NULL, prv_create, NULL},
		{ "resume", "Resume transfer file.", NULL, prv_resume, NULL},
		{ "suspend", "Suspend transfer file.", NULL, prv_suspend, NULL},
		{ "quit", "Quit.", NULL, prv_quit, NULL },

		COMMAND_END_LIST
};

static int on_keyboard_received(int fd, enum watch_io id, void *user_data)
{
	char buffer[MAX_PACKET_SIZE];

	if (fgets(buffer, MAX_PACKET_SIZE, stdin) == NULL)
		return 1;
	handle_command(commands, (char *) buffer);
	fprintf(stdout, "\r\n");
	return 1;
}

static void print_devices(artik_bt_device *devices, int num)
{
	int i = 0;
	char *re_name;

	for (i = 0; i < num; i++) {
		fprintf(stdout, "[Device]: %s\t",
			devices[i].remote_address ? devices[i].
			remote_address : "(null)");
		re_name = (devices[i].remote_name ? devices[i].
			remote_name : "(null)");
		if (strlen(re_name) < 8) {
			fprintf(stdout, "%s\t\t",
				devices[i].remote_name ? devices[i].
				remote_name : "(null)");
			}
		else{
			fprintf(stdout, "%s\t",
				devices[i].remote_name ? devices[i].
				remote_name : "(null)");
			}
		fprintf(stdout, "RSSI: %d\t", devices[i].rssi);
		fprintf(stdout, "Bonded: %s\n",
			devices[i].is_bonded ? "true" : "false");
	}
}

static void on_scan(void *data, void *user_data)
{
	artik_bt_device *dev = (artik_bt_device *) data;

	print_devices(dev, 1);
}

static void on_bond(void *data, void *user_data)
{
	artik_bluetooth_module *bt = (artik_bluetooth_module *)
		artik_request_api_module("bluetooth");
	char *remote_address = (char *)user_data;
	bool paired = *(bool *)data;

	if (paired) {
		fprintf(stdout, "<FTP>: %s - %s\n", __func__, "Paired");
		fprintf(stdout, "<FTP>: %s - start session\n", __func__);

		if (bt->ftp_create_session(remote_address) != S_OK)
			fprintf(stdout, "<FTP>: Start session error!\n");

		fprintf(stdout, "<FTP>: Start session success!\n");
		loop_main->add_fd_watch(STDIN_FILENO,
			(WATCH_IO_IN | WATCH_IO_ERR | WATCH_IO_HUP
			| WATCH_IO_NVAL),
			on_keyboard_received, NULL, NULL);
	} else {
		fprintf(stdout, "<FTP>: %s - %s\n", __func__, "Unpaired");
	}
	artik_release_api_module(bt);
}

static void user_callback(artik_bt_event event, void *data, void *user_data)
{
	switch (event) {
	case BT_EVENT_SCAN:
		on_scan(data, user_data);
		break;
	case BT_EVENT_BOND:
		on_bond(data, user_data);
		break;
	default:
		break;
	}
}

static void scan_timeout_callback(void *user_data)
{
	artik_loop_module *loop = (artik_loop_module *) user_data;

	fprintf(stdout, "<FTP>: %s - stop scan\n", __func__);
	loop->quit();
}

artik_error bluetooth_scan(void)
{
	artik_loop_module *loop = (artik_loop_module *)
		artik_request_api_module("loop");
	artik_bluetooth_module *bt = (artik_bluetooth_module *)
		artik_request_api_module("bluetooth");
	artik_error ret = S_OK;
	int timeout_id = 0;

	fprintf(stdout, "<FTP>: %s - starting\n", __func__);

	ret = bt->remove_devices();
	if (ret != S_OK)
		goto exit;

	ret = bt->set_callback(BT_EVENT_SCAN, user_callback, NULL);
	if (ret != S_OK)
		goto exit;

	ret = bt->start_scan();
	if (ret != S_OK)
		goto exit;

	loop->add_timeout_callback(&timeout_id,
		SCAN_TIME_MILLISECONDS, scan_timeout_callback,
		(void *)loop);
	loop->run();

exit:
	ret = bt->stop_scan();
	ret = bt->unset_callback(BT_EVENT_SCAN);
	fprintf(stdout, "<FTP>: %s - %s\n", __func__,
		(ret == S_OK) ? "succeeded" : "failed");

	artik_release_api_module(loop);
	artik_release_api_module(bt);
	return ret;
}

artik_error get_addr(char *remote_addr)
{
	char mac_other[2] = "";
	artik_error ret = S_OK;

	fprintf(stdout, "\n<FTP>: Input Server MAC address:\n");
	if (fgets(remote_addr, MAX_BDADDR_LEN + 1, stdin) == NULL)
		return E_BT_ERROR;
	if (fgets(mac_other, 2, stdin) == NULL)
		return E_BT_ERROR;
	if (strlen(remote_addr) != MAX_BDADDR_LEN)
		ret =  E_BT_ERROR;
	return ret;
}

static artik_error set_callback(char *remote_addr)
{
	artik_bluetooth_module *bt = (artik_bluetooth_module *)
		artik_request_api_module("bluetooth");
	artik_error ret = S_OK;

	ret = bt->set_callback(BT_EVENT_BOND, user_callback,
		(void *)remote_addr);
	if (ret != S_OK)
		goto exit;

	ret = bt->set_callback(BT_EVENT_FTP, prop_callback, NULL);
	if (ret != S_OK)
		goto exit;

exit:
	artik_release_api_module(bt);
	return ret;
}

int main(int argc, char *argv[])
{
	artik_error ret = S_OK;
	signal_fuc signal_uninit = uninit;
	char remote_address[MAX_BDADDR_LEN] = "";
	artik_bluetooth_module *bt_main = NULL;

	if (!artik_is_module_available(ARTIK_MODULE_BLUETOOTH)) {
		fprintf(stdout,
			"<FTP>:Bluetooth is not available\n");
		goto loop_quit;
	}

	if (!artik_is_module_available(ARTIK_MODULE_LOOP)) {
		fprintf(stdout,
			"<FTP>:Loop module is not available\n");
		goto loop_quit;
	}
	bt_main = (artik_bluetooth_module *) artik_request_api_module("bluetooth");
	loop_main = (artik_loop_module *) artik_request_api_module("loop");
	if (!bt_main || !loop_main)
		goto loop_quit;

	ret = bluetooth_scan();
	if (ret != S_OK) {
		fprintf(stdout, "<FTP>: Bluetooth scan error!\n");
		goto loop_quit;
	}

	ret = get_addr(remote_address);
	if (ret != S_OK) {
		fprintf(stdout, "<FTP>: Bluetooth get address error!\n");
		goto loop_quit;
	}

	ret = set_callback(remote_address);
	if (ret != S_OK) {
		fprintf(stdout, "<FTP>: Bluetooth set callback error!\n");
		goto loop_quit;
	}

	bt_main->start_bond(remote_address);
	signal(SIGINT, signal_uninit);
	loop_main->run();

loop_quit:
	if (bt_main)
		artik_release_api_module(bt_main);
	if (loop_main)
		artik_release_api_module(loop_main);

	fprintf(stdout, "<FTP>: Quit FTP session ...!\n");
	return S_OK;
}
