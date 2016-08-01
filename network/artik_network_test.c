#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <artik_module.h>
#include <artik_network.h>
#include <artik_loop.h>

static artik_error test_get_current_ip(void)
{
	artik_network_module *network = (artik_network_module *)artik_request_api_module("network");
	artik_error ret;
	artik_network_ip *current_ip;

	current_ip = (artik_network_ip *) malloc(sizeof(artik_network_ip));
	fprintf(stdout, "TEST: %s\n", __func__);

	/* Check Current IP */
	ret = network->get_current_ip(current_ip);
	if (ret == S_OK)
		fprintf(stdout, "Your IP Address: %s\n", current_ip->address);
	else {
		fprintf(stdout, "TEST: %s failed (err=%d)\n", __func__, ret);
		goto exit;
	}

	fprintf(stdout, "TEST: %s succeeded\n", __func__);

exit:
	free(current_ip);

	artik_release_api_module(network);

	return ret;

}

void _callback(int *data, void *user_data)
{
	if (*data == 1)
		fprintf(stdout, "Network Connetecd\n");
	else
		fprintf(stdout, "Network doesn't connect\n");

	test_get_current_ip();
}

artik_error test_get_online_status(void)
{
	artik_network_module *network = (artik_network_module *)artik_request_api_module("network");
	artik_error ret;

	ret = network->get_online_status(_callback, NULL);
	if (ret < S_OK)
		fprintf(stdout, "TEST: %s failed (err=%d)\n", __func__, ret);

	artik_release_api_module(network);

	return ret;
}

int main(void)
{
	artik_error ret = S_OK;
	artik_loop_module *loop = (artik_loop_module *)artik_request_api_module("loop");

	if (!artik_is_module_available(ARTIK_MODULE_NETWORK)) {
		fprintf(stdout,
			"TEST: NETWORK module is not available, skipping test...\n");
		return -1;
	}

	ret = test_get_online_status();
	if (ret >= S_OK)
		loop->run();

	artik_release_api_module(loop);

	return (ret == S_OK) ? 0 : -1;
}
