#include <artik_module.h>
#include <artik_bluetooth.h>
#include <artik_loop.h>
#include <stdio.h>

#define SERVICE_UUID_16_BIT "ffff"
#define SERVICE_UUID_32_BIT "0000180d"
#define SERVICE_UUID_128_BIT "00001802-0000-1000-8000-00805f9b34fb"

#define SERVICE_DATA_UUID_16_BIT "1234"

static void set_advertisement(artik_bt_advertisement *adv)
{
	adv->type = BT_ADV_TYPE_BROADCAST;
	adv->svc_uuid_len = 3;
	adv->svc_uuid = (const char **)malloc(sizeof(SERVICE_UUID_128_BIT)
			* adv->svc_uuid_len);
	adv->svc_uuid[0] = SERVICE_UUID_16_BIT;
	adv->svc_uuid[1] = SERVICE_UUID_32_BIT;
	adv->svc_uuid[2] = SERVICE_UUID_128_BIT;

	adv->mfr_id = 0x0075; // Samsung Electronics Co. Ltd.
	adv->mfr_data_len = 4;
	adv->mfr_data = (unsigned char*)malloc(sizeof(unsigned char*) * 4);
	adv->mfr_data[0] = 0x01;
	adv->mfr_data[1] = 0x02;
	adv->mfr_data[2] = 0x03;
	adv->mfr_data[3] = 0x04;

	adv->svc_id = SERVICE_DATA_UUID_16_BIT;
	adv->svc_data_len = 4;
	adv->svc_data = (unsigned char*)malloc(sizeof(unsigned char*) * 4);
	adv->svc_data[0] = 0x05;
	adv->svc_data[1] = 0x06;
	adv->svc_data[2] = 0x07;
	adv->svc_data[3] = 0x08;

	adv->tx_power = true;
}

int main (void)
{
	artik_bluetooth_module *bt;
	artik_loop_module *loop;
	artik_bt_advertisement adv;
	int adv_id;

	bt = (artik_bluetooth_module *)artik_request_api_module("bluetooth");
	loop = (artik_loop_module *)artik_request_api_module("loop");

	fprintf(stdout, "start broadcasting with random address\n");
	fprintf(stdout, "service includes: \n %s (16bit UUID) \n %s (32bit UUID)"
			"\n %s (128bit UUID)\n", SERVICE_UUID_16_BIT, SERVICE_UUID_32_BIT,
			SERVICE_UUID_128_BIT);

	set_advertisement(&adv);
	bt->register_advertisement(&adv, &adv_id);

	loop->run();

	bt->unregister_advertisement(adv_id);

	artik_release_api_module(bt);
	artik_release_api_module(loop);
	return 0;
}
