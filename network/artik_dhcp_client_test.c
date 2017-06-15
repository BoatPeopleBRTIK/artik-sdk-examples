#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <artik_module.h>
#include <artik_network.h>

#define CHECK_RET(x)	{ if (x != S_OK) goto exit; }

static artik_error test_dhcp_client(artik_network_interface_t interface)
{
	artik_network_module *network = (artik_network_module *)artik_request_api_module("network");
	artik_error ret;
	artik_network_dhcp_client_handle handle;
	fprintf(stdout, "TEST: %s starting\n", __func__);

	fprintf(stdout, "Starting DHCP Client\n");

	/* Start DHCP Client */
	ret = network->dhcp_client_start(&handle, interface);

	if (ret != S_OK){
		fprintf(stdout, "TEST: %s failed (err=%d)\n", __func__, ret);
		goto exit;
	}

	fprintf(stdout, "Stopping DHCP Client\n");

	/* Stop DHCP Client */
	ret = network->dhcp_client_stop(handle);

	if (ret != S_OK){
		fprintf(stdout, "TEST: %s failed (err=%d)\n", __func__, ret);
		goto exit;
	}

exit:
	artik_release_api_module(network);

	return ret;
}

int main(int argc, char *argv[])
{
	artik_error ret = S_OK;
	int opt;
	artik_network_interface_t interface = ARTIK_WIFI;

	if (!artik_is_module_available(ARTIK_MODULE_NETWORK)) {
		fprintf(stdout,
			"TEST: NETWORK module is not available, skipping test...\n");
		return -1;
	}

	while ((opt = getopt(argc, argv, "e")) != -1){
		switch(opt){
		case 'e':
			interface = ARTIK_ETHERNET;
			break;
		default:
			printf("Usage : artik-dhcp-client-test [-e for ethernet] (wifi by default)\n");
			return 0;

		}
	}

	ret = test_dhcp_client(interface);
	CHECK_RET(ret);

exit:
	return (ret == S_OK) ? 0 : -1;
}
