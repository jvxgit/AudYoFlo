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
	auto val = jvx_ffmpeg_parameter_2_codec_token(cParams, CjvxAudioCodec_genpcg::general.buffersize.value);
	astr->assign(val);
	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxAuCFfmpegAudioEncoder::update_configure_token(const char* token)
{
	jvxSize bSize = 0;
	jvxFfmpegFileAudioParameter nParams;
	jvx_ffmpeg_codec_token_2_parameter(token, nParams, bSize);
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

		cParams.sRate = CjvxAudioCodec_genpcg::general.samplerate.value;
		cParams.nChans = CjvxAudioCodec_genpcg::general.num_audio_channels.value = neg_input._latest_results.number_channels;
		cParams.frameSizeMax = CjvxAudioCodec_genpcg::general.buffersize.value * cParams.nChans * cParams.bitsPerCoded / 8;

		res = _test_connect_icon(true, JVX_CONNECTION_FEEDBACK_CALL(fdb));
		if (res == JVX_NO_ERROR)
		{
		}
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
	_common_set_ldslave.theData_out.con_params.format_group = JVX_DATAFORMAT_GROUP_FFMPEG_BUFFER_FWD;
	_common_set_ldslave.theData_out.con_params.segmentation.x = _common_set_ldslave.theData_out.con_params.buffersize;
	_common_set_ldslave.theData_out.con_params.segmentation.y = 1;
	_common_set_ldslave.theData_out.con_params.data_flow = jvxDataflow::JVX_DATAFLOW_PUSH_ON_PULL;

	accept_output_parameters();
}

void
CjvxAuCFfmpegAudioEncoder::accept_output_parameters()
{
	_common_set_ldslave.theData_out.con_params.format_spec = jvx_ffmpeg_parameter_2_codec_token(cParams, CjvxAudioCodec_genpcg::general.buffersize.value);
}