#ifndef __FFMPEG_HELPERS_H__
#define __FFMPEG_HELPERS_H__

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

#endif
