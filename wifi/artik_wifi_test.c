#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <artik_module.h>
#include <artik_loop.h>
#include <artik_wifi.h>

#define AP_TO_CONNECT	"<put a SSID name here>"
#define AP_PASSWORD		"<put the password of the SSID here>"
#define CHECK_RET(x)	{ if (x != S_OK) goto exit; }

static char *ssid = AP_TO_CONNECT;
static char *psk = AP_PASSWORD;

static void get_scan_result(void)
{
	artik_wifi_module *wifi = (artik_wifi_module *)artik_request_api_module("wifi");
	artik_wifi_ap *list = NULL;
	int count = 0;
	int ret = 0;
	int i = 0;

	ret = wifi->get_scan_result(&list, &count);
	CHECK_RET(ret);

	for (i = 0; i < count; i++)
		fprintf(stdout, "%-20s %s 0x%X\n", list[i].bssid, list[i].name,
			list[i].encryption_flags);

	artik_release_api_module(wifi);

	return;

exit:

	artik_release_api_module(wifi);
	fprintf(stdout, "failed");
}

static void on_scan_result(void *user_data)
{
	artik_loop_module *loop = (artik_loop_module *)artik_request_api_module("loop");
	artik_wifi_module *wifi = (artik_wifi_module *)artik_request_api_module("wifi");

	fprintf(stdout, "on_scan_result\n");

	get_scan_result();

	wifi->deinit();
	loop->quit();

	artik_release_api_module(loop);
	artik_release_api_module(wifi);
}

static void on_scan_timeout(void *user_data)
{
	artik_loop_module *loop = (artik_loop_module *)artik_request_api_module("loop");
	artik_wifi_module *wifi = (artik_wifi_module *)artik_request_api_module("wifi");

	fprintf(stdout, "TEST: %s reached timeout\n", __func__);

	get_scan_result();

	wifi->deinit();
	loop->quit();

	artik_release_api_module(loop);
	artik_release_api_module(wifi);
}

artik_error test_wifi_scan(void)
{
	artik_loop_module *loop = (artik_loop_module *)artik_request_api_module("loop");
	artik_wifi_module *wifi = (artik_wifi_module *)artik_request_api_module("wifi");
	artik_error ret;
	int timeout_id = 0;

	fprintf(stdout, "TEST: %s starting\n", __func__);

	ret = wifi->init();
	CHECK_RET(ret);
	ret = wifi->set_scan_result_callback(on_scan_result, NULL);
	CHECK_RET(ret);
	ret = wifi->scan_request();
	CHECK_RET(ret);

	loop->add_timeout_callback(&timeout_id, 10 * 1000, on_scan_timeout,
				   NULL);
	loop->run();

exit:
	fprintf(stdout, "TEST: %s %s\n", __func__,
		(ret == S_OK) ? "succeeded" : "failed");

	artik_release_api_module(loop);
	artik_release_api_module(wifi);

	return ret;
}

static void on_connect(void *user_data)
{
	artik_loop_module *loop = (artik_loop_module *)artik_request_api_module("loop");
	artik_wifi_module *wifi = (artik_wifi_module *)artik_request_api_module("wifi");

	fprintf(stdout, "on_connect\n");

	wifi->deinit();
	loop->quit();

	artik_release_api_module(loop);
	artik_release_api_module(wifi);
}

artik_error test_wifi_connect(void)
{
	artik_loop_module *loop = (artik_loop_module *)artik_request_api_module("loop");
	artik_wifi_module *wifi = (artik_wifi_module *)artik_request_api_module("wifi");
	artik_error ret = S_OK;

	fprintf(stdout, "TEST: %s starting\n", __func__);

	ret = wifi->init();
	CHECK_RET(ret);
	ret = wifi->set_connect_callback(on_connect, NULL);
	CHECK_RET(ret);
	ret = wifi->connect(ssid, psk, 0);
	CHECK_RET(ret);

	loop->run();

exit:
	fprintf(stdout, "TEST: %s %s\n", __func__,
		(ret == S_OK) ? "succeeded" : "failed");

	artik_release_api_module(loop);
	artik_release_api_module(wifi);

	return ret;
}

int main(int argc, char *argv[])
{
	artik_error ret = S_OK;

	if (!artik_is_module_available(ARTIK_MODULE_WIFI)) {
		fprintf(stdout,
			"TEST: Wifi module is not available, skipping test...\n");
		return -1;
	}

	ret = test_wifi_scan();
	CHECK_RET(ret);

	if (argc == 3) {
		ssid = argv[1];
		psk = argv[2];
		fprintf(stdout, "ssid=%s, psk=%s\n", ssid, psk);

		ret = test_wifi_connect();
		CHECK_RET(ret);
	}

exit:
	return (ret == S_OK) ? 0 : -1;
}
