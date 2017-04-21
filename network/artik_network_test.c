#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <artik_module.h>
#include <artik_network.h>
#include <artik_loop.h>

#define CHECK_RET(x)	{ if (x != S_OK) goto exit; }

typedef struct {
	int deconnection_detected;
	int reconnection_detected;
} watch_online_status_t;

static artik_error test_get_current_public_ip(void) {
	artik_network_module *network = (artik_network_module *)artik_request_api_module("network");
	artik_error ret;
	artik_network_ip current_ip;
	fprintf(stdout, "TEST: %s starting\n", __func__);

	/* Check Current IP */
	ret = network->get_current_public_ip(&current_ip);
	if (ret == S_OK)
		fprintf(stdout, "Your IP Address: %s\n", current_ip.address);
	else {
		fprintf(stdout, "TEST: %s failed (err=%d)\n", __func__, ret);
		goto exit;
	}

	fprintf(stdout, "TEST: %s succeeded\n", __func__);

exit:
	artik_release_api_module(network);

	return ret;

}

static artik_error test_get_online_status(void) {
	artik_network_module *network = (artik_network_module *)artik_request_api_module("network");
	artik_error ret;
	bool online_status;
	fprintf(stdout, "TEST: %s starting\n", __func__);

	ret = network->get_online_status(&online_status);
	if (ret < S_OK)
		fprintf(stdout, "TEST: %s failed (err=%d)\n", __func__, ret);

	if (ret == S_OK)
		fprintf(stdout, "TEST: %s succeeded (online status=%d)\n", __func__, online_status);

	artik_release_api_module(network);

	return ret;
}

static void disconnect(void *user_data) {
	system("ifconfig wlan0 down");
}

static void reconnect(void *user_data) {
	system("ifconfig wlan0 up; sleep 1; pkill dhclient; sleep 1; dhclient wlan0");
}

static void quit(void *user_data) {
	artik_loop_module* loop = (artik_loop_module *)artik_request_api_module("loop");

	loop->quit();
}

static void _callback(bool online_status, void *user_data) {
	watch_online_status_t *data = user_data;
	if (online_status == 1) {
		data->deconnection_detected = 1;
		fprintf(stdout, "Network Connected\n");
	} else {
		data->reconnection_detected = 1;
		fprintf(stdout, "Network could not be connected\n");
	}

}

static artik_error test_watch_online_status(void) {
	artik_error ret = S_OK;
	int timeout_disconnect_id, timeout_reconnect_id, timeout_quit_id;
	artik_network_module* network = (artik_network_module *)artik_request_api_module("network");
	artik_loop_module* loop = (artik_loop_module *)artik_request_api_module("loop");
	watch_online_status_handle handle;
	watch_online_status_handle handle2;
	watch_online_status_t data = {0, 0};
	watch_online_status_t data2 = {0, 0};
	fprintf(stdout, "TEST: %s starting\n", __func__);
	ret = network->add_watch_online_status(&handle, _callback, &data);
	ret = network->add_watch_online_status(&handle2, _callback, &data2);

	loop->add_timeout_callback(&timeout_disconnect_id, 1000, disconnect, NULL);
	loop->add_timeout_callback(&timeout_reconnect_id, 2000, reconnect, NULL);
	loop->add_timeout_callback(&timeout_quit_id, 4000, quit, NULL);

	loop->run();

	if (!data.deconnection_detected && !data.reconnection_detected
	    && !data2.deconnection_detected && !data2.reconnection_detected) {
		ret = -1;
		fprintf(stderr, "TEST: %s failed (deconnection = %d, reconnection = %d)\n",
			__func__, data.deconnection_detected, data.reconnection_detected);
		goto exit;
	}

	fprintf(stdout, "TEST: %s succeeded\n", __func__);
exit:
	network->remove_watch_online_status(handle);
	network->remove_watch_online_status(handle2);
	artik_release_api_module(loop);
	artik_release_api_module(network);
	return ret;
}

int main(int argc, char *argv[]) {
	artik_error ret = S_OK;
	int opt;
	int execute_all_tests = 0;

	if (!artik_is_module_available(ARTIK_MODULE_NETWORK)) {
		fprintf(stdout,
			"TEST: NETWORK module is not available, skipping test...\n");
		return -1;
	}

	while ((opt = getopt(argc, argv, "a")) != -1) {
		switch (opt) {
		case 'a':
			execute_all_tests = 1;
			break;
		}
	}

	if (!execute_all_tests) {
		printf("Execute only get_online_status and get_current_public_ip tests.\n");
		printf("i.e: Use option '-a' to execute all tests (test_watch_online_status  shutdowns the wlan0 interface)\n");
	}

	ret = test_get_online_status();
	CHECK_RET(ret);

	ret = test_get_current_public_ip();
	CHECK_RET(ret);

	if (execute_all_tests) {
		ret = test_watch_online_status();
		CHECK_RET(ret);
	}

exit:
	return (ret == S_OK) ? 0 : -1;
}
