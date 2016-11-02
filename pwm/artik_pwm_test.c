#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <artik_module.h>
#include <artik_platform.h>
#include <artik_pwm.h>

#define CHECK_RET(x)	{ if (x != S_OK) goto exit; }

static artik_pwm_config config = {
	1,
	"pwm",
	400000,
	200000,
	ARTIK_PWM_POLR_NORMAL,
	NULL
};

static artik_error pwm_test_frequency(int platid)
{
	artik_pwm_handle handle;
	artik_error ret = S_OK;
	artik_pwm_module *pwm = (artik_pwm_module *)artik_request_api_module("pwm");

	if (platid == ARTIK520)
		config.pin_num = ARTIK_A520_PWM1;
	else if (platid == ARTIK1020)
		config.pin_num = ARTIK_A1020_PWM1;
	else
		config.pin_num = ARTIK_A710_PWM0;

	fprintf(stdout, "TEST: %s\n", __func__);

	ret = pwm->request(&handle, &config);
	if (ret != S_OK) {
		fprintf(stderr, "TEST: %s - Failed to request PWM (err=%d)\n", __func__, ret);
		return ret;
	}

	/* Wait for X seconds */
	usleep(3 * 1000 * 1000);

	pwm->release(handle);

	fprintf(stdout, "TEST: %s succeeded\n", __func__);

	artik_release_api_module(pwm);

	return ret;
}

int main(void)
{
	artik_error ret = S_OK;
	int platid = artik_get_platform();

	if ((platid == ARTIK520) || (platid == ARTIK1020)  || (platid == ARTIK710)) {
		ret = pwm_test_frequency(platid);
		CHECK_RET(ret);
	}

exit:
	return (ret == S_OK) ? 0 : -1;
}
