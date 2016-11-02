/*! \file artik_time_test.c
 *
 *  \brief TIME Test example in C
 *
 *  Instance of usage TIME module with
 *  a program developed in C.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#include <artik_module.h>
#include <artik_time.h>

#define MAX_SIZE 128
#define CHECK_RET(x)	{ if (x != S_OK) goto exit; }

static int end = 1;
artik_time_module *time_module_p;
artik_time_handle handle;
char *hostname = "fr.pool.ntp.org";

static void sig_handler(int sig)
{
	time_module_p->cancel_alarm(handle);
}

typedef void (*t_ptr_func) (int);

static artik_error test_time_loopback(void)
{
	artik_error ret = S_OK;
	artik_msecond valtime = 0, oldtime = 0, nb_seconds = 1500;
	artik_time_t val;
	t_ptr_func prev = (t_ptr_func) signal(SIGINT, sig_handler);
	artik_time_t alarm_date;
	char date[MAX_SIZE] = "";

	alarm_date.second = 0;
	alarm_date.minute = 12;
	alarm_date.hour = 17;
	alarm_date.day = 19;
	alarm_date.month = 4;
	alarm_date.year = 2016;
	alarm_date.day_of_week = 0;
	alarm_date.msecond = 0;
	val = (time_module_p->get_time(ARTIK_TIME_UTC));
	time_module_p->get_time_str(date, MAX_SIZE, 0, ARTIK_TIME_UTC);
	fprintf(stdout, "UTC GET TIME STR : %s\n", date);
	fprintf(stdout, "succeed set time [%d]\n",
		time_module_p->set_time(alarm_date));
	fprintf(stdout, "GET TIME : Val year (%u) month(%u) hour(%d)\n",
		val.year, val.month, val.hour);
	time_module_p->get_time_str(date, MAX_SIZE, 0, ARTIK_TIME_GMT2);
	fprintf(stdout, "GMT GET TIME STR : %s\n", date);
	time_module_p->get_time_str(date, MAX_SIZE, "Y/M/D h:m:s:S",
				    ARTIK_TIME_UTC);
	fprintf(stdout, "UTC GET TIME STR : %s\n", date);
	while (end) {
		valtime = time_module_p->get_tick();
		if (((valtime % nb_seconds) == 0 && valtime != oldtime)) {
			time_module_p->get_time_str(date, MAX_SIZE,
						    ARTIK_TIME_DFORMAT,
						    ARTIK_TIME_GMT2);
			fprintf(stdout, "%s\n", date);
			end--;
			oldtime = valtime;
		}
	}
	end = 1;
	time_module_p->create_alarm_second(ARTIK_TIME_GMT2, 2, &handle);
	fprintf(stdout,
		"Wait for the alarm...(Click on ctrl C for passing to next step)\n");
	while (end) {
		time_module_p->wait_alarm(handle);
		valtime = time_module_p->get_delay_alarm(handle);
		if (end != 0 && (artik_msecond)end != valtime) {
			end = valtime;
			fprintf(stdout, "end = %d seconds before end\n", end);
			fprintf(stdout,
				"Wait for the alarm...(Click on ctrl C for passing to next step)\n");
		}
	}
	signal(SIGINT, prev);
	time_module_p->delete_alarm(handle);
	fprintf(stdout, "Release TIME Module\n");

	return ret;
}

artik_error test_time_sync_ntp(void)
{
	artik_error ret;
	time_t curr_time;

	fprintf(stdout, "TEST: %s started\n", __func__);

	curr_time = time(0);
	fprintf(stdout, "Current system time: %s", ctime(&curr_time));

	ret = time_module_p->sync_ntp(hostname);
	if (ret != S_OK) {
		fprintf(stdout, "TEST: %s failed: ERROR(%d)\n", __func__,
			ret);
		return ret;
	}

	curr_time = time(0);
	fprintf(stdout, "Modified system time: %s", ctime(&curr_time));
	fprintf(stdout, "TEST: %s finished\n", __func__);

	return ret;
}

int main(void)
{
	artik_error ret = S_OK;

	time_module_p = (artik_time_module *)artik_request_api_module("time");

	ret = test_time_loopback();
	CHECK_RET(ret);

	ret = test_time_sync_ntp();
	CHECK_RET(ret);

exit:
	artik_release_api_module(time_module_p);

	return (ret == S_OK) ? 0 : -1;
}
