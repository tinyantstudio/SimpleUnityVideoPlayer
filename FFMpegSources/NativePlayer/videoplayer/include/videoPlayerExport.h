#ifndef VIDEOPLAYER_EXPORT_H
#define VIDEOPLAYER_EXPORT_H

#ifdef VIDEOPLAYERLIB_EXPORTS
#define VIDEOPLAYERLIB_API __declspec(dllexport)
#else
#define VIDEOPLAYERLIB_API __declspec(dllimport)
#endif

// define C interface
extern "C"
{
	int VIDEOPLAYERLIB_API enter_test_videoPlayer_static_Lib();
	int VIDEOPLAYERLIB_API make_sub_please(int a, int b);

	int VIDEOPLAYERLIB_API get_framebuffer_size();
	unsigned char * VIDEOPLAYERLIB_API get_framebuffer_data();

}

#endif