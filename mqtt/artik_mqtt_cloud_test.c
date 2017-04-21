#include <artik_module.h>
#include <artik_platform.h>
#include <artik_loop.h>
#include <artik_mqtt.h>
#include <stdio.h>

#define MAX_MSG_LEN		512
#define MAX_UUID_LEN	128

static artik_mqtt_module *mqtt = NULL;
static artik_loop_module *loop = NULL;

static char device_id[MAX_UUID_LEN] = "< fill up with AKC device ID >";
static char token[MAX_UUID_LEN] = "< fill up with AKC token >";
static char pub_msg[MAX_MSG_LEN] = "< fill up with message to send >";

void on_connect_subscribe(artik_mqtt_config *client_config, void *user_data, artik_error result)
{
	artik_mqtt_handle *client_data = (artik_mqtt_handle *) client_config->handle;
	artik_mqtt_msg *msg = (artik_mqtt_msg *)user_data;
	char pub_topic[MAX_UUID_LEN + 128];
	artik_error ret;

	if (result == S_OK && client_data) {
		/* Subscribe to receive actions */
		ret = mqtt->subscribe(client_data, msg->qos, msg->topic);
		if (ret == S_OK)
			fprintf(stdout, "subscribe success\n");
		else
			fprintf(stderr, "subscribe err: %s\n", error_msg(ret));

		/* Publish message */
		snprintf(pub_topic, sizeof(pub_topic), "/v1.1/messages/%s", device_id);
		ret = mqtt->publish(client_data, 0, false, pub_topic, strlen(pub_msg), pub_msg);
		if (ret == S_OK)
			fprintf(stdout, "publish success\n");
		else
			fprintf(stderr, "publish err: %s\n", error_msg(ret));
	}
}

void on_message_disconnect(artik_mqtt_config *client_config, void *user_data, artik_mqtt_msg *msg)
{
	artik_mqtt_handle *client_data;
	artik_mqtt_module *user_mqtt = (artik_mqtt_module *) user_data;

	if (msg && client_config) {
		fprintf(stdout, "topic %s, content %s\n", msg->topic, (char *)msg->payload);
		client_data = (artik_mqtt_handle *) client_config->handle;
		user_mqtt->disconnect(client_data);
	}
}

void on_disconnect(artik_mqtt_config *client_config, void *user_data, artik_error result)
{
	artik_mqtt_handle *client_data = (artik_mqtt_handle *) client_config->handle;
	artik_mqtt_module *user_mqtt = (artik_mqtt_module *) user_data;

	if (result == S_OK) {
		fprintf(stdout, "disconnected\n");
		if (client_data) {
			user_mqtt->destroy_client(client_data);
			client_data = NULL;
			loop->quit();
		}
	}
}

void on_publish(artik_mqtt_config *client_config, void *user_data, int result)
{
	fprintf(stdout, "message published (%d)\n", result);
}

int main(int argc, char *argv[])
{
	char *host = "api.artik.cloud";
	int broker_port = 8883;
	char sub_topic[MAX_UUID_LEN + 128];
	artik_mqtt_config config;
	artik_mqtt_msg subscribe_msg;
	artik_mqtt_handle client;

	/* Use parameters if provided, keep defaults otherwise */
	if (argc > 2) {
		memset(device_id, 0, MAX_UUID_LEN);
		strncpy(device_id, argv[1], MAX_UUID_LEN);
		memset(token, 0, MAX_UUID_LEN);
		strncpy(token, argv[2], MAX_UUID_LEN);

		if (argc > 3) {
			memset(pub_msg, 0, MAX_MSG_LEN);
			strncpy(pub_msg, argv[3], MAX_MSG_LEN);
		}
	}

	fprintf(stdout, "Using ID: %s\n", device_id);
	fprintf(stdout, "Using token: %s\n", token);
	fprintf(stdout, "Message: %s\n", pub_msg);

	mqtt = (artik_mqtt_module *)artik_request_api_module("mqtt");
	loop = artik_request_api_module("loop");

	memset(&subscribe_msg, 0, sizeof(artik_mqtt_msg));
	snprintf(sub_topic, sizeof(sub_topic), "/v1.1/actions/%s", device_id);
	subscribe_msg.topic = sub_topic;
	subscribe_msg.qos = 0;

	memset(&config, 0, sizeof(artik_mqtt_config));
	config.client_id = "sub_client";
	config.block = true;
	config.user_name = device_id;
	config.pwd = token;

	/* Connect to server */
	mqtt->create_client(&client, &config);
	mqtt->tls_set(client, "/etc/ssl/certs/ca-bundle.crt", NULL, NULL, NULL, NULL);

	mqtt->set_connect(client, on_connect_subscribe, &subscribe_msg);
	mqtt->set_disconnect(client, on_disconnect, mqtt);
	mqtt->set_publish(client, on_publish, mqtt);
	mqtt->set_message(client, on_message_disconnect, mqtt);

	mqtt->connect(client, host, broker_port);

	loop->run();

	artik_release_api_module(mqtt);
	artik_release_api_module(loop);

	return 0;
}
