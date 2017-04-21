#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <gio/gio.h>
#include <stdbool.h>
#include <errno.h>
#include <signal.h>

#include <artik_module.h>
#include <artik_loop.h>
#include <artik_bluetooth.h>

#define MAX_BDADDR_LEN 17
#define BUFFER_LEN 128
#define SCAN_TIME_MILLISECONDS (20*1000)

typedef void (*signal_fuc)(int);

static artik_bluetooth_module *bt;
static artik_loop_module *loop;
static artik_error err;
static char *remote_address;

static void user_callback(artik_bt_event event, void *data, void *user_data);

void uninit(int signal)
{
	err = bt->pan_disconnect();
	if (err != S_OK)
		printf("Disconnect Error:%d!\r\n", err);
	else
		printf("Disconnect OK!\r\n");
	loop->quit();
}

static void print_devices(artik_bt_device *devices, int num)
{
	int i = 0, j = 0;

	for (i = 0; i < num; i++) {
		fprintf(stdout, "Address: %s\n",
			devices[i].remote_address ? devices[i].
			remote_address : "(null)");
		fprintf(stdout, "Name: %s\n",
			devices[i].remote_name ? devices[i].
			remote_name : "(null)");
		fprintf(stdout, "RSSI: %d\n", devices[i].rssi);
		fprintf(stdout, "Bonded: %s\n",
			devices[i].is_bonded ? "true" : "false");
		fprintf(stdout, "Connected: %s\n",
			devices[i].is_connected ? "true" : "false");
		fprintf(stdout, "Authorized: %s\n",
			devices[i].is_authorized ? "true" : "false");
		fprintf(stdout, "Class:\n");
		fprintf(stdout, "\tMajor:0x%02x\n", devices[i].cod.major);
		fprintf(stdout, "\tMinor:0x%02x\n", devices[i].cod.minor);
		fprintf(stdout, "\tService:0x%04x\n",
			devices[i].cod.service_class);
		if (devices[i].uuid_length > 0) {
			fprintf(stdout, "UUIDs:\n");
			for (j = 0; j < devices[i].uuid_length; j++) {
				fprintf(stdout, "\t%s [%s]\n",
					devices[i].uuid_list[j].uuid_name,
					devices[i].uuid_list[j].uuid);
			}
		}

		fprintf(stdout, "\n");
	}
}

static void on_scan(void *data, void *user_data)
{
	artik_bt_device *dev = (artik_bt_device *) data;

	print_devices(dev, 1);
}

static void on_connect(void)
{
	char buf[BUFFER_LEN];
	char *interface = NULL;

	if (S_OK == bt->pan_get_interface(&interface)) {
		snprintf(buf, BUFFER_LEN, "dhclient -r %s", interface);
		if (system(buf) < 0) {
			fprintf(stdout, "cmd system error\n");
			return;
		}
		snprintf(buf, BUFFER_LEN, "dhclient %s", interface);
		if (system(buf) < 0) {
			fprintf(stdout, "cmd system error\n");
			return;
		}
		snprintf(buf, BUFFER_LEN, "ifconfig eth0 down");
		if (system(buf) < 0) {
			fprintf(stdout, "cmd system error\n");
			return;
		}
		fprintf(stdout, "Please input test command(max length is 127) "
			"or 'q' to exit\n");
		for (;;) {
			memset(buf, 0, BUFFER_LEN);
			if (fgets(buf, BUFFER_LEN, stdin) == NULL) {
				fprintf(stdout, "cmd system error\n");
				break;
			}
			if (strlen(buf) > 1) {
				if (buf[strlen(buf)-1] == '\n')
					buf[strlen(buf)-1] = '\0';
				if (strcmp(buf, "q") == 0)
					break;
				if (system(buf) < 0) {
					fprintf(stdout, "cmd system error\n");
					break;
				}
			}
		}
		uninit(SIGINT);
	}
}
static void on_bond(void *data, void *user_data)
{
	const char *uuid = "nap";
	char *address = (char *)user_data;
	bool paired = *(bool *)data;
	char *network_interface = NULL;
	const int MAX_RETRY_TIMES = 30;
	int retry_times = 0;

	fprintf(stdout, "on_bond %s\n", paired ? "Paired" : "UnPaired");
	if (paired) {
		fprintf(stdout, "remote address is %s\n", address);
		while (!bt->is_paired(address)) {
			sleep(1);
			++retry_times;
			if (retry_times > MAX_RETRY_TIMES) {
				fprintf(stdout, "bond %s failed\n", address);
				return;
			}
		}
		err = bt->pan_connect(address, uuid, &network_interface);
		if (S_OK == err && network_interface)
			on_connect();
	}
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

static void on_timeout_callback(void *user_data)
{
	char mac_addr[MAX_BDADDR_LEN + 1];

	fprintf(stdout, "%s: stop scanning\n", __func__);
	bt->stop_scan();
	bt->unset_callback(BT_EVENT_SCAN);

	fprintf(stdout, "\nPlease input NAP device MAC address:\n");
	memset(mac_addr, 0, MAX_BDADDR_LEN + 1);
	if (fgets(mac_addr, MAX_BDADDR_LEN + 1, stdin) == NULL) {
		fprintf(stdout, "\ncmd fgets error\n");
		return;
	}

	if (remote_address) {
		free(remote_address);
		remote_address = NULL;
	}
	remote_address = strdup(mac_addr);
	err = bt->set_callback(BT_EVENT_BOND, user_callback,
		     (void *)remote_address);
	err = bt->start_bond(mac_addr);
}

int main(int argc, char *argv[])
{
	artik_error ret = S_OK;
	signal_fuc signal_uninit = uninit;
	int status = -1;
	int timeout_id = 0;

	if (!artik_is_module_available(ARTIK_MODULE_BLUETOOTH)) {
		printf("TEST:Bluetooth module is not available,skipping test...\n");
		return -1;
	}

	status = system("systemctl stop connman");
	if (-1 == status || !WIFEXITED(status) || 0 != WEXITSTATUS(status)) {
		printf("stop connman service failed\r\n");
		return -1;
	}

	bt = (artik_bluetooth_module *) artik_request_api_module("bluetooth");
	loop = (artik_loop_module *)
			artik_request_api_module("loop");

	if (!bt || !loop)
		goto out;
	bt->remove_devices();
	ret = bt->set_callback(BT_EVENT_SCAN, user_callback, (void *)bt);
	if (ret != S_OK)
		goto out;
	ret = bt->start_scan();
	if (ret != S_OK)
		goto out;
	loop->add_timeout_callback(&timeout_id, SCAN_TIME_MILLISECONDS,
		on_timeout_callback, (void *)loop);

	signal(SIGINT, signal_uninit);
	loop->run();
out:
	if (bt)
		artik_release_api_module(bt);
	if (loop)
		artik_release_api_module(loop);
	if (remote_address)
		free(remote_address);
	return 0;
}
