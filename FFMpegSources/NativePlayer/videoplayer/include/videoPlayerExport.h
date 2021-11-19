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
	int VIDEOPLAYERLIB_API enter_test_videoPlayer_static_Lib();

	void VIDEOPLAYERLIB_API player_init();
	void VIDEOPLAYERLIB_API player_pause();
	void VIDEOPLAYERLIB_API player_resume();
	void VIDEOPLAYERLIB_API player_getInformation();
	void VIDEOPLAYERLIB_API player_shutdown();
}

#endif