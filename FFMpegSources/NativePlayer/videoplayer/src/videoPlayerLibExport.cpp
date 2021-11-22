#include "videoPlayerExport.h"
#include "simpleVideoPlayer.h"
#include "IUnityRenderingExtensions.h"
#include <math.h>
#include <stdint.h>
#include <stdlib.h>

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

void* player_get_y_buffer()
{
	return m_player.getYBuffer();
}

void* player_get_u_buffer()
{
	return m_player.getUBuffer();
}

void* player_get_v_buffer()
{
	return m_player.getVBuffer();
}

// https://github.com/keijiro/TextureUpdateExample
// how to update texture
uint32_t Plasma(int x, int y, int width, int height, unsigned int frame)
{
	float px = (float)x / width;
	float py = (float)y / height;
	float time = frame / 60.0f;

	float l = sinf(px * sinf(time * 1.3f) + sinf(py * 4 + time) * sinf(time));

	uint32_t r = sinf(l * 6) * 127 + 127;
	uint32_t g = sinf(l * 7) * 127 + 127;
	uint32_t b = sinf(l * 10) * 127 + 127;

	return r + (g << 8) + (b << 16) + 0xff000000u;
}

void unity_texture_update_callback(int eventID, void* data)
{
	//if (eventID == kUnityRenderingExtEventUpdateTextureBeginV2)
	//{
	//	// UpdateTextureBegin: Generate and return texture image data.
	//	UnityRenderingExtTextureUpdateParamsV2 *params = (UnityRenderingExtTextureUpdateParamsV2 *)data;
	//	unsigned int frame = params->userData;

	//	uint32_t *img = (uint32_t *)malloc(params->width * params->height * 4);
	//	for (int y = 0; y < params->height; y++)
	//		for (int x = 0; x < params->width; x++)
	//			img[y * params->width + x] =
	//			Plasma(x, y, params->width, params->height, frame);

	//	params->texData = img;
	//}
	//else if (eventID == kUnityRenderingExtEventUpdateTextureEndV2)
	//{
	//	// UpdateTextureEnd: Free up the temporary memory.
	//	UnityRenderingExtTextureUpdateParamsV2 *params = (UnityRenderingExtTextureUpdateParamsV2 *)data;
	//	free(params->texData);
	//}

	if (eventID == kUnityRenderingExtEventUpdateTextureBeginV2)
	{
		// UpdateTextureBegin: Generate and return texture image data.
		UnityRenderingExtTextureUpdateParamsV2 *params = (UnityRenderingExtTextureUpdateParamsV2 *)data;
		unsigned int planeType = params->userData;
		int ret = player_renderOneFrame();
		void* framedata = NULL;
		if (ret == 0)
		{
			// y -> 0
			// u -> 1
			// v -> 2
			if (planeType == 0)
			{
				framedata = player_get_y_buffer();
			}
			else if (planeType == 1)
			{
				framedata = player_get_u_buffer();
			}
			else if (planeType == 2)
			{
				framedata = player_get_v_buffer();
			}
			params->texData = (unsigned char*)framedata;
		}
		else
		{
			params->texData = NULL;
		}
	}
	else if (eventID == kUnityRenderingExtEventUpdateTextureEndV2)
	{
		UnityRenderingExtTextureUpdateParamsV2 *params = (UnityRenderingExtTextureUpdateParamsV2 *)data;
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