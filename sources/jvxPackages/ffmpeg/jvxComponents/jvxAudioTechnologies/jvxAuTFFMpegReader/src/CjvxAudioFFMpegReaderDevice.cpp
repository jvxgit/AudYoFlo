#include "CjvxAudioFFMpegReaderDevice.h"
#include "CjvxAudioFFMpegReaderTechnology.h"
#include "jvx-helpers-cpp.h"
#include "ffmpeg-helpers.h"

CjvxAudioFFMpegReaderDevice::CjvxAudioFFMpegReaderDevice(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE) :
		CjvxAudioDevice(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
{
	_common_set.theObjectSpecialization = reinterpret_cast<jvxHandle*>(static_cast<IjvxDevice*>(this));
	_common_set_audio_device.formats.push_back(JVX_DATAFORMAT_BYTE);
	
	_common_set_properties.moduleReference = _common_set.theModuleName;
	//_config.reportMissedCallbacks = false;

	_common_set.thisisme = static_cast<IjvxObject*>(this);
	fParams.reset();
}


CjvxAudioFFMpegReaderDevice::~CjvxAudioFFMpegReaderDevice()
{
}

// =============================================================================


/** 
 * This function is called before all other member calls. Hence, the associated file is open unless the device is removed.
 */
jvxErrorType
CjvxAudioFFMpegReaderDevice::init_from_filename(const std::string& fnameArg, CjvxAudioFFMpegReaderTechnology* par)
{
	jvxSize i;
	int ret = 0;
	jvxErrorType res = JVX_NO_ERROR;
	AVDictionary* format_opts = nullptr, * codec_opts = nullptr, *filtered_opts = nullptr;
	parentTech = par;	
	_common_set_device.report = par;

	// This code prepares the input from a file. The code has been copied in parts from ffplay app code in ffplay.c

	// Step II: Allocate the context
	fParams.ic = avformat_alloc_context();

	// I think that this is not required
	/*
	if (!av_dict_get(format_opts, "scan_all_pmts", NULL, AV_DICT_MATCH_CASE)) 
	{
		av_dict_set(&format_opts, "scan_all_pmts", "1", AV_DICT_DONT_OVERWRITE);
		scan_all_pmts_set = 1;
	}
	*/

	// We try to open the file here. If an error occurs, the context ic is freed in the called function
	// and the pointer is set to nullptr
	int err = avformat_open_input(&fParams.ic, fnameArg.c_str(), fParams.iformat, &format_opts);
	if (err < 0)
	{
		res = JVX_ERROR_ELEMENT_NOT_FOUND;
		goto failed;
	}

	// Generate all pointers
	// fParams.ic->flags |= AVFMT_FLAG_GENPTS;

	// ============================================================
	// This code returns the codec options for the detected type
	AVDictionary** opts = setup_find_stream_info_opts(fParams.ic, codec_opts);
	int orig_nb_streams = fParams.ic->nb_streams;

	/*
	* We could set the buffersize here: this influences the sizes of the first buffers to be 
	* taken from the file. The buffersize may afterwards change if the chunk size was modified after
	* the initial setup.
	* 
	unsigned char* max_size_ptr = nullptr;
	int ret = av_opt_get(fParams.ic, "max_size", AV_OPT_SEARCH_CHILDREN,&max_size_ptr);
	if (max_size_ptr)
	{
		jvxBool err = false;
		std::string bsizeAdapted = (const char*)max_size_ptr;
		jvxSize newVal = jvx_string2Size(bsizeAdapted, err);
		av_free(max_size_ptr);		
	}
	*/

	err = avformat_find_stream_info(fParams.ic, opts);

	for (i = 0; i < orig_nb_streams; i++)
		av_dict_free(&opts[i]);
	av_freep(&opts);
	// ============================================================

	// Seems to be a required hack
	if (fParams.ic->pb)
	{
		fParams.ic->pb->eof_reached = 0;
	}

	for (i = 0; i < fParams.ic->nb_streams; i++)
	{
		AVStream* st = fParams.ic->streams[i];
		jvxSize newVal = 0;
		unsigned char* max_size_ptr = nullptr;

		if (st->codecpar->codec_type == AVMEDIA_TYPE_AUDIO)
		{
			fParams.fName = fnameArg;

			// Get the file properties from open structure
			fParams.chanLayout = st->codecpar->ch_layout;
			
			fParams.nChans = st->codecpar->ch_layout.nb_channels; // st->codecpar->channels;

			// Note: ic->duration / (jvxData)AV_TIME_BASE is another way to obtain duration
			fParams.lengthSamples = (jvxData)st->duration;
			fParams.lengthSecs = fParams.lengthSamples * (jvxData)st->time_base.num / (jvxData)st->time_base.den;
			fParams.numFrames = (jvxData)st->nb_frames;
			fParams.sRate = st->codecpar->sample_rate;
			fParams.frameSize = st->codecpar->frame_size;
			fParams.bitRate = st->codecpar->bit_rate;

			// AVSampleFormat format = (AVSampleFormat)st->codecpar->format;
			
			// Should be there always
			fParams.fFormatTag = fParams.ic->iformat->name;
			fParams.tpCodec = st->codecpar->codec_type;
			fParams.sFormatId = (AVSampleFormat)st->codecpar->format;
			fParams.bitsPerRaw = st->codecpar->bits_per_raw_sample;
			fParams.bitsPerCoded = st->codecpar->bits_per_coded_sample;
			fParams.idCodec = st->codecpar->codec_id;
			fParams.bSizeAudio = JVX_SIZE_UNSELECTED;
			
			switch (fParams.idCodec)
			{
				// Here, list up all PCM/WAV formats
			case AV_CODEC_ID_PCM_S16LE:
			case AV_CODEC_ID_PCM_S24LE:
			case AV_CODEC_ID_PCM_S32LE:
			case AV_CODEC_ID_PCM_S64LE:
			case AV_CODEC_ID_PCM_F64LE:
			case AV_CODEC_ID_PCM_F32LE:

				// The number of bits should be in this field
				assert(fParams.bitsPerCoded);

				// We try to find an estimate for the maximum number of samples passed via buffer
				// FFMPEG pre-reads the buffers and stores them in memory. Therefore, it could happen
				// that the buffersize changes during runtime. 
				switch (fParams.idCodec)
				{
				case AV_CODEC_ID_PCM_S16LE:
					fParams.isFloat = false;
					break;
				case AV_CODEC_ID_PCM_S24LE:
					fParams.isFloat = false;
					break;
				case AV_CODEC_ID_PCM_S32LE:
					fParams.isFloat = false;
					break;
				case AV_CODEC_ID_PCM_F32LE:
					fParams.isFloat = true;
					break;
				case AV_CODEC_ID_PCM_S64LE:
					fParams.isFloat = false;
					break;
				case AV_CODEC_ID_PCM_F64LE:
					fParams.isFloat = true;
					break;
				}

				ret = av_opt_get(fParams.ic, "max_size", AV_OPT_SEARCH_CHILDREN, &max_size_ptr);
				if (max_size_ptr)
				{
					jvxBool err = false;
					std::string bsizeAdapted = (const char*)max_size_ptr;
					newVal = jvx_string2Size(bsizeAdapted, err);
					av_free(max_size_ptr);
					fParams.bSizeAudio = ceil((jvxData)newVal / (jvxData)(fParams.nChans * fParams.bitsPerCoded / 8));
				}
				break;
			case AV_CODEC_ID_MP3:
				break;
			case AV_CODEC_ID_AAC:
				break;
			default:

				std::cout << "Unable to open file type <" << fParams.fFormatTag << "[" << fParams.fFormatTag << "]>." << std::endl;
				res = JVX_ERROR_UNSUPPORTED;
				goto failed;
			}

			jvx_ffmpeg_printout_file_params(fParams);

			// I think this is to ignore all incoming packets when not yet setup propperly
			st->discard = AVDISCARD_ALL;
			/*
			* I do not know what this is all good for - AVDISCARD_ALL seems to be not corrct though
			if (type >= 0 && wanted_stream_spec[type] && st_index[type] == -1)
				if (avformat_match_stream_specifier(ic, st, wanted_stream_spec[type]) > 0)
					st_index[type] = i;
			*/
			fParams.st = st;
			break;
		}

		if (fParams.st == nullptr)
		{
			res = JVX_ERROR_INVALID_FORMAT;
			goto failed;
		}

		

#if 0
		// Open the stream
		if (st_index[AVMEDIA_TYPE_AUDIO] >= 0) {
			// stream_component_open(is, st_index[AVMEDIA_TYPE_AUDIO]);
		}

		// In loop:
		int ret = av_read_frame(ic, pkt);
#endif
	}

	return JVX_NO_ERROR;

failed:
	term_file();
	return res;
}

jvxErrorType
CjvxAudioFFMpegReaderDevice::term_file()
{
	jvxErrorType res = JVX_NO_ERROR;

	if (fParams.ic)
	{
		avformat_free_context(fParams.ic);
		fParams.ic = nullptr;
	}

	fParams.reset();
	return res;
}

std::string 
CjvxAudioFFMpegReaderDevice::get_last_error()
{
	return last_error;
}

// =============================================================================
// =============================================================================

jvxErrorType
CjvxAudioFFMpegReaderDevice::select(IjvxObject* owner)
{
	jvxErrorType res = CjvxAudioDevice::select(owner);
	if (res == JVX_NO_ERROR)
	{	
		if (_common_set_device.report)
		{
			_common_set_device.report->on_device_caps_changed(static_cast<IjvxDevice*>(this));
		}
	}
	return (res);
}


jvxErrorType
CjvxAudioFFMpegReaderDevice::unselect()
{
	jvxErrorType res = CjvxAudioDevice::unselect();
	if (res == JVX_NO_ERROR)
	{
		if (_common_set_device.report)
		{
			_common_set_device.report->on_device_caps_changed(static_cast<IjvxDevice*>(this));
		}
	}
	return (res);
}

jvxErrorType
CjvxAudioFFMpegReaderDevice::activate()
{
	jvxSize i, j;

	jvxErrorType res = CjvxAudioDevice::activate();
	if (res == JVX_NO_ERROR)
	{
		genFFMpegReader_device::init_all();
		genFFMpegReader_device::allocate_all();
		genFFMpegReader_device::register_all(static_cast<CjvxProperties*>(this));
		genFFMpegReader_device::register_callbacks(
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
		for (i = 0; i < fParams.nChans; i++)
		{
			CjvxAudioDevice::properties_active.inputchannelselection.value.entries.push_back("In #" + jvx_size2String(i));
			jvx_bitSet(CjvxAudioDevice::properties_active.inputchannelselection.value.selection(), i);
		}
		CjvxAudioDevice::properties_active.samplerate.value = fParams.sRate;

		CjvxAudioDevice::properties_active.outputchannelselection.value.entries.clear();
		jvx_bitFClear(CjvxAudioDevice::properties_active.outputchannelselection.value.selection());

		if (fParams.frameSize == 0)
		{
			CjvxAudioDevice::properties_active.buffersize.value = fParams.bSizeAudio;
		}
		else
		{
			CjvxAudioDevice::properties_active.buffersize.value = fParams.frameSize;
		}

		_update_property_access_type(JVX_PROPERTY_ACCESS_READ_ONLY, CjvxAudioDevice::properties_active.buffersize);

		this->updateDependentVariables_lock(-1, JVX_PROPERTY_CATEGORY_PREDEFINED, true);

		genFFMpegReader_device::file.length_sec.value = fParams.lengthSecs;

#if 0
		genFileReader_device::translate__wav_properties__sample_type_to(file_params.sample_type);
		genFileReader_device::translate__wav_properties__sample_resolution_to(file_params.resolution);
		genFileReader_device::translate__wav_properties__endian_to(file_params.endian);
		genFileReader_device::translate__wav_properties__sub_type_to(file_params.sub_type);
		file_params.bsize = CjvxAudioDevice::properties_active.buffersize.value;
		file_params.fsizemax = jvx_wav_compute_bsize_bytes_pcm(&file_params, file_params.bsize);
#endif

		// ==============================================================================

		_update_property_access_type(JVX_PROPERTY_ACCESS_READ_ONLY, CjvxAudioDevice::properties_active.outputchannelselection);
		_update_property_access_type(JVX_PROPERTY_ACCESS_READ_ONLY, CjvxAudioDevice::properties_active.inputchannelselection);
		_update_property_access_type(JVX_PROPERTY_ACCESS_READ_ONLY, CjvxAudioDevice::properties_active.samplerate);

		CjvxAudioDevice::properties_active.sourceName.value = jvx_shortenStringName(32, fParams.fName);
		CjvxAudioDevice::properties_active.sinkName.value = "not-connected";

		if (_common_set_device.report)
		{
			_common_set_device.report->on_device_caps_changed(static_cast<IjvxDevice*>(this));
		}

	}
	return (res);
}

jvxErrorType
CjvxAudioFFMpegReaderDevice::deactivate()
{
	jvxErrorType res = _pre_check_deactivate();
	if (res == JVX_NO_ERROR)
	{
		_undo_update_property_access_type(CjvxAudioDevice::properties_active.outputchannelselection);
		_undo_update_property_access_type(CjvxAudioDevice::properties_active.inputchannelselection);
		_undo_update_property_access_type(CjvxAudioDevice::properties_active.samplerate);

		if (fParams.frameSize != 0)
		{
			_undo_update_property_access_type(CjvxAudioDevice::properties_active.buffersize);
		}

		genFFMpegReader_device::unregister_callbacks(
			static_cast<CjvxProperties*>(this),
			nullptr);
		genFFMpegReader_device::unregister_all(static_cast<CjvxProperties*>(this));
		genFFMpegReader_device::deallocate_all();
		res = CjvxAudioDevice::deactivate();

		if (_common_set_device.report)
		{
			_common_set_device.report->on_device_caps_changed(static_cast<IjvxDevice*>(this));
		}
	}
	return (res);
}

// ===============================================================================

jvxErrorType
CjvxAudioFFMpegReaderDevice::prepare()
{
	jvxSize i;
	jvxErrorType res = CjvxAudioDevice::prepare();
	jvxErrorType resL = JVX_NO_ERROR;

	if (res == JVX_NO_ERROR)
	{        
		if (_common_set_device.report)
		{
			_common_set_device.report->on_device_caps_changed(static_cast<IjvxDevice*>(this));
		}
	}
	return (res);
}

jvxErrorType
CjvxAudioFFMpegReaderDevice::start()
{
	jvxSize i;
	jvxErrorType res = CjvxAudioDevice::start();

	if (res == JVX_NO_ERROR)
	{
		if (_common_set_device.report)
		{
			_common_set_device.report->on_device_caps_changed(static_cast<IjvxDevice*>(this));
		}
	}
	return (res);
}

jvxErrorType
CjvxAudioFFMpegReaderDevice::stop()
{
	jvxErrorType res = CjvxAudioDevice::stop();
	jvxSize numNotDeallocated = 0;
	jvxErrorType resL;
	if (res == JVX_NO_ERROR)
	{
		if (_common_set_device.report)
		{
			_common_set_device.report->on_device_caps_changed(static_cast<IjvxDevice*>(this));
		}
	}
	return (res);
}

jvxErrorType
CjvxAudioFFMpegReaderDevice::postprocess()
{
	jvxErrorType res = CjvxAudioDevice::postprocess();
	jvxSize numNotDeallocated = 0;
	jvxErrorType resL;
	if (res == JVX_NO_ERROR)
	{		
		if (_common_set_device.report)
		{
			_common_set_device.report->on_device_caps_changed(static_cast<IjvxDevice*>(this));
		}
	}
	return (res);
}

// ===============================================================================

jvxErrorType
CjvxAudioFFMpegReaderDevice::put_configuration(jvxCallManagerConfiguration* callConf, IjvxConfigProcessor* processor,
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
CjvxAudioFFMpegReaderDevice::get_configuration(jvxCallManagerConfiguration* callConf, IjvxConfigProcessor* processor, jvxHandle* sectionWhereToAddAllSubsections)
{
	// Write all parameters from base class
    CjvxAudioDevice::get_configuration(callConf, processor, sectionWhereToAddAllSubsections);

	return (JVX_NO_ERROR);
}

jvxErrorType 
CjvxAudioFFMpegReaderDevice::test_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res =  CjvxAudioDevice::test_chain_master(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	if (res == JVX_ERROR_REQUEST_CALL_AGAIN)
	{
		res = CjvxAudioDevice::test_chain_master(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	}
	return res;
}

jvxErrorType
CjvxAudioFFMpegReaderDevice::test_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	// This endpoint not necessarily is required. We should not use it.
	return JVX_ERROR_UNSUPPORTED;
}

void
CjvxAudioFFMpegReaderDevice::test_set_output_parameters()
{
	// The buffersize as it is derived from the buffersize as set by the audio framing definition
// You can specify the buffersize by setting the standard parameter for the device buffersize
	_common_set_ocon.theData_out.con_params.number_channels = 1; // Byte field is always 1 channel
	_common_set_ocon.theData_out.con_params.rate = JVX_SIZE_DONTCARE; // This indicates that the rate is in the format_spec
	_common_set_ocon.theData_out.con_params.buffersize = JVX_SIZE_DONTCARE;
	
	// Type is modified as the data is coded
	_common_set_ocon.theData_out.con_params.format = JVX_DATAFORMAT_POINTER;
	_common_set_ocon.theData_out.con_params.format_group = JVX_DATAFORMAT_GROUP_FFMPEG_PACKET_FWD;
	_common_set_ocon.theData_out.con_params.segmentation.x = _common_set_ocon.theData_out.con_params.buffersize;
	_common_set_ocon.theData_out.con_params.segmentation.y = 1;
	_common_set_ocon.theData_out.con_params.data_flow = jvxDataflow::JVX_DATAFLOW_PUSH_ON_PULL;

	fParams.bSizeAudio = CjvxAudioDevice::properties_active.buffersize.value;
	// This field is only used for wav pcm
	jvx_ffmpeg_wav_params(fParams);
	
	_common_set_ocon.theData_out.con_params.format_spec = jvx_ffmpeg_parameter_2_codec_token(fParams);	
}

jvxErrorType
CjvxAudioFFMpegReaderDevice::prepare_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
    jvxErrorType res = JVX_NO_ERROR;
	std::string bsizeAdapted;
	jvxUByte* max_size_ptr = NULL;
	int ret = 0;
	AVDictionary* format_opts = nullptr, * codec_opts = nullptr, * filtered_opts = nullptr;

	// Auto-run the prepare function
	res = prepare_chain_master_autostate(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	assert(res == JVX_NO_ERROR);

	// HK, 21042023: I have moved the restart to the postprocess_chain_master call. Also,
	// I have really restarted the input file there since the very first fragment otherwise is audible
	// Restart the stream - it is not reset automatically once we stop!
	// this->restart_stream();

	// Get the codec parameters
	AVStream* codecParamPointer = fParams.st;
	jvxLinkDataAttachedStringDetect<jvxLinkDataAttachedChain> attachObject("/ffmpeg/audiostream/avstream", fParams.st);

	_common_set_ocon.theData_out.con_link.attached_chain_single_pass = jvx_attached_push_front(_common_set_ocon.theData_out.con_link.attached_chain_single_pass, &attachObject);

	// Prepare processing parameters
	_common_set_ocon.theData_out.con_data.number_buffers = 1;
	jvx_bitZSet(_common_set_ocon.theData_out.con_data.alloc_flags, 
		(int)jvxDataLinkDescriptorAllocFlags::JVX_LINKDATA_ALLOCATION_FLAGS_IS_ZEROCOPY_CHAIN_SHIFT);	

	// New type of connection by propagating through linked elements
    res = _prepare_chain_master(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	
	jvxLinkDataAttachedChain* ptrHere = nullptr;
	_common_set_ocon.theData_out.con_link.attached_chain_single_pass = jvx_attached_pop_front(_common_set_ocon.theData_out.con_link.attached_chain_single_pass, &ptrHere);
	assert(ptrHere == &attachObject);

	if (res == JVX_NO_ERROR)
	{
		// Monitoring feedback
		genFFMpegReader_device::monitor.progress_percent.value = 0;

		// Start the reader
		// Here we allow input packets
		fParams.st->discard = AVDISCARD_DEFAULT;
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
CjvxAudioFFMpegReaderDevice::postprocess_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxErrorType resL;

	// Set the stream to the end when shutting down
	std::string fName = fParams.fName;

	// This is a really hard restart - the "soft" version does not remove the very first samples
	// this->restart_stream();	
	term_file();
	init_from_filename(fName, parentTech);

	_postprocess_chain_master(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	assert(res == JVX_NO_ERROR);

	// If the chain is postprocessed, the object itself should also be postprocessed if not done so before
	resL = postprocess_chain_master_autostate(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	assert(resL == JVX_NO_ERROR);

	return res;

}

jvxErrorType
CjvxAudioFFMpegReaderDevice::start_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
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

	if (res != JVX_NO_ERROR)
	{
	    _stop_chain_master(NULL);
	    goto leave_error;
	}
	
	triggeredRestart = true;
	statusOutput = processingState::JVX_STATUS_RUNNING;
	genFFMpegReader_device::translate__monitor__file_status_to(statusOutput);

	return res;
leave_error:
	return res;
}

jvxErrorType
CjvxAudioFFMpegReaderDevice::stop_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxErrorType resL;

	statusOutput = processingState::JVX_STATUS_READY;
	genFFMpegReader_device::translate__monitor__file_status_to(statusOutput);

	res = _stop_chain_master(NULL);
	assert(res == JVX_NO_ERROR);

	// If the chain is stopped, the object itself should also be stopped if not done so before
	resL = stop_chain_master_autostate(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	assert(resL == JVX_NO_ERROR);

	return res;
}

jvxErrorType
CjvxAudioFFMpegReaderDevice::prepare_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
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
CjvxAudioFFMpegReaderDevice::postprocess_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;

	res = deallocate_pipeline_and_buffers_postprocess_to();

	return res;
}

jvxErrorType
CjvxAudioFFMpegReaderDevice::process_buffers_icon(jvxSize mt_mask, jvxSize idx_stage)
{
	jvxErrorType res = JVX_NO_ERROR;
	// This is the return from the link list
	return res;
}

jvxErrorType
CjvxAudioFFMpegReaderDevice::process_start_icon(jvxSize pipeline_offset, jvxSize* idx_stage,
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
CjvxAudioFFMpegReaderDevice::process_stop_icon(jvxSize idx_stage, jvxBool shift_fwd,
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
CjvxAudioFFMpegReaderDevice::transfer_backward_ocon(jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxLinkDataDescriptor* ld_cp = nullptr;
	jvxErrorType res = JVX_NO_ERROR;

	switch (tp)
	{
	case JVX_LINKDATA_TRANSFER_REQUEST_DATA:
		res = send_one_buffer();		
		break;
	case JVX_LINKDATA_TRANSFER_COMPLAIN_DATA_SETTINGS:

		ld_cp = (jvxLinkDataDescriptor*)data;
		// Do something to select another codec setting

		// No real negotiation..
		if (ld_cp->con_params.format_spec != _common_set_ocon.theData_out.con_params.format_spec)
		{
			res = JVX_ERROR_INVALID_SETTING;
		}

		return res;
	default:
		CjvxAudioDevice::transfer_backward_ocon(tp, data JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
	}
	return res;
}

jvxErrorType 
CjvxAudioFFMpegReaderDevice::set_property(jvxCallManagerProperties& callGate,
	const jvx::propertyRawPointerType::IjvxRawPointerType& rawPtr,
	const jvx::propertyAddress::IjvxPropertyAddress& ident,
	const jvx::propertyDetail::CjvxTranferDetail& trans)
{
	return CjvxAudioDevice::set_property(callGate,
		rawPtr, ident, trans);
}

jvxErrorType
CjvxAudioFFMpegReaderDevice::send_one_buffer()
{
	jvxErrorType res = JVX_NO_ERROR;
	if (_common_set_ocon.theData_out.con_link.connect_to)
	{
		if (statusOutput == processingState::JVX_STATUS_RUNNING)
		{
			res = _common_set_ocon.theData_out.con_link.connect_to->process_start_icon();
			if (res == JVX_NO_ERROR)
			{
				res = send_buffer_direct();
				_common_set_ocon.theData_out.con_link.connect_to->process_buffers_icon();
				_common_set_ocon.theData_out.con_link.connect_to->process_stop_icon();
			}
		}
		else
		{
			res = JVX_ERROR_WRONG_STATE;
		}
	}
	return res;
}

jvxErrorType
CjvxAudioFFMpegReaderDevice::send_buffer_direct()
{
	jvxSize bufIdx = *_common_set_ocon.theData_out.con_pipeline.idx_stage_ptr;
	jvxBool requires_new_data = false;
	jvxData progress = 0;
	jvxErrorType res = JVX_NO_ERROR;;

	// static int fCnt = 0;
	AVPacket* thePacket = (AVPacket*)_common_set_ocon.theData_out.con_data.buffers[bufIdx];

	if (triggeredRestart)
	{
		restart_stream();		
		triggeredRestart = false;
		// fCnt = 0;
	}

	if (fwd10triggered)
	{
		skip_stream(10 * fParams.st->time_base.den, latest_pts);
		fwd10triggered = false;
	}

	if (bwd10triggered)
	{
		skip_stream(-10 * fParams.st->time_base.den, latest_pts);
		bwd10triggered = false;
	}
	// If not in state RUNNING, we 
	assert(statusOutput == processingState::JVX_STATUS_RUNNING);
	int ret = av_read_frame(fParams.ic, thePacket);
	if (ret == 0)
	{
		latest_pts = thePacket->pts;
		genFFMpegReader_device::monitor.progress_percent.value = 100.0 * (jvxData)thePacket->pts / (jvxData)fParams.st->duration;

		// genFFMpegReader_device::monitor.progress_percent.value = ((jvxData)progress / (jvxData)fParams.lengthSecs * 100.0);
	}
	else
	{
		switch (ret)
		{
		case AVERROR_EOF:
			if (genFFMpegReader_device::config.loop.value)
			{
				triggeredRestart = true;
			}
			else
			{
				statusOutput = processingState::JVX_STATUS_DONE;
				genFFMpegReader_device::translate__monitor__file_status_to(statusOutput);
			}
			break;
		default:
			statusOutput = processingState::JVX_STATUS_ERROR;
			genFFMpegReader_device::translate__monitor__file_status_to(statusOutput);
			break;
		}
	}
	return res;
	// fCnt++;
}

void
CjvxAudioFFMpegReaderDevice::restart_stream()
{
	int ret = -1;
	int64_t startTime = 0;

	if (fParams.st->start_time != AV_NOPTS_VALUE)
	{
		startTime = fParams.st->start_time;
	}

	int64_t seek_target = startTime;
	int64_t seek_min = INT64_MIN; // is->seek_rel > 0 ? seek_target - is->seek_rel + 2 : INT64_MIN;
	int64_t seek_max = INT64_MAX; //  is->seek_rel < 0 ? seek_target - is->seek_rel - 2 : INT64_MAX;
	int64_t seek_flags = 0;
	// is->seek_flags &= ~AVSEEK_FLAG_BYTE;
	// AVSEEK_FLAG_BACKWARD | AVSEEK_FLAG_FRAME

	// FIXME the +-2 is due to rounding being not done in the correct direction in generation
	//      of the seek_pos/seek_rel variables

	ret = avformat_seek_file(fParams.ic, 0, seek_min, seek_target, seek_max, seek_flags);
	if (ret == 0)
	{
		statusOutput = processingState::JVX_STATUS_RUNNING;
		genFFMpegReader_device::translate__monitor__file_status_to(statusOutput);
	}
}

void
CjvxAudioFFMpegReaderDevice::skip_stream(jvxData skipPos, uint64_t curPos)
{
	int64_t seek_min = INT64_MIN;
	int64_t seek_max = INT64_MAX;
	int64_t seek_target = curPos + skipPos;
	int64_t seek_flags = 0;
	avformat_seek_file(fParams.ic, 0, seek_min, seek_target, seek_max, seek_flags);
	
}

JVX_PROPERTIES_FORWARD_C_CALLBACK_EXECUTE_FULL(CjvxAudioFFMpegReaderDevice, set_config)
{	
	return JVX_NO_ERROR;
}

JVX_PROPERTIES_FORWARD_C_CALLBACK_EXECUTE_FULL(CjvxAudioFFMpegReaderDevice, trigger_command)
{
	if (JVX_PROPERTY_CHECK_ID_CAT(ident.id, ident.cat, genFFMpegReader_device::command.restart))
	{
		// Designed for application during application.
		// Most of the functions should silently failed if uused when not online
		if (genFFMpegReader_device::command.restart.value)
		{
			triggeredRestart = true;
			statusOutput = processingState::JVX_STATUS_RUNNING;
			genFFMpegReader_device::translate__monitor__file_status_to(statusOutput);
		}
		genFFMpegReader_device::command.restart.value = false;
	}

	if (JVX_PROPERTY_CHECK_ID_CAT(ident.id, ident.cat, genFFMpegReader_device::command.fwd10))
	{
		fwd10triggered = true;
		genFFMpegReader_device::command.fwd10.value = false;
	}

	if (JVX_PROPERTY_CHECK_ID_CAT(ident.id, ident.cat, genFFMpegReader_device::command.bwd10))
	{
		bwd10triggered = true;
		genFFMpegReader_device::command.bwd10.value = false;
	}

	if (JVX_PROPERTY_CHECK_ID_CAT(ident.id, ident.cat, genFFMpegReader_device::command.togpause))
	{
		if (statusOutput == processingState::JVX_STATUS_RUNNING)
		{
			statusOutput = processingState::JVX_STATUS_PAUSED;
		}
		else
		{
			if (statusOutput == processingState::JVX_STATUS_PAUSED)
			{
				statusOutput = processingState::JVX_STATUS_RUNNING;
			}
		}
		genFFMpegReader_device::translate__monitor__file_status_to(statusOutput);
		genFFMpegReader_device::command.togpause.value = false;
	}

	if (JVX_PROPERTY_CHECK_ID_CAT(ident.id, ident.cat, genFFMpegReader_device::command.close))
	{
		parentTech->request_kill_device(this);
	}

	return JVX_NO_ERROR;
}

