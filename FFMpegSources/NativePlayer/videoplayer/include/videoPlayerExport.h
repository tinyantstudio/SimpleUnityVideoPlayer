#ifndef VIDEOPLAYER_EXPORT_H
#define VIDEOPLAYER_EXPORT_H

#include "IUnityGraphics.h"

//#ifdef VIDEOPLAYERLIB_EXPORTS
//#define VIDEOPLAYERLIB_API __declspec(dllexport)
//#else
//#define VIDEOPLAYERLIB_API __declspec(dllimport)
//#endif

#if defined(__CYGWIN32__)
    #define VIDEOPLAYERLIB_API __stdcall
    #define VIDEOPLAYERLIB_EXPORT __declspec(dllexport)
#elif defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(_WIN64) || defined(WINAPI_FAMILY)
    #define VIDEOPLAYERLIB_API __stdcall
    #define VIDEOPLAYERLIB_EXPORT __declspec(dllexport)
#elif defined(__MACH__) || defined(__ANDROID__) || defined(__linux__)
    #define VIDEOPLAYERLIB_API
    #define VIDEOPLAYERLIB_EXPORT
#else
    #define VIDEOPLAYERLIB_API
    #define VIDEOPLAYERLIB_EXPORT
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
	VIDEOPLAYERLIB_API void* player_peek_y_buffer();
	VIDEOPLAYERLIB_API void* player_peek_u_buffer();
	VIDEOPLAYERLIB_API void* player_peek_v_buffer();

	VIDEOPLAYERLIB_API void* player_get_pop_y_buffer();
	VIDEOPLAYERLIB_API void* player_get_pop_u_buffer();
	VIDEOPLAYERLIB_API void* player_get_pop_v_buffer();
}

// using unity3d texture updating event (Y,UV texture data)
extern "C" UnityRenderingEventAndData UNITY_INTERFACE_EXPORT get_texture_callback_yuv_plane();

#endif
