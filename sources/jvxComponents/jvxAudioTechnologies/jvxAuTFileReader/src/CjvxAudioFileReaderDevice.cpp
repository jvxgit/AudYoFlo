#include "CjvxAudioFileReaderDevice.h"
#include "CjvxAudioFileReaderTechnology.h"
#include "jvx-helpers-cpp.h"


CjvxAudioFileReaderDevice::CjvxAudioFileReaderDevice(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE) :
		CjvxAudioDevice(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
{
	_common_set.theObjectSpecialization = reinterpret_cast<jvxHandle*>(static_cast<IjvxDevice*>(this));
	_common_set_audio_device.formats.push_back(JVX_DATAFORMAT_BYTE);
	
	_common_set_properties.moduleReference = _common_set.theModuleName;
	//_config.reportMissedCallbacks = false;

	_common_set.thisisme = static_cast<IjvxObject*>(this);
	JVX_INITIALIZE_MUTEX(safeAccessRead);
	reset_props();
}


CjvxAudioFileReaderDevice::~CjvxAudioFileReaderDevice()
{
	JVX_TERMINATE_MUTEX(safeAccessRead);
}

// =============================================================================

void
CjvxAudioFileReaderDevice::reset_props()
{
	/*
	fileprops_wav.nchans = 0;
	fileprops_wav.l_samples = 0;
	fileprops_wav.rate = 0;
	fileprops_wav.fformat = JVX_FILEFORMAT_NONE;
	fileprops_wav.lendian = true;
	fileprops_wav.bitssample = 16;
	fileprops_wav.subtype = 0;
	*/
}

jvxErrorType
CjvxAudioFileReaderDevice::init_from_filename(const std::string& fnameArg, CjvxAudioFileReaderTechnology* par)
{
	jvxErrorType res = JVX_NO_ERROR;

	parentTech = par;
	fname = fnameArg;

	// Try the wav file reader
	res = wavFileReader.select(fname);
	if (res == JVX_NO_ERROR)
	{
		int errCode = 0;
		res = wavFileReader.activate(errCode);

		if (res == JVX_NO_ERROR)
		{
			jvxSize nchans = 0;
			jvxInt32 rate = 0;
			jvxFileFormat fformat = JVX_FILEFORMAT_NONE;
			jvxBool lendian = true;
			jvxSize bitssample = 16;
			jvxSize subtype = 0;
			l_samples = 0;

			wavFileReader.get_file_properties(
				&nchans,
				&l_samples,
				&rate,
				&fformat,
				&lendian,
				&bitssample,
				&subtype);

			file_params.srate = rate;
			//bsize = JVX_SIZE_UNSELECTED;
			// file_params.fsizemax
			file_params.nchans = nchans;
			if (lendian)
			{
				file_params.endian = audio_codec_wav_endian::AUDIO_CODEC_WAV_LITTLE_ENDIAN;
			}
			else
			{
				file_params.endian = audio_codec_wav_endian::AUDIO_CODEC_WAV_BIG_ENDIAN;
			}
			switch (bitssample)
			{
			case 16:
				file_params.resolution = audio_codec_wav_resolution::AUDIO_CODEC_WAV_RESOLUTION_16BIT;
				break;
			case 24:
				file_params.resolution = audio_codec_wav_resolution::AUDIO_CODEC_WAV_RESOLUTION_24BIT;
				break;
			case 32:
				file_params.resolution = audio_codec_wav_resolution::AUDIO_CODEC_WAV_RESOLUTION_32BIT;
				break;
			case 64:
				file_params.resolution = audio_codec_wav_resolution::AUDIO_CODEC_WAV_RESOLUTION_64BIT;
				break;
			default:
				assert(0);
			}

			switch (fformat)
			{
			case JVX_FILEFORMAT_PCM_FIXED:
				file_params.sample_type = audio_codec_wav_sample_type::AUDIO_CODEC_WAV_FIXED_POINT;
				break;

			case JVX_FILEFORMAT_IEEE_FLOAT:
				file_params.sample_type = audio_codec_wav_sample_type::AUDIO_CODEC_WAV_FLOATING_POINT;
				break;

			default:
				assert(0);
			}

			switch (subtype)
			{
			case JVX_WAV_64_SONY:

				file_params.sub_type = audio_codec_wav_sub_type::AUDIO_CODEC_WAV_SUBTYPE_WAV_64_SONY;
				break;
			case JVX_WAV_64:

				file_params.sub_type = audio_codec_wav_sub_type::AUDIO_CODEC_WAV_SUBTYPE_WAV_64;
				break;
			case JVX_WAV_32:

				file_params.sub_type = audio_codec_wav_sub_type::AUDIO_CODEC_WAV_SUBTYPE_WAV_32;
				break;
			default: 
				assert(0);
			}

			/*
			 * Test the wav file reader here
			wavFileReader.prepare(256);
			wavFileReader.start();
			jvxByte buf[1024];
			while (1)
			{
				jvxErrorType res = wavFileReader.read_one_buf_raw(buf, 1024);
				if (res != JVX_NO_ERROR)
				{
					break;
				}
			}
			*/
		}
		else
		{
			wavFileReader.unselect();
		}
	}
	else
	{
		std::cout << "Failed to open file <" << fnameArg << ">, reason: " << std::endl;
		// wavFileReader.
	}

	return res;
}

jvxErrorType
CjvxAudioFileReaderDevice::term_file()
{
	jvxErrorType res = wavFileReader.deactivate();
	assert(res == JVX_NO_ERROR);

	res = wavFileReader.unselect();
	assert(res == JVX_NO_ERROR);

	reset_props();

	return res;
}

std::string 
CjvxAudioFileReaderDevice::get_last_error()
{
	return last_error;
}

// =============================================================================
// =============================================================================

jvxErrorType
CjvxAudioFileReaderDevice::select(IjvxObject* owner)
{
	jvxErrorType res = CjvxAudioDevice::select(owner);
	if (res == JVX_NO_ERROR)
	{		
	}
	return (res);
}


jvxErrorType
CjvxAudioFileReaderDevice::unselect()
{
	jvxErrorType res = CjvxAudioDevice::unselect();
	if (res == JVX_NO_ERROR)
	{
	}
	return (res);
}

jvxErrorType
CjvxAudioFileReaderDevice::activate()
{
	jvxSize i,j;

	jvxErrorType res = CjvxAudioDevice::activate();
	if (res == JVX_NO_ERROR)
	{
		genFileReader_device::init_all();
		genFileReader_device::allocate_all();
		genFileReader_device::register_all(static_cast<CjvxProperties*>(this));
		genFileReader_device::register_callbacks(
			static_cast<CjvxProperties*>(this),
			set_config, 
			trigger_command,
			reinterpret_cast<jvxHandle*>(this),
			nullptr);

		// An audio device that provides data has input channels. Nevertheless, the 
		// input channels are forwarded as output channels towards the next chain element
		//  
		// On a filereader the channel number is related to the input and will be forwarded 
		// on the output side during processing
		// 
		// We show the audio properties here, not the bitstream parameters which are different
		// 
		CjvxAudioDevice::properties_active.inputchannelselection.value.entries.clear();
		jvx_bitFClear(CjvxAudioDevice::properties_active.inputchannelselection.value.selection());
		for (i = 0; i < file_params.nchans; i++)
		{
			CjvxAudioDevice::properties_active.inputchannelselection.value.entries.push_back("In #" + jvx_size2String(i));
			jvx_bitSet(CjvxAudioDevice::properties_active.inputchannelselection.value.selection(), i);
		}
		CjvxAudioDevice::properties_active.samplerate.value = file_params.srate;
		
		CjvxAudioDevice::properties_active.outputchannelselection.value.entries.clear();
		jvx_bitFClear(CjvxAudioDevice::properties_active.outputchannelselection.value.selection());

		this->updateDependentVariables_lock(-1, JVX_PROPERTY_CATEGORY_PREDEFINED, true);

		genFileReader_device::wav_properties.file_length.value = l_samples;

		genFileReader_device::translate__wav_properties__sample_type_to(file_params.sample_type);
		genFileReader_device::translate__wav_properties__sample_resolution_to(file_params.resolution);
		genFileReader_device::translate__wav_properties__endian_to(file_params.endian);
		genFileReader_device::translate__wav_properties__sub_type_to(file_params.sub_type);
		file_params.bsize = CjvxAudioDevice::properties_active.buffersize.value;
		file_params.fsizemax = jvx_wav_compute_bsize_bytes_pcm(&file_params, JVX_SIZE_UNSELECTED);

		// ==============================================================================

		_update_property_access_type(JVX_PROPERTY_ACCESS_READ_ONLY, CjvxAudioDevice::properties_active.outputchannelselection);
		_update_property_access_type(JVX_PROPERTY_ACCESS_READ_ONLY, CjvxAudioDevice::properties_active.inputchannelselection);
		_update_property_access_type(JVX_PROPERTY_ACCESS_READ_ONLY, CjvxAudioDevice::properties_active.samplerate);

		CjvxAudioDevice::properties_active.sourceName.value = jvx_shortenStringName(32, fname);
		// ==============================================================================
		// 
		wavFileReader.set_loop(genFileReader_device::config.loop.value == c_true);

    }
	return (res);
}

jvxErrorType
CjvxAudioFileReaderDevice::deactivate()
{
	jvxErrorType res = _pre_check_deactivate();
	if (res == JVX_NO_ERROR)
	{
		_undo_update_property_access_type(CjvxAudioDevice::properties_active.outputchannelselection);
		_undo_update_property_access_type(CjvxAudioDevice::properties_active.inputchannelselection);
		_undo_update_property_access_type(CjvxAudioDevice::properties_active.samplerate);

		genFileReader_device::unregister_callbacks(
			static_cast<CjvxProperties*>(this),
			nullptr);
		genFileReader_device::unregister_all(static_cast<CjvxProperties*>(this));
		genFileReader_device::deallocate_all();
		res = CjvxAudioDevice::deactivate();


	}
	return (res);
}

// ===============================================================================

jvxErrorType
CjvxAudioFileReaderDevice::prepare()
{
	jvxSize i;
	jvxErrorType res = CjvxAudioDevice::prepare();
	jvxErrorType resL = JVX_NO_ERROR;

	if (res == JVX_NO_ERROR)
	{        
	}
	return (res);
}


jvxErrorType
CjvxAudioFileReaderDevice::postprocess()
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
CjvxAudioFileReaderDevice::put_configuration(jvxCallManagerConfiguration* callConf, IjvxConfigProcessor* processor,
	jvxHandle* sectionToContainAllSubsectionsForMe, const char* filename, jvxInt32 lineno)
{
    jvxSize i;
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
CjvxAudioFileReaderDevice::get_configuration(jvxCallManagerConfiguration* callConf, IjvxConfigProcessor* processor, jvxHandle* sectionWhereToAddAllSubsections)
{
	// Write all parameters from base class
    CjvxAudioDevice::get_configuration(callConf, processor, sectionWhereToAddAllSubsections);

	return (JVX_NO_ERROR);
}

void
CjvxAudioFileReaderDevice::updateChainOutputParameter()
{	
	CjvxAudioDevice::updateChainOutputParameter();

	reconfigure_bsize(_common_set_ocon.theData_out.con_params.buffersize);
}

jvxErrorType 
CjvxAudioFileReaderDevice::test_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res =  CjvxAudioDevice::test_chain_master(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	if (res == JVX_ERROR_REQUEST_CALL_AGAIN)
	{
		res = CjvxAudioDevice::test_chain_master(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	}
	return res;
}

jvxErrorType
CjvxAudioFileReaderDevice::test_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	// This endpoint not necessarily is required. We should not use it.
	return JVX_ERROR_UNSUPPORTED;
}

void
CjvxAudioFileReaderDevice::test_set_output_parameters()
{
	// The buffersize as it is derived from the buffersize as set by the audio framing definition
	// You can specify the buffersize by setting the standard parameter for the device buffersize
	_common_set_ocon.theData_out.con_params.number_channels = 1; // Byte field is always 1 channel
	_common_set_ocon.theData_out.con_params.rate = JVX_SIZE_DONTCARE; // This indicates that the rate is in the format_spec
	_common_set_ocon.theData_out.con_params.buffersize =
		jvx_wav_compute_bsize_bytes_pcm(&file_params, JVX_SIZE_UNSELECTED);
	
	// Type is modified as the data is coded
	_common_set_ocon.theData_out.con_params.format = JVX_DATAFORMAT_BYTE;
	_common_set_ocon.theData_out.con_params.format_group = JVX_DATAFORMAT_GROUP_AUDIO_CODED_GENERIC;
	_common_set_ocon.theData_out.con_params.segmentation.x = _common_set_ocon.theData_out.con_params.buffersize;
	_common_set_ocon.theData_out.con_params.segmentation.y = 1;
	_common_set_ocon.theData_out.con_params.data_flow = jvxDataflow::JVX_DATAFLOW_PUSH_ON_PULL;
	_common_set_ocon.theData_out.con_params.format_spec = jvx_wav_produce_codec_token(&file_params);
}

jvxErrorType
CjvxAudioFileReaderDevice::prepare_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
    jvxErrorType res = JVX_NO_ERROR;

    // Auto-run the prepare function
	res = prepare_chain_master_autostate(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	assert(res == JVX_NO_ERROR);

	// Prepare processing parameters
	_common_set_ocon.theData_out.con_data.number_buffers = 1;
	jvx_bitZSet(_common_set_ocon.theData_out.con_data.alloc_flags, 
		(int)jvxDataLinkDescriptorAllocFlags::JVX_LINKDATA_ALLOCATION_FLAGS_IS_ZEROCOPY_CHAIN_SHIFT);

	jvx_bitSet(_common_set_ocon.theData_out.con_data.alloc_flags,
		(int)jvxDataLinkDescriptorAllocFlags::JVX_LINKDATA_ALLOCATION_FLAGS_EXPECT_FHEIGHT_INFO_SHIFT);

	// New type of connection by propagating through linked elements
    res = _prepare_chain_master(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	if (res == JVX_NO_ERROR)
	{

		// All parameters were set before, only very few need update
		_common_set_ocon.theData_out.con_data.number_buffers = 1;

		// Monitoring feedback
		genFileReader_device::monitor.progress_percent.value = 0;
		genFileReader_device::monitor.num_lost.value = 0;

		// Start the reader
		wavFileReader.prepare(JVX_SIZE_UNSELECTED); // No read limit in module
		wavFileReader.start();
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
CjvxAudioFileReaderDevice::postprocess_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxErrorType resL;


	// Stop the reader
	wavFileReader.stop();
	wavFileReader.postprocess();

	_postprocess_chain_master(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	assert(res == JVX_NO_ERROR);

	// If the chain is postprocessed, the object itself should also be postprocessed if not done so before
	resL = postprocess_chain_master_autostate(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	assert(resL == JVX_NO_ERROR);

	return res;

}

jvxErrorType
CjvxAudioFileReaderDevice::start_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
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
CjvxAudioFileReaderDevice::stop_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxErrorType resL;

	res = _stop_chain_master(NULL);
	assert(res == JVX_NO_ERROR);

	// If the chain is stopped, the object itself should also be stopped if not done so before
	resL = stop_chain_master_autostate(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	assert(resL == JVX_NO_ERROR);

	return res;
}

jvxErrorType
CjvxAudioFileReaderDevice::prepare_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;
	// This is the return from the link list

	_common_set_icon.theData_in->con_params.buffersize = _inproc.buffersize;
	_common_set_icon.theData_in->con_params.format = _inproc.format;
	_common_set_icon.theData_in->con_data.buffers = NULL;
	_common_set_icon.theData_in->con_data.number_buffers = JVX_MAX(_common_set_icon.theData_in->con_data.number_buffers, 1);
	_common_set_icon.theData_in->con_params.number_channels = _inproc.numOutputs;
	_common_set_icon.theData_in->con_params.rate = _inproc.samplerate;

	// Connect the output side and start this link
	res = allocate_pipeline_and_buffers_prepare_to();

	// Do not attach any user hint into backward direction
	_common_set_icon.theData_in->con_compat.user_hints = NULL;
	// _common_set_icon.theData_in->pipeline.idx_stage = 0;

	return res;
}

jvxErrorType
CjvxAudioFileReaderDevice::postprocess_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;

	res = deallocate_pipeline_and_buffers_postprocess_to();

	return res;
}

jvxErrorType
CjvxAudioFileReaderDevice::process_buffers_icon(jvxSize mt_mask, jvxSize idx_stage)
{
	jvxErrorType res = JVX_NO_ERROR;
	// This is the return from the link list
	return res;
}

jvxErrorType
CjvxAudioFileReaderDevice::process_start_icon(jvxSize pipeline_offset, jvxSize* idx_stage,
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
CjvxAudioFileReaderDevice::process_stop_icon(jvxSize idx_stage, jvxBool shift_fwd,
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
CjvxAudioFileReaderDevice::transfer_backward_ocon(jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxLinkDataDescriptor* ld_cp = nullptr;
	jvxErrorType res = JVX_NO_ERROR;

	switch (tp)
	{
	case JVX_LINKDATA_TRANSFER_REQUEST_DATA:

		// Here is our entry to provide new audio data!
		send_one_buffer();		
		break;
	case JVX_LINKDATA_TRANSFER_COMPLAIN_DATA_SETTINGS:

		ld_cp = (jvxLinkDataDescriptor*)data;
		// Do something to select another codec setting
		CjvxAudioDevice_genpcg::properties_active.buffersize.value = 
			jvx_wav_compute_bsize_audio(&file_params, ld_cp->con_params.buffersize);

		reconfigure_bsize(CjvxAudioDevice_genpcg::properties_active.buffersize.value);

		_common_set_ocon.theData_out.con_params.buffersize = file_params.fsizemax;
		_common_set_ocon.theData_out.con_params.segmentation.x = _common_set_ocon.theData_out.con_params.buffersize;
		
		_common_set_ocon.theData_out.con_params.format_spec = jvx_wav_produce_codec_token(&file_params);
		return JVX_NO_ERROR;
	default:
		CjvxAudioDevice::transfer_backward_ocon(tp, data JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
	}
	return res;
}

jvxErrorType 
CjvxAudioFileReaderDevice::set_property(jvxCallManagerProperties& callGate,
	const jvx::propertyRawPointerType::IjvxRawPointerType& rawPtr,
	const jvx::propertyAddress::IjvxPropertyAddress& ident,
	const jvx::propertyDetail::CjvxTranferDetail& trans)
{
	return CjvxAudioDevice::set_property(callGate,
		rawPtr, ident, trans);
}

void
CjvxAudioFileReaderDevice::send_one_buffer()
{
	jvxErrorType res = JVX_NO_ERROR;
	if (_common_set_ocon.theData_out.con_link.connect_to)
	{
		res = _common_set_ocon.theData_out.con_link.connect_to->process_start_icon();
		if (res == JVX_NO_ERROR)
		{
			send_buffer_direct();

			_common_set_ocon.theData_out.con_link.connect_to->process_buffers_icon();
			_common_set_ocon.theData_out.con_link.connect_to->process_stop_icon();
		}
	}
}
/*
JVX_PROPERTIES_FORWARD_C_CALLBACK_EXECUTE_FULL(CjvxAudioWindowsDevice, set_new_rate)
{
	return JVX_NO_ERROR;
}
*/

void
CjvxAudioFileReaderDevice::send_buffer_direct()
{
	jvxSize bufIdx = *_common_set_ocon.theData_out.con_pipeline.idx_stage_ptr;
	jvxBool requires_new_data = false;
	jvxByte** bufsOut = jvx_process_icon_extract_output_buffers<jvxByte>(
		_common_set_ocon.theData_out);
	if (bufsOut)
	{
		jvxSize copymax = _common_set_ocon.theData_out.con_params.buffersize;
		jvxByte* ptr_to = (jvxByte*)bufsOut[0];
		jvxSize numcopy = copymax;
		jvxSize num_copied = 0;

		// We should prevent to rewind while we are reading!
		JVX_LOCK_MUTEX(safeAccessRead);
		jvxErrorType resRead = wavFileReader.read_one_buf_raw(ptr_to, numcopy, &num_copied);
		JVX_UNLOCK_MUTEX(safeAccessRead);
		assert(_common_set_ocon.theData_out.con_data.fHeights);
		_common_set_ocon.theData_out.con_data.fHeights[bufIdx].x = num_copied;
		
		jvxSize progress = 0;
		wavFileReader.current_progress(&progress);
		genFileReader_device::monitor.progress_percent.value = ((jvxData)progress / (jvxData)l_samples * 100.0);
	}
}



void
CjvxAudioFileReaderDevice::reconfigure_bsize(jvxSize bsize)
{
	file_params.bsize = bsize;
	file_params.fsizemax = jvx_wav_compute_bsize_bytes_pcm(&file_params, JVX_SIZE_UNSELECTED);
}

JVX_PROPERTIES_FORWARD_C_CALLBACK_EXECUTE_FULL(CjvxAudioFileReaderDevice, set_config)
{
	if (JVX_PROPERTY_CHECK_ID_CAT(ident.id, ident.cat, genFileReader_device::command.restart))
	{
		wavFileReader.set_loop(genFileReader_device::config.loop.value == c_true);
	}
	return JVX_NO_ERROR;
}

JVX_PROPERTIES_FORWARD_C_CALLBACK_EXECUTE_FULL(CjvxAudioFileReaderDevice, trigger_command)
{
	if (JVX_PROPERTY_CHECK_ID_CAT(ident.id, ident.cat, genFileReader_device::command.restart))
	{
		// Designed for application during application.
		// Most of the functions should silently failed if uused when not online
		if (genFileReader_device::command.restart.value)
		{
			JVX_LOCK_MUTEX(safeAccessRead);
			wavFileReader.rewind();			
			JVX_UNLOCK_MUTEX(safeAccessRead);
		}
		genFileReader_device::command.restart.value = false;
	}
	return JVX_NO_ERROR;
}

