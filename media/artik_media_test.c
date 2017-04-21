
#include <stdio.h>
#include <unistd.h>

#include <artik_module.h>
#include <artik_media.h>
#include <artik_loop.h>

#define CHECK_RET(x)	{ if (x != S_OK) goto exit; }

char *sound_filename = NULL;

static void on_finished(void *userdata)
{
	artik_loop_module *loop = (artik_loop_module *)userdata;

	fprintf(stdout, "TEST: finished playing sound\n");

	loop->quit();
}

static artik_error media_test_sound_playback(void)
{
	artik_error ret = S_OK;
	artik_media_module *media = (artik_media_module *)artik_request_api_module("media");
	artik_loop_module *loop = (artik_loop_module *)artik_request_api_module("loop");

	fprintf(stdout, "TEST: %s starting\n", __func__);

	ret = media->play_sound_file(sound_filename);
	if (ret != S_OK)
		goto exit;

	ret = media->set_finished_callback(on_finished, (void*)loop);
	if (ret != S_OK)
		goto exit;

	loop->run();

exit:
	fprintf(stdout, "TEST: %s %s\n", __func__, (ret == S_OK) ? "succeeded" : "failed");

	artik_release_api_module(media);
	artik_release_api_module(loop);

	return ret;
}

int main(int argc, char *argv[])
{
	int opt;
	artik_error ret = S_OK;

	while ((opt = getopt(argc, argv, "f:")) != -1) {
		switch (opt) {
		case 'f':
			sound_filename = strndup(optarg, strlen(optarg));
			break;
		default:
			printf("Usage: media-test [-f <filename to play>] \r\n");
			return 0;
		}
	}

	ret = media_test_sound_playback();
	CHECK_RET(ret);

exit:
	if (sound_filename != NULL)
		free(sound_filename);

	return (ret == S_OK) ? 0 : -1;
}
