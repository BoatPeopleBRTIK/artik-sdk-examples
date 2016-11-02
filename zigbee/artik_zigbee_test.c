#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <string.h>
#include <assert.h>
#include <artik_log.h>
#include <artik_loop.h>
#include <artik_zigbee.h>
#include <artik_module.h>
#include <artik_platform.h>

#define COMMAND1	"Easy network form"
#define COMMAND2	"Network leave"
#define COMMAND3	"Network find & join"
#define COMMAND4	"Get my network status"
#define COMMAND5	"Get device information"
#define COMMAND6	"On/Off"
#define COMMAND7	"Identify"
#define COMMAND8	"Manual form & join"
#define COMMAND9	"Level control"


#define EASY_PJOIN_DURATION			0x3C
#define EASY_IDENTIFY_DURATION		10	/* seconds */
#define KEYBOARD_INPUT_SIZE			100
#define MANUAL_TEST_CHANNEL			25
#define MANUAL_TEST_TX_POWER		ZIGBEE_TX_POWER_2
#define MANUAL_TEST_PANID1			0x1234
#define MANUAL_TEST_PANID2			0x4321

#define STR_ON_OFF_SWITCH			"ON_OFF_SWITCH"
#define STR_ON_OFF_LIGHT			"ON_OFF_LIGHT"
#define STR_DIMMABLE_LIGHT			"DIMMABLE_LIGHT"
#define STR_LEVEL_CONTROL_SWITCH	"LEVEL_CONTROL_SWITCH"

static struct zigbee_network_info saved_network_info;

/* TODO: this is used to check all device information for debugging only */
extern struct zigbee_device_info *get_device_info(void);

static void _print_prompt(void)
{
	printf("[test_zigbee] Select command: ");
}

static void _usage_print(void)
{
	fprintf(stdout, "Usage\n");
	fprintf(stdout, "h : usage\n");
	fprintf(stdout, "1 : %s\n", COMMAND1);
	fprintf(stdout, "2 : %s\n", COMMAND2);
	fprintf(stdout, "3 : %s\n", COMMAND3);
	fprintf(stdout, "4 : %s\n", COMMAND4);
	fprintf(stdout, "5 : %s\n", COMMAND5);
	fprintf(stdout, "6 : %s\n", COMMAND6);
	fprintf(stdout, "7 : %s\n", COMMAND7);
	fprintf(stdout, "8 : %s\n", COMMAND8);
	fprintf(stdout, "9 : %s\n", COMMAND9);
	fprintf(stdout, "q : quit\n");
}

static void _release_network_info(void)
{
	memset(&saved_network_info, 0, sizeof(struct zigbee_network_info));
}

static void _save_network_info(const struct zigbee_network_info *network_info)
{
	if (saved_network_info.channel != 0) {
		warn("saved network info is waiting for user action!");
		return;
	}
	saved_network_info.channel = network_info->channel;
	saved_network_info.pan_id = network_info->pan_id;
	saved_network_info.tx_power = network_info->tx_power;
}

static void _check_saved_network(char command)
{
	enum zigbee_notification result;
	artik_zigbee_module *zb = (artik_zigbee_module *)artik_request_api_module("zigbee");

	if (saved_network_info.channel != 0) {
		switch (command) {
		case 'Y':
		case 'y':
			result = zb->network_join_manually(&saved_network_info);
			if (result != ZIGBEE_CMD_SUCCESS) {
				warn("In callback, zigbee_network_join failed:%d", result);
				return;
			}
			break;
		}
		_release_network_info();
	}
	artik_release_api_module(zb);
}

static void _network_leave(void)
{
	enum zigbee_notification result;
	artik_zigbee_module *zb = (artik_zigbee_module *)artik_request_api_module("zigbee");

	result = zb->network_leave();
	if (result != ZIGBEE_CMD_SUCCESS)
		fprintf(stdout, "zigbee_network_leave failed:%d\n", result);
	artik_release_api_module(zb);
}

static void _do_manual_form_join_request(void)
{
	enum zigbee_notification result;
	struct zigbee_network_info network_info;
	artik_zigbee_module *zb = (artik_zigbee_module *)artik_request_api_module("zigbee");

	dbg("Manual form %d %d 0x%4X", MANUAL_TEST_CHANNEL, MANUAL_TEST_TX_POWER, MANUAL_TEST_PANID1);
	network_info.channel = MANUAL_TEST_CHANNEL;
	network_info.tx_power = MANUAL_TEST_TX_POWER;
	network_info.pan_id = MANUAL_TEST_PANID1;

	result = zb->network_form_manually(&network_info);

	if (result == ZIGBEE_CMD_SUCCESS)
		dbg("Succeeded");
	else
		warn("Failed to form network manually");
	sleep(2);

	dbg("Leaving from the network");
	result = zb->network_leave();
	if (result == ZIGBEE_CMD_SUCCESS)
		dbg("Succeeded");
	else
		warn("Failed to leave from the network");
	sleep(2);

	dbg("Manual join %d %d 0x%4X", MANUAL_TEST_CHANNEL, MANUAL_TEST_TX_POWER,
															MANUAL_TEST_PANID2);
	network_info.pan_id = MANUAL_TEST_PANID2;
	result = zb->network_join_manually(&network_info);
	if (result == ZIGBEE_CMD_SUCCESS)
		dbg("Succeeded");
	else
		warn("Failed to join network manually");

	artik_release_api_module(zb);
}

static void _do_easy_device_onoff(void)
{
	int i;
	enum zigbee_notification result;
	struct zigbee_endpoint *endpoint;
	struct zigbee_endpoint_list endpointList;
	artik_zigbee_module *zb = (artik_zigbee_module *)artik_request_api_module("zigbee");

	zb->device_find_by_cluster(&endpointList, ZCL_ON_OFF_CLUSTER_ID, 1);
	if (endpointList.num == 0)
		fprintf(stdout, "there is no endpoint to send on/off command\n");
	else {
		fprintf(stdout, "found %d endpoints to request on/off command\n", endpointList.num);
		for (i = 0; i < endpointList.num; i++) {
			endpoint = &endpointList.endpoint[i];
			fprintf(stdout, "sending command to device 0x%4x, endpoint %d\n", endpoint->device_id, endpoint->endpoint_id);
			result = zb->onoff_command(endpoint, ZIGBEE_ONOFF_TOGGLE);
			if (result != ZIGBEE_CMD_SUCCESS)
				fprintf(stdout, "failed to send command: %d\n", result);
			else
				fprintf(stdout, "In do_easy_device_onoff, sent\n");
			sleep(2);

			result = zb->onoff_command(endpoint, ZIGBEE_ONOFF_OFF);
			if (result != ZIGBEE_CMD_SUCCESS)
				fprintf(stdout, "failed to send command: %d\n", result);
			else
				fprintf(stdout, "In do_easy_device_onoff, sent\n");
			sleep(2);

			result = zb->onoff_command(endpoint, ZIGBEE_ONOFF_ON);
			if (result != ZIGBEE_CMD_SUCCESS)
				fprintf(stdout, "failed to send command: %d\n", result);
			else
				fprintf(stdout, "In do_easy_device_onoff, sent\n");
		}
	}

	artik_release_api_module(zb);
}

static void _do_easy_identify_request(void)
{
	int i, j;
	int remained_time;
	enum zigbee_notification result;
	struct zigbee_endpoint *endpoint;
	struct zigbee_endpoint_list endpointList;
	artik_zigbee_module *zb = (artik_zigbee_module *)artik_request_api_module("zigbee");

	zb->device_find_by_cluster(&endpointList, ZCL_IDENTIFY_CLUSTER_ID, 1);
	if (endpointList.num == 0)
		warn("there is no endpoint to send identify command");
	else {
		dbg("found %d endpoints to request identify command",
															endpointList.num);
		for (i = 0; i < endpointList.num; i++) {
			endpoint = &endpointList.endpoint[i];
			result = zb->identify_request(endpoint,	EASY_IDENTIFY_DURATION + i);
			if (result != ZIGBEE_CMD_SUCCESS)
				warn("failed to send command: %d", result);
			else
				dbg("In do_easy_identify_request, sent");
			sleep((int)((EASY_IDENTIFY_DURATION + i) / 2));

			for (j = 0; j < (EASY_IDENTIFY_DURATION + i) / 2; j++) {
				remained_time = zb->identify_get_remaining_time(endpoint);
				if (remained_time < 0) {
					warn("failed to get remaining time!");
					break;
				}
				info("In do_easy_identify_request, remained time %d",
																remained_time);
				if (remained_time == 0)
					break;
				sleep(1);
			}
		}
	}

	artik_release_api_module(zb);
}

static void _do_easy_device_level_control_request(int with_onoff)
{
	int i;
	enum zigbee_notification result;
	struct zigbee_endpoint *endpoint;
	struct zigbee_endpoint_list endpointList;
	struct zigbee_level_control_command level_control_cmd = {0x0,};
	artik_zigbee_module *zb = (artik_zigbee_module *)artik_request_api_module("zigbee");

	zb->device_find_by_cluster(&endpointList, ZCL_LEVEL_CONTROL_CLUSTER_ID, 1);
	if (endpointList.num == 0)
		warn("there is no endpoint to send level control command");
	else {
		dbg("found %d endpoints to request level control command",
															endpointList.num);
		for (i = 0; i < endpointList.num; i++) {
			endpoint = &endpointList.endpoint[i];
			info("sending command to device 0x%4x, endpoint %d",
								endpoint->device_id, endpoint->endpoint_id);

			level_control_cmd.control_type =
				with_onoff ? ZIGBEE_MOVE_TO_LEVEL_ONOFF : ZIGBEE_MOVE_TO_LEVEL;
			level_control_cmd.parameters.move_to_level.level = 0;
			level_control_cmd.parameters.move_to_level.transition_time = 2;

			result = zb->level_control_request(endpoint,
														&level_control_cmd);
			if (result != ZIGBEE_CMD_SUCCESS)
				warn("failed to send command: %d", result);
			else
				dbg("In _do_easy_device_level_control_request, sent");

			sleep(2);

			level_control_cmd.control_type =
								with_onoff ? ZIGBEE_MOVE_ONOFF: ZIGBEE_MOVE;
			level_control_cmd.parameters.move.control_mode =
													ZIGBEE_LEVEL_CONTROL_UP;
			level_control_cmd.parameters.move.rate = 5;

			result = zb->level_control_request(endpoint,
														&level_control_cmd);
			if (result != ZIGBEE_CMD_SUCCESS)
				warn("failed to send command: %d", result);
			else
				dbg("In _do_easy_device_level_control_request, sent");

			sleep(2);

			level_control_cmd.control_type =
								with_onoff ? ZIGBEE_STEP_ONOFF: ZIGBEE_STEP;
			level_control_cmd.parameters.step.control_mode =
													ZIGBEE_LEVEL_CONTROL_DOWN;
			level_control_cmd.parameters.step.step_size = 1;
			level_control_cmd.parameters.step.transition_time = 2;

			result = zb->level_control_request(endpoint,
														&level_control_cmd);
			if (result != ZIGBEE_CMD_SUCCESS)
				warn("failed to send command: %d", result);
			else
				dbg("In _do_easy_device_level_control_request, sent");

			sleep(2);

			level_control_cmd.control_type =
								with_onoff ? ZIGBEE_STOP_ONOFF: ZIGBEE_STOP;
			result = zb->level_control_request(endpoint,
														&level_control_cmd);
			if (result != ZIGBEE_CMD_SUCCESS)
				warn("failed to send command: %d", result);
			else
				dbg("In _do_easy_device_level_control_request, sent");

			sleep(2);
		}
	}

	artik_release_api_module(zb);
}

static void _print_device_info(struct zigbee_device_info *device_info)
{
	int i, j, k;
	struct zigbee_device *device = NULL;

	if (device_info) {
		fprintf(stdout, "device count %d\n", device_info->num);
		for (i = 0; i < device_info->num; i++) {
			device = &device_info->device[i];
			if (device->device_id == 0 && device->endpoint_count == 0)
				continue;
			fprintf(stdout, "device %d: eui 0x%x%x%x%x%x%x%x%x, device id 0x%2x\n", i, device->eui64[0], device->eui64[1], device->eui64[2],
				device->eui64[3], device->eui64[4], device->eui64[5], device->eui64[6], device->eui64[7], device->device_id);
			for (j = 0; j < MAX_ENDPOINT_SIZE && j < device->endpoint_count; j++) {
				fprintf(stdout, "\nendpoint %d\n", device->endpoint[j].endpoint_id);
				for (k = 0; k < MAX_CLUSTER_SIZE && device->endpoint[j].server_cluster[k] >= 0; k++)
					fprintf(stdout, "\ncluster id 0x%x, SERVER\n", device->endpoint[j].server_cluster[k]);
				for (k = 0; k < MAX_CLUSTER_SIZE && device->endpoint[j].client_cluster[k] >= 0; k++)
					fprintf(stdout, "\ncluster id 0x%x, CLIENT\n", device->endpoint[j].client_cluster[k]);
			}
			fprintf(stdout, "\n");
		}
	} else
		fprintf(stdout, "device info is NULL\n");
}

void _callback(void *data, void *user_data)
{
	struct zigbee_response *response = (struct zigbee_response *)data;
	struct zigbee_network_info *network_info = NULL;
	struct zigbee_onoff_info *onoff_info = NULL;
	struct zigbee_groups_info *group_info = NULL;
	struct zigbee_level_control_command *level_command = NULL;
	struct zigbee_level_control_update *level_update = NULL;
	int result;
	int onoff_result = ZIGBEE_ONOFF_OFF;
	artik_zigbee_module *zb = (artik_zigbee_module *)artik_request_api_module("zigbee");

	fprintf(stdout, "In callback, response type : %d\n", response->type);

	switch (response->type) {
	case ZIGBEE_RESPONSE_NOTIFICATION:
		result = (int)strtol(response->payload, NULL, 10);
		switch (result) {
		case ZIGBEE_CMD_SUCCESS:
			fprintf(stdout, "In callback, ZIGBEE_CMD_SUCCESS\n");
			break;
		case ZIGBEE_CMD_ERR_PORT_PROBLEM:
		case ZIGBEE_CMD_ERR_NO_SUCH_COMMAND:
		case ZIGBEE_CMD_ERR_WRONG_NUMBER_OF_ARGUMENTS:
		case ZIGBEE_CMD_ERR_ARGUMENT_OUT_OF_RANGE:
		case ZIGBEE_CMD_ERR_ARGUMENT_SYNTAX_ERROR:
		case ZIGBEE_CMD_ERR_STRING_TOO_LONG:
		case ZIGBEE_CMD_ERR_INVALID_ARGUMENT_TYPE:
		case ZIGBEE_CMD_ERR:
			fprintf(stdout, "In callback, COMMAND ERROR(%d)!\n", result);
			break;
		case ZIGBEE_NO_MESSAGE:
			fprintf(stdout, "In callback, ZIGBEE_NO_MESSAGE\n");
			break;
		default:
			fprintf(stdout, "In callback, response %d\n", result);
			break;
		}
		break;
	case ZIGBEE_RESPONSE_NETWORK_NOTIFICATION:
		result = (int)strtol(response->payload, NULL, 10);
		switch (result) {
		case ZIGBEE_NETWORK_JOIN:
			fprintf(stdout, "In callback, ZIGBEE_NETWORK_JOIN\n");
			break;
		case ZIGBEE_NETWORK_LEAVE:
			fprintf(stdout, "In callback, ZIGBEE_NETWORK_LEAVE\n");
			break;
		case ZIGBEE_NETWORK_FIND_FORM:
			result = zb->network_permitjoin(EASY_PJOIN_DURATION);
			if (result != ZIGBEE_CMD_SUCCESS) {
				fprintf(stdout, "zigbee_network_permitjoin failed:%d\n", result);
				return;
			}
			break;
		case ZIGBEE_NETWORK_FIND_FORM_FAILED:
			fprintf(stdout, "In callback, ZIGBEE_NETWORK_FIND_FORM_FAILED\n");
			break;
		case ZIGBEE_NETWORK_FIND_JOIN:
			fprintf(stdout, "In callback, ZIGBEE_NETWORK_FIND_JOIN\n");
			break;
		case ZIGBEE_NETWORK_FIND_JOIN_FAILED:
			fprintf(stdout, "In callback, ZIGBEE_NETWORK_FIND_JOIN_FAILED\n");
			break;
		default:
			fprintf(stdout, "In callback, response %d\n", result);
			break;
		}
		break;
	case ZIGBEE_RESPONSE_NETWORK_SCAN_RESULT:
		network_info = (struct zigbee_network_info *)response->payload;
		if (!network_info)
			return;
		info("Scan result- channel %d, tx power %d, pan id 0x%4X",
		network_info->channel, network_info->tx_power, network_info->pan_id);
		_save_network_info(network_info);
		info("Will you join? [y/n]");
		break;
	case ZIGBEE_RESPONSE_DEVICE_INFO:
		fprintf(stdout, "Received new device information\n");
		_print_device_info((struct zigbee_device_info *)response->payload);
		break;
	case ZIGBEE_RESPONSE_ONOFF_COMMAND:
		onoff_info = (struct zigbee_onoff_info *)response->payload;
		if (!onoff_info)
			return;
		fprintf(stdout, "ZIGBEE ONOFF endpoint [%d] command [%d]: [%d] -> [%d]\n", onoff_info->endpoint_id, onoff_info->command, onoff_info->prev_value, onoff_info->curr_value);
		onoff_result = zb->onoff_get_value(onoff_info->endpoint_id);
		if (onoff_result == ZIGBEE_ONOFF_ON)
			fprintf(stdout, "ZIGBEE ONOFF Get Value [ON]\n");
		else if (onoff_result == ZIGBEE_ONOFF_OFF)
			fprintf(stdout, "ZIGBEE ONOFF Get Value [OFF]\n");
		else
			fprintf(stdout, "ZIGBEE ONOFF Get Value [ERR]\n");
		break;
	case ZIGBEE_RESPONSE_GROUPS_INFO:
		group_info = (struct zigbee_groups_info *)response->payload;
		if (!group_info)
			return;
		info("ZIGBEE_RESPONSE_GROUPS_INFO group ID [%d] group cmd [%d]",
								group_info->group_id, group_info->group_cmd);
		if (group_info->group_cmd == ZIGBEE_GROUPS_ADD_IF_IDENTIFYING)
			info("ZIGBEE_GROUPS_ADD_IF_IDENTIFYING");

		if (zb->groups_name_support(group_info->endpoint_id))
			info("ZIGBEE_GROUPS : name supported");
		else
			info("ZIGBEE_GROUPS : name not supported");
		break;
	case ZIGBEE_RESPONSE_LEVEL_CONTROL_COMMAND:
		level_command = (struct zigbee_level_control_command *)response->payload;
		if (!level_command)
			return;
		switch(level_command->control_type) {
		case ZIGBEE_MOVE_TO_LEVEL:
			info("Move to level, level %d, transition time %dms",
					level_command->parameters.move_to_level.level,
					level_command->parameters.move_to_level.transition_time);
			break;
		case ZIGBEE_MOVE:
			info("Move, mode %d, rate %d",
					level_command->parameters.move.control_mode,
					level_command->parameters.move.rate);
			break;
		case ZIGBEE_STEP:
			info("Step, mode %d, step size %d, transition time %dms",
					level_command->parameters.step.control_mode,
					level_command->parameters.step.step_size,
					level_command->parameters.step.transition_time);
			break;
		case ZIGBEE_STOP:
			info("Stop");
			break;
		case ZIGBEE_MOVE_TO_LEVEL_ONOFF:
			info("Move to level(on/off), level %d, transition time %dms",
					level_command->parameters.move_to_level.level,
					level_command->parameters.move_to_level.transition_time);
			break;
		case ZIGBEE_MOVE_ONOFF:
			info("Move(on/off), mode %d, rate %d",
					level_command->parameters.move.control_mode,
					level_command->parameters.move.rate);
			break;
		case ZIGBEE_STEP_ONOFF:
			info("Step(on/off), mode %d, step size %d, transition time %dms",
					level_command->parameters.step.control_mode,
					level_command->parameters.step.step_size,
					level_command->parameters.step.transition_time);
			break;
		case ZIGBEE_STOP_ONOFF:
			info("Stop(on/off)");
			break;
		default: break;
		}
		break;
	case ZIGBEE_RESPONSE_LEVEL_CONTROL_UPDATE:
		level_update = (struct zigbee_level_control_update *)response->payload;
		info("Current level update from %d to %d", level_update->prev_level,
				level_update->curr_level);

		result = zb->level_control_get_value(level_update->endpoint_id);
		info("Double check of Current Level: %d", result);
		break;
	default:
		break;
	}
	fprintf(stdout, "[testzigbee] callback end\n");

	artik_release_api_module(zb);
}

static void _print_network_status(int network_state)
{
	switch (network_state) {
	case ZIGBEE_NO_NETWORK:
		fprintf(stdout, "state ZIGBEE_NO_NETWORK\n");
		break;
	case ZIGBEE_JOINING_NETWORK:
		fprintf(stdout, "state ZIGBEE_JOINING_NETWORK\n");
		break;
	case ZIGBEE_JOINED_NETWORK:
		fprintf(stdout, "state ZIGBEE_JOINED_NETWORK\n");
		break;
	case ZIGBEE_JOINED_NETWORK_NO_PARENT:
		fprintf(stdout, "state ZIGBEE_JOINED_NETWORK_NO_PARENT\n");
		break;
	case ZIGBEE_LEAVING_NETWORK:
		fprintf(stdout, "state ZIGBEE_LEAVING_NETWORK\n");
		break;
	default:
		break;
	}
}

static void _print_node_type(int node_type)
{
	switch (node_type) {
	case ZIGBEE_UNKNOWN_DEVICE:
		fprintf(stdout, "node type ZIGBEE_UNKNOWN_DEVICE\n");
		break;
	case ZIGBEE_COORDINATOR:
		fprintf(stdout, "node type ZIGBEE_COORDINATOR\n");
		break;
	case ZIGBEE_ROUTER:
		fprintf(stdout, "node type ZIGBEE_ROUTER\n");
		break;
	case ZIGBEE_END_DEVICE:
		fprintf(stdout, "node type ZIGBEE_END_DEVICE\n");
		break;
	case ZIGBEE_SLEEPY_END_DEVICE:
		fprintf(stdout, "node type ZIGBEE_SLEEPY_END_DEVICE\n");
		break;
	}
}

static int _get_device_id(const char *str_device_id)
{
	if (!strcmp(str_device_id, STR_ON_OFF_SWITCH))
		return DEVICE_ON_OFF_SWITCH;
	else if (!strcmp(str_device_id, STR_ON_OFF_LIGHT))
		return DEVICE_ON_OFF_LIGHT;
	else if (!strcmp(str_device_id, STR_DIMMABLE_LIGHT))
		return DEVICE_DIMMABLE_LIGHT;
	else if (!strcmp(str_device_id, STR_LEVEL_CONTROL_SWITCH))
		return DEVICE_LEVEL_CONTROL_SWITCH;
	else
		return -1;
}

static int _on_keyboard_received(int fd, enum watch_io io, void *user_data)
{
	char command[KEYBOARD_INPUT_SIZE];
	char *line_p = NULL;
	artik_loop_module *loop = (artik_loop_module *)artik_request_api_module("loop");
	artik_zigbee_module *zb = (artik_zigbee_module *)artik_request_api_module("zigbee");

	if (!(fd == STDIN_FILENO))
		fprintf(stdout, "%s STDIN_FILENO failed\n", __func__);
	if (!(io == WATCH_IO_IN || io == WATCH_IO_ERR || io == WATCH_IO_HUP || io == WATCH_IO_NVAL))
		fprintf(stdout, "%s io failed\n", __func__);
	if (user_data)
		fprintf(stdout, "%p %s user_data failed\n", user_data, __func__);
	if (fgets(command, KEYBOARD_INPUT_SIZE, stdin) == NULL)
		return 1;

	if ((line_p = strchr(command, '\n')) != NULL)
		*line_p ='\0';

	if (strlen(command) < 1) {
		_print_prompt();
		return 1;
	}

	switch (command[0]) {
	case 'h':
		_usage_print();
		break;
	case '1':
		zb->network_form();
		fprintf(stdout, "Wait for response\n");
		break;
	case '2':
		_network_leave();
		fprintf(stdout, "Done\n");
		break;
	case '3':
		zb->network_join();
		fprintf(stdout, "Wait for response\n");
		break;
	case '4':
		_print_network_status(zb->network_request_my_network_status());
		_print_node_type(zb->device_request_my_node_type());
		fprintf(stdout, "Done\n");
		break;
	case '5':
		zb->device_discover();
		dbg("Wait for response");
		break;
	case '6':
		_do_easy_device_onoff();
		dbg("Done");
		break;
	case '7':
		_do_easy_identify_request();
		dbg("Done");
		break;
	case '8':
		_do_manual_form_join_request();
		dbg("Done");
		break;
	case '9':
		_do_easy_device_level_control_request(0);
		_do_easy_device_level_control_request(1);
		dbg("Done");
		break;
	case 'q':
		loop->quit();
		return 0;
	/* These are for callback */
	case 'y':
	case 'Y':
	case 'n':
	case 'N':
		_check_saved_network(command[0]);
		dbg("Done");
		break;
	default:
		break;
	}
	fprintf(stdout, "\n[testzigbee] Select command:\n");

	artik_release_api_module(loop);
	artik_release_api_module(zb);

	return 1;
}

int main(int argc, char *argv[])
{
	artik_error ret = S_OK;
	int deviceId = -1;
	int i;
	int device_list[MAX_ENDPOINT_SIZE];
	int count = 0;

	_release_network_info();
	artik_loop_module *loop = (artik_loop_module *)artik_request_api_module("loop");
	artik_zigbee_module *zb = (artik_zigbee_module *)artik_request_api_module("zigbee");

	if (!artik_is_module_available(ARTIK_MODULE_ZIGBEE)) {
		fprintf(stdout, "TEST: Zigbee module is not available, skipping test...\n");
		return -1;
	}

	if (argc > 1) {
		if (argc > MAX_ENDPOINT_SIZE + 1)
			fprintf(stdout, "only %d device type is supported currently!\n", MAX_ENDPOINT_SIZE);
		for (i = 1; i < argc && i < MAX_ENDPOINT_SIZE + 1; i++) {
			fprintf(stdout, "Device type is %s\n", argv[i]);
			deviceId = _get_device_id(argv[i]);
			if (deviceId == -1)
				fprintf(stdout, "not supported device type!!\n");
			else
				device_list[count++] = deviceId;
		}

		if (count == 0)
			ret = zb->initialize(_callback, NULL, 0, NULL);
		else
			ret = zb->initialize(_callback, NULL, count, device_list);
	} else
		ret = zb->initialize(_callback, NULL, 0, NULL);

	if (ret == E_NOT_SUPPORTED) {
		artik_release_api_module(loop);
		artik_release_api_module(zb);
		return -1;
	}

	if (zb->network_start() == ZIGBEE_JOINED_NETWORK) {
		_print_network_status(zb->network_request_my_network_status());
		_print_node_type(zb->device_request_my_node_type());
	} else
		fprintf(stdout, "Privious Network : Non Exist\n");
	loop->add_fd_watch(STDIN_FILENO, (WATCH_IO_IN | WATCH_IO_ERR | WATCH_IO_HUP | WATCH_IO_NVAL), _on_keyboard_received, NULL, NULL);

	loop->run();

	artik_release_api_module(loop);
	artik_release_api_module(zb);

	return (ret == S_OK) ? 0 : -1;
}
