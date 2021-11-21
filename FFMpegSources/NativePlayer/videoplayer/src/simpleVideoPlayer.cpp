#include "simpleVideoPlayer.h"



simpleVideoPlayer::simpleVideoPlayer() :
	_pFormatContext(NULL),
	_hasInitSuc(false),
	_currentFrame(0),
	_video_stream_index(-1),
	_pFrameYUV(NULL),
	_pOriginFrame(NULL),
	_img_convert_ctx(NULL),
	_pCodec(NULL),
	_pCodecParameters(NULL),
	_readyToRenderVideoFrame(false)
{
}

int simpleVideoPlayer::init()
{
	logging("simpleVideoPlayer::init");
	_pFormatContext = avformat_alloc_context();
	if (!_pFormatContext) {
		logging("ERROR could not allocate memory for Format Context");
		_hasInitSuc = false;
		return -1;
	}
	_hasInitSuc = true;
	return 0;
}
int simpleVideoPlayer::shutdown()
{
	logging("simpleVideoPlayer::shutdown()");

	if (!_hasInitSuc)
		return  -1;
	sws_freeContext(_img_convert_ctx);
	avformat_close_input(&_pFormatContext);
	av_packet_free(&_pPacket);
	av_frame_free(&_pOriginFrame);
	av_frame_free(&_pFrameYUV);
	avcodec_free_context(&_pCodecContext);
	return 0;
}

int simpleVideoPlayer::startPlayVideo(std::string filepath)
{
	const char *inputmediaPath = filepath.c_str();
	logging("simpleVideoPlayer::startPlayVideo() opening the input file (%s) and loading format (container) header", inputmediaPath);
	if (avformat_open_input(&_pFormatContext, inputmediaPath, NULL, NULL) != 0) {
		logging("ERROR could not open the file");
		return -1;
	}

	if (avformat_find_stream_info(_pFormatContext, NULL) < 0) {
		logging("ERROR could not get the stream info");
		return -1;
	}

	// find video frame stream
	for (int i = 0; i < _pFormatContext->nb_streams; i++)
	{
		AVCodecParameters *pLocalCodecParameters = NULL;
		pLocalCodecParameters = _pFormatContext->streams[i]->codecpar;
		AVCodec *pLocalCodec = NULL;
		pLocalCodec = avcodec_find_decoder(pLocalCodecParameters->codec_id);
		if (pLocalCodec == NULL)
		{
			logging("ERROR unsupported codec!");
			return -1;
		}

		// when the stream is a video we store its index, codec parameters and codec
		if (pLocalCodecParameters->codec_type == AVMEDIA_TYPE_VIDEO)
		{
			if (_video_stream_index == -1)
			{
				_video_stream_index = i;
				_pCodec = pLocalCodec;
				_pCodecParameters = pLocalCodecParameters;
			}
		}
		else if (pLocalCodecParameters->codec_type == AVMEDIA_TYPE_AUDIO)
		{
			logging("Just Skip Audio Codec: %d channels, sample rate %d", pLocalCodecParameters->channels, pLocalCodecParameters->sample_rate);
		}

		if (_video_stream_index != -1)
		{
			logging("We find video stream just skip other stream");
			break;
		}
	}


	_pCodecContext = avcodec_alloc_context3(_pCodec);
	if (!_pCodecContext)
	{
		logging("failed to allocated memory for AVCodecContext");
		return -1;
	}

	// Fill the codec context based on the values from the supplied codec parameters
	if (avcodec_parameters_to_context(_pCodecContext, _pCodecParameters) < 0)
	{
		logging("failed to copy codec params to codec context");
		return -1;
	}
	// Initialize the AVCodecContext to use the given AVCodec.
	if (avcodec_open2(_pCodecContext, _pCodec, NULL) < 0)
	{
		logging("failed to open codec through avcodec_open2");
		return -1;
	}
	_pOriginFrame = av_frame_alloc();
	_pFrameYUV = av_frame_alloc();
	if (!_pFrameYUV || !_pOriginFrame)
	{
		logging("failed to allocated memory for AVFrame and YUVFrame");
		return -1;
	}
	unsigned char *out_buffer;
	out_buffer = (unsigned char *)av_malloc(av_image_get_buffer_size(
		AV_PIX_FMT_YUV420P,
		_pCodecContext->width,
		_pCodecContext->height, 1));

	av_image_fill_arrays(_pFrameYUV->data, _pFrameYUV->linesize, out_buffer,
		AV_PIX_FMT_YUV420P,
		_pCodecContext->width,
		_pCodecContext->height,
		1);

	_pPacket = av_packet_alloc();
	if (!_pPacket)
	{
		logging("failed to allocated memory for AVPacket");
		return -1;
	}

	// for image convert
	_img_convert_ctx = sws_getContext(_pCodecContext->width, _pCodecContext->height, _pCodecContext->pix_fmt,
		_pCodecContext->width, _pCodecContext->height, AV_PIX_FMT_YUV420P, SWS_BICUBIC, NULL, NULL, NULL);

	logging("simpleVideoPlayer::startPlayVideo() success ready for render video frame now");
	_readyToRenderVideoFrame = true;

	logging("video width: %d,height: %d", _pCodecContext->width, _pCodecContext->height);
	return 0;
}

int simpleVideoPlayer::getWidth()
{
	if (!_readyToRenderVideoFrame)
		return -1;
	return _pCodecContext->width;
}

int simpleVideoPlayer::getHeight()
{
	if (!_readyToRenderVideoFrame)
		return -1;
	return _pCodecContext->height;
}

unsigned long simpleVideoPlayer::getDuration()
{
	if (!_readyToRenderVideoFrame)
		return 0;
	//std::cout << _pFormatContext->duration << std::endl;
	//std::cout << (_pFormatContext->duration / AV_TIME_BASE) << std::endl;
	return static_cast<unsigned long>(_pFormatContext->duration / AV_TIME_BASE);
}

int simpleVideoPlayer::renderFrame()
{
	logging("simpleVideoPlayer::renderFrame");

	if (!_hasInitSuc || !_readyToRenderVideoFrame)
	{
		logging("not ready to render one frame");
		return -1;
	}
	int response = 0;
	while (av_read_frame(_pFormatContext, _pPacket) >= 0)
	{
		if (_pPacket->stream_index == _video_stream_index)
		{
			logging("AVPacket->pts %" PRId64, _pPacket->pts);
			response = decodePacket();
			if (response < 0)
			{
				logging("renderFrame() fail just set _readyToRenderVideoFrame to false");
				_readyToRenderVideoFrame = false;
				break;
			}
			else
			{
				_currentFrame++;
				break;
			}
		}
		else
		{
			logging("not video frame...");
		}
		av_packet_unref(_pPacket);
	}
	return 0;
}

int simpleVideoPlayer::decodePacket()
{
	logging("simpleVideoPlayer::decodePacket()");
	if (!_hasInitSuc || !_readyToRenderVideoFrame)
	{
		logging("not ready to decodePacket");
		return -1;
	}

	int response = avcodec_send_packet(_pCodecContext, _pPacket);
	if (response < 0)
	{
		print_error("Error while sending a packet to the decoder:", response);
		return response;
	}
	while (response >= 0)
	{
		// Return decoded output data (into a frame) from a decoder
		response = avcodec_receive_frame(_pCodecContext, _pOriginFrame);
		if (response == AVERROR(EAGAIN) || response == AVERROR_EOF) {
			break;
		}
		else if (response < 0)
		{
			print_error("Error while receiving a frame from the decoder: %s", response);
			return response;
		}

		if (response >= 0)
		{
			logging(
				"Frame %d (type=%c, size=%d bytes) pts %d key_frame %d [DTS %d]",
				_pCodecContext->frame_number,
				av_get_picture_type_char(_pOriginFrame->pict_type),
				_pOriginFrame->pkt_size,
				_pOriginFrame->pts,
				_pOriginFrame->key_frame,
				_pOriginFrame->coded_picture_number
			);

			// convert to YUV420 frame use sws_scale
			// pCodecContext->pix_fmt has be YUV420 ?
			// just simple check if YUV420 Frame
			AVFrame *targetFrame = _pOriginFrame;
			sws_scale(_img_convert_ctx,
				(const unsigned char* const *)_pOriginFrame->data,
				_pOriginFrame->linesize, 0, _pCodecContext->height,
				_pFrameYUV->data, _pFrameYUV->linesize);

			targetFrame = _pFrameYUV;
			int y_size = _pOriginFrame->width * _pOriginFrame->height;
			logging("decode one video frame success and save to buffer container size: %d", y_size);

			int buffersize = y_size + y_size / 2;
			unsigned char* newbuffer = new unsigned char[buffersize];
			memcpy(newbuffer, targetFrame->data[0], y_size);
			memcpy(newbuffer + y_size, targetFrame->data[1], y_size / 4);
			memcpy(newbuffer + y_size + y_size / 4, targetFrame->data[2], y_size / 4);

			// debug save to local file
			//char filename[1024];
			//snprintf(filename, sizeof(filename), "%s-full-frame-%d-y.origin", "frame", _pCodecContext->frame_number);
			//save_bytes_to_file(newbuffer, buffersize, filename);
			//logging("debug save one frame to local file path: %s", filename);
			// delete[] newbuffer;

			_backBuffer.push_back(newbuffer);
			std::cout << "buf size: " << _backBuffer.size() << std::endl;
		}
		else {
			std::cout << "respone code:  " << response << std::endl;
		}
	}
	return 0;
}

// TODO need to release the buffer on caller side
void* simpleVideoPlayer::getFrameBuffer()
{
	if (_backBuffer.size() > 0)
	{
		unsigned char* buf = _backBuffer.front();
		_backBuffer.pop_front();
		return buf;
	}
	return NULL;
}

void simpleVideoPlayer::freeFrameBuffer(unsigned char* buf)
{
	if (buf != NULL)
	{
		delete[] buf;
	}
}