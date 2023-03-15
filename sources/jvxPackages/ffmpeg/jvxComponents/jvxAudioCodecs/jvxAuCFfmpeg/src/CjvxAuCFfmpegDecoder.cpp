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
	jvxErrorType res = JVX_NO_ERROR;
	
	// Generate a fHeight entry in the data buffers
	jvx_bitSet(_common_set_ldslave.theData_out.con_data.alloc_flags,
		(int)jvxDataLinkDescriptorAllocFlags::JVX_LINKDATA_ALLOCATION_FLAGS_EXPECT_FHEIGHT_INFO_SHIFT);

	CjvxAudioDecoder::prepare_connect_icon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	if (res == JVX_NO_ERROR)
	{
		AVDictionary* format_opts = nullptr, * codec_opts = nullptr, * filtered_opts = nullptr;
		AVStream* st = nullptr;

		// Extract the connection parameters for decoding
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
		for (jvxSize i = 0; i < _common_set_ldslave.theData_in->con_data.number_buffers; i++)
		{
			_common_set_ldslave.theData_in->con_data.buffers[i] = reinterpret_cast<jvxHandle**>(av_packet_alloc());
		}
	}
	return res;
}

jvxErrorType
CjvxAuCFfmpegAudioDecoder::postprocess_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;

	// We need to remove the buffers first
	for (jvxSize i = 0; i < _common_set_ldslave.theData_in->con_data.number_buffers; i++)
	{
		AVPacket* oneBuf = reinterpret_cast<AVPacket*>(_common_set_ldslave.theData_in->con_data.buffers[i]);
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
		jvx_bitClear(_common_set_ldslave.theData_out.con_data.alloc_flags,
			(int)jvxDataLinkDescriptorAllocFlags::JVX_LINKDATA_ALLOCATION_FLAGS_EXPECT_FHEIGHT_INFO_SHIFT);
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

	// Decode the input packet here!!
	int num = 0;
	AVPacket* pkt = (AVPacket*)_common_set_ldslave.theData_in->con_data.buffers[idx_stage_local];

	// If we see a packet without reference, we must prevent the decoder to run. Otherwise it will stop due to end of file
	if (!pkt->size)
	{
		// Do not forward anything here!!
		_common_set_ldslave.theData_out.con_data.fHeights[*_common_set_ldslave.theData_out.con_pipeline.idx_stage_ptr].x = 0;
	}
	else
	{
		int ret = avcodec_send_packet(cParams.cc, pkt);
		av_packet_unref(pkt);

		ret = avcodec_receive_frame(cParams.cc, cParams.frame);

		int requiredSize = av_samples_get_buffer_size(nullptr, cParams.frame->ch_layout.nb_channels,
			cParams.frame->nb_samples, (AVSampleFormat)cParams.frame->format, 1);

		bufsOutData = jvx_process_icon_extract_output_buffers<jvxData>(&_common_set_ldslave.theData_out);
		_common_set_ldslave.theData_out.con_data.fHeights[*_common_set_ldslave.theData_out.con_pipeline.idx_stage_ptr].x = cParams.frame->duration;

		assert(cParams.frame->duration <= _common_set_ldslave.theData_out.con_params.buffersize);

		switch (cParams.frame->format)
		{
		case AV_SAMPLE_FMT_S16:
			for (i = 0; i < cParams.nChans; i++)
			{
				jvx_convertSamples_from_fxp_norm_to_flp<jvxInt16, jvxData>(
					(jvxInt16*)cParams.frame->data[0],
					(jvxData*)bufsOutData[i],
					cParams.frame->duration,
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
					cParams.frame->duration,
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
					cParams.frame->duration,
					JVX_MAX_INT_64_DIV,
					i, cParams.nChans,
					0, 1);
			}
			break;
		default:

			// AV_SAMPLE_FMT_S32,         ///< signed 32 bits
			// AV_SAMPLE_FMT_FLT,         ///< float
			// AV_SAMPLE_FMT_DBL
			for (i = 0; i < cParams.nChans; i++)
			{
				memset(bufsOutData[i], 0, cParams.frame->duration * jvxDataFormatGroup_getsize(_common_set_ldslave.theData_out.con_params.format));
			}
			break;

		}

		//_common_set_ldslave.theData_out.con_params.fHeight_x
		av_frame_unref(cParams.frame);
	}
		
	return fwd_process_buffers_icon(mt_mask, idx_stage);
}

jvxErrorType
CjvxAuCFfmpegAudioDecoder::transfer_backward_ocon(jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxErrorType res = JVX_ERROR_UNSUPPORTED;
	jvxLinkDataDescriptor* ld_cp = nullptr;
	jvxLinkDataDescriptor forward;
	switch (tp)
	{
	case JVX_LINKDATA_TRANSFER_COMPLAIN_DATA_SETTINGS:
		ld_cp = (jvxLinkDataDescriptor*)data;
		if (ld_cp)
		{
			if (
				(ld_cp->con_params.rate == _common_set_ldslave.theData_out.con_params.rate) &&
				(ld_cp->con_params.number_channels == _common_set_ldslave.theData_out.con_params.number_channels) &&
				(ld_cp->con_params.format == _common_set_ldslave.theData_out.con_params.format) &&
				(ld_cp->con_params.format_group == _common_set_ldslave.theData_out.con_params.format_group))
			{
				if (
					(ld_cp->con_params.buffersize != _common_set_ldslave.theData_out.con_params.buffersize))
				{
					forward = *_common_set_ldslave.theData_in;

					cParams.frameSizeMax = ld_cp->con_params.buffersize * cParams.nChans * cParams.bitsPerCoded; // cParams.sizePerSample;
					cParams.frameSizeMax /= 8;
					forward.con_params.format_spec = jvx_ffmpeg_parameter_2_codec_token(cParams, ld_cp->con_params.buffersize);
					res = _transfer_backward_ocon(true, tp, &forward JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
					if (res == JVX_NO_ERROR)
					{
						set_configure_token(forward.con_params.format_spec.c_str());

						// 
						/*
						_common_set_ldslave.theData_out.con_params.buffersize = params.bsize;
						_common_set_ldslave.theData_out.con_params.segmentation.x = params.bsize;
						CjvxAudioCodec_genpcg::general.buffersize.value = params.bsize;
						*/
					}					
				}
				else
				{
					res = JVX_NO_ERROR;
				}
			}
			else
			{
				res = JVX_ERROR_UNSUPPORTED;
			}
		}
		break;
	default:
		res = _transfer_backward_ocon(true, tp, data JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
	}
	return res;

}
jvxErrorType
CjvxAuCFfmpegAudioDecoder::configure_decoder(const char* tokenArg)
{
	jvxSize bsize = 0;
	jvxErrorType res = jvx_ffmpeg_codec_token_2_parameter(tokenArg, cParams, bsize);
	if (res == JVX_NO_ERROR)
	{
		CjvxAudioCodec_genpcg::general.max_number_bytes.value = JVX_SIZE_INT32(cParams.frameSizeMax);
		CjvxAudioCodec_genpcg::general.samplerate.value = JVX_SIZE_INT32(cParams.sRate);
		CjvxAudioCodec_genpcg::general.num_audio_channels.value = cParams.nChans;
		if (cParams.frameSize)
		{
			CjvxAudioCodec_genpcg::general.buffersize.value = cParams.frameSize;
		}
		else
		{
			CjvxAudioCodec_genpcg::general.buffersize.value = bsize;
		}
		CjvxAudioCodec_genpcg::general.audio_format.value = JVX_DATAFORMAT_DATA;
	}
	return res;
}
