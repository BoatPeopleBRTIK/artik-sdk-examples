#include <sys/select.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <pthread.h>

#include <artik_module.h>
#include <artik_platform.h>
#include <artik_serial.h>
#include <artik_loop.h>

#define CHECK_RET(x)	{ if (x != S_OK) goto exit; }

/*
 * This is a loopback test. On Artik 520 development platform,
 * connect a wire between "TX" and "RX" pins
 * on connector J26.
 */
static artik_serial_config config = {
	ARTIK_A520_SCOM_XSCOM4,
	"UART3",
	ARTIK_SERIAL_BAUD_115200,
	ARTIK_SERIAL_PARITY_NONE,
	ARTIK_SERIAL_DATA_8BIT,
	ARTIK_SERIAL_STOP_1BIT,
	ARTIK_SERIAL_FLOWCTRL_NONE,
	NULL
};

#define MAX_RX_BUF	64

static void forward_data(void *param, char *buf)
{
	if (buf != NULL)
		fprintf(stdout, "Forward read: %s\n", buf);
	else {
		artik_serial_module *serial = (artik_serial_module *)artik_request_api_module("serial");
		artik_loop_module *loop = (artik_loop_module *)artik_request_api_module("loop");
		artik_serial_handle handle = (artik_serial_handle)param;

		serial->unset_received_callback(handle);
		loop->quit();

		artik_release_api_module(serial);
		artik_release_api_module(loop);
	}
}

static artik_error test_serial_loopback(int platid)
{
	artik_serial_module *serial = (artik_serial_module *)artik_request_api_module("serial");
	artik_loop_module *loop = (artik_loop_module *)artik_request_api_module("loop");
	artik_serial_handle handle = NULL;
	artik_error ret = S_OK;
	char tx_buf[] = "This is a test buffer containing test data\0";
	int tx_len = strlen(tx_buf);
	char buff[128] = "";
	int len = 128;
	int maxlen = 0;

	if (platid == ARTIK520) {
		config.port_num = ARTIK_A520_SCOM_XSCOM4;
		config.name = "UART3";
	} else if (platid == ARTIK1020) {
		config.port_num = ARTIK_A1020_SCOM_XSCOM2;
		config.name = "UART1";
	} else {
		config.port_num = ARTIK_A710_UART0;
		config.name = "UART4";
	}

	fprintf(stdout, "TEST: %s\n", __func__);

	serial->request(&handle, &config);
	if (ret != S_OK) {
		fprintf(stderr, "TEST: %s failed to request serial port (%d)\n",
			__func__, ret);
		return ret;
	}
	/* Send test data */
	ret = serial->write(handle, tx_buf, &tx_len);
	if (ret != S_OK) {
		fprintf(stderr, "TEST: %s failed to send data (%d)\n", __func__, ret);
		goto exit;
	}

	fprintf(stdout, "TEST: with read\n");
	while (maxlen < tx_len) {
		serial->read(handle, &buff[maxlen], &len);
		if (strlen(buff) > (unsigned int)maxlen)
			maxlen = strlen(buff);
	}
	fprintf(stdout, "buff : %s\n", buff);
	fprintf(stdout, "TEST: with callback\n");
	/* Wait for read data to become available */
	char tx_bufs[] = "This is a second test\0";

	tx_len = strlen(tx_bufs);
	ret = serial->write(handle, tx_bufs, &tx_len);
	ret = serial->set_received_callback(handle, forward_data, (void *)handle);
	loop->run();
	if (ret != S_OK) {
		fprintf(stderr, "TEST: %s failed while waiting for RX data (%d)\n", __func__, ret);
		goto exit;
	}
	serial->unset_received_callback(handle);
	fprintf(stdout, "TEST: %s succeeded\n", __func__);
exit:
	serial->release(handle);

	artik_release_api_module(serial);
	artik_release_api_module(loop);
	return ret;
}

int main(void)
{
	artik_error ret = S_OK;
	int platid = artik_get_platform();

	if (!artik_is_module_available(ARTIK_MODULE_SERIAL)) {
		fprintf(stdout, "TEST: Serial module is not available, skipping test...\n");
		return -1;
	}

	if ((platid == ARTIK520) || (platid == ARTIK1020) || (platid == ARTIK710)) {
		ret = test_serial_loopback(platid);
		CHECK_RET(ret);
	}

exit:
	return (ret == S_OK) ? 0 : -1;
}
