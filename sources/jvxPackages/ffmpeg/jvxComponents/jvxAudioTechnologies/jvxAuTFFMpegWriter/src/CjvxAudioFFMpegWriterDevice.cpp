#include "CjvxAudioFFMpegWriterDevice.h"
#include "CjvxAudioFFMpegWriterTechnology.h"
#include "jvx-helpers-cpp.h"
#include "jvx_audiocodec_helpers.h"
#include "CjvxAudioFFMpegWriterCommon.h"


CjvxAudioFFMpegWriterDevice::CjvxAudioFFMpegWriterDevice(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE) :
		CjvxAudioDevice(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
{
	_common_set.theObjectSpecialization = reinterpret_cast<jvxHandle*>(static_cast<IjvxDevice*>(this));
	_common_set_audio_device.formats.push_back(JVX_DATAFORMAT_BYTE);
	
	_common_set_properties.moduleReference = _common_set.theModuleName;
	//_config.reportMissedCallbacks = false;

	_common_set.thisisme = static_cast<IjvxObject*>(this);
}


CjvxAudioFFMpegWriterDevice::~CjvxAudioFFMpegWriterDevice()
{
}

// =============================================================================

jvxErrorType
CjvxAudioFFMpegWriterDevice::init_parameters(
	const std::string& folder,
	const std::string& prefix,
	const std::string& filesuffix,
	const std::string& fSType,
	wav_params* wav_init_params_arg,
	const std::string& cfg_compact,
	CjvxAudioFFMpegWriterTechnology* par)
{
	jvxErrorType res = JVX_NO_ERROR;
	JVX_MAKE_DIRECTORY_RETVAL retVal = JVX_MAKE_DIRECTORY_POS;
	if (JVX_DIRECTORY_EXISTS(folder.c_str()) == c_false)
	{
		retVal = JVX_MAKE_DIRECTORY(folder.c_str());
	}
	
	if(retVal == JVX_MAKE_DIRECTORY_POS)
	{
		foldername = folder;
		fileprefix = prefix;
	}
	else
	{
		res = JVX_ERROR_INVALID_ARGUMENT;
	}

	if (res == JVX_NO_ERROR)
	{
		fParams.fName = foldername + JVX_SEPARATOR_DIR_CHAR + prefix + "." + filesuffix;
		fParams.fPrefix = foldername + JVX_SEPARATOR_DIR_CHAR + prefix;
		fParams.fSuffix = filesuffix;
		fParams.subTypeSpec = fSType;
		/* allocate the output media context */

		res = reallocate_output_context();
	}

	if (res == JVX_NO_ERROR)
	{		

		// ==============================================
		fParams.fFormatTag = fParams.fmt->name;
		fParams.idCodec = fParams.fmt->audio_codec;

		if (fParams.fmt->name == (std::string)"wav")
		{
			fParams.idSub = 0;
			while (1)
			{
				if (optionsWav[fParams.idSub] == nullptr)
				{
					break;
				}
				else
				{
					if (optionsWav[fParams.idSub] == fParams.subTypeSpec)
					{
						break;
					}
					else
					{
						fParams.idSub++;
					}
				}
			}

			// Setup some default settings
			fParams.sRate = 48000;
			fParams.nChans = 2;
			fParams.chanLayout.nb_channels = 2;
			fParams.chanLayout.order = AV_CHANNEL_ORDER_UNSPEC;
			fParams.chanLayout.u.mask = 0;
			fParams.tpCodec = AVMEDIA_TYPE_AUDIO;
			fParams.bSizeAudio = 1024;

			jvx_ffmpeg_update_format_settings_wav(fParams, fParams.idSub);
		}
		else if (fParams.fmt->name == (std::string)"mp3")
		{	
			fParams.idSub = 0;
			while (1)
			{
				if (optionsMp3M4a[fParams.idSub] == nullptr)
				{
					break;
				}
				else
				{
					if (optionsMp3M4a[fParams.idSub] == fParams.subTypeSpec)
					{
						break;
					}
					else
					{
						fParams.idSub++;
					}
				}
			}

			// Setup some default settings
			fParams.sRate = 48000;
			fParams.nChans = 2;
			fParams.chanLayout.nb_channels = 2;
			fParams.chanLayout.order = AV_CHANNEL_ORDER_UNSPEC;
			fParams.chanLayout.u.mask = 0;
			fParams.tpCodec = AVMEDIA_TYPE_AUDIO;
			fParams.bSizeAudio = 1024;

			jvx_ffmpeg_update_format_settings_mp3(fParams, fParams.idSub);
		}
		else if (fParams.fmt->name == (std::string)"ipod")
		{
			fParams.idSub = 0;
			while (1)
			{
				if (optionsMp3M4a[fParams.idSub] == nullptr)
				{
					break;
				}
				else
				{
					if (optionsMp3M4a[fParams.idSub] == fParams.subTypeSpec)
					{
						break;
					}
					else
					{
						fParams.idSub++;
					}
				}
			}

			// Setup some default settings
			fParams.sRate = 48000;
			fParams.nChans = 2;
			fParams.chanLayout.nb_channels = 2;
			fParams.chanLayout.order = AV_CHANNEL_ORDER_UNSPEC;
			fParams.chanLayout.u.mask = 0;
			fParams.tpCodec = AVMEDIA_TYPE_AUDIO;
			fParams.bSizeAudio = 1024;
#ifdef JVX_OS_WINDOWS
			fParams.codec_selection = "aac_mf";
#endif

			jvx_ffmpeg_update_format_settings_mp3(fParams, fParams.idSub);
		}
		else
		{
			assert(0);
		}

		jvx_ffmpeg_verify_correct_codec_settings(fParams);
		jvx_ffmpeg_printout_file_params(fParams);
	}
	

	if (res == JVX_NO_ERROR)
	{
		parentTech = par;
		config_compact = cfg_compact;
		if (wav_init_params_arg)
		{
			wav_params* params_prop_wav = (wav_params*)wav_init_params_arg;
			file_params = *params_prop_wav;
		}
	}

	_common_set_min.theOwner = static_cast<IjvxObject*>(parentTech);
	return res;
}

jvxErrorType 
CjvxAudioFFMpegWriterDevice::reallocate_output_context()
{
	jvxErrorType res = JVX_NO_ERROR;

	// aac_mf

	if (fParams.oc)
		return JVX_ERROR_WRONG_STATE;

	avformat_alloc_output_context2(&fParams.oc, NULL, NULL, fParams.fName.c_str());
	if (!fParams.oc)
	{
		avformat_alloc_output_context2(&fParams.oc, NULL, "mpeg", fParams.fName.c_str());
	}

	if (!fParams.oc)
	{
		res = JVX_ERROR_INVALID_SETTING;
	}
	if (res == JVX_NO_ERROR)
	{
		fParams.fmt = fParams.oc->oformat;
	}
	return res;
}

jvxErrorType
CjvxAudioFFMpegWriterDevice::term_file()
{
	jvxErrorType res = JVX_NO_ERROR;
	
	foldername = "";
	fileprefix = "";

	return res;
}

std::string 
CjvxAudioFFMpegWriterDevice::get_last_error()
{
	return last_error;
}

// =============================================================================
// =============================================================================

jvxErrorType
CjvxAudioFFMpegWriterDevice::select(IjvxObject* owner)
{
	jvxErrorType res = CjvxAudioDevice::select(owner);
	if (res == JVX_NO_ERROR)
	{		
			
	}
	return (res);
}


jvxErrorType
CjvxAudioFFMpegWriterDevice::unselect()
{
	jvxErrorType res = CjvxAudioDevice::unselect();
	if (res == JVX_NO_ERROR)
	{
	}
	return (res);
}

jvxErrorType
CjvxAudioFFMpegWriterDevice::activate()
{
	jvxSize cnt = 0;
	jvxErrorType res = CjvxAudioDevice::activate();
	if (res == JVX_NO_ERROR)
	{
		genFFMpegWriter_device::init_all();
		genFFMpegWriter_device::allocate_all();
		genFFMpegWriter_device::register_all(static_cast<CjvxProperties*>(this));
		genFFMpegWriter_device::register_callbacks(
			static_cast<CjvxProperties*>(this),
			set_fixed, set_file_type,
			reinterpret_cast<jvxHandle*>(this),
			nullptr);

		if (fParams.fmt->name == (std::string)"wav")
		{
			genFFMpegWriter_device::file.file_sub_type.value.entries.clear();
			while (optionsWav[cnt])
			{
				genFFMpegWriter_device::file.file_sub_type.value.entries.push_back(optionsWav[cnt]);
				cnt++;
			}
			jvx_bitZSet(genFFMpegWriter_device::file.file_sub_type.value.selection(0), fParams.idSub);
		}
		else if (fParams.fmt->name == (std::string)"mp3")
		{
			genFFMpegWriter_device::file.file_sub_type.value.entries.clear();
			while (optionsMp3M4a[cnt])
			{
				genFFMpegWriter_device::file.file_sub_type.value.entries.push_back(optionsMp3M4a[cnt]);
				cnt++;
			}
			jvx_bitZSet(genFFMpegWriter_device::file.file_sub_type.value.selection(0), fParams.idSub);
		}
		else if(fParams.fmt->name == (std::string)"m4a")
		{
			genFFMpegWriter_device::file.file_sub_type.value.entries.clear();
			while (optionsMp3M4a[cnt])
			{
				genFFMpegWriter_device::file.file_sub_type.value.entries.push_back(optionsMp3M4a[cnt]);
				cnt++;
			}
			jvx_bitZSet(genFFMpegWriter_device::file.file_sub_type.value.selection(0), fParams.idSub);
		}

		file_props_2_ayf_props();

		std::string iFile = foldername + JVX_SEPARATOR_DIR + fileprefix;
		genFFMpegWriter_device::file.name.value = iFile;
		if (!config_compact.empty())
		{
			jvxComponentIdentification tp = JVX_COMPONENT_CONFIG_PROCESSOR;
			refComp<IjvxConfigProcessor> cfg;
			tp.slotid = JVX_SIZE_SLOT_OFF_SYSTEM;
			cfg = reqRefTool<IjvxConfigProcessor>(_common_set.theToolsHost, JVX_COMPONENT_CONFIG_PROCESSOR);
			if (cfg.cpPtr)
			{
				jvxErrorType resL = cfg.cpPtr->parseTextField(config_compact.c_str(), "STORED", 0);
				if (resL == JVX_NO_ERROR)
				{
					jvxConfigData* datSec = nullptr;
					cfg.cpPtr->getConfigurationHandle(&datSec);
					if (datSec)
					{
						jvxCallManagerConfiguration callMan;
						this->put_configuration(
							&callMan,
							cfg.cpPtr,
							datSec, "STORED", 0);
						cfg.cpPtr->removeHandle(datSec);
					}
				}
				retRefTool<IjvxConfigProcessor>(_common_set.theToolsHost, JVX_COMPONENT_CONFIG_PROCESSOR, cfg);
			}
		}
	}
	return (res);
}

jvxErrorType
CjvxAudioFFMpegWriterDevice::deactivate()
{
	jvxErrorType res = _pre_check_deactivate();
	if (res == JVX_NO_ERROR)
	{
		genFFMpegWriter_device::unregister_callbacks(
			static_cast<CjvxProperties*>(this),
			nullptr);
		genFFMpegWriter_device::unregister_all(static_cast<CjvxProperties*>(this));
		genFFMpegWriter_device::deallocate_all();
		res = CjvxAudioDevice::deactivate();


	}
	return (res);
}

// ===============================================================================

/*
// An audio device that provides data has input channels. Nevertheless, the 
		// input channels are forwarded as output channels towards the next chain element
		//  
		// On a filereader the channel number is related to the input and will be forwarded 
		// on the output side during processing
CjvxAudioDevice::properties_active.inputchannelselection.value.entries.clear();
jvx_bitFClear(CjvxAudioDevice::properties_active.inputchannelselection.value.selection);
for (i = 0; i < fileprops_wav.nchans; i++)
{
	CjvxAudioDevice::properties_active.inputchannelselection.value.entries.push_back("Channel #" + jvx_size2String(i));
	jvx_bitSet(CjvxAudioDevice::properties_active.inputchannelselection.value.selection, i);
}
CjvxAudioDevice::properties_active.samplerate.value = fileprops_wav.rate;

CjvxAudioDevice::properties_active.outputchannelselection.value.entries.clear();
jvx_bitFClear(CjvxAudioDevice::properties_active.outputchannelselection.value.selection);

this->updateDependentVariables_lock(-1, JVX_PROPERTY_CATEGORY_PREDEFINED, true);

*/
jvxErrorType
CjvxAudioFFMpegWriterDevice::prepare()
{
	jvxErrorType res = CjvxAudioDevice::prepare();

	if (res == JVX_NO_ERROR)
	{        		
	}
	return (res);
}


jvxErrorType
CjvxAudioFFMpegWriterDevice::postprocess()
{
	jvxErrorType res = CjvxAudioDevice::postprocess();
	jvxSize numNotDeallocated = 0;
	jvxErrorType resL;
	if (res == JVX_NO_ERROR)
	{		
	}
	return (res);
}

// ===============================================================================

jvxErrorType
CjvxAudioFFMpegWriterDevice::put_configuration(jvxCallManagerConfiguration* callConf, IjvxConfigProcessor* processor,
	jvxHandle* sectionToContainAllSubsectionsForMe, const char* filename, jvxInt32 lineno)
{
	jvxErrorType res = JVX_NO_ERROR;
	std::vector<std::string> warnings;

	// Read all parameters for base class
	CjvxAudioDevice::put_configuration(callConf, processor, sectionToContainAllSubsectionsForMe, filename, lineno);

	if (this->_common_set_min.theState == JVX_STATE_ACTIVE)
	{

	}
	return (res);
}

jvxErrorType
CjvxAudioFFMpegWriterDevice::get_configuration(jvxCallManagerConfiguration* callConf, IjvxConfigProcessor* processor, jvxHandle* sectionWhereToAddAllSubsections)
{
	// Write all parameters from base class
    CjvxAudioDevice::get_configuration(callConf, processor, sectionWhereToAddAllSubsections);

	return (JVX_NO_ERROR);
}

jvxErrorType 
CjvxAudioFFMpegWriterDevice::test_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res =  CjvxAudioDevice::test_chain_master(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	if (res == JVX_ERROR_REQUEST_CALL_AGAIN)
	{
		res = CjvxAudioDevice::test_chain_master(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	}
	return res;
}

jvxErrorType
CjvxAudioFFMpegWriterDevice::test_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_ERROR_INVALID_ARGUMENT;
	std::string config_token;
	wav_params passed_props;
	jvxApiString famToken;
	jvxSize i;
	jvxSize bSize = 0;
	jvxFfmpegAudioParameter nParams;
	jvxLinkDataDescriptor ld;

	// Here we detect the provided parameters. We may setup another setup

	res = JVX_NO_ERROR;
	ld.con_params = _common_set_icon.theData_in->con_params;

	if (
		(ld.con_params.buffersize != JVX_SIZE_DONTCARE) ||
		(ld.con_params.rate != JVX_SIZE_DONTCARE) ||
		(ld.con_params.number_channels != 1) ||
		(ld.con_params.format != JVX_DATAFORMAT_POINTER) ||
		(ld.con_params.format_group != JVX_DATAFORMAT_GROUP_FFMPEG_PACKET_FWD))
	{
		res = _common_set_icon.theData_in->con_link.connect_from->transfer_backward_ocon(JVX_LINKDATA_TRANSFER_COMPLAIN_DATA_SETTINGS,
			&ld JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
	}

	if (res == JVX_NO_ERROR)
	{
		config_token = _common_set_icon.theData_in->con_params.format_spec.std_str();
		res = jvx_ffmpeg_codec_token_2_parameter(config_token.c_str(), nParams);
	}

	if (res == JVX_NO_ERROR)
	{
		jvxBool requiresUpdatePrior = false;

		if (
			(nParams.nChans != fParams.nChans) ||
			(nParams.bSizeAudio != fParams.bSizeAudio) ||
			(nParams.sRate != fParams.sRate))
		{
			fParams.nChans = nParams.nChans;
			fParams.sRate = nParams.sRate;
			fParams.bSizeAudio = nParams.bSizeAudio;
		}
		
		if (JVX_CHECK_SIZE_SELECTED(genFFMpegWriter_device::fixed.srate.value))
		{
			if(fParams.sRate != genFFMpegWriter_device::fixed.srate.value)
			{ 
				fParams.sRate = genFFMpegWriter_device::fixed.srate.value;
				requiresUpdatePrior = true;
			}
		}

		if (JVX_CHECK_SIZE_SELECTED(genFFMpegWriter_device::fixed.nchans.value))
		{
			if (fParams.nChans != genFFMpegWriter_device::fixed.nchans.value)
			{
				fParams.nChans = genFFMpegWriter_device::fixed.nchans.value;
				requiresUpdatePrior = true;
			}
		}
		// =====================================================

		if (fParams.frameSize)
		{
			if (
				(nParams.fFormatTag != fParams.fFormatTag) ||
				(nParams.frameSize != fParams.frameSize) ||
				(nParams.frameSizeMax != fParams.frameSizeMax) ||
				(nParams.bitsPerCoded != fParams.bitsPerCoded) ||
				(nParams.isFloat != fParams.isFloat))
			{
				requiresUpdatePrior = true;
			}
		}
		else
		{
			if (
				(nParams.fFormatTag != fParams.fFormatTag) ||
				(nParams.frameSizeMax != fParams.frameSizeMax) ||
				(nParams.bitsPerCoded != fParams.bitsPerCoded) ||
				(nParams.frameSizeMax != fParams.frameSizeMax) ||
				(nParams.isFloat != fParams.isFloat))
			{
				requiresUpdatePrior = true;
			}
		}
		if (requiresUpdatePrior)
		{
			jvx_ffmpeg_wav_params(fParams);
			ld.con_params.format_spec = jvx_ffmpeg_parameter_2_codec_token(fParams);

			res = _common_set_icon.theData_in->con_link.connect_from->transfer_backward_ocon(JVX_LINKDATA_TRANSFER_COMPLAIN_DATA_SETTINGS,
				&ld JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
			if (res == JVX_NO_ERROR)
			{
				// We want to check that this really is ok?
				std::string cfg_token_local = jvx_ffmpeg_parameter_2_codec_token(fParams);
				std::string cfg_token_prev = _common_set_icon.theData_in->con_params.format_spec.std_str().c_str();
			}
		}
	} // if (res == JVX_NO_ERROR)

	if (res == JVX_NO_ERROR)
	{
		file_props_2_ayf_props();

		updateDependentVariables_lock(-1,
			JVX_PROPERTY_CATEGORY_PREDEFINED, true,
			JVX_PROPERTY_CALL_PURPOSE_NONE_SPECIFIC,
			true);
	}
	return res;
}

void
CjvxAudioFFMpegWriterDevice::file_props_2_ayf_props()
{
	jvxSize i;
	if (fParams.fFormatTag == "wav")
	{
		assert(fParams.frameSize == 0);
		CjvxAudioDevice_genpcg::properties_active.buffersize.value = fParams.frameSizeMax / fParams.nChans / (fParams.bitsPerCoded / 8);
	}
	else
	{
		CjvxAudioDevice_genpcg::properties_active.buffersize.value = fParams.frameSize;
	}

	CjvxAudioDevice_genpcg::properties_active.samplerate.value = fParams.sRate;
	CjvxAudioDevice_genpcg::properties_active.inputchannelselection.value.entries.clear();
	CjvxAudioDevice_genpcg::properties_active.outputchannelselection.value.entries.clear();
	jvx_bitFClear(CjvxAudioDevice_genpcg::properties_active.outputchannelselection.value.selection());
	for (i = 0; i < fParams.nChans; i++)
	{
		CjvxAudioDevice_genpcg::properties_active.outputchannelselection.value.entries.push_back(
			"Channel #" + jvx_size2String(i));
		jvx_bitSet(CjvxAudioDevice_genpcg::properties_active.outputchannelselection.value.selection(), i);
	}
	CjvxAudioDevice_genpcg::properties_active.format.value = JVX_DATAFORMAT_BYTE;
}

/*
jvxSize 
CjvxAudioFFMpegWriterDevice::compute_bsize_pcm(jvxSize nchans, jvxSize bitssample, jvxSize nomBsize)
{
	return (nchans * bitssample * nomBsize / 8);

}
*/

void
CjvxAudioFFMpegWriterDevice::test_set_output_parameters()
{
	// _common_set
	
	// We need to make some specific adaptations here which can not be 
	// made in the standard implementation

	// The format text specification to configure the decoder
	// The buffersize as it is derived from the buffersize as set by the audio framing definition
	// You can specify the buffersize by setting the standard parameter for the device buffersize
	_common_set_ocon.theData_out.con_params.buffersize = 0;
	_common_set_ocon.theData_out.con_params.rate = 0;
	_common_set_ocon.theData_out.con_params.number_channels = 0;
	_common_set_ocon.theData_out.con_params.format = JVX_DATAFORMAT_NONE;
	_common_set_ocon.theData_out.con_params.segmentation.x = 0;
	_common_set_ocon.theData_out.con_params.segmentation.y = 0;

	// The only purpose is to trigger the output side
	_common_set_ocon.theData_out.con_params.format_group = JVX_DATAFORMAT_GROUP_TRIGGER_ONLY;
}

/*
std::string
CjvxAudioFFMpegWriterDevice::produce_decoder_token()
{
	std::string propstring;

	std::string token_type;
	wav_params params;
	params.bitspersample = fileprops_wav.bitssample;
	params.bsize = CjvxAudioDevice_genpcg::properties_active.buffersize.value;
	if (fileprops_wav.fformat == JVX_FILEFORMAT_PCM_FIXED)
		params.fixedpt = true;
	else
		params.fixedpt = false;
	params.lendian = fileprops_wav.lendian;
	params.fsizemax = _common_set_ocon.theData_out.con_params.buffersize;
	params.nchans = fileprops_wav.nchans;
	params.srate = fileprops_wav.rate;

	switch (fileprops_wav.subtype)
	{
	case JVX_WAV_UNKNOWN:
		token_type = "wav0";
		break;
	case JVX_WAV_64_SONY:
		token_type = "wav64sony";
		break;
	case JVX_WAV_64:
		token_type = "wav64";
		break;
	case JVX_WAV_32:
		token_type = "wav32";
		break;
	default:
		assert(0);
	}

	jvx_wav_values_2_configtoken(propstring,
		&params,
		token_type);
	return propstring;
}
*/

jvxErrorType
CjvxAudioFFMpegWriterDevice::prepare_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
    jvxErrorType res = JVX_NO_ERROR;

    // Auto-run the prepare function
	res = prepare_chain_master_autostate(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	assert(res == JVX_NO_ERROR);

	// Prepare processing parameters
	_common_set_ocon.theData_out.con_data.number_buffers = 1;
	jvx_bitZSet(_common_set_ocon.theData_out.con_data.alloc_flags, 
		(int)jvxDataLinkDescriptorAllocFlags::JVX_LINKDATA_ALLOCATION_FLAGS_IS_ZEROCOPY_CHAIN_SHIFT);

	// New type of connection by propagating through linked elements
    res = _prepare_chain_master(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	if (res == JVX_NO_ERROR)
	{
		/*
		involve_read_thread = false;
		if (genFFMpegWriter_device::exteded_properties.threaded_read.value == c_true)
		{
			jvxSize bytesonesample = fileprops_wav.nchans * fileprops_wav.bitssample / 8;
			jvxData numsamples = genFFMpegWriter_device::exteded_properties.preuse_bsize_msecs.value * 0.001 * fileprops_wav.rate;
			jvxSize numbytes = (jvxSize)numsamples * bytesonesample;
			jvxSize numbuffers = ceil((jvxData)numbytes / (jvxData)_common_set_ocon.theData_out.con_params.buffersize);

			preuse_buffer_sz = JVX_MAX(1, numbuffers) * _common_set_ocon.theData_out.con_params.buffersize;
			JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(preuse_buffer_ptr, jvxByte, preuse_buffer_sz);
			readposi = 0;
			fHeight = 0;
			readsize = _common_set_ocon.theData_out.con_params.buffersize;
			bufstatus = jvxAudioFileWriterBufferStatus::JVX_BUFFER_STATUS_NONE;

			involve_read_thread = true;
		}
		*/

		// All parameters were set before, only very few need update
		_common_set_ocon.theData_out.con_data.number_buffers = 1;

		// Start the reader
		//wavFileReader.prepare(JVX_SIZE_UNSELECTED); // No read limit in module
		//wavFileReader.start();
	}
	else
	{
		_postprocess_chain_master(NULL);		
		goto leave_error;
    }
    
    return(res);
leave_error:
		
    return res;
}

jvxErrorType
CjvxAudioFFMpegWriterDevice::postprocess_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxErrorType resL;

	
	// Stop the reader
	//wavFileReader.stop();
	//wavFileReader.postprocess();

	_postprocess_chain_master(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	assert(res == JVX_NO_ERROR);

	// If the chain is postprocessed, the object itself should also be postprocessed if not done so before
	resL = postprocess_chain_master_autostate(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	assert(resL == JVX_NO_ERROR);

	return res;

}

jvxErrorType
CjvxAudioFFMpegWriterDevice::start_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;

	// If the chain is started, the object itself should also be started if not done so before
	jvxErrorType resL = start_chain_master_autostate(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	assert(resL == JVX_NO_ERROR);

	res = _start_chain_master(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	if(res != JVX_NO_ERROR)
	{
	    goto leave_error;
	}

	// Start core function

	if (res != JVX_NO_ERROR)
	{
	    _stop_chain_master(NULL);
	    goto leave_error;
	}

	return res;
leave_error:
	return res;
}

jvxErrorType
CjvxAudioFFMpegWriterDevice::stop_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxErrorType resL;

	// Core stop function here

	res = _stop_chain_master(NULL);
	assert(res == JVX_NO_ERROR);

	// If the chain is stopped, the object itself should also be stopped if not done so before
	resL = stop_chain_master_autostate(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	assert(resL == JVX_NO_ERROR);

	return res;
}

jvxErrorType
CjvxAudioFFMpegWriterDevice::prepare_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	int ret = 0;
	jvxErrorType res = JVX_NO_ERROR;
	// This is the return from the link list

	// Connect the output side and start this link
	res = allocate_pipeline_and_buffers_prepare_to();

	// Do not attach any user hint into backward direction
	_common_set_icon.theData_in->con_compat.user_hints = NULL;
	// _common_set_icon.theData_in->pipeline.idx_stage = 0;

	fParams.st = avformat_new_stream(fParams.oc, NULL);

	// Extract the connection parameters for decoding
	jvxLinkDataAttachedChain* ptrEncCtx = _common_set_icon.theData_in->con_link.attached_chain_single_pass;
	while (ptrEncCtx)
	{
		jvxLinkDataAttachedStringDetect<jvxLinkDataAttachedChain>* ptr_stream_txt = reinterpret_cast<jvxLinkDataAttachedStringDetect<jvxLinkDataAttachedChain>*>(ptrEncCtx->if_specific(JVX_LINKDATA_ATTACHED_STRING_DETECT));
		if (ptr_stream_txt->descr)
		{
			if ((std::string)ptr_stream_txt->descr == "/ffmpeg/audiostream/codecctx")
			{
				fParams.enc_ctx = (AVCodecContext*)ptr_stream_txt->data;
				break;
			}
		}
		ptrEncCtx = ptrEncCtx->next;
	}

	// Attach the packet containers
	for (jvxSize i = 0; i < _common_set_icon.theData_in->con_data.number_buffers; i++)
	{
		_common_set_icon.theData_in->con_data.buffers[i] = reinterpret_cast<jvxHandle**>(av_packet_alloc());
	}

	inThreadInit = false;
	if (jvx_bitTest(_common_set_icon.theData_in->con_data.alloc_flags, (int)jvxDataLinkDescriptorAllocFlags::JVX_LINKDATA_ALLOCATION_FLAGS_THREAD_INIT_PRE_RUN))
	{
		inThreadInit = true;
	}

	if (inThreadInit == false)
	{
		writer_allocate_core();
	}
	return res;
}

jvxErrorType
CjvxAudioFFMpegWriterDevice::postprocess_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;

	writer_deallocate_core();

	// Attach the packet containers
	for (jvxSize i = 0; i < _common_set_icon.theData_in->con_data.number_buffers; i++)
	{
		AVPacket* pkt = reinterpret_cast<AVPacket*>(_common_set_icon.theData_in->con_data.buffers[i]);
		_common_set_icon.theData_in->con_data.buffers[i] = nullptr;	
		av_packet_free(&pkt);
	}

	avformat_free_context(fParams.oc);
	fParams.oc = nullptr;

	res = reallocate_output_context();
	assert(res == JVX_NO_ERROR);

	fParams.enc_ctx = nullptr;
	fParams.st = nullptr;

	res = deallocate_pipeline_and_buffers_postprocess_to();

	return res;
}

jvxErrorType
CjvxAudioFFMpegWriterDevice::process_buffers_icon(jvxSize mt_mask, jvxSize idx_stage)
{
	jvxErrorType res = JVX_NO_ERROR;
	AVPacket* pkt = nullptr;
	jvxSize idx_stage_local = idx_stage;

	if (JVX_CHECK_SIZE_UNSELECTED(idx_stage_local))
	{
		idx_stage_local = *_common_set_icon.theData_in->con_pipeline.idx_stage_ptr;
	}

	pkt = (AVPacket*)_common_set_icon.theData_in->con_data.buffers[idx_stage_local];

	// av_interleaved_write_frame(fmt_ctx, pkt);
	if (pkt->size)
	{
		av_write_frame(fParams.oc, pkt);
		av_packet_unref(pkt);
	}
	return res;
}

jvxErrorType
CjvxAudioFFMpegWriterDevice::process_start_icon(jvxSize pipeline_offset, jvxSize* idx_stage,
					jvxSize tobeAccessedByStage,
					callback_process_start_in_lock clbk,
					jvxHandle* priv_ptr)
{
	jvxErrorType res = JVX_NO_ERROR;
	res = shift_buffer_pipeline_idx_on_start( pipeline_offset,  idx_stage,
	    tobeAccessedByStage,
	    clbk, priv_ptr);
	return res;
}

jvxErrorType
CjvxAudioFFMpegWriterDevice::process_stop_icon(jvxSize idx_stage, jvxBool shift_fwd,
    jvxSize tobeAccessedByStage,
	    callback_process_stop_in_lock clbk,
	    jvxHandle* priv_ptr)
{
	jvxErrorType res = JVX_NO_ERROR;

	// Unlock the buffer for this pipeline
	res = shift_buffer_pipeline_idx_on_stop( idx_stage,  shift_fwd,
	    tobeAccessedByStage, clbk, priv_ptr);

	return res;
}

jvxErrorType
CjvxAudioFFMpegWriterDevice::transfer_forward_icon(jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;
	if (tp == JVX_LINKDATA_TRANSFER_REQUEST_THREAD_INIT_PRERUN)
	{
		assert(inThreadInit);
		writer_allocate_core();
	}
	res = CjvxAudioDevice::transfer_forward_icon(tp, data JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
	if (res == JVX_NO_ERROR)
	{
		if (tp == JVX_LINKDATA_TRANSFER_REQUEST_THREAD_TERM_POSTRUN)
		{
			assert(inThreadInit);
			writer_deallocate_core();
		}
	}
	return res;
}

jvxErrorType 
CjvxAudioFFMpegWriterDevice::transfer_backward_ocon(jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxLinkDataDescriptor* ld_cp = nullptr;
	jvxErrorType res = JVX_NO_ERROR;

	switch (tp)
	{
	case JVX_LINKDATA_TRANSFER_REQUEST_DATA:
		trigger_one_buffer();	
		break;
	case JVX_LINKDATA_TRANSFER_COMPLAIN_DATA_SETTINGS:

		ld_cp = (jvxLinkDataDescriptor*)data;
		// Do something to select another codec setting
		CjvxAudioDevice_genpcg::properties_active.buffersize.value = 1024; // ld_cp->con_params.buffersize / (fileprops_wav.bitssample * fileprops_wav.nchans / 8);
		_common_set_ocon.theData_out.con_params.buffersize = CjvxAudioDevice_genpcg::properties_active.buffersize.value;
		_common_set_ocon.theData_out.con_params.buffersize = 1024;
			//compute_bsize_pcm(fileprops_wav.nchans, fileprops_wav.bitssample, _common_set_ocon.theData_out.con_params.buffersize);
		_common_set_ocon.theData_out.con_params.segmentation.x = _common_set_ocon.theData_out.con_params.buffersize;
		// format_descriptor.assign(produce_decoder_token());
		return JVX_NO_ERROR;
	}
	return res;
}

jvxErrorType 
CjvxAudioFFMpegWriterDevice::set_property(jvxCallManagerProperties& callGate,
	const jvx::propertyRawPointerType::IjvxRawPointerType& rawPtr,
	const jvx::propertyAddress::IjvxPropertyAddress& ident,
	const jvx::propertyDetail::CjvxTranferDetail& trans)
{
	return CjvxAudioDevice::set_property(callGate,
		rawPtr, ident, trans);	
}

void
CjvxAudioFFMpegWriterDevice::trigger_one_buffer()
{
	jvxErrorType res = JVX_NO_ERROR;
	if (_common_set_ocon.theData_out.con_link.connect_to)
	{
		res = _common_set_ocon.theData_out.con_link.connect_to->process_start_icon();
		if (res == JVX_NO_ERROR)
		{
			// Prepare one frame here!

			_common_set_ocon.theData_out.con_link.connect_to->process_buffers_icon();
			_common_set_ocon.theData_out.con_link.connect_to->process_stop_icon();
		}
	}
}

JVX_PROPERTIES_FORWARD_C_CALLBACK_EXECUTE_FULL(CjvxAudioFFMpegWriterDevice, set_fixed)
{
	this->request_test_chain(_common_set.theReport);
	return JVX_NO_ERROR;
}

JVX_PROPERTIES_FORWARD_C_CALLBACK_EXECUTE_FULL(CjvxAudioFFMpegWriterDevice, set_file_type)
{
	fParams.idSub = JVX_PROP_BIT_2_SEL_ID(genFFMpegWriter_device::file.file_sub_type);
	if (fParams.fFormatTag == "wav")
	{
		jvx_ffmpeg_update_format_settings_wav(fParams, fParams.idSub);
	}
	this->request_test_chain(_common_set.theReport);
	return JVX_NO_ERROR;
}

void
CjvxAudioFFMpegWriterDevice::writer_allocate_core()
{
	int ret = avcodec_parameters_from_context(fParams.st->codecpar, fParams.enc_ctx);
	assert(ret == 0);

	jvxSize cnt = 1;
	std::string fnameUsed = fParams.fPrefix + "." + fParams.fSuffix;
	while (1)
	{
		int ret = -1;
		if (!JVX_FILE_EXISTS(fnameUsed.c_str()))
		{
			av_dump_format(fParams.oc, 0, fnameUsed.c_str(), 1);
			ret = avio_open(&fParams.oc->pb, fnameUsed.c_str(), AVIO_FLAG_WRITE);
		}
		if (ret < 0)
		{
			fnameUsed = fParams.fPrefix + "(" + jvx_size2String(cnt) + ")." + fParams.fSuffix;
			cnt++;
		}
		else
		{
			break;
		}
	}

	ret = avformat_write_header(fParams.oc, nullptr);
	assert(ret == 0);
}

void
CjvxAudioFFMpegWriterDevice::writer_deallocate_core()
{
	av_write_trailer(fParams.oc);
	avio_close(fParams.oc->pb);
}