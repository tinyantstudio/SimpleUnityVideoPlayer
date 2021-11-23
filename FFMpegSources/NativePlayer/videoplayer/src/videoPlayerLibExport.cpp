#include "videoPlayerExport.h"
#include "simpleVideoPlayer.h"
#include "IUnityRenderingExtensions.h"

// use for measure time consuming
#include <chrono>
using namespace std::chrono;

simpleVideoPlayer m_player;

int player_init()
{
	return m_player.init();
}

void player_setconfig(bool splityuv)
{
	m_player.setconfig(splityuv);
}

int player_startPlayVideo(const char* filepath)
{
	std::string path(filepath);
	return m_player.startPlayVideo(path);
}

int player_renderOneFrame()
{
	return m_player.renderFrame();
}

void* player_getOneFrameBuffer()
{
	return m_player.getFrameBuffer();
}

void player_getOneFrameBuffer_Done(unsigned char* buf)
{
	m_player.freeFrameBuffer(buf);
}

int player_shutdown()
{
	return m_player.shutdown();
}

int player_get_width()
{
	return m_player.getWidth();
}

int player_get_height()
{
	return m_player.getHeight();
}

unsigned long player_get_duration()
{
	return m_player.getDuration();
}

void* player_peek_y_buffer()
{
	return m_player.get_peek_yBuffer();
}

void* player_peek_u_buffer()
{
	return m_player.get_peek_uBuffer();
}

void* player_peek_v_buffer()
{
	return m_player.get_peek_vBuffer();
}

void* player_get_pop_y_buffer()
{
	return m_player.get_pop_yBuffer();
}

void* player_get_pop_u_buffer()
{
	return m_player.get_pop_uBuffer();
}

void* player_get_pop_v_buffer()
{
	return m_player.get_pop_vBuffer();
}

// unity3d input calba data just support uint 
void unity_texture_update_callback(int eventID, void* data)
{
	if (eventID == kUnityRenderingExtEventUpdateTextureBeginV2)
	{
		// render in unity3d
		// in renderthread just take render result
		UnityRenderingExtTextureUpdateParamsV2 *params = (UnityRenderingExtTextureUpdateParamsV2 *)data;
		unsigned int planeType = params->userData;
	
		auto millisec_since_epoch = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
		std::cout << "texture update begin type: " << planeType << ",time: " << millisec_since_epoch << std::endl;
		
		void* framedata = NULL;
		// y -> 0
		// u -> 1
		// v -> 2
		std::cout << "videoPlayerExport::unity_texture_update_callback type: " << planeType << std::endl;
		if (planeType == 0)
			framedata = player_get_pop_y_buffer();
		else if (planeType == 1)
			framedata = player_get_pop_u_buffer();
		else if (planeType == 2)
			framedata = player_get_pop_v_buffer();
		if (framedata != NULL)
			std::cout << "videoPlayerExport::unity_texture_update_callback frame data is not null..." << std::endl;
		else
			std::cout << "videoPlayerExport::unity_texture_update_callback frame data is null..." << std::endl;
		params->texData = framedata;
	}
	else if (eventID == kUnityRenderingExtEventUpdateTextureEndV2)
	{
		UnityRenderingExtTextureUpdateParamsV2 *params = (UnityRenderingExtTextureUpdateParamsV2 *)data;
		unsigned int planeType = params->userData;

		auto millisec_since_epoch = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
		std::cout << "texture update end type: " << planeType << ",time: " << millisec_since_epoch << std::endl;

		params = (UnityRenderingExtTextureUpdateParamsV2 *)data;
		if (params->texData != NULL)
		{
			player_getOneFrameBuffer_Done((unsigned char*)params->texData);
		}
	}
}

UnityRenderingEventAndData get_texture_callback_yuv_plane()
{
	return unity_texture_update_callback;
}