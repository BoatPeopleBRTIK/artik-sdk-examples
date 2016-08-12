#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>

#include <artik_module.h>
#include <artik_loop.h>
#include <artik_cloud.h>

#define TEST_TIMEOUT_MS	(30*1000)

const char device_id[128] = "<device ID>";
const char access_token[128] = "<access token>";
const char sdr_device_id[128] = "<SDR device ID>";
const char sdr_access_token[128] = "<SDR access token>";
char test_message[256] = "{\"data\": {\"actions\": {\"name\": \"setOn\"}},\"ddid\": \"<device ID>\",\"sdid\": \"<device ID>\",\"type\": \"message\"}";
char sdr_test_message[256] = "{\"data\": {\"actions\": {\"name\": \"setOn\"}},\"ddid\": \"<SDR device ID>\",\"sdid\": \"<SDR device ID>\",\"type\": \"message\"}";

static void on_timeout_callback(void *user_data)
{
	artik_loop_module *loop = (artik_loop_module *) user_data;

	fprintf(stdout, "TEST: %s stop scanning, exiting loop\n", __func__);

	loop->quit();
}

void websocket_receive_callback(void *user_data, void *result)
{
	char *buffer = (char *)result;
	if (buffer == NULL) {
		fprintf(stdout, "receive failed\n");
		return;
	}
	printf("received: %s\n", buffer);
	free(result);
}

static artik_error test_websocket_read(int timeout_ms)
{
	artik_error ret = S_OK;
	artik_cloud_module *cloud = (artik_cloud_module *)artik_request_api_module("cloud");
	artik_loop_module *loop = (artik_loop_module *)artik_request_api_module("loop");

	artik_websocket_handle handle;
	int timeout_id = 0;

	/* Open websocket to ARTIK Cloud and register device to receive messages from cloud */
	ret = cloud->websocket_open_stream(&handle, access_token, device_id, false);
	if (ret != S_OK) {
		fprintf(stderr, "TEST failed, could not open Websocket (%d)\n", ret);
		return ret;
	}

	ret = cloud->websocket_set_receive_callback(handle, websocket_receive_callback, &handle);
	if (ret != S_OK) {
		fprintf(stderr, "TEST failed, could not open Websocket (%d)\n", ret);
		return ret;
	}

	ret = loop->add_timeout_callback(&timeout_id, timeout_ms, on_timeout_callback,
					   (void *)loop);

	loop->run();

	cloud->websocket_close_stream(handle);

	fprintf(stdout, "TEST: %s finished\n", __func__);

	artik_release_api_module(cloud);
	artik_release_api_module(loop);

	return ret;
}

void websocket_receive_write_callback(void *user_data, void *result)
{
	artik_cloud_module *cloud = (artik_cloud_module *)artik_request_api_module("cloud");
	char *buffer = (char *)result;
	if (buffer == NULL) {
		fprintf(stdout, "receive failed\n");
		return;
	}
	printf("received: %s\n", buffer);
	free(result);

	cloud->websocket_send_message(*(artik_websocket_handle *)user_data, test_message);
}

static artik_error test_websocket_write(int timeout_ms)
{
	artik_error ret = S_OK;
	artik_cloud_module *cloud = (artik_cloud_module *)artik_request_api_module("cloud");
	artik_loop_module *loop = (artik_loop_module *)artik_request_api_module("loop");

	artik_websocket_handle handle;
	int timeout_id = 0;

	/* Open websocket to ARTIK Cloud and register device to receive message from cloud */
	ret = cloud->websocket_open_stream(&handle, access_token, device_id, false);
	if (ret != S_OK) {
		fprintf(stderr, "TEST failed, could not open Websocket (%d)\n", ret);
		return ret;
	}

	ret = cloud->websocket_set_receive_callback(handle, websocket_receive_write_callback, &handle);
	if (ret != S_OK) {
		fprintf(stderr, "TEST failed, could not open Websocket (%d)\n", ret);
		return ret;
	}

	ret = loop->add_timeout_callback(&timeout_id, timeout_ms, on_timeout_callback,
					   (void *)loop);

	loop->run();

	cloud->websocket_close_stream(handle);

	fprintf(stdout, "TEST: %s finished\n", __func__);

	artik_release_api_module(cloud);
	artik_release_api_module(loop);

	return ret;
}

void websocket_sdr_receive_write_callback(void *user_data, void *result)
{
	artik_cloud_module *cloud = (artik_cloud_module *)artik_request_api_module("cloud");
	char *buffer = (char *)result;
	if (buffer == NULL) {
		fprintf(stdout, "receive failed\n");
		return;
	}
	printf("received: %s\n", buffer);
	free(result);

	cloud->websocket_send_message(*(artik_websocket_handle *)user_data, sdr_test_message);
}

static artik_error test_websocket_sdr(int timeout_ms)
{
	artik_error ret = S_OK;
	artik_cloud_module *cloud = (artik_cloud_module *)artik_request_api_module("cloud");
	artik_loop_module *loop = (artik_loop_module *)artik_request_api_module("loop");

	artik_websocket_handle handle;
	int timeout_id = 0;

	/* Open websocket to ARTIK Cloud and register device to receive message from cloud */
	ret = cloud->websocket_open_stream(&handle, sdr_access_token, sdr_device_id, true);
	if (ret != S_OK) {
		fprintf(stderr, "TEST failed, could not open Websocket (%d)\n", ret);
		return ret;
	}

	ret = cloud->websocket_set_receive_callback(handle, websocket_sdr_receive_write_callback, &handle);
	if (ret != S_OK) {
		fprintf(stderr, "TEST failed, could not open Websocket (%d)\n", ret);
		return ret;
	}

	ret = loop->add_timeout_callback(&timeout_id, timeout_ms, on_timeout_callback,
					   (void *)loop);

	loop->run();

	cloud->websocket_close_stream(handle);

	fprintf(stdout, "TEST: %s finished\n", __func__);

	artik_release_api_module(cloud);
	artik_release_api_module(loop);

	return ret;
}

int main(int argc, char *argv[])
{
	artik_error ret = S_OK;

	ret = test_websocket_sdr(TEST_TIMEOUT_MS);
	ret = test_websocket_write(TEST_TIMEOUT_MS);
	ret = test_websocket_read(TEST_TIMEOUT_MS);

	return (ret == S_OK) ? 0 : -1;
}
