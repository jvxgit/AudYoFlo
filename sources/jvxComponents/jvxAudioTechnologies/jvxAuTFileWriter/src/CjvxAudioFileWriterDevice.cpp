#include "CjvxAudioFileWriterDevice.h"
#include "CjvxAudioFileWriterTechnology.h"
#include "jvx-helpers-cpp.h"
#include "jvx_audiocodec_helpers.h"


CjvxAudioFileWriterDevice::CjvxAudioFileWriterDevice(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE) :
		CjvxAudioDevice(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
{
	_common_set.theObjectSpecialization = reinterpret_cast<jvxHandle*>(static_cast<IjvxDevice*>(this));
	_common_set_audio_device.formats.push_back(JVX_DATAFORMAT_BYTE);
	
	_common_set_properties.moduleReference = _common_set.theModuleName;
	//_config.reportMissedCallbacks = false;

	_common_set.thisisme = static_cast<IjvxObject*>(this);
}


CjvxAudioFileWriterDevice::~CjvxAudioFileWriterDevice()
{
}

// =============================================================================


jvxErrorType
CjvxAudioFileWriterDevice::init_parameters(
	const std::string& folder,
	const std::string& prefix,
	jvxAudioFileWriteFiletype fType,
	jvxSize fSType,
	wav_params* wav_init_params_arg,
	const std::string& cfg_compact,
	CjvxAudioFileWriterTechnology* par)
{
	jvxErrorType res = JVX_NO_ERROR;
	
	if (JVX_DIRECTORY_EXISTS(folder.c_str()) == c_true)
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
		parentTech = par;
		config_compact = cfg_compact;
		if (wav_init_params_arg)
		{
			wav_params* params_prop_wav = (wav_params*)wav_init_params_arg;
			file_params = *params_prop_wav;

			genFileWriter_device::translate__wav_properties__sample_type_to(file_params.sample_type);
			genFileWriter_device::translate__wav_properties__sample_resolution_to(file_params.resolution);
			genFileWriter_device::translate__wav_properties__endian_to(file_params.endian);
			genFileWriter_device::translate__wav_properties__sub_type_to(file_params.sub_type);
		}
	}

	_common_set_min.theOwner = static_cast<IjvxObject*>(parentTech);
	return res;
}

jvxErrorType
CjvxAudioFileWriterDevice::term_file()
{
	jvxErrorType res = JVX_NO_ERROR;
	
	foldername = "";
	fileprefix = "";

	return res;
}

std::string 
CjvxAudioFileWriterDevice::get_last_error()
{
	return last_error;
}

// =============================================================================
// =============================================================================

jvxErrorType
CjvxAudioFileWriterDevice::select(IjvxObject* owner)
{
	jvxErrorType res = CjvxAudioDevice::select(owner);
	if (res == JVX_NO_ERROR)
	{		
			
	}
	return (res);
}


jvxErrorType
CjvxAudioFileWriterDevice::unselect()
{
	jvxErrorType res = CjvxAudioDevice::unselect();
	if (res == JVX_NO_ERROR)
	{
	}
	return (res);
}

jvxErrorType
CjvxAudioFileWriterDevice::activate()
{
	jvxErrorType res = CjvxAudioDevice::activate();
	if (res == JVX_NO_ERROR)
	{
		genFileWriter_device::init_all();
		genFileWriter_device::allocate_all();
		genFileWriter_device::register_all(static_cast<CjvxProperties*>(this));
		genFileWriter_device::register_callbacks(
			static_cast<CjvxProperties*>(this),
			set_wav_parameters,
			reinterpret_cast<jvxHandle*>(this),
			nullptr);

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
CjvxAudioFileWriterDevice::deactivate()
{
	jvxErrorType res = _pre_check_deactivate();
	if (res == JVX_NO_ERROR)
	{
		genFileWriter_device::unregister_callbacks(
			static_cast<CjvxProperties*>(this),
			nullptr);
		genFileWriter_device::unregister_all(static_cast<CjvxProperties*>(this));
		genFileWriter_device::deallocate_all();
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
CjvxAudioFileWriterDevice::prepare()
{
	jvxErrorType res = CjvxAudioDevice::prepare();

	if (res == JVX_NO_ERROR)
	{        		
	}
	return (res);
}


jvxErrorType
CjvxAudioFileWriterDevice::postprocess()
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
CjvxAudioFileWriterDevice::put_configuration(jvxCallManagerConfiguration* callConf, IjvxConfigProcessor* processor,
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
CjvxAudioFileWriterDevice::get_configuration(jvxCallManagerConfiguration* callConf, IjvxConfigProcessor* processor, jvxHandle* sectionWhereToAddAllSubsections)
{
	// Write all parameters from base class
    CjvxAudioDevice::get_configuration(callConf, processor, sectionWhereToAddAllSubsections);

	return (JVX_NO_ERROR);
}

jvxErrorType 
CjvxAudioFileWriterDevice::test_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res =  CjvxAudioDevice::test_chain_master(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	if (res == JVX_ERROR_REQUEST_CALL_AGAIN)
	{
		res = CjvxAudioDevice::test_chain_master(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	}
	return res;
}

jvxErrorType
CjvxAudioFileWriterDevice::test_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_ERROR_INVALID_ARGUMENT;
	std::string config_token;
	wav_params passed_props;
	jvxSize i;
	// Here we detect the provided parameters. We may setup another setup
	if (_common_set_ldslave.theData_in->con_params.format_spec)
	{
		res = JVX_NO_ERROR;
		config_token = _common_set_ldslave.theData_in->con_params.format_spec->std_str();

		jvxErrorType resL = jvx_wav_configtoken_2_values(
			config_token.c_str(),
			&passed_props);

		file_params.bsize = passed_props.bsize;
		file_params.srate = passed_props.srate;
		file_params.nchans = passed_props.nchans;

		if (
			(file_params.endian != passed_props.endian) ||
			(file_params.resolution != passed_props.resolution) ||
			(file_params.sample_type != passed_props.sample_type) ||
			(file_params.sub_type != passed_props.sub_type))
		{
			std::string propstring = jvx_wav_produce_codec_token(&file_params);
			jvxApiString astr;
			astr.assign(propstring);
			jvxLinkDataDescriptor ld_params;
			ld_params.con_params.format_spec = &astr;
			ld_params.con_params.buffersize = file_params.fsizemax;
			ld_params.con_params.number_channels = 1;
			ld_params.con_params.format = JVX_DATAFORMAT_BYTE;
			ld_params.con_params.caps.format_group = JVX_DATAFORMAT_GROUP_AUDIO_CODED_GENERIC;
			ld_params.con_params.segmentation_x = ld_params.con_params.buffersize;
			ld_params.con_params.segmentation_y = 1;
			res = _common_set_ldslave.theData_in->con_link.connect_from->transfer_backward_ocon(
				JVX_LINKDATA_TRANSFER_COMPLAIN_DATA_SETTINGS,
				&ld_params
				JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
		}
		if (res == JVX_NO_ERROR)
		{
			CjvxAudioDevice_genpcg::properties_active.buffersize.value = file_params.bsize;
			CjvxAudioDevice_genpcg::properties_active.samplerate.value = file_params.srate;
			CjvxAudioDevice_genpcg::properties_active.inputchannelselection.value.entries.clear();
			CjvxAudioDevice_genpcg::properties_active.outputchannelselection.value.entries.clear();
			jvx_bitFClear(CjvxAudioDevice_genpcg::properties_active.outputchannelselection.value.selection());
			for (i = 0; i < file_params.nchans; i++)
			{
				CjvxAudioDevice_genpcg::properties_active.outputchannelselection.value.entries.push_back(
					"Channel #" + jvx_size2String(i));
				jvx_bitSet(CjvxAudioDevice_genpcg::properties_active.outputchannelselection.value.selection(), i);
			}

			updateDependentVariables_lock(-1,
				JVX_PROPERTY_CATEGORY_PREDEFINED, true,
				JVX_PROPERTY_CALL_PURPOSE_NONE_SPECIFIC,
				true);

		}
	}
	return res;
}

/*
jvxSize 
CjvxAudioFileWriterDevice::compute_bsize_pcm(jvxSize nchans, jvxSize bitssample, jvxSize nomBsize)
{
	return (nchans * bitssample * nomBsize / 8);

}
*/

void
CjvxAudioFileWriterDevice::test_set_output_parameters()
{
	// _common_set
	
	// We need to make some specific adaptations here which can not be 
	// made in the standard implementation

	// The format text specification to configure the decoder
	// The buffersize as it is derived from the buffersize as set by the audio framing definition
	// You can specify the buffersize by setting the standard parameter for the device buffersize
	_common_set_ldslave.theData_out.con_params.buffersize = 0;
	_common_set_ldslave.theData_out.con_params.rate = 0;
	_common_set_ldslave.theData_out.con_params.number_channels = 0;
	_common_set_ldslave.theData_out.con_params.format = JVX_DATAFORMAT_NONE;
	_common_set_ldslave.theData_out.con_params.segmentation_x = 0;
	_common_set_ldslave.theData_out.con_params.segmentation_y = 0;

	// The only purpose is to trigger the output side
	_common_set_ldslave.theData_out.con_params.caps.format_group = JVX_DATAFORMAT_GROUP_TRIGGER_ONLY;
}

/*
std::string
CjvxAudioFileWriterDevice::produce_decoder_token()
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
	params.fsizemax = _common_set_ldslave.theData_out.con_params.buffersize;
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
CjvxAudioFileWriterDevice::prepare_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
    jvxErrorType res = JVX_NO_ERROR;

    // Auto-run the prepare function
	res = prepare_chain_master_autostate(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	assert(res == JVX_NO_ERROR);

	// Prepare processing parameters
	_common_set_ldslave.theData_out.con_data.number_buffers = 1;
	jvx_bitZSet(_common_set_ldslave.theData_out.con_data.alloc_flags, 
		(int)jvxDataLinkDescriptorAllocFlags::JVX_LINKDATA_ALLOCATION_FLAGS_IS_ZEROCOPY_CHAIN_SHIFT);

	// New type of connection by propagating through linked elements
    res = _prepare_chain_master(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	if (res == JVX_NO_ERROR)
	{
		/*
		involve_read_thread = false;
		if (genFileWriter_device::exteded_properties.threaded_read.value == c_true)
		{
			jvxSize bytesonesample = fileprops_wav.nchans * fileprops_wav.bitssample / 8;
			jvxData numsamples = genFileWriter_device::exteded_properties.preuse_bsize_msecs.value * 0.001 * fileprops_wav.rate;
			jvxSize numbytes = (jvxSize)numsamples * bytesonesample;
			jvxSize numbuffers = ceil((jvxData)numbytes / (jvxData)_common_set_ldslave.theData_out.con_params.buffersize);

			preuse_buffer_sz = JVX_MAX(1, numbuffers) * _common_set_ldslave.theData_out.con_params.buffersize;
			JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(preuse_buffer_ptr, jvxByte, preuse_buffer_sz);
			readposi = 0;
			fHeight = 0;
			readsize = _common_set_ldslave.theData_out.con_params.buffersize;
			bufstatus = jvxAudioFileWriterBufferStatus::JVX_BUFFER_STATUS_NONE;

			involve_read_thread = true;
		}
		*/

		// All parameters were set before, only very few need update
		_common_set_ldslave.theData_out.con_data.number_buffers = 1;

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
CjvxAudioFileWriterDevice::postprocess_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
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
CjvxAudioFileWriterDevice::start_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
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
CjvxAudioFileWriterDevice::stop_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
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
CjvxAudioFileWriterDevice::prepare_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;
	// This is the return from the link list

	// Connect the output side and start this link
	res = allocate_pipeline_and_buffers_prepare_to();

	// Do not attach any user hint into backward direction
	_common_set_ldslave.theData_in->con_compat.user_hints = NULL;
	// _common_set_ldslave.theData_in->pipeline.idx_stage = 0;

	open_wav_file_for_writing();
	return res;
}

jvxErrorType
CjvxAudioFileWriterDevice::postprocess_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;

	close_wav_file_for_writing();

	res = deallocate_pipeline_and_buffers_postprocess_to();

	return res;
}

jvxErrorType
CjvxAudioFileWriterDevice::process_buffers_icon(jvxSize mt_mask, jvxSize idx_stage)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize num_written = 0;
	// This is the return from the link list
	jvxByte** bufsByte = jvx_process_icon_extract_input_buffers<jvxByte>(_common_set_ldslave.theData_in, idx_stage);
	wavFileWriter.write_one_buf_raw(bufsByte[0], _common_set_ldslave.theData_in->con_params.buffersize, &num_written);
	return res;
}

jvxErrorType
CjvxAudioFileWriterDevice::process_start_icon(jvxSize pipeline_offset, jvxSize* idx_stage,
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
CjvxAudioFileWriterDevice::process_stop_icon(jvxSize idx_stage, jvxBool shift_fwd,
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
CjvxAudioFileWriterDevice::transfer_backward_ocon(jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxLinkDataDescriptor* ld_cp = nullptr;
	jvxErrorType res = JVX_NO_ERROR;
	jvxMasterSourceType* trigTp = nullptr;

	switch (tp)
	{
	case JVX_LINKDATA_TRANSFER_REQUEST_DATA:
		trigger_one_buffer();	
		break;
	case JVX_LINKDATA_TRANSFER_COMPLAIN_DATA_SETTINGS:

		ld_cp = (jvxLinkDataDescriptor*)data;
		// Do something to select another codec setting
		CjvxAudioDevice_genpcg::properties_active.buffersize.value = 1024; // ld_cp->con_params.buffersize / (fileprops_wav.bitssample * fileprops_wav.nchans / 8);
		_common_set_ldslave.theData_out.con_params.buffersize = CjvxAudioDevice_genpcg::properties_active.buffersize.value;
		_common_set_ldslave.theData_out.con_params.buffersize = 1024;
			//compute_bsize_pcm(fileprops_wav.nchans, fileprops_wav.bitssample, _common_set_ldslave.theData_out.con_params.buffersize);
		_common_set_ldslave.theData_out.con_params.segmentation_x = _common_set_ldslave.theData_out.con_params.buffersize;
		// format_descriptor.assign(produce_decoder_token());
		return JVX_NO_ERROR;
	case JVX_LINKDATA_TRANSFER_REQUEST_TRIGGER_TYPE:

		// Indicate that master requires a "trigger" to output data
		trigTp = (jvxMasterSourceType*)data;
		if (trigTp)
		{
			*trigTp = jvxMasterSourceType::JVX_MASTER_SOURCE_EXTERNAL_TRIGGER;
		}
		res = JVX_NO_ERROR;
	}
	return res;
}

jvxErrorType 
CjvxAudioFileWriterDevice::set_property(jvxCallManagerProperties& callGate,
	const jvx::propertyRawPointerType::IjvxRawPointerType& rawPtr,
	const jvx::propertyAddress::IjvxPropertyAddress& ident,
	const jvx::propertyDetail::CjvxTranferDetail& trans)
{
	return CjvxAudioDevice::set_property(callGate,
		rawPtr, ident, trans);
}

void
CjvxAudioFileWriterDevice::trigger_one_buffer()
{
	jvxErrorType res = JVX_NO_ERROR;
	if (_common_set_ldslave.theData_out.con_link.connect_to)
	{
		res = _common_set_ldslave.theData_out.con_link.connect_to->process_start_icon();
		if (res == JVX_NO_ERROR)
		{
			// Prepare one frame here!

			_common_set_ldslave.theData_out.con_link.connect_to->process_buffers_icon();
			_common_set_ldslave.theData_out.con_link.connect_to->process_stop_icon();
		}
	}
}

/*
JVX_PROPERTIES_FORWARD_C_CALLBACK_EXECUTE_FULL(CjvxAudioWindowsDevice, set_new_rate)
{
	return JVX_NO_ERROR;
}
*/

#if 0
void
CjvxAudioFileWriterDevice::send_buffer_thread()
{
	// Copy the content into the buffers
	jvxSize bufIdx = *_common_set_ldslave.theData_out.con_pipeline.idx_stage_ptr;
	jvxBool requires_new_data = false;
	jvxByte** bufsOut = jvx_process_icon_extract_output_buffers<jvxByte>(
		&_common_set_ldslave.theData_out);
	if (bufsOut)
	{
		JVX_TRY_LOCK_MUTEX_RESULT_TYPE resLock = JVX_TRY_LOCK_MUTEX_NO_SUCCESS;
		JVX_TRY_LOCK_MUTEX(resLock, safeAccessRead);
		if (resLock == JVX_TRY_LOCK_MUTEX_SUCCESS)
		{
			if (
				(bufstatus == jvxAudioFileWriterBufferStatus::JVX_BUFFER_STATUS_OPERATION) ||
				(bufstatus == jvxAudioFileWriterBufferStatus::JVX_BUFFER_STATUS_COMPLETE))
			{
				jvxSize copymax = _common_set_ldslave.theData_out.con_params.buffersize;
				jvxByte* ptr_to = (jvxByte*)bufsOut[0];

				// Here it is only a read to the fHeight, therefore no lock
				while (copymax && fHeight)
				{
					jvxSize l1 = JVX_MIN(copymax, (preuse_buffer_sz - readposi));
					l1 = JVX_MIN(l1, fHeight);
					jvxByte* ptr_from = preuse_buffer_ptr + readposi; // <- read only access, readposi only in use by this function, not in parallel thread
					memcpy(ptr_to, ptr_from, l1);
					ptr_to += l1;
					copymax -= l1;

					// readposi and fHeight must be consistent, therefore we must lock
					JVX_LOCK_MUTEX(safeAccessBuffer);
					readposi = (readposi + l1) % preuse_buffer_sz;
					fHeight -= l1;
					JVX_UNLOCK_MUTEX(safeAccessBuffer);
					requires_new_data = true;
				}
				_common_set_ldslave.theData_out.con_params.fHeight_x =
					_common_set_ldslave.theData_out.con_params.buffersize - copymax;

				if (fHeight == 0)
				{
					// From operation we may either switch to CHARGING or to COMPLETE in different threads
					// Therefore a lock is required
					JVX_LOCK_MUTEX(safeAccessBuffer);
					if (bufstatus == jvxAudioFileWriterBufferStatus::JVX_BUFFER_STATUS_OPERATION)
					{
						genFileWriter_device::monitor.num_lost.value++;
						bufstatus = jvxAudioFileWriterBufferStatus::JVX_BUFFER_STATUS_CHARGING;
					}
					JVX_UNLOCK_MUTEX(safeAccessBuffer);
				}

				if (requires_new_data)
				{
					refThreads.cpPtr->trigger_wakeup();
				}
			}
			else
			{
				// e.g. if bufstatus == jvxAudioFileReaderBufferStatus::JVX_BUFFER_STATUS_NONE
				genFileWriter_device::monitor.num_lost.value++;
				_common_set_ldslave.theData_out.con_params.fHeight_x = 0;
			}
			JVX_UNLOCK_MUTEX(safeAccessRead);
		}
		else
		{
			// If we could not acquire the look, send zero data
			_common_set_ldslave.theData_out.con_params.fHeight_x = 0;
		}
	}
}


void
CjvxAudioFileWriterDevice::send_buffer_direct()
{
	jvxSize bufIdx = *_common_set_ldslave.theData_out.con_pipeline.idx_stage_ptr;
	jvxBool requires_new_data = false;
	jvxByte** bufsOut = jvx_process_icon_extract_output_buffers<jvxByte>(
		&_common_set_ldslave.theData_out);
	if (bufsOut)
	{
		jvxSize copymax = _common_set_ldslave.theData_out.con_params.buffersize;
		jvxByte* ptr_to = (jvxByte*)bufsOut[0];
		jvxSize numcopy = copymax;
		jvxSize num_copied = 0;
		jvxErrorType resRead = wavFileReader.read_one_buf_raw(ptr_to, numcopy, &num_copied);
		_common_set_ldslave.theData_out.con_params.fHeight_x = num_copied;

		jvxSize progress = 0;
		wavFileReader.current_progress(&progress);
		genFileWriter_device::monitor.progress_percent.value = ((jvxData)progress / (jvxData)fileprops_wav.l_samples * 100.0);
	}
}

jvxErrorType 
CjvxAudioFileWriterDevice::startup(jvxInt64 timestamp_us)
{
	read_samples_to_buffer();
	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxAudioFileWriterDevice::expired(jvxInt64 timestamp_us, jvxSize* delta_ms)
{
	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxAudioFileWriterDevice::wokeup(jvxInt64 timestamp_us, jvxSize* delta_ms)
{
	read_samples_to_buffer();
	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxAudioFileWriterDevice::stopped(jvxInt64 timestamp_us)
{
	return JVX_NO_ERROR;
}

void
CjvxAudioFileWriterDevice::read_samples_to_buffer()
{
	jvxSize readposil = 0;
	jvxSize fheightl = 0;
	jvxBool stayInLoop = true;

	while (stayInLoop)
	{
		if (bufstatus == jvxAudioFileWriterBufferStatus::JVX_BUFFER_STATUS_COMPLETE)
		{
			break;
		}

		// This must be put into a lock in case we trigger a rewind
		JVX_LOCK_MUTEX(safeAccessRead);

		// Get a buffer status update
		JVX_LOCK_MUTEX(safeAccessBuffer);
		readposil = readposi;
		fheightl = fHeight;
		JVX_UNLOCK_MUTEX(safeAccessBuffer);

		jvxSize szbuffer = preuse_buffer_sz - fheightl;
		if (szbuffer == 0)
		{
			stayInLoop = false;
		}
		else
		{
			jvxSize writeposi = (readposil + fHeight) % preuse_buffer_sz;
			jvxSize sztoend = preuse_buffer_sz - writeposi;
			jvxSize numcopy = JVX_MIN(sztoend, szbuffer);
			jvxSize num_copied = 0;
			numcopy = JVX_MIN(numcopy, readsize);
			jvxByte* ptrread = preuse_buffer_ptr + writeposi;

			jvxErrorType resRead = wavFileReader.read_one_buf_raw(ptrread, numcopy, &num_copied);

			JVX_LOCK_MUTEX(safeAccessBuffer);
			fHeight += num_copied;
			switch (bufstatus)
			{
			case jvxAudioFileWriterBufferStatus::JVX_BUFFER_STATUS_NONE:
				bufstatus = jvxAudioFileWriterBufferStatus::JVX_BUFFER_STATUS_CHARGING;
				// No break here to fall through

			case jvxAudioFileWriterBufferStatus::JVX_BUFFER_STATUS_CHARGING:

				if (fHeight > preuse_buffer_sz / 2)
				{
					bufstatus = jvxAudioFileWriterBufferStatus::JVX_BUFFER_STATUS_OPERATION;
				}
				break;
			}
			if (resRead != JVX_NO_ERROR)
			{
				bufstatus = jvxAudioFileWriterBufferStatus::JVX_BUFFER_STATUS_COMPLETE;
			}
			JVX_UNLOCK_MUTEX(safeAccessBuffer);
		}
		JVX_UNLOCK_MUTEX(safeAccessRead);

	}
	jvxSize progress = 0;
	wavFileReader.current_progress(&progress);
	genFileWriter_device::monitor.progress_percent.value = ((jvxData)progress / (jvxData)fileprops_wav.l_samples * 100.0);
}
#endif

jvxErrorType
CjvxAudioFileWriterDevice::open_wav_file_for_writing()
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize cnt = 0;
	std::string fNamePrefix = fileprefix;
	std::string fName;
		int errCode = 0;
		jvxFileFormat ff = JVX_FILEFORMAT_PCM_FIXED;
 jvxSize numBitsSample = 16;
 jvxInt32 rate = file_params.srate;
	jvxSize sub_type = JVX_WAV_32;
	
	// To do: Create filename by replacing specific tokens..
	// fNamePrefix = jvx_replaceStrInStr(...);

	while (1)
	{
		fName = jvx_makePathExpr(foldername, fNamePrefix, false, false);
		if (cnt > 0)
		{
			fName += "_";
			fName += jvx_size2String(cnt);
		}
		fName += ".wav";
		if (JVX_FILE_EXISTS(fName.c_str()))
		{
			cnt++;
		}
		else
		{
			break;
		}
	}

	res = wavFileWriter.select(fName);
	if (res != JVX_NO_ERROR)
	{
		goto exit_fail_I;
	}

	
	if (file_params.sample_type == audio_codec_wav_sample_type::AUDIO_CODEC_WAV_FLOATING_POINT)
	{
		ff = JVX_FILEFORMAT_IEEE_FLOAT;
	}
	
	switch (file_params.resolution)
	{
	case audio_codec_wav_resolution::AUDIO_CODEC_WAV_RESOLUTION_24BIT:
		numBitsSample = 24;
		break;
	case audio_codec_wav_resolution::AUDIO_CODEC_WAV_RESOLUTION_32BIT:
		numBitsSample = 32;
		break;
	case audio_codec_wav_resolution::AUDIO_CODEC_WAV_RESOLUTION_64BIT:
		numBitsSample = 64;
		break;
	}
	
	switch (file_params.sub_type)
	{
	case audio_codec_wav_sub_type::AUDIO_CODEC_WAV_SUBTYPE_WAV_64:
		sub_type = JVX_WAV_64;
		break;
	case audio_codec_wav_sub_type::AUDIO_CODEC_WAV_SUBTYPE_WAV_64_SONY:
		sub_type = JVX_WAV_64_SONY;
		break;
	}

	res = wavFileWriter.activate(errCode);
	assert(res == JVX_NO_ERROR);

	res = wavFileWriter.set_file_properties(
		&file_params.nchans,
		&rate,
		&ff,
		&numBitsSample,
		&sub_type);
	assert(res == JVX_NO_ERROR);

	res = wavFileWriter.prepare(JVX_SIZE_UNSELECTED);
	assert(res == JVX_NO_ERROR);

	res = wavFileWriter.start();
	assert(res == JVX_NO_ERROR);

exit_fail_I:
	return res;
}

jvxErrorType
CjvxAudioFileWriterDevice::close_wav_file_for_writing()
{
	jvxErrorType res = JVX_NO_ERROR;

	res = wavFileWriter.stop();
	assert(res == JVX_NO_ERROR);

	res = wavFileWriter.postprocess();
	assert(res == JVX_NO_ERROR);

	res = wavFileWriter.deactivate();
	assert(res == JVX_NO_ERROR);

	res = wavFileWriter.unselect();
	assert(res == JVX_NO_ERROR);

	return res;
}

JVX_PROPERTIES_FORWARD_C_CALLBACK_EXECUTE_FULL(CjvxAudioFileWriterDevice, set_wav_parameters)
{
	if (
		JVX_PROPERTY_CHECK_ID_CAT_SIMPLE(genFileWriter_device::wav_properties.endian) ||
		JVX_PROPERTY_CHECK_ID_CAT_SIMPLE(genFileWriter_device::wav_properties.sample_type) ||
		JVX_PROPERTY_CHECK_ID_CAT_SIMPLE(genFileWriter_device::wav_properties.sample_resolution) ||
		JVX_PROPERTY_CHECK_ID_CAT_SIMPLE(genFileWriter_device::wav_properties.sub_type))
	{
		file_params.endian = genFileWriter_device::translate__wav_properties__endian_from();
		file_params.sample_type = genFileWriter_device::translate__wav_properties__sample_type_from();
		file_params.resolution = genFileWriter_device::translate__wav_properties__sample_resolution_from();
		file_params.sub_type = genFileWriter_device::translate__wav_properties__sub_type_from();

		if (_common_set_ld_master.refProc)
		{
			jvxSize uId = JVX_SIZE_UNSELECTED;
			_common_set_ld_master.refProc->unique_id_connections(&uId);
			this->_request_test_chain_master(uId);
		}
	}
	return JVX_NO_ERROR;
}
