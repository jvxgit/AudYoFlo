#include "CjvxAuCFfmpegDecoder.h"

CjvxAuCFfmpegAudioDecoder::CjvxAuCFfmpegAudioDecoder(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE):
	CjvxAudioDecoder(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
{

}

/*
	avcodec_parameters_alloc()
	avcodec_parameters_free()
*/

jvxErrorType
CjvxAuCFfmpegAudioDecoder::set_configure_token(const char* tokenArg)
{
	jvxErrorType res = JVX_NO_ERROR;

	res = configure_decoder(tokenArg);
	if (res == JVX_NO_ERROR)
	{
		neg_input._set_parameters_fixed(
			1, JVX_SIZE_DONTCARE,
			JVX_SIZE_DONTCARE,
			JVX_DATAFORMAT_POINTER,
			JVX_DATAFORMAT_GROUP_FFMPEG_BUFFER_FWD,
			JVX_DATAFLOW_NONE,
			JVX_SIZE_UNSELECTED, 1); // <- number_channels and segment are different arguments here

		neg_output._set_parameters_fixed(
			CjvxAudioCodec_genpcg::general.num_audio_channels.value,
			CjvxAudioCodec_genpcg::general.buffersize.value,
			CjvxAudioCodec_genpcg::general.samplerate.value,
			(jvxDataFormat)CjvxAudioCodec_genpcg::general.audio_format.value,
			JVX_DATAFORMAT_GROUP_AUDIO_PCM_DEINTERLEAVED);
	}
	return res;
}

jvxErrorType 
CjvxAuCFfmpegAudioDecoder::prepare_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = CjvxAudioDecoder::prepare_connect_icon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	if (res == JVX_NO_ERROR)
	{
		AVDictionary* format_opts = nullptr, * codec_opts = nullptr, * filtered_opts = nullptr;
		AVStream* st = nullptr;

		jvxLinkDataAttachedChain* ptrStream = _common_set_ldslave.theData_in->con_link.attached_chain_single_pass;
		while (ptrStream)
		{
			jvxLinkDataAttachedStringDetect<jvxLinkDataAttachedChain>* ptr_stream_txt = reinterpret_cast<jvxLinkDataAttachedStringDetect<jvxLinkDataAttachedChain>*>(ptrStream->if_specific(JVX_LINKDATA_ATTACHED_STRING_DETECT));
			if (ptr_stream_txt->descr)
			{
				if ((std::string)ptr_stream_txt->descr == "/ffmpeg/audiostream/avstream")
				{
					st = (AVStream*)ptr_stream_txt->data;
					break;
				}
			}
			ptrStream = ptrStream->next;
		}

		if (st)
		{
			cParams.cc = avcodec_alloc_context3(NULL);
			int ret = avcodec_parameters_to_context(cParams.cc, st->codecpar);
			assert(ret == 0);
			cParams.cc->pkt_timebase = st->time_base;

			cParams.codec = avcodec_find_decoder(st->codecpar->codec_id);
			// Alternative call here if we knwo the name
			// avcodec_find_decoder_by_name(forced_codec_name);
			assert(cParams.codec);

			// Make sure..
			cParams.cc->codec_id = cParams.codec->id;

			/*
			filtered_opts = filter_codec_opts(codec_opts, st->codecpar->codec_id, cParams.ic, st, cParams.codec);
			if (!av_dict_get(filtered_opts, "threads", NULL, 0))
			av_dict_set(&filtered_opts, "threads", "auto", 0);
			*/

			ret = avcodec_open2(cParams.cc, cParams.codec, &filtered_opts);
			assert(ret == 0);
		}
		else
		{
			std::cout << __FUNCTION__ << ": Failed to reconstruct ffmpeg stream reference." << std::endl;
		}
	}
	return res;
}

jvxErrorType
CjvxAuCFfmpegAudioDecoder::process_buffers_icon(jvxSize mt_mask, jvxSize idx_stage)
{
	jvxSize i;
	jvxData** bufsOutData = nullptr;

	jvxSize idx_stage_local = idx_stage;
	if (JVX_CHECK_SIZE_UNSELECTED(idx_stage_local))
	{
		idx_stage_local = *_common_set_ldslave.theData_in->con_pipeline.idx_stage_ptr;
	}

	AVPacket* pkt = (AVPacket*)_common_set_ldslave.theData_in->con_data.buffers[idx_stage_local];

	// Decode the input packet here!!

	bufsOutData = jvx_process_icon_extract_output_buffers<jvxData>(&_common_set_ldslave.theData_out);
	for (i = 0; i < cParams.nChans; i++)
	{
		memset(bufsOutData[i], 0, _common_set_ldslave.theData_out.con_params.buffersize *jvxDataFormatGroup_getsize(_common_set_ldslave.theData_out.con_params.format));
	}
	return fwd_process_buffers_icon(mt_mask, idx_stage);
}

jvxErrorType
CjvxAuCFfmpegAudioDecoder::configure_decoder(const char* tokenArg)
{
	jvxErrorType res = jvx_ffmpeg_codec_token_2_parameter(tokenArg, cParams);
	if (res == JVX_NO_ERROR)
	{
		CjvxAudioCodec_genpcg::general.max_number_bytes.value = JVX_SIZE_INT32(cParams.frameSizeMax);
		CjvxAudioCodec_genpcg::general.samplerate.value = JVX_SIZE_INT32(cParams.sRate);
		CjvxAudioCodec_genpcg::general.num_audio_channels.value = cParams.nChans;
		CjvxAudioCodec_genpcg::general.buffersize.value = cParams.frameSize;
		CjvxAudioCodec_genpcg::general.audio_format.value = JVX_DATAFORMAT_DATA;
	}
	return res;
}
