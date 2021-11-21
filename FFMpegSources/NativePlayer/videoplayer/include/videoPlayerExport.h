#ifndef VIDEOPLAYER_EXPORT_H
#define VIDEOPLAYER_EXPORT_H

#ifdef VIDEOPLAYERLIB_EXPORTS
#define VIDEOPLAYERLIB_API __declspec(dllexport)
#else
#define VIDEOPLAYERLIB_API __declspec(dllimport)
#endif

// define C interface
// video plyer interface
extern "C"
{
	VIDEOPLAYERLIB_API int enter_test_videoPlayer_static_Lib();

	VIDEOPLAYERLIB_API int player_init();
	VIDEOPLAYERLIB_API int player_startPlayVideo(const char* filepath);
	VIDEOPLAYERLIB_API int player_renderOneFrame();
	VIDEOPLAYERLIB_API void* player_getOneFrameBuffer();
	VIDEOPLAYERLIB_API int player_shutdown();
	VIDEOPLAYERLIB_API int player_get_width();
	VIDEOPLAYERLIB_API int player_get_height();
}

#endif