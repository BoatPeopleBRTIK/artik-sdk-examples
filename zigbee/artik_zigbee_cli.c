#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <assert.h>

#include <artik_log.h>
#include <artik_loop.h>
#include <artik_zigbee.h>
#include <artik_module.h>
#include <artik_platform.h>

#define STR_ON_OFF_SWITCH			"ON_OFF_SWITCH"
#define STR_ON_OFF_LIGHT			"ON_OFF_LIGHT"
#define STR_DIMMABLE_LIGHT			"DIMMABLE_LIGHT"
#define STR_LEVEL_CONTROL_SWITCH	"LEVEL_CONTROL_SWITCH"
#define STR_COLOR_DIMMABLE_LIGHT	"COLOR_DIMMABLE_LIGHT"
#define STR_ON_OFF_LIGHT_SWITCH		"ON_OFF_LIGHT_SWITCH"
#define STR_DIMMER_SWITCH			"DIMMER_SWITCH"
#define STR_COLOR_DIMMER_SWITCH		"COLOR_DIMMER_SWITCH"
#define STR_LIGHT_SENSOR			"LIGHT_SENSOR"
#define STR_OCCUPANCY_SENSOR		"OCCUPANCY_SENSOR"
#define STR_HEATING_COOLING_UNIT	"HEATING_COOLING_UNIT"
#define STR_THERMOSTAT				"THERMOSTAT"
#define STR_TEMPERATURE_SENSOR		"TEMPERATURE_SENSOR"
#define STR_REMOTE_CONTROL			"REMOTE_CONTROL"

#define KEYBOARD_INPUT_SIZE			100

void _callback(void *user_data, artik_zigbee_response_type response_type,
			   void *payload)
{
	artik_zigbee_notification notification;
	artik_zigbee_network_notification network_notification;

	log_dbg("In callback, response type : %d", response_type);

	switch (response_type) {
	case ZIGBEE_RESPONSE_NOTIFICATION:
		notification = *((artik_zigbee_notification *) payload);
		switch (notification) {
		case ZIGBEE_CMD_SUCCESS:
			log_info("In callback, ZIGBEE_CMD_SUCCESS");
			break;
		case ZIGBEE_CMD_ERR_PORT_PROBLEM:
		case ZIGBEE_CMD_ERR_NO_SUCH_COMMAND:
		case ZIGBEE_CMD_ERR_WRONG_NUMBER_OF_ARGUMENTS:
		case ZIGBEE_CMD_ERR_ARGUMENT_OUT_OF_RANGE:
		case ZIGBEE_CMD_ERR_ARGUMENT_SYNTAX_ERROR:
		case ZIGBEE_CMD_ERR_STRING_TOO_LONG:
		case ZIGBEE_CMD_ERR_INVALID_ARGUMENT_TYPE:
		case ZIGBEE_CMD_ERR:
			log_err("In callback, COMMAND ERROR(%d)!", notification);
			break;
		default:
			log_dbg("In callback, response %d", notification);
			break;
		}
		break;
	case ZIGBEE_RESPONSE_NETWORK_NOTIFICATION:
		network_notification = *((artik_zigbee_network_notification *) payload);
		switch (network_notification) {
		case ZIGBEE_NETWORK_JOIN:
			log_info("In callback, ZIGBEE_NETWORK_JOIN");
			break;
		case ZIGBEE_NETWORK_LEAVE:
			log_info("In callback, ZIGBEE_NETWORK_LEAVE");
			break;
		case ZIGBEE_NETWORK_FIND_JOIN_SUCCESS:
			log_info("In callback, ZIGBEE_NETWORK_FIND_JOIN_SUCCESS");
			break;
		case ZIGBEE_NETWORK_FIND_JOIN_FAILED:
			log_warn("In callback, ZIGBEE_NETWORK_FIND_JOIN_FAILED");
			break;
		default:
			log_dbg("In callback, response %d", network_notification);
			break;
		}
		break;
	default:
		break;
	}
	log_dbg("callback end");
}

static int _on_keyboard_received(int fd, enum watch_io io, void *user_data)
{
	char command[KEYBOARD_INPUT_SIZE];
	artik_zigbee_module *zb = (artik_zigbee_module *)artik_request_api_module("zigbee");

	assert(fd == STDIN_FILENO);
	assert(io == WATCH_IO_IN || io == WATCH_IO_ERR || io == WATCH_IO_HUP
														|| io == WATCH_IO_NVAL);
	assert(user_data == NULL);

	if (fgets(command, KEYBOARD_INPUT_SIZE, stdin) == NULL)
		return 1;
	if (strlen(command) > 1)
		zb->raw_request(command);

	artik_release_api_module(zb);
	return 1;
}

static artik_error _get_device_id(artik_zigbee_module *zb,
								  const char *str_device_id,
								  artik_zigbee_endpoint_handle *handle)
{
	artik_error result;

	if (!strcmp(str_device_id, STR_ON_OFF_SWITCH))
		result = zb->get_device(1, ZIGBEE_PROFILE_HA,
								DEVICE_ON_OFF_SWITCH, handle);
	else if (!strcmp(str_device_id, STR_ON_OFF_LIGHT))
		result = zb->get_device(19, ZIGBEE_PROFILE_HA,
								DEVICE_ON_OFF_LIGHT, handle);
	else if (!strcmp(str_device_id, STR_DIMMABLE_LIGHT))
		result = zb->get_device(20, ZIGBEE_PROFILE_HA,
								DEVICE_DIMMABLE_LIGHT, handle);
	else if (!strcmp(str_device_id, STR_LEVEL_CONTROL_SWITCH))
		result = zb->get_device(2, ZIGBEE_PROFILE_HA,
								DEVICE_LEVEL_CONTROL_SWITCH, handle);
	else if (!strcmp(str_device_id, STR_COLOR_DIMMABLE_LIGHT))
		result = zb->get_device(21, ZIGBEE_PROFILE_HA,
								DEVICE_COLOR_DIMMABLE_LIGHT, handle);
	else if (!strcmp(str_device_id, STR_ON_OFF_LIGHT_SWITCH))
		result = zb->get_device(22, ZIGBEE_PROFILE_HA,
								DEVICE_ON_OFF_LIGHT_SWITCH, handle);
	else if (!strcmp(str_device_id, STR_DIMMER_SWITCH))
		result = zb->get_device(23, ZIGBEE_PROFILE_HA,
								DEVICE_DIMMER_SWITCH, handle);
	else if (!strcmp(str_device_id, STR_COLOR_DIMMER_SWITCH))
		result = zb->get_device(24, ZIGBEE_PROFILE_HA,
								DEVICE_COLOR_DIMMER_SWITCH, handle);
	else if (!strcmp(str_device_id, STR_LIGHT_SENSOR))
		result = zb->get_device(25, ZIGBEE_PROFILE_HA,
								DEVICE_LIGHT_SENSOR, handle);
	else if (!strcmp(str_device_id, STR_OCCUPANCY_SENSOR))
		result = zb->get_device(26, ZIGBEE_PROFILE_HA,
								DEVICE_OCCUPANCY_SENSOR, handle);
	else if (!strcmp(str_device_id, STR_HEATING_COOLING_UNIT))
		result = zb->get_device(31, ZIGBEE_PROFILE_HA,
								DEVICE_HEATING_COOLING_UNIT, handle);
	else if (!strcmp(str_device_id, STR_THERMOSTAT))
		result = zb->get_device(32, ZIGBEE_PROFILE_HA,
								DEVICE_THERMOSTAT, handle);
	else if (!strcmp(str_device_id, STR_TEMPERATURE_SENSOR))
		result = zb->get_device(33, ZIGBEE_PROFILE_HA,
								DEVICE_TEMPERATURE_SENSOR, handle);
	else if (!strcmp(str_device_id, STR_REMOTE_CONTROL))
		result = zb->get_device(34, ZIGBEE_PROFILE_HA,
								DEVICE_REMOTE_CONTROL, handle);
	else
		return E_BAD_ARGS;

	return result;
}

static void _print_network_status(int network_state)
{
	switch (network_state) {
	case ZIGBEE_NO_NETWORK:
		log_info("state ZIGBEE_NO_NETWORK");
		break;
	case ZIGBEE_JOINING_NETWORK:
		log_info("state ZIGBEE_JOINING_NETWORK");
		break;
	case ZIGBEE_JOINED_NETWORK:
		log_info("state ZIGBEE_JOINED_NETWORK");
		break;
	case ZIGBEE_JOINED_NETWORK_NO_PARENT:
		log_info("state ZIGBEE_JOINED_NETWORK_NO_PARENT");
		break;
	case ZIGBEE_LEAVING_NETWORK:
		log_info("state ZIGBEE_LEAVING_NETWORK");
		break;
	default:
		break;
	}
}

static void _print_node_type(int node_type)
{
	switch (node_type) {
	case ZIGBEE_UNKNOWN_DEVICE:
		log_info("node type ZIGBEE_UNKNOWN_DEVICE");
		break;
	case ZIGBEE_COORDINATOR:
		log_info("node type ZIGBEE_COORDINATOR");
		break;
	case ZIGBEE_ROUTER:
		log_info("node type ZIGBEE_ROUTER");
		break;
	case ZIGBEE_END_DEVICE:
		log_info("node type ZIGBEE_END_DEVICE");
		break;
	case ZIGBEE_SLEEPY_END_DEVICE:
		log_info("node type ZIGBEE_SLEEPY_END_DEVICE");
		break;
	}
}

int main(int argc, char *argv[])
{
	artik_error ret = S_OK;
	int i;
	artik_zigbee_endpoint_handle device_list[MAX_ENDPOINT_SIZE];
	artik_zigbee_network_state state;
	artik_zigbee_node_type type;
	int count = 0;

	artik_loop_module *loop = (artik_loop_module *)artik_request_api_module("loop");
	artik_zigbee_module *zb = (artik_zigbee_module *)artik_request_api_module("zigbee");

	if (!artik_is_module_available(ARTIK_MODULE_ZIGBEE)) {
		fprintf(stdout, "TEST: Zigbee module is not available, skipping test...\n");
		return -1;
	}

	log_dbg("Start zigbee cli program");

	if (argc == 1) {
		log_err("Usage: zigbee_cli [DEVICE TYPE] ...");
		return 0;
	}

	if (argc > 1) {
		if (argc > MAX_ENDPOINT_SIZE + 1)
			log_warn("only %d device type is supported currently!",
															MAX_ENDPOINT_SIZE);
		for (i = 1; i < argc && i < MAX_ENDPOINT_SIZE + 1; i++) {
			log_info("Device type is %s", argv[i]);
			ret = _get_device_id(zb, argv[i], &device_list[count]);
			if (ret != S_OK)
				log_warn("not supported device type!!");
			else
				count++;
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

	if (zb->network_start(NULL) == ZIGBEE_JOINED_NETWORK) {
		ret = zb->network_request_my_network_status(&state);
		if (ret == S_OK)
			_print_network_status(state);
		else
			log_err("get network status failed: %s", error_msg(ret));
		ret = zb->device_request_my_node_type(&type);
		if (ret == S_OK)
			_print_node_type(type);
		else
			log_err("get device ndoe type failed: %s", error_msg(ret));
	} else
		log_dbg("Privious Network : Non Exist");

	loop->add_fd_watch(STDIN_FILENO, (WATCH_IO_IN | WATCH_IO_ERR | WATCH_IO_HUP | WATCH_IO_NVAL),
					   _on_keyboard_received, NULL, NULL);

	loop->run();

	for (i = 0; i < count; i++)
		zb->release_device(device_list[i]);

	artik_release_api_module(loop);
	artik_release_api_module(zb);

	log_dbg("Stop zigbee cli program");

	return (ret == S_OK) ? 0 : -1;
}
