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

#define KEYBOARD_INPUT_SIZE			100

void _callback(void *data, void *user_data)
{
	artik_zigbee_response *response = (artik_zigbee_response *)data;
	int result;

	log_dbg("In callback, response type : %d", response->type);

	switch (response->type) {
	case ZIGBEE_RESPONSE_NOTIFICATION:
		result = (int)strtol(response->payload, NULL, 10);
		switch (result) {
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
			log_err("In callback, COMMAND ERROR(%d)!", result);
			break;
		case ZIGBEE_NO_MESSAGE:
			log_warn("In callback, ZIGBEE_NO_MESSAGE");
			break;
		default:
			log_dbg("In callback, response %d", result);
			break;
		}
		break;
	case ZIGBEE_RESPONSE_NETWORK_NOTIFICATION:
		result = (int)strtol(response->payload, NULL, 10);
		switch (result) {
		case ZIGBEE_NETWORK_JOIN:
			log_info("In callback, ZIGBEE_NETWORK_JOIN");
			break;
		case ZIGBEE_NETWORK_LEAVE:
			log_info("In callback, ZIGBEE_NETWORK_LEAVE");
			break;
		case ZIGBEE_NETWORK_FIND_JOIN:
			log_info("In callback, ZIGBEE_NETWORK_FIND_JOIN");
			break;
		case ZIGBEE_NETWORK_FIND_JOIN_FAILED:
			log_warn("In callback, ZIGBEE_NETWORK_FIND_JOIN_FAILED");
			break;
		default:
			log_dbg("In callback, response %d", result);
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
	int deviceId = -1;
	int i;
	int device_list[MAX_ENDPOINT_SIZE];
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
			deviceId = _get_device_id(argv[i]);
			if (deviceId == -1)
				log_warn("not supported device type!!");
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
		log_dbg("Privious Network : Non Exist");

	loop->add_fd_watch(STDIN_FILENO, (WATCH_IO_IN | WATCH_IO_ERR | WATCH_IO_HUP | WATCH_IO_NVAL), _on_keyboard_received, NULL, NULL);

	loop->run();

	artik_release_api_module(loop);
	artik_release_api_module(zb);

	log_dbg("Stop zigbee cli program");

	return (ret == S_OK) ? 0 : -1;
}
