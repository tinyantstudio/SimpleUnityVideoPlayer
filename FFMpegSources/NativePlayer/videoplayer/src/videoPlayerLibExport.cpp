#include "videoPlayerExport.h"
#include "simpleVideoPlayer.h"


static simpleVideoPlayer m_player;

int player_init()
{
	return m_player.init();
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