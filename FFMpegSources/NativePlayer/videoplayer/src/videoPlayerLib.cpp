#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <inttypes.h>

#include <chrono>
#include "videoPlayerExport.h"

#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif

//#define _CRT_SECURE_NO_WARNINGS
#pragma warning(disable:4996)

// import ffmpeg contents

using namespace std;

// learn to media program
// 1. https://github.com/leandromoreira/ffmpeg-libav-tutorial#video---what-you-see

/*
 * http://ffmpeg.org/doxygen/trunk/index.html
 *
 * Main components
 *
 * Format (Container) - a wrapper, providing sync, metadata and muxing for the streams.
 * Stream - a continuous stream (audio or video) of data over time.
 * Codec - defines how data are enCOded (from Frame to Packet)
 *        and DECoded (from Packet to Frame).
 * Packet - are the data (kind of slices of the stream data) to be decoded as raw frames.
 * Frame - a decoded raw frame (to be encoded or filtered).
 */


//
// Example for decode video frame with ffmpeg lib
// 

 // https://stackoverflow.com/questions/1041866/what-is-the-effect-of-extern-c-in-c
extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h> 
};

// print out the steps and errors
static void logging(const char *fmt, ...);
// decode packets into frames
static int decode_packet(AVPacket *pPacket, AVCodecContext *pCodecContext, AVFrame *pFrame);
// save bytes to local 
static void save_bytes_to_file(unsigned char* buf, int size, char *filename);
static void updateFrame(float deltatime);
static void init();
static void mainLoop();

// save to YUV files
static FILE *fp_yuv = NULL;
static AVFrame *frameYUV = NULL;
static struct SwsContext *img_convert_ctx;

// frame buffer to unity
static int _frameBufferSize = 0;
static unsigned char* _frameBuffer = NULL;

// inter update loop
static std::chrono::steady_clock::time_point _lastUpdate;
static float _deltaTime = 0.0f;
static float _lastInterval = 0.0f;
static int targetFPS = 60;
static float _interval = 0.0f;

// 
// make engine config
// 

void updateFrame(float deltatime)
{
	// std::cout << "update time: " << deltatime << std::endl;
}

void init()
{
	_interval = 1.0f / targetFPS;
	_deltaTime = 0.0f;
	_lastUpdate = std::chrono::steady_clock::now();

	std::cout << "FPS: " << targetFPS << std::endl;
	std::cout << "Interval: " << _interval << "(ms)" << std::endl;
}

void mainLoop()
{
	while (true)
	{
		auto now = std::chrono::steady_clock::now();
		_deltaTime = std::chrono::duration_cast<std::chrono::microseconds>(now - _lastUpdate).count() / 1000000.0f;
		_lastInterval += _deltaTime;
		if (_lastInterval >= _interval)
		{
			updateFrame(_deltaTime);
			_lastInterval = 0.0f;
		}
		else
		{
			Sleep(1);
		}
		_lastUpdate = now;
	}
}

// 
//  Test for ffmpeg decode one frame
//
int enter_test_videoPlayer_static_Lib()
{
	std::cout << "Make ffmpeg exmaple project" << std::endl;
	// std::string mediapath = "E:/Lab/simplevideodemo/FFMpegSources/NativePlayer/build/NativePlayer/Debug/small_bunny_1080p_60fps.mp4";
	std::string mediapath = "E:/Lab/simplevideodemo/FFMpegSources/Resources/big_buck_bunny.mp4";

	// mediapath = "./../../../../Resources/small_bunny_1080p_60fps.mp4";
	std::string mark = "\\";
	//#ifdef _WIN32
	//	mark = "\\";
	//#else
	//	mark = "/";
	//#endif
	//
	//	std::string exePath = std::string(argv[0]);
	//	std::string base = exePath.substr(0, exePath.find_last_of(mark));
	//	std::cout << "Exe folder::";
	//	std::cout << base << std::endl;

	std::cout << "Try open media Path : " << mediapath << endl;

	// return 0;
	// init();
	// mainLoop();
	// return 0;
	const char *inputmediaPath = mediapath.c_str();

	fp_yuv = fopen("output.yuv", "wb+");

	logging("initializing all the containers, codecs and protocols.");

	// AVFormatContext holds the header information from the format (Container)
	// Allocating memory for this component
	// http://ffmpeg.org/doxygen/trunk/structAVFormatContext.html
	AVFormatContext *pFormatContext = avformat_alloc_context();
	if (!pFormatContext) {
		logging("ERROR could not allocate memory for Format Context");
		return -1;
	}

	logging("opening the input file (%s) and loading format (container) header", inputmediaPath);
	// Open the file and read its header. The codecs are not opened.
	// The function arguments are:
	// AVFormatContext (the component we allocated memory for),
	// url (filename),
	// AVInputFormat (if you pass NULL it'll do the auto detect)
	// and AVDictionary (which are options to the demuxer)
	// http://ffmpeg.org/doxygen/trunk/group__lavf__decoding.html#ga31d601155e9035d5b0e7efedc894ee49
	if (avformat_open_input(&pFormatContext, inputmediaPath, NULL, NULL) != 0) {
		logging("ERROR could not open the file");
		return -1;
	}

	// now we have access to some information about our file
	// since we read its header we can say what format (container) it's
	// and some other information related to the format itself.
	logging("Input Video Informations ---->Format %s, duration %lld us, bit_rate %lld", pFormatContext->iformat->name, pFormatContext->duration, pFormatContext->bit_rate);

	logging("finding stream info from format");
	// read Packets from the Format to get stream information
	// this function populates pFormatContext->streams
	// (of size equals to pFormatContext->nb_streams)
	// the arguments are:
	// the AVFormatContext
	// and options contains options for codec corresponding to i-th stream.
	// On return each dictionary will be filled with options that were not found.
	// https://ffmpeg.org/doxygen/trunk/group__lavf__decoding.html#gad42172e27cddafb81096939783b157bb
	if (avformat_find_stream_info(pFormatContext, NULL) < 0) {
		logging("ERROR could not get the stream info");
		return -1;
	}

	// the component that knows how to enCOde and DECode the stream
	// it's the codec (audio or video)
	// http://ffmpeg.org/doxygen/trunk/structAVCodec.html
	AVCodec *pCodec = NULL;
	// this component describes the properties of a codec used by the stream i
	// https://ffmpeg.org/doxygen/trunk/structAVCodecParameters.html
	AVCodecParameters *pCodecParameters = NULL;
	int video_stream_index = -1;

	// just decode video stream
	int findVideoStreamFlag = 0;

	// loop though all the streams and print its main information
	for (int i = 0; i < pFormatContext->nb_streams; i++)
	{
		AVCodecParameters *pLocalCodecParameters = NULL;
		pLocalCodecParameters = pFormatContext->streams[i]->codecpar;
		logging("AVStream->time_base before open coded %d/%d", pFormatContext->streams[i]->time_base.num, pFormatContext->streams[i]->time_base.den);
		logging("AVStream->r_frame_rate before open coded %d/%d", pFormatContext->streams[i]->r_frame_rate.num, pFormatContext->streams[i]->r_frame_rate.den);
		logging("AVStream->start_time %" PRId64, pFormatContext->streams[i]->start_time);
		logging("AVStream->duration %" PRId64, pFormatContext->streams[i]->duration);

		logging("finding the proper decoder (CODEC)");

		AVCodec *pLocalCodec = NULL;

		// finds the registered decoder for a codec ID
		// https://ffmpeg.org/doxygen/trunk/group__lavc__decoding.html#ga19a0ca553277f019dd5b0fec6e1f9dca
		pLocalCodec = avcodec_find_decoder(pLocalCodecParameters->codec_id);

		if (pLocalCodec == NULL) {
			logging("ERROR unsupported codec!");
			return -1;
		}

		// when the stream is a video we store its index, codec parameters and codec
		if (pLocalCodecParameters->codec_type == AVMEDIA_TYPE_VIDEO) {
			if (video_stream_index == -1) {
				video_stream_index = i;
				pCodec = pLocalCodec;
				pCodecParameters = pLocalCodecParameters;
				findVideoStreamFlag = 1;
			}
			logging("Video Codec: resolution %d x %d", pLocalCodecParameters->width, pLocalCodecParameters->height);
		}
		else if (pLocalCodecParameters->codec_type == AVMEDIA_TYPE_AUDIO) {
			logging("Just Skip Audio Codec: %d channels, sample rate %d", pLocalCodecParameters->channels, pLocalCodecParameters->sample_rate);
		}

		// print its name, id and bitrate
		logging("\tCodec %s ID %d bit_rate %lld", pLocalCodec->name, pLocalCodec->id, pLocalCodecParameters->bit_rate);

		if (findVideoStreamFlag == 1)
		{
			logging("we find video stream just skip other stream");
			break;
		}
	}
	// https://ffmpeg.org/doxygen/trunk/structAVCodecContext.html
	AVCodecContext *pCodecContext = avcodec_alloc_context3(pCodec);
	if (!pCodecContext)
	{
		logging("failed to allocated memory for AVCodecContext");
		return -1;
	}

	// Fill the codec context based on the values from the supplied codec parameters
	// https://ffmpeg.org/doxygen/trunk/group__lavc__core.html#gac7b282f51540ca7a99416a3ba6ee0d16
	if (avcodec_parameters_to_context(pCodecContext, pCodecParameters) < 0)
	{
		logging("failed to copy codec params to codec context");
		return -1;
	}

	// Initialize the AVCodecContext to use the given AVCodec.
	// https://ffmpeg.org/doxygen/trunk/group__lavc__core.html#ga11f785a188d7d9df71621001465b0f1d
	if (avcodec_open2(pCodecContext, pCodec, NULL) < 0)
	{
		logging("failed to open codec through avcodec_open2");
		return -1;
	}

	// https://ffmpeg.org/doxygen/trunk/structAVFrame.html
	AVFrame *pFrame = av_frame_alloc();
	frameYUV = av_frame_alloc();

	if (!pFrame || !frameYUV)
	{
		logging("failed to allocated memory for AVFrame");
		return -1;
	}

	unsigned char *out_buffer;
	out_buffer = (unsigned char *)av_malloc(av_image_get_buffer_size(
		AV_PIX_FMT_YUV420P,
		pCodecContext->width,
		pCodecContext->height, 1));

	av_image_fill_arrays(frameYUV->data, frameYUV->linesize, out_buffer,
		AV_PIX_FMT_YUV420P,
		pCodecContext->width,
		pCodecContext->height,
		1);

	// https://ffmpeg.org/doxygen/trunk/structAVPacket.html
	AVPacket *pPacket = av_packet_alloc();
	if (!pPacket)
	{
		logging("failed to allocated memory for AVPacket");
		return -1;
	}

	img_convert_ctx = sws_getContext(pCodecContext->width, pCodecContext->height, pCodecContext->pix_fmt,
		pCodecContext->width, pCodecContext->height, AV_PIX_FMT_YUV420P, SWS_BICUBIC, NULL, NULL, NULL);

	int response = 0;
	// int how_many_packets_to_process = 30 * 60;
	int how_many_packets_to_process = 20;
	// fill the Packet with data from the Stream
	// https://ffmpeg.org/doxygen/trunk/group__lavf__decoding.html#ga4fdb3084415a82e3810de6ee60e46a61
	while (av_read_frame(pFormatContext, pPacket) >= 0)
	{
		// if it's the video stream
		if (pPacket->stream_index == video_stream_index) {
			logging("AVPacket->pts %" PRId64, pPacket->pts);
			response = decode_packet(pPacket, pCodecContext, pFrame);
			if (response < 0)
				break;
			// stop it, otherwise we'll be saving hundreds of frames
			if (--how_many_packets_to_process <= 0) break;
		}
		// https://ffmpeg.org/doxygen/trunk/group__lavc__packet.html#ga63d5a489b419bd5d45cfd09091cbcbc2
		av_packet_unref(pPacket);
	}

	logging("releasing all the resources");
	fclose(fp_yuv);
	sws_freeContext(img_convert_ctx);
	avformat_close_input(&pFormatContext);
	av_packet_free(&pPacket);
	av_frame_free(&pFrame);
	av_frame_free(&frameYUV);
	avcodec_free_context(&pCodecContext);
	return 0;
}

static void logging(const char *fmt, ...)
{
	va_list args;
	fprintf(stderr, "LOG: ");
	va_start(args, fmt);
	vfprintf(stderr, fmt, args);
	va_end(args);
	fprintf(stderr, "\n");
}

static void print_error(const char * premessage, int response)
{
	char errStr[1024] = { 0 };
	av_strerror(response, errStr, sizeof(errStr));
	logging("%s: %s", premessage, errStr);
}

static int decode_packet(AVPacket *pPacket, AVCodecContext *pCodecContext, AVFrame *pFrame)
{
	// Supply raw packet data as input to a decoder
	// https://ffmpeg.org/doxygen/trunk/group__lavc__decoding.html#ga58bc4bf1e0ac59e27362597e467efff3
	int response = avcodec_send_packet(pCodecContext, pPacket);

	if (response < 0) {
		print_error("Error while sending a packet to the decoder:", response);
		return response;
	}

	while (response >= 0)
	{
		// Return decoded output data (into a frame) from a decoder
		// https://ffmpeg.org/doxygen/trunk/group__lavc__decoding.html#ga11e6542c4e66d3028668788a1a74217c
		response = avcodec_receive_frame(pCodecContext, pFrame);
		if (response == AVERROR(EAGAIN) || response == AVERROR_EOF) {
			break;
		}
		else if (response < 0) {
			print_error("Error while receiving a frame from the decoder: %s", response);
			return response;
		}

		logging("---------------->Codec Context AVPixelFormat: %s", av_get_pix_fmt_name(pCodecContext->pix_fmt));

		if (response >= 0) {
			logging(
				"Frame %d (type=%c, size=%d bytes) pts %d key_frame %d [DTS %d]",
				pCodecContext->frame_number,
				av_get_picture_type_char(pFrame->pict_type),
				pFrame->pkt_size,
				pFrame->pts,
				pFrame->key_frame,
				pFrame->coded_picture_number
			);

			// convert to YUV420 frame use sws_scale
			// pCodecContext->pix_fmt has be YUV420 ?
			// just simple check if YUV420 Frame
			AVFrame *targetFrame = pFrame;
			sws_scale(img_convert_ctx,
				(const unsigned char* const *)pFrame->data,
				pFrame->linesize, 0, pCodecContext->height,
				frameYUV->data, frameYUV->linesize);

			// memory align
			// line size
			std::cout << "frameYUV Linesize:" << frameYUV->linesize[0] << ",pFrame Line Size:" << pFrame->linesize[0] << endl;
			std::cout << "frameYUV Linesize:" << frameYUV->linesize[1] << ",pFrame Line Size:" << pFrame->linesize[1] << endl;
			std::cout << "frameYUV Linesize:" << frameYUV->linesize[2] << ",pFrame Line Size:" << pFrame->linesize[2] << endl;

			targetFrame = frameYUV;
			std::cout << "use sws_scale convert frame to YUV420 frame data" << endl;

			int y_size = pFrame->width * pFrame->height;
			int tmpsize = frameYUV->width * pFrame->height;
			std::cout << "yuv size: " << tmpsize << std::endl;

			fwrite(targetFrame->data[0], 1, y_size, fp_yuv);
			fwrite(targetFrame->data[1], 1, y_size / 4, fp_yuv);
			fwrite(targetFrame->data[2], 1, y_size / 4, fp_yuv);

			// split to single YUV file
			char filename[1024];
			snprintf(filename, sizeof(filename), "%s-%d-y.origin", "frame", pCodecContext->frame_number);
			save_bytes_to_file(targetFrame->data[0], y_size, filename);
			cout << "-> save Y :" << filename << endl;

			snprintf(filename, sizeof(filename), "%s-%d-u.origin", "frame", pCodecContext->frame_number);
			save_bytes_to_file(targetFrame->data[1], y_size / 4, filename);
			cout << "-> save U :" << filename << endl;

			snprintf(filename, sizeof(filename), "%s-%d-v.origin", "frame", pCodecContext->frame_number);
			save_bytes_to_file(targetFrame->data[2], y_size / 4, filename);
			cout << "-> save V :" << filename << endl;

			logging("success to decode 1 YUV frame");
		}
	}
	return 0;
}


static void save_bytes_to_file(unsigned char* buf, int size, char *filename)
{
	FILE *f;
	f = fopen(filename, "w");
	fwrite(buf, 1, size, f);
	fclose(f);
}
