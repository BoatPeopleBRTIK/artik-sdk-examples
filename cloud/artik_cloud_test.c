#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>

#include <artik_module.h>
#include <artik_cloud.h>

#define CHECK_RET(x)	{ if (x != S_OK) goto exit; }

static char *token = NULL;
static char *device_id = NULL;
static char *user_id = NULL;
static char *app_id = NULL;
static char *message = NULL;
static char *action = NULL;
static char *device_type_id = NULL;

static char *parse_json_object(const char *data, const char *obj)
{
	char *res = NULL;
	char prefix[256];
	char *substr = NULL;

	snprintf(prefix, 256, "\"%s\":\"", obj);

	substr = strstr(data, prefix);
	if (substr != NULL) {
		int idx = 0;

		/* Start after substring */
		substr += strlen(prefix);

		/* Count number of bytes to extract */
		while (substr[idx] != '\"')
			idx++;
		/* Copy the extracted string */
		res = strndup(substr, idx);
	}

	return res;
}

static artik_error test_get_user_profile(const char *t)
{
	artik_cloud_module *cloud = (artik_cloud_module *)artik_request_api_module("cloud");
	artik_error ret = S_OK;
	char *response = NULL;

	fprintf(stdout, "TEST: %s starting\n", __func__);

	ret = cloud->get_current_user_profile(t, &response);

	if (response) {
		fprintf(stdout, "TEST: %s response data: %s\n", __func__,
			response);
		free(response);
	}

	if (ret != S_OK) {
		fprintf(stdout, "TEST: %s failed (err=%d)\n", __func__, ret);
		return ret;
	}

	fprintf(stdout, "TEST: %s succeeded\n", __func__);

	return ret;
}

static artik_error test_get_user_devices(const char *t, const char *uid)
{
	artik_cloud_module *cloud = (artik_cloud_module *)artik_request_api_module("cloud");
	artik_error ret = S_OK;
	char *response = NULL;

	fprintf(stdout, "TEST: %s starting\n", __func__);

	ret = cloud->get_user_devices(t, 100, false, 0, uid, &response);

	if (response) {
		fprintf(stdout, "TEST: %s response data: %s\n", __func__,
			response);
		free(response);
	}

	if (ret != S_OK) {
		fprintf(stdout, "TEST: %s failed (err=%d)\n", __func__, ret);
		return ret;
	}

	fprintf(stdout, "TEST: %s succeeded\n", __func__);

	artik_release_api_module(cloud);

	return ret;
}

static artik_error test_get_user_device_types(const char *t, const char *uid)
{
	artik_cloud_module *cloud = (artik_cloud_module *)artik_request_api_module("cloud");
	artik_error ret = S_OK;
	char *response = NULL;

	fprintf(stdout, "TEST: %s starting\n", __func__);

	ret = cloud->get_user_device_types(t, 100, false, 0, uid, &response);

	if (response) {
		fprintf(stdout, "TEST: %s response data: %s\n", __func__,
			response);
		free(response);
	}

	if (ret != S_OK) {
		fprintf(stdout, "TEST: %s failed (err=%d)\n", __func__, ret);
		return ret;
	}

	fprintf(stdout, "TEST: %s succeeded\n", __func__);

	artik_release_api_module(cloud);

	return ret;
}

static artik_error test_get_user_application_properties(const char *t,
							const char *uid,
							const char *aid)
{
	artik_cloud_module *cloud = (artik_cloud_module *)artik_request_api_module("cloud");
	artik_error ret = S_OK;
	char *response = NULL;

	fprintf(stdout, "TEST: %s starting\n", __func__);

	ret = cloud->get_user_application_properties(t, uid, aid, &response);

	if (response) {
		fprintf(stdout, "TEST: %s response data: %s\n", __func__,
			response);
		free(response);
	}

	if (ret != S_OK) {
		fprintf(stdout, "TEST: %s failed (err=%d)\n", __func__, ret);
		return ret;
	}

	fprintf(stdout, "TEST: %s succeeded\n", __func__);

	artik_release_api_module(cloud);

	return ret;
}

static artik_error test_get_device(const char *t, const char *did)
{
	artik_cloud_module *cloud = (artik_cloud_module *)artik_request_api_module("cloud");
	artik_error ret = S_OK;
	char *response = NULL;

	fprintf(stdout, "TEST: %s starting\n", __func__);

	ret = cloud->get_device(t, did, true, &response);

	if (response) {
		fprintf(stdout, "TEST: %s response data: %s\n", __func__,
			response);
		free(response);
	}

	if (ret != S_OK) {
		fprintf(stdout, "TEST: %s failed (err=%d)\n", __func__, ret);
		return ret;
	}

	fprintf(stdout, "TEST: %s succeeded\n", __func__);

	artik_release_api_module(cloud);

	return ret;
}

static artik_error test_get_device_token(const char *t, const char *did)
{
	artik_cloud_module *cloud = (artik_cloud_module *)artik_request_api_module("cloud");
	artik_error ret = S_OK;
	char *response = NULL;

	fprintf(stdout, "TEST: %s starting\n", __func__);

	ret = cloud->get_device_token(t, did, &response);

	if (response) {
		fprintf(stdout, "TEST: %s response data: %s\n", __func__,
			response);
		free(response);
	}

	if (ret != S_OK) {
		fprintf(stdout, "TEST: %s failed (err=%d)\n", __func__, ret);
		return ret;
	}

	fprintf(stdout, "TEST: %s succeeded\n", __func__);

	artik_release_api_module(cloud);

	return ret;
}

static artik_error test_cloud_message(const char *t, const char *did,
					  const char *msg)
{
	artik_cloud_module *cloud = (artik_cloud_module *)artik_request_api_module("cloud");
	artik_error ret = S_OK;
	char *response = NULL;

	fprintf(stdout, "TEST: %s starting\n", __func__);

	ret = cloud->send_message(t, did, msg, &response);

	if (response) {
		fprintf(stdout, "TEST: %s response data: %s\n", __func__,
			response);
		free(response);
	}

	if (ret != S_OK) {
		fprintf(stdout, "TEST: %s failed (err=%d)\n", __func__, ret);
		return ret;
	}

	fprintf(stdout, "TEST: %s succeeded\n", __func__);

	artik_release_api_module(cloud);

	return ret;
}

static artik_error test_cloud_action(const char *t, const char *did,
					 const char *act)
{
	artik_cloud_module *cloud = (artik_cloud_module *)artik_request_api_module("cloud");
	artik_error ret = S_OK;
	char *response = NULL;

	fprintf(stdout, "TEST: %s starting\n", __func__);

	ret = cloud->send_action(t, did, act, &response);

	if (response) {
		fprintf(stdout, "TEST: %s response data: %s\n", __func__,
			response);
		free(response);
	}
	if (ret != S_OK) {
		fprintf(stdout, "TEST: %s failed (err=%d)\n", __func__, ret);
		return ret;
	}

	fprintf(stdout, "TEST: %s succeeded\n", __func__);

	artik_release_api_module(cloud);

	return ret;
}

static artik_error test_update_device_token(const char *t, const char *did)
{
	artik_cloud_module *cloud = (artik_cloud_module *)artik_request_api_module("cloud");
	artik_error ret = S_OK;
	char *response = NULL;

	fprintf(stdout, "TEST: %s starting\n", __func__);

	ret = cloud->update_device_token(t, did, &response);

	if (response) {
		fprintf(stdout, "TEST: %s response data: %s\n", __func__,
			response);
		free(response);
	}

	if (ret != S_OK) {
		fprintf(stdout, "TEST: %s failed (err=%d)\n", __func__, ret);
		return ret;
	}

	fprintf(stdout, "TEST: %s succeeded\n", __func__);

	artik_release_api_module(cloud);

	return ret;
}

static artik_error test_delete_device_token(const char *t, const char *did)
{
	artik_cloud_module *cloud = (artik_cloud_module *)artik_request_api_module("cloud");
	artik_error ret = S_OK;
	char *response = NULL;

	fprintf(stdout, "TEST: %s starting\n", __func__);

	ret = cloud->delete_device_token(t, did, &response);

	if (response) {
		fprintf(stdout, "TEST: %s response data: %s\n", __func__,
			response);
		free(response);
	}

	if (ret != S_OK) {
		fprintf(stdout, "TEST: %s failed (err=%d)\n", __func__, ret);
		return ret;
	}

	fprintf(stdout, "TEST: %s succeeded\n", __func__);

	artik_release_api_module(cloud);

	return ret;
}

static artik_error test_add_delete_device(const char *t, const char *uid, const char *dtid)
{
	artik_cloud_module *cloud = (artik_cloud_module *)artik_request_api_module("cloud");
	artik_error ret = S_OK;
	char *response = NULL;
	char *device_id = NULL;

	fprintf(stdout, "TEST: %s starting\n", __func__);

	/* Create a new device */
	ret = cloud->add_device(t, uid, dtid, "Test Device", &response);

	if (ret != S_OK) {
		fprintf(stdout, "TEST: %s failed (err=%d)\n", __func__, ret);
		goto exit;
	}

	if (response) {
		fprintf(stdout, "TEST: %s response data: %s\n", __func__, response);
		device_id = parse_json_object(response, "id");
		free(response);
	} else {
		fprintf(stdout, "TEST: %s did not receive response\n", __func__);
		ret = E_BAD_ARGS;
		goto exit;
	}

	if (!device_id)	{
		fprintf(stdout, "TEST: %s failed to parse response\n", __func__);
		ret = E_BAD_ARGS;
		goto exit;
	}

	/* Check if the device has been created */
	ret = cloud->get_device(t, device_id, false, &response);

	if (ret != S_OK) {
		fprintf(stdout, "TEST: %s failed (err=%d)\n", __func__, ret);
		goto exit;
	}

	if (response) {
		fprintf(stdout, "TEST: %s response data: %s\n", __func__, response);
		free(response);
	} else {
		fprintf(stdout, "TEST: %s did not receive response\n", __func__);
		ret = E_BAD_ARGS;
		goto exit;
	}

	/* Delete the device */
	ret = cloud->delete_device(t, device_id, &response);

	if (ret != S_OK) {
		fprintf(stdout, "TEST: %s failed (err=%d)\n", __func__, ret);
		goto exit;
	}

	if (response) {
		fprintf(stdout, "TEST: %s response data: %s\n", __func__, response);
		free(response);
	} else {
		fprintf(stdout, "TEST: %s did not receive response\n", __func__);
		ret = E_BAD_ARGS;
		goto exit;
	}

exit:
	artik_release_api_module(cloud);

	fprintf(stdout, "TEST: %s %s\n", __func__, ret == S_OK ? "succeeded" : "failed");

	return ret;
}

int main(int argc, char *argv[])
{
	int opt;
	artik_error ret = S_OK;

	if (!artik_is_module_available(ARTIK_MODULE_CLOUD)) {
		fprintf(stdout,
			"TEST: Cloud module is not available, skipping test...\n");
		return -1;
	}

	while ((opt = getopt(argc, argv, "t:d:u:p:m:a:y:")) != -1) {
		switch (opt) {
		case 't':
			token = strndup(optarg, strlen(optarg));
			break;
		case 'd':
			device_id = strndup(optarg, strlen(optarg));
			break;
		case 'u':
			user_id = strndup(optarg, strlen(optarg));
			break;
		case 'p':
			app_id = strndup(optarg, strlen(optarg));
			break;
		case 'm':
			message = strndup(optarg, strlen(optarg));
			break;
		case 'a':
			action = strndup(optarg, strlen(optarg));
			break;
		case 'y':
			device_type_id = strndup(optarg, strlen(optarg));
			break;
		default:
			printf("Usage: cloud-test [-t <access token>] [-d <device id>] [-u <user id>] \r\n");
			printf("\t[-p <app id>] [-m <JSON type message>] [-a <JSON type action>] \r\n");
			printf("\t[-y <device type id>]\r\n");
			return 0;
		}
	}

	ret = test_get_user_profile(token);
	CHECK_RET(ret);

	ret = test_get_user_devices(token, user_id);
	CHECK_RET(ret);

	ret = test_get_user_device_types(token, user_id);
	CHECK_RET(ret);

	ret = test_get_user_application_properties(token, user_id, app_id);
	CHECK_RET(ret);

	ret = test_get_device(token, device_id);
	CHECK_RET(ret);

	ret = test_get_device_token(token, device_id);
	CHECK_RET(ret);

	ret = test_cloud_message(token, device_id, message);
	CHECK_RET(ret);

	ret = test_cloud_action(token, device_id, action);
	CHECK_RET(ret);

	ret = test_update_device_token(token, device_id);
	CHECK_RET(ret);

	ret = test_delete_device_token(token, device_id);
	CHECK_RET(ret);

	ret = test_add_delete_device(token, user_id, device_type_id);
	CHECK_RET(ret);

exit:
	if (token != NULL)
		free(token);
	if (device_id != NULL)
		free(device_id);
	if (user_id != NULL)
		free(user_id);
	if (app_id != NULL)
		free(app_id);
	if (message != NULL)
		free(message);
	if (action != NULL)
		free(action);
	if (device_type_id != NULL)
		free(device_type_id);

	return (ret == S_OK) ? 0 : -1;
}
