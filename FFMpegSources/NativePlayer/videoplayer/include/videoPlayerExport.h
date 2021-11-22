#ifndef VIDEOPLAYER_EXPORT_H
#define VIDEOPLAYER_EXPORT_H

#include "IUnityGraphics.h"

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
	VIDEOPLAYERLIB_API void player_getOneFrameBuffer_Done(unsigned char* buf);
	VIDEOPLAYERLIB_API int player_shutdown();
	VIDEOPLAYERLIB_API int player_get_width();
	VIDEOPLAYERLIB_API int player_get_height();
	VIDEOPLAYERLIB_API unsigned long player_get_duration();

	VIDEOPLAYERLIB_API void player_setconfig(bool splityuv);

	// using unity command buffer
	VIDEOPLAYERLIB_API void* player_get_peek_y_buffer();
	VIDEOPLAYERLIB_API void* player_get_peek_u_buffer();
	VIDEOPLAYERLIB_API void* player_get_peek_v_buffer();

	VIDEOPLAYERLIB_API void* player_get_pop_y_buffer();
	VIDEOPLAYERLIB_API void* player_get_pop_u_buffer();
	VIDEOPLAYERLIB_API void* player_get_pop_v_buffer();
}

// using unity3d texture updating event (Y,UV texture data)
extern "C" UnityRenderingEventAndData UNITY_INTERFACE_EXPORT get_texture_callback_yuv_plane();

#endif