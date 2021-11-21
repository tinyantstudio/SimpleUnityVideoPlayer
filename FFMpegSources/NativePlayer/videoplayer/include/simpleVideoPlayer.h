#ifndef VIDEOPLAYER_LIB_H
#define VIDEOPLAYER_LIB_H

#include <iostream>
#include <string>
#include <sstream>
#include <deque>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
};

class simpleVideoPlayer
{
public:
	simpleVideoPlayer();
	int init();
	int shutdown();
	int startPlayVideo(std::string filepath);
	int renderFrame();
	void* getFrameBuffer();
	void freeFrameBuffer(unsigned char* buf);
	int getCurFrameIndex();

	int getWidth();
	int getHeight();

private:
	int decodePacket();
private:
	void logging(const char *fmt, ...)
	{
		va_list args;
		fprintf(stderr, "LOG: ");
		va_start(args, fmt);
		vfprintf(stderr, fmt, args);
		va_end(args);
		fprintf(stderr, "\n");
	}

	void print_error(const char * premessage, int response)
	{
		char errStr[1024] = { 0 };
		av_strerror(response, errStr, sizeof(errStr));
		logging("%s: %s", premessage, errStr);
	}

	void save_bytes_to_file(unsigned char* buf, int size, char* filename)
	{
		FILE *f;
		f = fopen(filename, "w");
		fwrite(buf, 1, size, f);
		fclose(f);
	}

private:
	int _currentFrame;
	int _video_stream_index;
	bool _hasInitSuc;
	bool _readyToRenderVideoFrame;
	std::deque<unsigned char*> _backBuffer;
private:
	AVFrame* _pFrameYUV;
	AVFrame* _pOriginFrame;
	struct SwsContext *_img_convert_ctx;
	AVFormatContext* _pFormatContext;
	AVCodec* _pCodec;
	AVCodecParameters* _pCodecParameters;
	AVPacket* _pPacket;
	AVCodecContext* _pCodecContext;
};

#endif