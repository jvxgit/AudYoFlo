#include "CjvxAuCFfmpegDecoder.h"

CjvxAuCFfmpegAudioDecoder::CjvxAuCFfmpegAudioDecoder(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE):
	CjvxAudioDecoder(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
{

}

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
			JVX_DATAFORMAT_GROUP_FFMPEG_PACKET_FWD,
			JVX_DATAFLOW_DONT_CARE,
			JVX_SIZE_UNSELECTED, 1); // <- number_channels and segment are different arguments here

		neg_output._set_parameters_fixed(
			cParams.nChans,
			cParams.bSizeAudio,
			cParams.sRate,
			JVX_DATAFORMAT_DATA,
			JVX_DATAFORMAT_GROUP_AUDIO_PCM_DEINTERLEAVED);
	}
	return res;
}

jvxErrorType 
CjvxAuCFfmpegAudioDecoder::prepare_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;
	
	// Generate a fHeight entry in the data buffers
	jvx_bitSet(_common_set_ocon.theData_out.con_data.alloc_flags,
		(int)jvxDataLinkDescriptorAllocFlags::JVX_LINKDATA_ALLOCATION_FLAGS_EXPECT_FHEIGHT_INFO_SHIFT);

	CjvxAudioDecoder::prepare_connect_icon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	if (res == JVX_NO_ERROR)
	{
		AVDictionary* format_opts = nullptr, * codec_opts = nullptr, * filtered_opts = nullptr;
		AVStream* st = nullptr;

		// Extract the connection parameters for decoding
		jvxLinkDataAttachedChain* ptrStream = _common_set_icon.theData_in->con_link.attached_chain_single_pass;
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

		// Actually allocate the decoder
		if (st)
		{
			cParams.frame = av_frame_alloc();
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

			// Open the decoder
			ret = avcodec_open2(cParams.cc, cParams.codec, &filtered_opts);
			assert(ret == 0);
		}
		else
		{
			std::cout << __FUNCTION__ << ": Failed to reconstruct ffmpeg stream reference." << std::endl;
		}

		// Allocate space for the data buffers
		for (jvxSize i = 0; i < _common_set_icon.theData_in->con_data.number_buffers; i++)
		{
			_common_set_icon.theData_in->con_data.buffers[i] = reinterpret_cast<jvxHandle**>(av_packet_alloc());
		}
	}
	return res;
}

jvxErrorType
CjvxAuCFfmpegAudioDecoder::postprocess_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;

	// We need to remove the buffers first
	for (jvxSize i = 0; i < _common_set_icon.theData_in->con_data.number_buffers; i++)
	{
		AVPacket* oneBuf = reinterpret_cast<AVPacket*>(_common_set_icon.theData_in->con_data.buffers[i]);
		av_packet_free(&oneBuf);
	}

	// Close the decoder and whatever is attached to the codec context
	int ret = avcodec_close(cParams.cc);
	assert(ret == 0);

	// Free the decoder context
	avcodec_free_context(&cParams.cc);

	// Free the frame
	av_frame_free(&cParams.frame);

	CjvxAudioDecoder::postprocess_connect_icon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	if (res == JVX_NO_ERROR)
	{
		// Clear the buffer for fHeight information
		jvx_bitClear(_common_set_ocon.theData_out.con_data.alloc_flags,
			(int)jvxDataLinkDescriptorAllocFlags::JVX_LINKDATA_ALLOCATION_FLAGS_EXPECT_FHEIGHT_INFO_SHIFT);
	}
	return res;
}


jvxErrorType
CjvxAuCFfmpegAudioDecoder::process_buffers_icon(jvxSize mt_mask, jvxSize idx_stage)
{
	jvxSize i;
	jvxData** bufsOutData = nullptr;
	const char* cbuf = nullptr;
	jvxSize idx_stage_local = idx_stage;
	if (JVX_CHECK_SIZE_UNSELECTED(idx_stage_local))
	{
		idx_stage_local = *_common_set_icon.theData_in->con_pipeline.idx_stage_ptr;
	}

	// Decode the input packet here!!
	int num = 0;
	AVPacket* pkt = (AVPacket*)_common_set_icon.theData_in->con_data.buffers[idx_stage_local];

	// If we see a packet without reference, we must prevent the decoder to run. Otherwise it will stop due to end of file
	if (!pkt->size)
	{
		// Do not forward anything here!!
		_common_set_ocon.theData_out.con_data.fHeights[*_common_set_ocon.theData_out.con_pipeline.idx_stage_ptr].x = 0;
	}
	else
	{
		int ret = avcodec_send_packet(cParams.cc, pkt);
		av_packet_unref(pkt);

		ret = avcodec_receive_frame(cParams.cc, cParams.frame);

		int requiredSize = av_samples_get_buffer_size(nullptr, cParams.frame->ch_layout.nb_channels,
			cParams.frame->nb_samples, (AVSampleFormat)cParams.frame->format, 1);

		// This must be the right size
		assert(cParams.frame->nb_samples <= _common_set_ocon.theData_out.con_params.buffersize);

		bufsOutData = jvx_process_icon_extract_output_buffers<jvxData>(_common_set_ocon.theData_out);
		_common_set_ocon.theData_out.con_data.fHeights[*_common_set_ocon.theData_out.con_pipeline.idx_stage_ptr].x = cParams.frame->nb_samples;


		switch (cParams.frame->format)
		{
			// ===============================================================
			// Single buffer, interleaved
			// ===============================================================

		case AV_SAMPLE_FMT_S16:
			for (i = 0; i < cParams.nChans; i++)
			{
				jvx_convertSamples_from_fxp_norm_to_flp<jvxInt16, jvxData>(
					(jvxInt16*)cParams.frame->data[0],
					(jvxData*)bufsOutData[i],
					cParams.frame->nb_samples,
					JVX_MAX_INT_16_DIV,
					i, cParams.nChans,
					0, 1);
			}
			break;

		case AV_SAMPLE_FMT_S32:
			for (i = 0; i < cParams.nChans; i++)
			{
				jvx_convertSamples_from_fxp_norm_to_flp<jvxInt32, jvxData>(
					(jvxInt32*)cParams.frame->data[0],
					(jvxData*)bufsOutData[i],
					cParams.frame->nb_samples,
					JVX_MAX_INT_32_DIV,
					i, cParams.nChans,
					0, 1);
			}
			break;
		case AV_SAMPLE_FMT_S64:
			for (i = 0; i < cParams.nChans; i++)
			{
				jvx_convertSamples_from_fxp_norm_to_flp<jvxInt64, jvxData>(
					(jvxInt64*)cParams.frame->data[0],
					(jvxData*)bufsOutData[i],
					cParams.frame->nb_samples,
					JVX_MAX_INT_64_DIV,
					i, cParams.nChans,
					0, 1);
			}
			break;
		case AV_SAMPLE_FMT_FLT:
			for (i = 0; i < cParams.nChans; i++)
			{
				jvx_convertSamples_from_float_to_data<float>(
					(float*)cParams.frame->data[0],
					(jvxData*)bufsOutData[i],
					cParams.frame->nb_samples,
					i, cParams.nChans,
					0, 1);
			}
			break;

		case AV_SAMPLE_FMT_DBL:
			for (i = 0; i < cParams.nChans; i++)
			{
				jvx_convertSamples_from_float_to_data<double>(
					(double*)cParams.frame->data[0],
					(jvxData*)bufsOutData[i],
					cParams.frame->nb_samples,
					i, cParams.nChans,
					0, 1);
			}
			break;

			// ===============================================================
			// Planar, hence, non-interleaved
			// ===============================================================

		case AV_SAMPLE_FMT_FLTP:

			// Planar data format
			for (i = 0; i < cParams.nChans; i++)
			{
#ifdef JVX_DSP_DATA_FORMAT_DOUBLE
				jvx_convertSamples_from_float_to_data<float>(
					(float*)cParams.frame->data[i],
					(jvxData*)bufsOutData[i],
					cParams.frame->nb_samples,
					0, 1, 0, 1);
#else
				jvx_convertSamples_memcpy(cParams.frame->data[i], bufsOutData[i], sizeof(float), cParams.frame->nb_samples);
#endif
			}
			break;
		case AV_SAMPLE_FMT_DBLP:
			for (i = 0; i < cParams.nChans; i++)
			{
#ifdef JVX_DSP_DATA_FORMAT_DOUBLE
				jvx_convertSamples_memcpy(cParams.frame->data[i], bufsOutData[i], sizeof(double), cParams.frame->nb_samples);
#else
				jvx_convertSamples_from_float_to_data<double>(
					(double*)cParams.frame->data[0],
					(jvxData*)bufsOutData[i],
					cParams.frame->nb_samples,
					0, 1, 0, 1);
#endif
			}
			break;
		default:

			cbuf = av_get_sample_fmt_name((AVSampleFormat)cParams.frame->format);
			if (cbuf)
			{
				std::cout << __FUNCTION__ << "Warning: Unsupported data format <" << cbuf << ">, introducing silence!" << std::endl;
				// av_free((void*)cbuf); <- the get function returns a static string
			}
			for (i = 0; i < cParams.nChans; i++)
			{
				//memset(bufsOutData[i], 0, cParams.frame->nb_samples* jvxDataFormatGroup_getsize(_common_set_ocon.theData_out.con_params.format));
				memset(bufsOutData[i], 0, cParams.frame->nb_samples* jvxDataFormat_getsize(_common_set_ocon.theData_out.con_params.format));
			}
			break;

		}

		//_common_set_ocon.theData_out.con_params.fHeight_x
		av_frame_unref(cParams.frame);
	}
		
	return fwd_process_buffers_icon(mt_mask, idx_stage);
}

jvxErrorType
CjvxAuCFfmpegAudioDecoder::updated_backward_format_spec(jvxLinkDataDescriptor& forward, jvxLinkDataDescriptor* ld_cp)
{
	jvxFfmpegAudioParameter cParamsNew = cParams;

	// Accept the buffersize
	cParamsNew.bSizeAudio = ld_cp->con_params.buffersize;
	jvx_ffmpeg_wav_params(cParamsNew);
	forward.con_params.format_spec = jvx_ffmpeg_parameter_2_codec_token(cParamsNew);
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxAuCFfmpegAudioDecoder::configure_decoder(const char* tokenArg)
{
	jvxErrorType res = jvx_ffmpeg_codec_token_2_parameter(tokenArg, cParams);	
	return res;
}

void
CjvxAuCFfmpegAudioDecoder::accept_input_parameters()
{

}
void
CjvxAuCFfmpegAudioDecoder::test_set_output_parameters()
{
	// The output side is "audio"
	_common_set_ocon.theData_out.con_params.rate = cParams.sRate;
	_common_set_ocon.theData_out.con_params.number_channels = cParams.nChans;
	_common_set_ocon.theData_out.con_params.buffersize = cParams.bSizeAudio;

	_common_set_ocon.theData_out.con_params.format = JVX_DATAFORMAT_DATA;
	_common_set_ocon.theData_out.con_params.format_group = JVX_DATAFORMAT_GROUP_AUDIO_PCM_DEINTERLEAVED;
	_common_set_ocon.theData_out.con_params.data_flow = _common_set_icon.theData_in->con_params.data_flow;
	_common_set_ocon.theData_out.con_params.segmentation.x = _common_set_ocon.theData_out.con_params.buffersize;
	_common_set_ocon.theData_out.con_params.segmentation.y = 1;
}