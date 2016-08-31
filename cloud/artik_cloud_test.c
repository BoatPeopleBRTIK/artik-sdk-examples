#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>

#include <artik_module.h>
#include <artik_cloud.h>

const char token[] = "";
const char device_id[] = "";
const char user_id[] = "";
const char app_id[] = "";
const char message[] = "";
const char action[] = "";
const char device_type_id[] = "";
const char sdr_device_type_id[] = "";
const char sdr_vendor_id[] = "";

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

	ret = cloud->send_message(t, did, act, &response);

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

static artik_error test_cloud_sdr_registration(void)
{
	artik_cloud_module *cloud = (artik_cloud_module *)artik_request_api_module("cloud");
	artik_error ret = S_OK;
	char *response = NULL;
	char *reg_id = NULL;
	char *reg_nonce = NULL;
	char *reg_pin = NULL;
	char *reg_status = NULL;
	char *device_id = NULL;

	fprintf(stdout, "TEST: %s starting\n", __func__);

	/* Start registration process */
	ret = cloud->sdr_start_registration(sdr_device_type_id, sdr_vendor_id, &response);
	if (ret != S_OK) {
		if (response)
			fprintf(stdout,
				"TEST: %s HTTP error, response: %s\n",
				__func__, response);
		goto exit;
	}

	if (response) {
		/* Find rid and nonce strings */
		reg_id = parse_json_object(response, "rid");
		reg_nonce = parse_json_object(response, "nonce");
		reg_pin = parse_json_object(response, "pin");
		free(response);
		response = NULL;
	}

	if (!reg_id || !reg_nonce || !reg_pin) {
		fprintf(stdout,
			"TEST: %s failed to parse result from the JSON response\n",
			__func__);
		ret = E_HTTP_ERROR;
		goto exit;
	}

	if (ret != S_OK) {
		fprintf(stdout, "TEST: %s failed (err=%d)\n", __func__, ret);
		goto exit;
	}

	fprintf(stdout, "TEST: %s Enter PIN %s\n", __func__, reg_pin);

	/* Wait for user to enter the PIN */
	while (true) {
		ret = cloud->sdr_registration_status(reg_id, &response);
		if (ret != S_OK) {
			fprintf(stdout,
				"TEST: %s failed to get status (err=%d)\n",
				__func__, ret);
			goto exit;
		}

		if (response) {
			/* Find status */
			reg_status = parse_json_object(response, "status");
			if (!reg_status) {
				fprintf(stdout,
					"TEST: %s failed to parse status from the JSON response\n",
					__func__);
				ret = E_HTTP_ERROR;
				goto exit;
			}

			/* Check if completed */
			if (strncmp
			    (reg_status, "PENDING_USER_CONFIRMATION", 128))
				break;
			free(reg_status);
			free(response);
			reg_status = NULL;
			response = NULL;
		}

		usleep(1000 * 1000);

		fprintf(stdout, ".");
		fflush(stdout);
	}

	fprintf(stdout, "\n");

	if (strncmp(reg_status, "PENDING_DEVICE_COMPLETION", 128)) {
		fprintf(stdout,
			"TEST: %s Registration failed, probably because it expired\n",
			__func__);
		ret = E_TIMEOUT;
		goto exit;
	}

	/* Finalize the registration */
	ret = cloud->sdr_complete_registration(reg_id, reg_nonce, &response);
	if (ret != S_OK) {
		fprintf(stdout,
			"TEST: %s Complete registration failed (err=%d)\n",
			__func__, ret);
		goto exit;
	}

	if (response) {
		device_id = parse_json_object(response, "did");
		if (!device_id) {
			fprintf(stdout,
				"TEST: %s Complete registration failed, could not pare JSON response\n",
				__func__);
			ret = E_TIMEOUT;
			goto exit;
		}

		fprintf(stdout, "TEST: %s Device registered with ID %s\n", __func__, device_id);

	} else {
		fprintf(stdout,
			"TEST: %s Complete registration failed, did not receive a response\n",
			__func__);
		ret = E_TIMEOUT;
		goto exit;
	}


	fprintf(stdout, "TEST: %s succeeded\n", __func__);

exit:
	if (response)
		free(response);

	if (device_id)
		free(device_id);

	if (reg_pin)
		free(reg_pin);

	if (reg_status)
		free(reg_status);

	if (reg_id)
		free(reg_id);

	if (reg_nonce)
		free(reg_nonce);

	artik_release_api_module(cloud);

	return ret;
}

int main(int argc, char *argv[])
{
	artik_error ret = S_OK;

	if (!artik_is_module_available(ARTIK_MODULE_CLOUD)) {
		fprintf(stdout,
			"TEST: Cloud module is not available, skipping test...\n");
		return -1;
	}

	ret = test_get_user_profile(token);
	ret = test_get_user_devices(token, user_id);
	ret = test_get_user_device_types(token, user_id);
	ret = test_get_user_application_properties(token, user_id, app_id);
	ret = test_get_device(token, device_id);
	ret = test_get_device_token(token, device_id);
	ret = test_cloud_message(token, device_id, message);
	ret = test_cloud_action(token, device_id, action);
	ret = test_update_device_token(token, device_id);
	ret = test_delete_device_token(token, device_id);
	ret = test_add_delete_device(token, user_id, device_type_id);
	ret = test_cloud_sdr_registration();

	return (ret == S_OK) ? 0 : -1;
}
