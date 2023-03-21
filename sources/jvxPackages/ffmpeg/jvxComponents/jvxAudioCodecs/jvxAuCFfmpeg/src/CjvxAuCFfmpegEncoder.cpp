#include "CjvxAuCFfmpegEncoder.h"

CjvxAuCFfmpegAudioEncoder::CjvxAuCFfmpegAudioEncoder(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE) :
	CjvxAudioEncoder(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
{
	// Setup some default settings
	cParams.sRate = 48000;
	cParams.nChans = 2;
	cParams.chanLayout.nb_channels = 2;
	cParams.chanLayout.order = AV_CHANNEL_ORDER_UNSPEC;
	cParams.chanLayout.u.mask = 0;
	cParams.tpCodec = AVMEDIA_TYPE_AUDIO;
	cParams.fFormatTag = "wav";

	// We only are allowed to input 16 bit audio
	cParams.bitsPerCoded = 16;
	cParams.idCodec = AV_CODEC_ID_PCM_S16LE;
	cParams.sFormatId = AV_SAMPLE_FMT_S16;
	cParams.isFloat = false;
	cParams.frameSize = 0;
	cParams.frameSizeMax = 0;
	cParams.bitRate = cParams.sRate * cParams.nChans * cParams.bitsPerCoded;
	cParams.bitsPerRaw = 0;
}

jvxErrorType
CjvxAuCFfmpegAudioEncoder::activate_encoder()
{
	CjvxAudioCodec_genpcg::general.num_audio_channels.value = cParams.nChans;
	CjvxAudioCodec_genpcg::general.samplerate.value = cParams.sRate;
	if (cParams.frameSize != 0)
	{
		CjvxAudioCodec_genpcg::general.buffersize.value = cParams.frameSize;
	}

	CjvxAudioCodec_genpcg::general.max_number_bytes.value = 123;
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxAuCFfmpegAudioEncoder::activate()
{
	jvxErrorType res = CjvxAudioEncoder::activate();
	if (res == JVX_NO_ERROR)
	{
		if (cParams.frameSize == 0)
		{
			// This is PCM
			neg_input._clear_parameters_fixed(false, true);
		}
	}
	return res;
}

// ====================================================================================

jvxErrorType 
CjvxAuCFfmpegAudioEncoder::get_configure_token(jvxApiString* astr)
{
	auto val = jvx_ffmpeg_parameter_2_codec_token(cParams);
	astr->assign(val);
	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxAuCFfmpegAudioEncoder::update_configure_token(const char* token)
{
	jvxFfmpegFileAudioParameter nParams;
	jvx_ffmpeg_codec_token_2_parameter(token, nParams);
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxAuCFfmpegAudioEncoder::test_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;

	res = neg_input._negotiate_connect_icon(_common_set_ldslave.theData_in,
		static_cast<IjvxObject*>(this),
		_common_set_ldslave.descriptor.c_str()
		JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
	if (res == JVX_NO_ERROR)
	{
		CjvxAudioCodec_genpcg::general.buffersize.value = neg_input._latest_results.buffersize;
		CjvxAudioCodec_genpcg::general.samplerate.value = neg_input._latest_results.rate;
		CjvxAudioCodec_genpcg::general.num_audio_channels.value = neg_input._latest_results.number_channels;

		// Accept these parameters from forward negotiations
		cParams.sRate = CjvxAudioCodec_genpcg::general.samplerate.value;
		cParams.nChans = CjvxAudioCodec_genpcg::general.num_audio_channels.value = neg_input._latest_results.number_channels;
		cParams.bSizeAudio = CjvxAudioCodec_genpcg::general.buffersize.value;

		res = _test_connect_icon(true, JVX_CONNECTION_FEEDBACK_CALL(fdb));
		if (res == JVX_NO_ERROR)
		{
		}
	}
	return res;
}

jvxErrorType
CjvxAuCFfmpegAudioEncoder::transfer_backward_ocon(jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxErrorType res = JVX_ERROR_UNSUPPORTED;
	jvxLinkDataDescriptor* ld_cp = nullptr;
	jvxLinkDataDescriptor forward;
	jvxApiString famToken;
	switch (tp)
	{
	case JVX_LINKDATA_TRANSFER_COMPLAIN_DATA_SETTINGS:
		ld_cp = (jvxLinkDataDescriptor*)data;
		if (ld_cp)
		{
			jvxBool forwardRequest = false;

			jvxFfmpegAudioParameter cParams_check;
			std::string config_token_complain = ld_cp->con_params.format_spec.std_str();
			jvxErrorType resL = jvx_ffmpeg_codec_token_2_parameter(config_token_complain.c_str(), cParams_check);
			if (resL != JVX_NO_ERROR)
			{
				forwardRequest = true;
			}
			else
			{
				if (
					(cParams_check.bSizeAudio == cParams.bSizeAudio) &&
					(cParams.sRate == cParams.sRate)
					)
				{
					// Accept the new parameters and report positive to caller
					*((jvxFfmpegAudioParameter*)&cParams) = cParams_check; // Downcast the target variable
					accept_output_parameters();
					res = JVX_NO_ERROR;
				}
				else
				{
					// Derive the parameters from input side for  the next forward step
					forward.con_params = _common_set_ldslave.theData_in->con_params;

					// This is the next level of modification: requires new buffersize and/or samplerate from previous module
					forward.con_params.buffersize = cParams_check.bSizeAudio;
					forward.con_params.rate = cParams_check.sRate;
					forward.con_params.number_channels = cParams_check.nChans;
					forwardRequest = true;
				}
			}

			if (forwardRequest)
			{
				res = _common_set_ldslave.theData_in->con_link.connect_from->transfer_backward_ocon(
					tp, &forward JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
				if (res == JVX_NO_ERROR)
				{
					// Here, we need to refresh all connection parameters.
					// We do take a shortcut here and do not run the negotiation

					CjvxAudioCodec_genpcg::general.buffersize.value = _common_set_ldslave.theData_in->con_params.buffersize;
					CjvxAudioCodec_genpcg::general.samplerate.value = _common_set_ldslave.theData_in->con_params.rate;
					CjvxAudioCodec_genpcg::general.num_audio_channels.value = _common_set_ldslave.theData_in->con_params.number_channels;

					derive_input_file_arguments();
					test_set_output_parameters();
				}
			}

		}
		break;
	default:
		res = _transfer_backward_ocon(true, tp, data JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
	}
	return res;
}

void
CjvxAuCFfmpegAudioEncoder::test_set_output_parameters()
{
	_common_set_ldslave.theData_out.con_params.number_channels = 1; // Byte field is always 1 channel
	_common_set_ldslave.theData_out.con_params.rate = JVX_SIZE_DONTCARE; // This indicates that the rate is in the format_spec
	_common_set_ldslave.theData_out.con_params.buffersize = JVX_SIZE_DONTCARE;

	// Type is modified as the data is coded
	_common_set_ldslave.theData_out.con_params.format = JVX_DATAFORMAT_POINTER;
	_common_set_ldslave.theData_out.con_params.format_group = JVX_DATAFORMAT_GROUP_FFMPEG_PACKET_FWD;
	_common_set_ldslave.theData_out.con_params.segmentation.x = _common_set_ldslave.theData_out.con_params.buffersize;
	_common_set_ldslave.theData_out.con_params.segmentation.y = 1;
	_common_set_ldslave.theData_out.con_params.data_flow = jvxDataflow::JVX_DATAFLOW_PUSH_ON_PULL;

	accept_output_parameters();
}

void
CjvxAuCFfmpegAudioEncoder::accept_output_parameters()
{
	// Derive the pcm related part
	jvx_ffmpeg_wav_params(cParams, CjvxAudioCodec_genpcg::general.buffersize.value);

	// Verify and correct remaining parameters
	jvx_ffmpeg_verify_correct_codec_settings(cParams);

	// Derive the token
	_common_set_ldslave.theData_out.con_params.format_spec = jvx_ffmpeg_parameter_2_codec_token(cParams);
}


jvxErrorType 
CjvxAuCFfmpegAudioEncoder::prepare_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	// Lookup the codec and start it
	cParams.enc = avcodec_find_encoder(cParams.idCodec);
	assert(cParams.enc);
	// avcodec_get_name(cParams.idCodec);

	// Allocate the encoder context
	cParams.enc_ctx = avcodec_alloc_context3(cParams.enc);

	cParams.enc_ctx->sample_fmt = cParams.sFormatId;
	cParams.enc_ctx->bit_rate = cParams.bitRate;
	cParams.enc_ctx->sample_rate = cParams.sRate;
	av_channel_layout_copy(&cParams.enc_ctx->ch_layout, &cParams.chanLayout);

	int ret = avcodec_open2(cParams.enc_ctx, cParams.enc, nullptr);
	assert(ret == 0);

	if (cParams.enc_ctx->codec->capabilities & AV_CODEC_CAP_VARIABLE_FRAME_SIZE)
	{
	}
	else
	{
	}

	// Forward the codec pointer to the following output instance
	jvxLinkDataAttachedStringDetect<jvxLinkDataAttachedChain> attachObject("/ffmpeg/audiostream/codecctx", cParams.enc_ctx);
	_common_set_ldslave.theData_out.con_link.attached_chain_single_pass = jvx_attached_push_front(_common_set_ldslave.theData_out.con_link.attached_chain_single_pass, &attachObject);

	// Prepare processing parameters
	_common_set_ldslave.theData_out.con_data.number_buffers = 1;
	jvx_bitZSet(_common_set_ldslave.theData_out.con_data.alloc_flags, (int)jvxDataLinkDescriptorAllocFlags::JVX_LINKDATA_ALLOCATION_FLAGS_IS_ZEROCOPY_CHAIN_SHIFT);

	jvxErrorType res = CjvxAudioEncoder::prepare_connect_icon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	if (res == JVX_NO_ERROR)
	{

	}

	jvxLinkDataAttachedChain* ptrHere = nullptr;
	_common_set_ldslave.theData_out.con_link.attached_chain_single_pass = jvx_attached_pop_front(_common_set_ldslave.theData_out.con_link.attached_chain_single_pass, &ptrHere);
	assert(ptrHere == &attachObject);

	cParams.frame = av_frame_alloc();

	cParams.frame->format = cParams.sFormatId;

	av_channel_layout_copy(&cParams.frame->ch_layout, &cParams.chanLayout);
	cParams.frame->sample_rate = cParams.sRate;
	cParams.frame->nb_samples = cParams.bSizeAudio;

	/* allocate the buffers for the frame data */
	ret = av_frame_get_buffer(cParams.frame, 0);
	assert(ret == 0);

	return res;
}

jvxErrorType
CjvxAuCFfmpegAudioEncoder::postprocess_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = CjvxAudioEncoder::postprocess_connect_icon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	if (res == JVX_NO_ERROR)
	{
		av_frame_free(&cParams.frame);

		jvx_bitFClear(_common_set_ldslave.theData_out.con_data.alloc_flags);
		avcodec_free_context(&cParams.enc_ctx);
		cParams.enc = nullptr;
	}
	return res;
}

jvxErrorType
CjvxAuCFfmpegAudioEncoder::process_buffers_icon(jvxSize mt_mask, jvxSize idx_stage)
{
	jvxSize i;
	int ret = 0;
	AVPacket* pkt = nullptr;
	const char* cbuf = nullptr;
	jvxData** bufsInData = jvx_process_icon_extract_input_buffers<jvxData>(_common_set_ldslave.theData_in, idx_stage);
	jvxSize idx_stage_out = *_common_set_ldslave.theData_out.con_pipeline.idx_stage_ptr;

	switch (cParams.sFormatId)
	{
	case AV_SAMPLE_FMT_S16:
		for (i = 0; i < cParams.nChans; i++)
		{
			jvx_convertSamples_from_flp_norm_to_fxp<jvxData, jvxInt16>(
				(jvxData*)bufsInData[i],
				(jvxInt16*)cParams.frame->data[0],
				cParams.frame->nb_samples,
				JVX_MAX_INT_16_DATA,
				0, 1,
				i, cParams.nChans);
		}
		break;


	case AV_SAMPLE_FMT_S32:
		for (i = 0; i < cParams.nChans; i++)
		{
			jvx_convertSamples_from_flp_norm_to_fxp<jvxData, jvxInt32>(
				(jvxData*)bufsInData[i],
				(jvxInt32*)cParams.frame->data[0],
				cParams.frame->nb_samples,
				JVX_MAX_INT_32_DATA,
				0, 1,
				i, cParams.nChans);

		}
		break;

	case AV_SAMPLE_FMT_S64:
		for (i = 0; i < cParams.nChans; i++)
		{
			jvx_convertSamples_from_flp_norm_to_fxp<jvxData, jvxInt64>(
				(jvxData*)bufsInData[i],
				(jvxInt64*)cParams.frame->data[0],
				cParams.frame->nb_samples,
				JVX_MAX_INT_64_DATA,
				0, 1,
				i, cParams.nChans);
		}
		break;

	case AV_SAMPLE_FMT_FLT:
		for (i = 0; i < cParams.nChans; i++)
		{
			jvx_convertSamples_from_data_to_float<float>(
				(jvxData*)bufsInData[i],
				(float*)cParams.frame->data[0],
				cParams.frame->nb_samples,
				0, 1,
				i, cParams.nChans);
		}
		break;

	case AV_SAMPLE_FMT_DBL:
		for (i = 0; i < cParams.nChans; i++)
		{
			jvx_convertSamples_from_data_to_float<double>(
				(jvxData*)bufsInData[i],
				(double*)cParams.frame->data[0],
				cParams.frame->nb_samples,
				0, 1,
				i, cParams.nChans);
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
			jvx_convertSamples_from_data_to_float<float>(
				(jvxData*)bufsInData[i],
				(float*)cParams.frame->data[i],
				cParams.frame->nb_samples,
				0, 1, 0, 1);
#else
			jvx_convertSamples_memcpy(bufsInData[i], cParams.frame->data[i], sizeof(float), cParams.frame->nb_samples);
#endif
		}
		break;
	case AV_SAMPLE_FMT_DBLP:
		for (i = 0; i < cParams.nChans; i++)
		{
#ifdef JVX_DSP_DATA_FORMAT_DOUBLE
			jvx_convertSamples_memcpy(bufsInData[i], cParams.frame->data[i], sizeof(double), cParams.frame->nb_samples);
#else
			jvx_convertSamples_from_data_to_float<double>(
				(jvxData*)bufsOutData[i],
				(double*)cParams.frame->data[0],
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
		break;
	}

	// Update the pts with a sample counter actually
	cParams.frame->pts = cParams.pts_current;
	cParams.pts_current += cParams.frame->nb_samples;

	// Next, convert frame to packet
	pkt = (AVPacket*)_common_set_ldslave.theData_out.con_data.buffers[idx_stage_out];

	// Encode the input frame into a packet here!!

	// send the frame to the encoder
	ret = avcodec_send_frame(cParams.enc_ctx, cParams.frame);
	assert(ret == 0);

	// obtain the package and forward it to the next stage
	ret = avcodec_receive_packet(cParams.enc_ctx, pkt);
	assert(ret == 0);

	// Forward the buffer
	return _process_buffers_icon(mt_mask, JVX_SIZE_UNSELECTED);
}
