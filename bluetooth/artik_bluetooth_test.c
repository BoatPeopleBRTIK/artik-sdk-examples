#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <artik_module.h>
#include <artik_loop.h>
#include <artik_bluetooth.h>

#define PXP_DEV_BDADDR		"<Test MAC Address>"
#define MAX_BDADDR_LEN		17

void print_devices(artik_bt_device *devices, int num)
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

static void scan_callback(artik_bt_event event, void *data, void *user_data)
{
	artik_bt_device *dev = (artik_bt_device *) data;

	print_devices(dev, 1);
}

static void on_timeout_callback(void *user_data)
{
	artik_loop_module *loop = (artik_loop_module *) user_data;

	fprintf(stdout, "TEST: %s stop scanning, exiting loop\n", __func__);

	loop->quit();
}

artik_error test_bluetooth_scan(void)
{
	artik_loop_module *loop = (artik_loop_module *)artik_request_api_module("loop");
	artik_bluetooth_module *bt = (artik_bluetooth_module *)artik_request_api_module("bluetooth");
	artik_error ret = S_OK;
	int timeout_id = 0;

	fprintf(stdout, "TEST: %s starting\n", __func__);

	ret = bt->set_callback(BT_EVENT_SCAN, scan_callback, NULL);
	if (ret != S_OK)
		goto exit;
	ret = bt->start_scan();
	if (ret != S_OK)
		goto exit;
	loop->add_timeout_callback(&timeout_id, 10000, on_timeout_callback,
				   (void *)loop);
	loop->run();

exit:
	ret = bt->stop_scan();
	ret = bt->unset_callback(BT_EVENT_SCAN);

	fprintf(stdout, "TEST: %s %s\n", __func__,
		(ret == S_OK) ? "succeeded" : "failed");

	artik_release_api_module(loop);
	artik_release_api_module(bt);

	return ret;
}

static artik_error test_bluetooth_devices(void)
{
	artik_bluetooth_module *bt = (artik_bluetooth_module *)artik_request_api_module("bluetooth");
	artik_error ret = S_OK;
	artik_bt_device *devices = NULL;
	int num = 0;

	fprintf(stdout, "TEST: %s starting\n", __func__);

	fprintf(stdout, "TEST: %s - list devices\n", __func__);
	ret = bt->get_devices(&devices, &num);
	if (ret != S_OK)
		goto exit;
	print_devices(devices, num);
	bt->free_devices(devices, num);
	devices = NULL;
	num = 0;

	fprintf(stdout, "TEST: %s - list paired devices\n", __func__);
	ret = bt->get_paired_devices(&devices, &num);
	if (ret != S_OK)
		goto exit;
	print_devices(devices, num);
	bt->free_devices(devices, num);
	devices = NULL;
	num = 0;

	fprintf(stdout, "TEST: %s - list connected devices\n", __func__);
	ret = bt->get_connected_devices(&devices, &num);
	if (ret != S_OK)
		goto exit;
	print_devices(devices, num);
	bt->free_devices(devices, num);
	devices = NULL;
	num = 0;

exit:
	fprintf(stdout, "TEST: %s %s\n", __func__,
		(ret == S_OK) ? "succeeded" : "failed");

	if (devices && (num > 0))
		bt->free_devices(devices, num);

	artik_release_api_module(bt);

	return ret;
}

static void on_scan(void *data, void *user_data)
{
	artik_bluetooth_module *bt = (artik_bluetooth_module *)artik_request_api_module("bluetooth");
	artik_bt_device *dev = (artik_bt_device *)data;
	char *remote_address = (char *)user_data;

	strncpy(remote_address, dev->remote_address, MAX_BDADDR_LEN);

	fprintf(stdout, "Address: %s\n", dev->remote_address);

	if (strncasecmp(PXP_DEV_BDADDR, remote_address, MAX_BDADDR_LEN) == 0) {
		bt->stop_scan();
		bt->start_bond(remote_address);
	}

	artik_release_api_module(bt);
}

static void on_bond(void *data, void *user_data)
{
	artik_bluetooth_module *bt = (artik_bluetooth_module *)artik_request_api_module("bluetooth");
	char *remote_address = (char *)user_data;
	bool paired = *(bool *)data;

	fprintf(stdout, "on_bond %s\n", paired ? "Paired" : "UnPaired");
	bt->connect(remote_address);

	artik_release_api_module(bt);
}

static void on_connect(void *data, void *user_data)
{
	artik_bluetooth_module *bt = (artik_bluetooth_module *)artik_request_api_module("bluetooth");
	char *remote_address = (char *)user_data;
	bool connected = *(bool *)data;

	fprintf(stdout, "on_connect %s\n",
		connected ? "Connected" : "Disconnected");
	bt->pxp_set_linkloss_level(remote_address, BT_ALERT_LEVEL_MILD);

	artik_release_api_module(bt);
}

static void on_proximity(void *data, void *user_data)
{
	artik_bt_gatt_data *bt_pxp_data = (artik_bt_gatt_data *) data;

	fprintf(stdout, "on_proximity : property [%s] value [%s]\n",
		bt_pxp_data->key, bt_pxp_data->value);
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
	case BT_EVENT_CONNECT:
		on_connect(data, user_data);
		break;
	case BT_EVENT_PROXIMITY:
		on_proximity(data, user_data);
		break;
	default:
		break;
	}
}

static void on_connect_timeout(void *user_data)
{
	artik_loop_module *loop = (artik_loop_module *)artik_request_api_module("loop");
	artik_bluetooth_module *bt = (artik_bluetooth_module *)artik_request_api_module("bluetooth");
	char *remote_address = (char *)user_data;

	fprintf(stdout, "TEST: %s reached timeout\n", __func__);

	if (strncmp(remote_address, "", MAX_BDADDR_LEN)) {
		bt->remove_unpaired_devices();
		bt->unset_callback(BT_EVENT_PROXIMITY);
		bt->remove_device(remote_address);
		bt->unset_callback(BT_EVENT_SCAN);
		bt->unset_callback(BT_EVENT_CONNECT);
	}

	loop->quit();

	artik_release_api_module(loop);
	artik_release_api_module(bt);
}

static artik_error test_bluetooth_connect(void)
{
	artik_loop_module *loop = (artik_loop_module *)artik_request_api_module("loop");
	artik_bluetooth_module *bt = (artik_bluetooth_module *)artik_request_api_module("bluetooth");
	artik_error ret = S_OK;
	int timeout_id = 0;
	char remote_address[MAX_BDADDR_LEN] = "";

	fprintf(stdout, "TEST: %s starting\n", __func__);

	ret =
	    bt->set_callback(BT_EVENT_SCAN, user_callback,
			     (void *)remote_address);
	if (ret != S_OK)
		goto exit;
	ret =
	    bt->set_callback(BT_EVENT_BOND, user_callback,
			     (void *)remote_address);
	if (ret != S_OK)
		goto exit;
	ret =
	    bt->set_callback(BT_EVENT_CONNECT, user_callback,
			     (void *)remote_address);
	if (ret != S_OK)
		goto exit;
	ret =
	    bt->set_callback(BT_EVENT_PROXIMITY, user_callback,
			     (void *)remote_address);
	if (ret != S_OK)
		goto exit;
	ret = bt->start_scan();
	if (ret != S_OK)
		goto exit;
	loop->add_timeout_callback(&timeout_id, 60000, on_connect_timeout,
				   (void *)remote_address);
	loop->run();

exit:
	fprintf(stdout, "TEST: %s %s\n", __func__,
		(ret == S_OK) ? "succeeded" : "failed");

	artik_release_api_module(loop);
	artik_release_api_module(bt);
	return ret;
}

int main(void)
{
	artik_error ret = S_OK;

	if (!artik_is_module_available(ARTIK_MODULE_BLUETOOTH)) {
		fprintf(stdout,
			"TEST: Bluetooth module is not available, skipping test...\n");
		return -1;
	}

	ret = test_bluetooth_scan();
	if (ret != S_OK)
		goto exit;
	ret = test_bluetooth_connect();
	if (ret != S_OK)
		goto exit;
	ret = test_bluetooth_devices();
	if (ret != S_OK)
		goto exit;
exit:
	return (ret == S_OK) ? 0 : -1;
}
