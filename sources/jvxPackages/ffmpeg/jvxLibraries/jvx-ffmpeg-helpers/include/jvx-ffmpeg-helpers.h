#ifndef __FFMPEG_HELPERS_H__
#define __FFMPEG_HELPERS_H__

#include "jvx.h"

extern "C"
{
#include "libavformat/avformat.h"
#include "libavutil/opt.h"
#include "libavcodec/avcodec.h"
}

int check_stream_specifier(AVFormatContext* s, AVStream* st, const char* spec);

AVDictionary* filter_codec_opts(AVDictionary* opts, enum AVCodecID codec_id,
	AVFormatContext* s, AVStream* st, const AVCodec* codec);

AVDictionary** setup_find_stream_info_opts(AVFormatContext* s,
	AVDictionary* codec_opts);

struct jvxFfmpegParameter
{
	jvxSize nChans = 0;
	jvxSize sRate = 0;
	jvxSize lengthSamples = 0;
	jvxData lengthSecs = 0;
	jvxSize numFrames = 0;
	jvxSize frameSize = 0;
	jvxSize frameSizeMax = 0;
	jvxSize bitRate = 0;
	jvxSize bitsPerRaw = 0;
	jvxSize bitsPerCoded = 0;
	enum AVCodecID idCodec = AV_CODEC_ID_NONE;

	std::string sFormat;
	std::string chanLayoutTag;
	std::string fFormatTag;
	std::string codecIdTag;
	std::string codecTypeTag;

	void reset()
	{
		nChans = 0;
		sRate = 0;
		lengthSamples = 0;
		lengthSecs = 0;
		numFrames = 0;
		frameSize = 0;
		bitRate = 0;
		bitsPerRaw = 0;
		bitsPerCoded = 0;

		sFormat.clear();
		chanLayoutTag.clear();
		fFormatTag.clear();
		codecIdTag.clear();
		codecTypeTag.clear();
	};
};

std::string jvx_ffmpeg_parameter_2_codec_token(const jvxFfmpegParameter& params, jvxSize bSize);
jvxErrorType jvx_ffmpeg_codec_token_2_parameter(const char* tokenArg, jvxFfmpegParameter& params);

#endif
