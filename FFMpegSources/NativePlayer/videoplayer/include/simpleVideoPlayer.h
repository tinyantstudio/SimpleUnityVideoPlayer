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
	void setconfig(bool splityuv);
	int shutdown();
	int startPlayVideo(std::string filepath);
	int renderFrame();
	void* getFrameBuffer();
	void freeFrameBuffer(unsigned char* buf);
	unsigned long getDuration();

	// if split y,u,v buffer
	void* get_pop_yBuffer();
	void* get_pop_uBuffer();
	void* get_pop_vBuffer();

	void* get_peek_yBuffer();
	void* get_peek_uBuffer();
	void* get_peek_vBuffer();

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
	// if we split yuv to single buffer
	bool _splitYUVBuffer;
	std::deque<unsigned char*> _frameBufferQueue;
	// 0,1,2 -> y,u,v if split
	std::deque<unsigned char*> _yBufferQueue;
	std::deque<unsigned char*> _uBufferQueue;
	std::deque<unsigned char*> _vBufferQueue;
	void* pop_buffer(std::deque<unsigned char*>& queue);
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