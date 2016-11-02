#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <artik_module.h>
#include <artik_http.h>

#define CHECK_RET(x)	{ if (x != S_OK) goto exit; }

artik_error test_http_get(void)
{
	artik_http_module *http = (artik_http_module *)artik_request_api_module("http");
	artik_error ret = S_OK;
	char *response = NULL;
	artik_http_headers headers;
	artik_http_header_field fields[] = {
		{"user-agent", "Artik browser"},
		{"Accept-Language", "en-US,en;q=0.8"},
	};

	headers.fields = fields;
	headers.num_fields = sizeof(fields) / sizeof(fields[0]);

	fprintf(stdout, "TEST: %s starting\n", __func__);

	ret = http->get("https://httpbin.org/get", &headers, &response, NULL, false);
	if (ret != S_OK) {
		fprintf(stdout, "TEST: %s failed (err=%d)\n", __func__, ret);
		return ret;
	}

	if (response) {
		fprintf(stdout, "TEST: %s response data: %s\n", __func__, response);
		free(response);
	}

	fprintf(stdout, "TEST: %s succeeded\n", __func__);

	artik_release_api_module(http);

	return ret;
}

artik_error test_http_post(void)
{
	artik_http_module *http = (artik_http_module *)artik_request_api_module("http");
	artik_error ret = S_OK;
	char *response = NULL;
	char body[] = "name=samsung&project=artik";
	artik_http_headers headers;
	artik_http_header_field fields[] = {
		{"user-agent", "Artik browser"},
		{"Accept-Language", "en-US,en;q=0.8"},
	};

	headers.fields = fields;
	headers.num_fields = sizeof(fields) / sizeof(fields[0]);

	fprintf(stdout, "TEST: %s starting\n", __func__);
	ret =
	    http->post("https://httpbin.org/post", &headers, body, &response,
		       NULL, false);
	if (ret != S_OK) {
		fprintf(stdout, "TEST: %s failed (err=%d)\n", __func__, ret);
		return ret;
	}

	if (response) {
		fprintf(stdout, "TEST: %s response data: %s\n", __func__, response);
		free(response);
	}

	fprintf(stdout, "TEST: %s succeeded\n", __func__);

	artik_release_api_module(http);

	return ret;
}

int main(void)
{
	artik_error ret = S_OK;

	if (!artik_is_module_available(ARTIK_MODULE_HTTP)) {
		fprintf(stdout,
			"TEST: HTTP module is not available, skipping test...\n");
		return -1;
	}

	ret = test_http_get();
	CHECK_RET(ret);

	ret = test_http_post();
	CHECK_RET(ret);

exit:
	return (ret == S_OK) ? 0 : -1;
}
