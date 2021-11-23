#include <iostream>
#include "videoPlayerExport.h"
#include "chatEngineExport.h"
#include <assert.h>

// use for measure time consuming
#include <chrono>
using namespace std::chrono;

//
// Test for videoPlayer
//

using namespace std;

int main(int argc, const char *argv[])
{
	std::cout << "Start Video Player Now..." << std::endl;
	int ret = -1;
	//ret = enter_test_videoPlayer_static_Lib();
	//if (ret)
	//	std::cout << "Fatal Error code: " << ret << std::endl;
	//else
	//{
	//	std::cout << "========================" << std::endl;
	//	std::cout << "Run success" << std::endl;
	//	std::cout << "========================" << std::endl;
	//}

	// Test for simplePlayer class
	ret = player_init();
	player_setconfig(true);
	std::cout << "ret: " << ret << endl;
	assert(ret == 0);
	std::string mediapath = "E:/Lab/simplevideodemo/FFMpegSources/Resources/small_bunny_1080p_60fps.mp4";
	// std::string mediapath = "E:/Lab/simplevideodemo/FFMpegSources/Resources/big_buck_4K_big.mp4";
	ret = player_startPlayVideo(mediapath.c_str());
	assert(ret == 0);

	int width = player_get_width();
	int height = player_get_height();

	std::cout << "Video Width: " << width << ", Height: " << height << std::endl;
	std::cout << "Video duration: " << player_get_duration() << endl;
	int framecount = 40;
	int validframe = 0;
	while (framecount-- >= 0)
	{
		ret = player_renderOneFrame();
		unsigned char* buf = (unsigned char*)player_getOneFrameBuffer();
		if (buf != NULL)
		{
			validframe++;
			std::cout << "-----> frame data :" << validframe << std::endl;
		}
		player_getOneFrameBuffer_Done(buf);
	}
	ret = player_shutdown();
	assert(ret == 0);
	return 0;
}

