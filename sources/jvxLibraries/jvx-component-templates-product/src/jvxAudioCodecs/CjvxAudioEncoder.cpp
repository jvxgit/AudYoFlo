#include "jvx.h"
#include "jvxAudioCodecs/CjvxAudioEncoder.h"
#include "jvx-helpers-cpp.h"

#ifdef JVX_PROJECT_NAMESPACE
namespace JVX_PROJECT_NAMESPACE {
#endif

CjvxAudioEncoder::CjvxAudioEncoder(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE):
	CjvxObject(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL), CjvxProperties(module_name, *this)
{
	_common_set.theComponentType.unselected(JVX_COMPONENT_AUDIO_ENCODER);
	_common_set.theObjectSpecialization = reinterpret_cast<jvxHandle*>(static_cast<IjvxAudioEncoder*>(this));
	_common_set.thisisme = static_cast<IjvxObject*>(this);
	_common_set.theInterfaceFactory = static_cast<IjvxInterfaceFactory*>(this);
	myParent = NULL;

	// params default is good
	params.bsize = 1024;
	params.nchans = 2;
	params.endian = audio_codec_wav_endian::AUDIO_CODEC_WAV_LITTLE_ENDIAN;
	params.resolution = audio_codec_wav_resolution::AUDIO_CODEC_WAV_RESOLUTION_16BIT;
	params.sample_type = audio_codec_wav_sample_type::AUDIO_CODEC_WAV_FIXED_POINT;
	params.sub_type = audio_codec_wav_sub_type::AUDIO_CODEC_WAV_SUBTYPE_WAV_32;
	params.fsizemax = jvx_wav_compute_bsize_bytes_pcm(& params, params.bsize);
	params.srate = 48000;
}

CjvxAudioEncoder::~CjvxAudioEncoder()
{
}

// ==============================================================================
jvxErrorType 
CjvxAudioEncoder::activate()
{
	jvxErrorType res = CjvxObject::_activate();
	if (res == JVX_NO_ERROR)
	{
		JVX_ACTIVATE_DEFAULT_ONE_IN_ONE_OUT_CONNECTORS(NULL, static_cast<IjvxObject*>(this), "default", NULL, _common_set.theModuleName);

		CjvxAudioCodec_genpcg::init_all();
		CjvxAudioCodec_genpcg::allocate_all();
		CjvxAudioCodec_genpcg::register_all(static_cast<CjvxProperties*>(this));
		//CjvxAudioCodec_genpcg::general_init_all();

		// Activate by calling specific function
		this->activate_encoder();

		// Set the codec parameters based on the properties
		neg_input._set_parameters_fixed(
			CjvxAudioCodec_genpcg::general.num_audio_channels.value,
			CjvxAudioCodec_genpcg::general.buffersize.value,
			CjvxAudioCodec_genpcg::general.samplerate.value,
			(jvxDataFormat)CjvxAudioCodec_genpcg::general.audio_format.value,
			JVX_DATAFORMAT_GROUP_AUDIO_PCM_DEINTERLEAVED);
		neg_output._set_parameters_fixed(
			CjvxAudioCodec_genpcg::general.num_audio_channels.value,
			CjvxAudioCodec_genpcg::general.max_number_bytes.value,
			CjvxAudioCodec_genpcg::general.samplerate.value,
			JVX_DATAFORMAT_BYTE, JVX_DATAFORMAT_GROUP_AUDIO_CODED_GENERIC);
	}
	return(res);
}

jvxErrorType 
CjvxAudioEncoder::activate_encoder()
{
	return JVX_NO_ERROR;
}

// ======================================================================================

jvxErrorType
CjvxAudioEncoder::deactivate()
{
	jvxErrorType res = CjvxObject::_deactivate();
	if (res == JVX_NO_ERROR)
	{
		CjvxAudioCodec_genpcg::unregister_all(static_cast<CjvxProperties*>(this));
		CjvxAudioCodec_genpcg::deallocate_all();

		this->deactivate_encoder();

		JVX_DEACTIVATE_DEFAULT_ONE_IN_ONE_OUT_CONNECTORS();
	}
	return(res);
}

jvxErrorType
CjvxAudioEncoder::deactivate_encoder()
{
	return JVX_NO_ERROR;
}

// ==========================================================================================

jvxErrorType 
CjvxAudioEncoder::is_ready(jvxBool* suc, jvxApiString* reasonIfNot)
{
	jvxErrorType res = CjvxObject::_is_ready(suc, reasonIfNot);
	return(res);
}

jvxErrorType
CjvxAudioEncoder::prepare()
{
	jvxErrorType res = CjvxObject::_prepare();
	return(res);
}

jvxErrorType
CjvxAudioEncoder::start()
{
	jvxErrorType res = CjvxObject::_start();
	return(res);
}

jvxErrorType
CjvxAudioEncoder::stop()
{
	jvxErrorType res = CjvxObject::_stop();
	return(res);
}

jvxErrorType
CjvxAudioEncoder::postprocess()
{
	jvxErrorType res = CjvxObject::_postprocess();
	return(res);
}

// ===================================================================
// ===========================================================================

jvxErrorType 
CjvxAudioEncoder::set_property(jvxCallManagerProperties& callGate,
	const jvx::propertyRawPointerType::IjvxRawPointerType& rawPtr,
	const jvx::propertyAddress::IjvxPropertyAddress& ident,
	const jvx::propertyDetail::CjvxTranferDetail& trans)
{
	jvxErrorType res = JVX_NO_ERROR;
	
	res = _set_property(
		callGate,
		rawPtr,
		ident, trans);
	if (res == JVX_NO_ERROR)
	{
		if(myParent)
		{
			myParent->computeRateAndOutBuffersize(static_cast<CjvxAudioCodec_genpcg*>(this));
		}
	}
	return res;
}

// ===================================================================
#if 0
jvxErrorType 
CjvxAudioEncoder::connect_connect_ocon(IjvxConnectionMaster* theMaster JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxDataProcessorHintDescriptor* user_hint = NULL;
	id_config = JVX_SIZE_UNSELECTED;
	_common_set.theToolsHost->obtain_unique_id(&id_config, "CjvxAudioEncoder::encoder_config_token");

	user_hint =
		jvx_hintDesriptor_push_front((jvxHandle*)my_config_token.c_str(), 
			JVX_DATAPROCESSOR_HINT_DESCRIPTOR_TYPE_CONFIGURATION_TEXT_DESCRIPTION,
			_common_set.theComponentType, user_hint,
			id_config);

	return _connect_connect_ocon(theMaster JVX_CONNECTION_FEEDBACK_CALL_A(fdb), user_hint);
}

jvxErrorType 
CjvxAudioEncoder::disconnect_connect_ocon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxDataProcessorHintDescriptor* user_hint = NULL;
	jvxHandle* hint = NULL;
	jvxComponentIdentification cpId = JVX_COMPONENT_UNKNOWN;
	jvxSize uId = JVX_SIZE_UNSELECTED;
	jvxDataProcessorHintDescriptorType htTp = JVX_DATAPROCESSOR_HINT_DESCRIPTOR_TYPE_NONE;
	jvxErrorType res = _disconnect_connect_ocon(JVX_CONNECTION_FEEDBACK_CALL(fdb), &user_hint);
	assert(res == JVX_NO_ERROR);

	user_hint = jvx_hintDesriptor_pop_front(
		user_hint,
		&hint, &htTp,  &cpId, &uId);

	assert(uId == id_config);
	id_config = JVX_SIZE_UNSELECTED;

	return res;
}
#endif

// ===================================================================

jvxErrorType
CjvxAudioEncoder::test_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
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

		params.bsize = CjvxAudioCodec_genpcg::general.buffersize.value;
		params.srate = CjvxAudioCodec_genpcg::general.samplerate.value;
		params.nchans = CjvxAudioCodec_genpcg::general.num_audio_channels.value;
		params.fsizemax = jvx_wav_compute_bsize_bytes_pcm(&params, params.bsize);

		res = _test_connect_icon(true, JVX_CONNECTION_FEEDBACK_CALL(fdb));
		if (res == JVX_NO_ERROR)
		{
		}
	}
	return res;
}

void 
CjvxAudioEncoder::test_set_output_parameters()
{
	std::string token_type = "pcm";

	_common_set_ldslave.theData_out.con_params.rate = params.srate;
	_common_set_ldslave.theData_out.con_params.number_channels = 1;
	_common_set_ldslave.theData_out.con_params.format = JVX_DATAFORMAT_BYTE;
	_common_set_ldslave.theData_out.con_params.buffersize = params.fsizemax;
	_common_set_ldslave.theData_out.con_params.format_group = JVX_DATAFORMAT_GROUP_AUDIO_CODED_GENERIC;
	_common_set_ldslave.theData_out.con_params.segmentation_x = _common_set_ldslave.theData_out.con_params.buffersize;
	_common_set_ldslave.theData_out.con_params.segmentation_y = 1;

	accept_output_parameters();

	// Pass forward content descriptor
	_common_set_ldslave.theData_out.con_params.format_spec = &formatSpec;
}

void
CjvxAudioEncoder::accept_output_parameters()
{
	std::string propstring;
	propstring = jvx_wav_produce_codec_token(&params);
	formatSpec.assign(propstring);

	params.fsizemax = jvx_wav_compute_bsize_bytes_pcm(&params, params.bsize);
	_common_set_ldslave.theData_out.con_params.buffersize = params.fsizemax;
	CjvxAudioCodec_genpcg::general.buffersize.value = _common_set_ldslave.theData_out.con_params.buffersize;
}

// ===================================================================
jvxErrorType
CjvxAudioEncoder::transfer_backward_ocon(jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
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
			if (ld_cp->con_params.format_spec)
			{
				wav_params params_check;
				std::string config_token_complain = ld_cp->con_params.format_spec->std_str();
				jvxErrorType resL = jvx_wav_configtoken_2_values(config_token_complain.c_str(), &params_check,
					&famToken);
				if (resL != JVX_NO_ERROR)
				{
					forwardRequest = true;
				}
				else
				{
					if (
						(params.bsize == params_check.bsize) &&
						(params.srate == params_check.srate)
						)
					{
						// Accept the new parameters and report positive to caller
						params = params_check;
						accept_output_parameters();
						res = JVX_NO_ERROR;
					}
					else
					{
						forwardRequest = true;
					}
				}

				if (forwardRequest)
				{
					return _common_set_ldslave.theData_in->con_link.connect_from->transfer_backward_ocon(
						tp, data JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
				}
			}
		}
		break;
	default:
		res = _transfer_backward_ocon(true, tp, data JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
	}
	return res;
}

// ===================================================================

jvxErrorType
CjvxAudioEncoder::process_buffers_icon(jvxSize mt_mask, jvxSize idx_stage)
{
	jvxSize i;
	jvxByte** bufsOut = jvx_process_icon_extract_output_buffers<jvxByte>(&_common_set_ldslave.theData_out);
	jvxData** bufsInData = nullptr;
	
	assert(params.nchans == _common_set_ldslave.theData_in->con_params.number_channels);
	assert(params.bsize == _common_set_ldslave.theData_in->con_params.buffersize);

	switch (_common_set_ldslave.theData_in->con_params.format)
	{
	case JVX_DATAFORMAT_DATA:
		bufsInData = jvx_process_icon_extract_input_buffers<jvxData>(_common_set_ldslave.theData_in, idx_stage);
		
		switch (params.sample_type)
		{
		case audio_codec_wav_sample_type::AUDIO_CODEC_WAV_FIXED_POINT:
			switch (params.resolution)
			{
			case audio_codec_wav_resolution::AUDIO_CODEC_WAV_RESOLUTION_16BIT:
				for (i = 0; i < params.nchans; i++)
				{
					jvx_convertSamples_from_flp_norm_to_fxp<jvxData, jvxInt16>(
						(jvxData*)bufsInData[i],
						(jvxInt16*)bufsOut[0],
						_common_set_ldslave.theData_in->con_params.buffersize,
						JVX_MAX_INT_16_DATA,
						0, 1,
						i, params.nchans);
				}
				break;
			case audio_codec_wav_resolution::AUDIO_CODEC_WAV_RESOLUTION_24BIT:
				for (i = 0; i < params.nchans; i++)
				{
					jvx_convertSamples_from_flp_norm_to_bytes<jvxData, jvxInt32>(
						(jvxData*)bufsInData[i],
						(jvxByte*)bufsOut[0],
						_common_set_ldslave.theData_in->con_params.buffersize,
						JVX_MAX_INT_24_DATA, 3,
						0, 1,
						i, params.nchans);
				}
				break;
			case audio_codec_wav_resolution::AUDIO_CODEC_WAV_RESOLUTION_32BIT:
				for (i = 0; i < params.nchans; i++)
				{
					jvx_convertSamples_from_flp_norm_to_fxp<jvxData, jvxInt32>(
						(jvxData*)bufsInData[i],
						(jvxInt32*)bufsOut[0],
						_common_set_ldslave.theData_in->con_params.buffersize,
						JVX_MAX_INT_32_DATA,
						0, 1,
						i, params.nchans);
				}
				break;
			default:
				assert(0);
			}
			break;
		case audio_codec_wav_sample_type::AUDIO_CODEC_WAV_FLOATING_POINT:
			switch (params.resolution)
			{
			case audio_codec_wav_resolution::AUDIO_CODEC_WAV_RESOLUTION_32BIT:
				for (i = 0; i < params.nchans; i++)
				{
					jvx_convertSamples_from_data_to_float<float>(
						(jvxData*)bufsInData[i],
						(float*)bufsOut[0],
						_common_set_ldslave.theData_in->con_params.buffersize,
						0, 1,
						i, params.nchans);
				}
				break;
			case audio_codec_wav_resolution::AUDIO_CODEC_WAV_RESOLUTION_64BIT:
				for (i = 0; i < params.nchans; i++)
				{
					jvx_convertSamples_from_data_to_float<double>(
						(jvxData*)bufsInData[i],
						(double*)bufsOut[0],
						_common_set_ldslave.theData_in->con_params.buffersize,
						0, 1,
						i, params.nchans);
				}
				break;
			default:
				assert(0);
			}
			break;
		default:
			assert(0);
		}
		break;
	case JVX_DATAFORMAT_8BIT:
		assert(0);
		/*
		switch(params_file.fformat)
		{
		case JVX_FILEFORMAT_PCM_FIXED:
			switch(params_file.dformat)
			{
			case JVX_DATAFORMAT_16BIT_LE:
				jvx_convertSamples_from_fxp_shiftleft_to_fxp<jvxInt8, jvxInt16, jvxInt16>(
					(jvxInt8*)fld_in[i],
					(jvxInt16*)runtime.oneChunk,
					writeSize, 8,
					inOutCnt, 1,
					i, params_file.channels);
				break;
			case JVX_DATAFORMAT_32BIT_LE:
				jvx_convertSamples_from_fxp_shiftleft_to_fxp<jvxInt8, jvxInt32, jvxInt32>(
					(jvxInt8*)fld_in[i],
					(jvxInt32*)runtime.oneChunk,
					writeSize, 24,
					inOutCnt, 1,
					i, params_file.channels);
				break;
			default:
				assert(0);
			}
			break;
		case JVX_FILEFORMAT_IEEE_FLOAT:
			switch(params_file.dformat)
			{
			case JVX_DATAFORMAT_16BIT_LE:
				assert(0);
				break;
			case JVX_DATAFORMAT_32BIT_LE:
				jvx_convertSamples_from_fxp_norm_to_flp<jvxInt8, float>(
					(jvxInt8*)fld_in[i],
					(float*)runtime.oneChunk,
					writeSize,
					(float)JVX_MAX_INT_8_DIV,
					inOutCnt, 1,
					i, params_file.channels);
				break;
			default:
				assert(0);
			}
			break;
		default:
			assert(0);
		}*/
		break;
	case JVX_DATAFORMAT_16BIT_LE:
		assert(0);
		/*
		switch (params_file.fformat)
		{
		case JVX_FILEFORMAT_PCM_FIXED:
			switch (params_file.bits_per_sample)
			{
			case 16:
				jvx_convertSamples_from_fxp_noshift_to_fxp<jvxInt16, jvxInt16, jvxInt16>(
					(jvxInt16*)fld_in[i],
					(jvxInt16*)runtime.oneChunk,
					writeSize,
					inOutCnt, 1,
					i, params_file.channels);
				break;
			case 24:
				jvx_convertSamples_from_fxp_shiftleft_to_bytes<jvxInt16, jvxInt32>(
					(jvxInt16*)fld_in[i],
					(jvxByte*)runtime.oneChunk,
					writeSize, 3, 8,
					inOutCnt, 1,
					i, params_file.channels);
				break;
			case 32:
				jvx_convertSamples_from_fxp_shiftleft_to_fxp<jvxInt16, jvxInt32, jvxInt32>(
					(jvxInt16*)fld_in[i],
					(jvxInt32*)runtime.oneChunk,
					writeSize, 16,
					inOutCnt, 1,
					i, params_file.channels);
				break;
			default:
				assert(0);
			}
			break;
		case JVX_FILEFORMAT_IEEE_FLOAT:
			switch (params_file.bits_per_sample)
			{
			case 32:
				jvx_convertSamples_from_fxp_norm_to_flp<jvxInt16, float>(
					(jvxInt16*)fld_in[i],
					(float*)runtime.oneChunk,
					writeSize,
					(float)JVX_MAX_INT_16_DIV,
					inOutCnt, 1,
					i, params_file.channels);
				break;
			default:
				assert(0);
			}
			break;
		default:
			assert(0);
		}
		break;
	case JVX_DATAFORMAT_32BIT_LE:
		switch (params_file.fformat)
		{
		case JVX_FILEFORMAT_PCM_FIXED:
			switch (params_file.bits_per_sample)
			{
			case 16:
				jvx_convertSamples_from_fxp_shiftright_to_fxp<jvxInt32, jvxInt16, jvxInt32>(
					(jvxInt32*)fld_in[i],
					(jvxInt16*)runtime.oneChunk,
					writeSize, 16,
					inOutCnt, 1,
					i, params_file.channels);
				break;
			case 24:
				jvx_convertSamples_from_fxp_shiftright_to_bytes<jvxInt32, jvxInt32>(
					(jvxInt32*)fld_in[i],
					(jvxByte*)runtime.oneChunk,
					writeSize, 3, 8,
					inOutCnt, 1,
					i, params_file.channels);
				break;
			case 32:
				jvx_convertSamples_from_fxp_noshift_to_fxp<jvxInt32, jvxInt32, jvxInt32>(
					(jvxInt32*)fld_in[i],
					(jvxInt32*)runtime.oneChunk,
					writeSize,
					inOutCnt, 1,
					i, params_file.channels);
				break;
			default:
				assert(0);
			}
			break;
		case JVX_FILEFORMAT_IEEE_FLOAT:
			switch (params_file.bits_per_sample)
			{
			case 32:
				jvx_convertSamples_from_fxp_norm_to_flp<jvxInt32, float>(
					(jvxInt32*)fld_in[i],
					(float*)runtime.oneChunk,
					writeSize,
					(float)JVX_MAX_INT_32_DIV,
					inOutCnt, 1,
					i, params_file.channels);
				break;
			default:
				assert(0);
			}
			break;
		default:
			assert(0);
		}
		break;
		*/
	case JVX_DATAFORMAT_64BIT_LE:
		assert(0);
		/*
		switch(params_file.fformat)
		{
		case JVX_FILEFORMAT_PCM_FIXED:
			switch(params_file.dformat)
			{
			case JVX_DATAFORMAT_16BIT_LE:
				jvx_convertSamples_from_fxp_shiftright_to_fxp<jvxInt64, jvxInt16, jvxInt64>(
					(jvxInt64*)fld_in[i],
					(jvxInt16*)runtime.oneChunk,
					writeSize, 48,
					inOutCnt, 1,
					i, params_file.channels);
				break;
			case JVX_DATAFORMAT_32BIT_LE:
				jvx_convertSamples_from_fxp_shiftright_to_fxp<jvxInt64, jvxInt32, jvxInt64>(
					(jvxInt64*)fld_in[i],
					(jvxInt32*)runtime.oneChunk,
					writeSize, 32,
					inOutCnt, 1,
					i, params_file.channels);
				break;
			default:
				assert(0);
			}
			break;
		case JVX_FILEFORMAT_IEEE_FLOAT:
			switch(params_file.dformat)
			{
			case JVX_DATAFORMAT_16BIT_LE:
				assert(0);
				break;
			case JVX_DATAFORMAT_32BIT_LE:
				jvx_convertSamples_from_fxp_norm_to_flp<jvxInt64, float>(
					(jvxInt64*)fld_in[i],
					(float*)runtime.oneChunk,
					writeSize,
					JVX_MAX_INT_64_DIV,
					inOutCnt, 1,
					i, params_file.channels);
				break;
			default:
				assert(0);
			}
			break;
		default:
			assert(0);
		}*/
		break;
	default:
		assert(0);
	}
	return _process_buffers_icon(mt_mask, idx_stage);
}

/*
jvxErrorType
CjvxAudioEncoder::prepare_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_ERROR_WRONG_STATE;
	if (_common_set_min.theState >= JVX_STATE_PREPARED)
	{
		res = jvx_allocate_pipeline_and_buffers_prepare_to(_common_set_ldslave.theData_in);
	}

	if (res == JVX_NO_ERROR)
	{
		_common_set_ldslave.theData_out.con_data.number_buffers = _common_set_ldslave.num_min_buffers_out;
		_common_set_ldslave.theData_out.con_pipeline.num_additional_pipleline_stages = 0;
		_common_set_ldslave.theData_out.con_sync.type_timestamp = _common_set_ldslave.theData_in->con_sync.type_timestamp;

		// If another user_hint list comes in, add the current list to the end
		_common_set_ldslave.theData_out.con_data.user_hints = jvx_hintDesriptor_push_front(
				_common_set_ldslave.theData_in->con_data.user_hints,
				_common_set_ldslave.theData_out.con_data.user_hints);

		res = _prepare_connect_icon(true JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
		if (res != JVX_NO_ERROR)
		{
			goto exit_error;
		}
	}

	return res;
exit_error:
	jvx_deallocate_pipeline_and_buffers_postprocess_to(_common_set_ldslave.theData_in);
	return res;
}

jvxErrorType
CjvxAudioEncoder::postprocess_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_ERROR_WRONG_STATE;
	if (_common_set_min.theState >= JVX_STATE_PREPARED)
	{
		res = jvx_deallocate_pipeline_and_buffers_postprocess_to(_common_set_ldslave.theData_in);
		res = _postprocess_connect_icon(true JVX_CONNECTION_FEEDBACK_CALL_A(fdb));

		_common_set_ldslave.theData_out.con_data.user_hints = jvx_hintDesriptor_pop_front(
			_common_set_ldslave.theData_in->con_data.user_hints,
			_common_set_ldslave.theData_out.con_data.user_hints);
	}

	return res;
}
*/

jvxErrorType 
CjvxAudioEncoder::get_configure_token(jvxApiString* astr)
{
	if (astr)
	{
		std::string conf_token;
		jvx_wav_values_2_configtoken(conf_token, & params);
		astr->assign(conf_token);
	}
	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxAudioEncoder::update_configure_token(const char* token)
{
	jvxErrorType res = JVX_ERROR_INVALID_FORMAT;
	if (token)
	{
		jvxApiString famToken;
		wav_params params_new;
		res = jvx_wav_configtoken_2_values( token,  & params_new, &famToken);

		// When do we accept new settings? We might restrict it to lower bit resolutions if
		// processing is already on
	}
	return JVX_ERROR_UNSUPPORTED;
}

#ifdef JVX_PROJECT_NAMESPACE
}
#endif
