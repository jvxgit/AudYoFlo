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
	JVX_INITIALIZE_MUTEX(safeAccessBuffer);
	JVX_INITIALIZE_MUTEX(safeAccessRead);
	fParams.reset();
}


CjvxAudioFFMpegReaderDevice::~CjvxAudioFFMpegReaderDevice()
{
	JVX_TERMINATE_MUTEX(safeAccessBuffer);
	JVX_TERMINATE_MUTEX(safeAccessRead);
}

// =============================================================================


/** 
 * This function is called before all other member calls. Hence, the associated file is open unless the device is removed.
 */
jvxErrorType
CjvxAudioFFMpegReaderDevice::init_from_filename(const std::string& fnameArg, CjvxAudioFFMpegReaderTechnology* par)
{
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;
	AVDictionary* format_opts = nullptr, * codec_opts = nullptr, *filtered_opts = nullptr;
	parentTech = par;	

	// This code prepares the input from a file. The code has been copied in parts from ffplay app code in ffplay.c

	// Step I: Allocate the packet container
	fParams.pkt = av_packet_alloc();

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

		if (st->codecpar->codec_type == AVMEDIA_TYPE_AUDIO)
		{
			fParams.fName = fnameArg;

			// Get the file properties from open structure
			char strBuf[512] = { 0 };
			av_channel_layout_describe(&st->codecpar->ch_layout, strBuf, sizeof(strBuf));

			fParams.nChans = st->codecpar->ch_layout.nb_channels; // st->codecpar->channels;

			// Note: ic->duration / (jvxData)AV_TIME_BASE is another way to obtain duration
			fParams.lengthSamples = (jvxData)st->duration;
			fParams.lengthSecs = fParams.lengthSamples * (jvxData)st->time_base.num / (jvxData)st->time_base.den;
			fParams.numFrames = (jvxData)st->nb_frames;
			fParams.sRate = st->codecpar->sample_rate;
			fParams.frameSize = st->codecpar->frame_size;
			fParams.bitRate = st->codecpar->bit_rate;

			// AVSampleFormat format = (AVSampleFormat)st->codecpar->format;
			fParams.chanLayoutTag = strBuf;

			// Should be there always
			fParams.fFormatTag = fParams.ic->iformat->name;

			// Get codec id name
			const char* cbuf = avcodec_get_name(st->codecpar->codec_id);
			if (cbuf)
			{
				fParams.codecIdTag = cbuf;
			}


			cbuf = av_get_media_type_string(st->codecpar->codec_type);
			if (cbuf)
			{
				fParams.codecTypeTag = cbuf;
			}

			if (st->codecpar->format != AV_SAMPLE_FMT_NONE)
			{
				cbuf = av_get_sample_fmt_name((AVSampleFormat)st->codecpar->format);
				if (cbuf)
				{
					fParams.sFormat = cbuf;
				}
			}
			fParams.bitsPerRaw = st->codecpar->bits_per_raw_sample;
			fParams.bitsPerCoded = st->codecpar->bits_per_coded_sample;

			std::cout << "Filename <" << fParams.fName << ">: " << std::endl;
			std::cout << " -> Codec <" << fParams.codecIdTag << ">: " << std::endl;
			std::cout << " -> File format <" << fParams.fFormatTag<< ">: " << std::endl;
			std::cout << " -> Bit rate <" << fParams.bitRate << ">: " << std::endl;
			std::cout << " -> Sample format <" << fParams.sFormat << ">: " << std::endl;
			std::cout << " -> Sample rate <" << fParams.sRate<< ">: " << std::endl;
			std::cout << " -> Frame size <" << fParams.frameSize << ">: " << std::endl;
			std::cout << " -> Number channels <" << fParams.nChans << ">: " << std::endl;
			std::cout << " -> Length [samples] <" << fParams.lengthSamples << ">: " << std::endl;
			std::cout << " -> Length [secs] <" << fParams.lengthSecs << ">: " << std::endl;
			std::cout << " -> Number frames <" << fParams.numFrames << ">: " << std::endl;

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
	if (fParams.pkt)
	{
		av_packet_free(&fParams.pkt);
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
	}
	return (res);
}


jvxErrorType
CjvxAudioFFMpegReaderDevice::unselect()
{
	jvxErrorType res = CjvxAudioDevice::unselect();
	if (res == JVX_NO_ERROR)
	{
	}
	return (res);
}

jvxErrorType
CjvxAudioFFMpegReaderDevice::activate()
{
	jvxSize i,j;

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

		if (fParams.frameSize != 0)
		{
			CjvxAudioDevice::properties_active.buffersize.value = fParams.frameSize;
			UPDATE_PROPERTY_ACCESS_TYPE(JVX_PROPERTY_ACCESS_READ_ONLY, CjvxAudioDevice::properties_active.buffersize);
		}

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

		UPDATE_PROPERTY_ACCESS_TYPE(JVX_PROPERTY_ACCESS_READ_ONLY, CjvxAudioDevice::properties_active.outputchannelselection);
		UPDATE_PROPERTY_ACCESS_TYPE(JVX_PROPERTY_ACCESS_READ_ONLY, CjvxAudioDevice::properties_active.inputchannelselection);
		UPDATE_PROPERTY_ACCESS_TYPE(JVX_PROPERTY_ACCESS_READ_ONLY, CjvxAudioDevice::properties_active.samplerate);

		CjvxAudioDevice::properties_active.sourceName.value = jvx_shortenStringName(32, fParams.fName);
		CjvxAudioDevice::properties_active.sinkName.value = "not-connected";

		// ==============================================================================
		// 
		// Obtain the thread handle here
		refThreads = reqInstTool<IjvxThreads>(
			_common_set.theToolsHost,
			JVX_COMPONENT_THREADS);
		assert(refThreads.cpPtr);

#if 0
		wavFileReader.set_loop(genFileReader_device::config.loop.value == c_true);
#endif
    }
	return (res);
}

jvxErrorType
CjvxAudioFFMpegReaderDevice::deactivate()
{
	jvxErrorType res = _pre_check_deactivate();
	if (res == JVX_NO_ERROR)
	{
		retInstTool<IjvxThreads>(
			_common_set.theToolsHost,
			JVX_COMPONENT_THREADS,
			refThreads);

		UNDO_UPDATE_PROPERTY_ACCESS_TYPE(CjvxAudioDevice::properties_active.outputchannelselection);
		UNDO_UPDATE_PROPERTY_ACCESS_TYPE(CjvxAudioDevice::properties_active.inputchannelselection);
		UNDO_UPDATE_PROPERTY_ACCESS_TYPE(CjvxAudioDevice::properties_active.samplerate);

		if (fParams.frameSize != 0)
		{
			UNDO_UPDATE_PROPERTY_ACCESS_TYPE(CjvxAudioDevice::properties_active.buffersize);
		}

		genFFMpegReader_device::unregister_callbacks(
			static_cast<CjvxProperties*>(this),
			nullptr);
		genFFMpegReader_device::unregister_all(static_cast<CjvxProperties*>(this));
		genFFMpegReader_device::deallocate_all();
		res = CjvxAudioDevice::deactivate();


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

std::string 
CjvxAudioFFMpegReaderDevice::jvx_ffmpeg_produce_codec_token(_parameter_t& params, jvxSize bSize)
{
	std::string strText;

	strText = "fam=ffmpeg";

	strText += ";tp=" + fParams.codecTypeTag + ":" + fParams.fFormatTag + fParams.codecIdTag;
	strText += ";cid=" + jvx_size2String((jvxSize)fParams.st->codecpar->codec_id);
	if (fParams.fFormatTag == "wav")
	{
		switch (fParams.st->codecpar->codec_id)
		{
		case AV_CODEC_ID_PCM_S16LE:
			strText += ";cidn=" "AV_CODEC_ID_PCM_S16LE";
			break;
		case AV_CODEC_ID_PCM_S16BE:
			strText += ";cidn=" "AV_CODEC_ID_PCM_S16BE";
			break;
		case AV_CODEC_ID_PCM_U16LE:
			strText += ";cidn=" "AV_CODEC_ID_PCM_U16LE";
			break;
		case AV_CODEC_ID_PCM_U16BE:
			strText += ";cidn=" "AV_CODEC_ID_PCM_U16BE";
			break;
		case AV_CODEC_ID_PCM_S8:
			strText += ";cidn=" "AV_CODEC_ID_PCM_S8";
			break;
		case AV_CODEC_ID_PCM_U8:
			strText += ";cidn=" "AV_CODEC_ID_PCM_U8";
			break;
		case AV_CODEC_ID_PCM_MULAW:
			strText += ";cidn=" "AV_CODEC_ID_PCM_MULAW";
			break;
		case AV_CODEC_ID_PCM_ALAW:
			strText += ";cidn=" "AV_CODEC_ID_PCM_ALAW";
			break;
		case AV_CODEC_ID_PCM_S32LE:
			strText += ";cidn=" "AV_CODEC_ID_PCM_S32LE";
			break;
		case AV_CODEC_ID_PCM_S32BE:
			strText += ";cidn=" "AV_CODEC_ID_PCM_S32BE";
			break;
		case AV_CODEC_ID_PCM_U32LE:
			strText += ";cidn=" "AV_CODEC_ID_PCM_U32LE";
			break;
		case AV_CODEC_ID_PCM_U32BE:
			strText += ";cidn=" "AV_CODEC_ID_PCM_U32BE";
			break;
		case AV_CODEC_ID_PCM_S24LE:
			strText += ";cidn=" "AV_CODEC_ID_PCM_S24LE";
			break;
		case AV_CODEC_ID_PCM_S24BE:
			strText += ";cidn=" "AV_CODEC_ID_PCM_S24BE";
			break;
		case AV_CODEC_ID_PCM_U24LE:
			strText += ";cidn=" "AV_CODEC_ID_PCM_U24LE";
			break;
		case AV_CODEC_ID_PCM_U24BE:
			strText += ";cidn=" "AV_CODEC_ID_PCM_U24BE";
			break;
		case AV_CODEC_ID_PCM_S24DAUD:
			strText += ";cidn=" "AV_CODEC_ID_PCM_S24DAUD";
			break;
		case AV_CODEC_ID_PCM_ZORK:
			strText += ";cidn=" "AV_CODEC_ID_PCM_ZORK";
			break;
		case AV_CODEC_ID_PCM_S16LE_PLANAR:
			strText += ";cidn=" "AV_CODEC_ID_PCM_S16LE_PLANAR";
			break;
		case AV_CODEC_ID_PCM_DVD:
			strText += ";cidn=" "AV_CODEC_ID_PCM_DVD";
			break;
		case AV_CODEC_ID_PCM_F32BE:
			strText += ";cidn=" "AV_CODEC_ID_PCM_F32BE";
			break;
		case AV_CODEC_ID_PCM_F32LE:
			strText += ";cidn=" "";
			break;
		case AV_CODEC_ID_PCM_F64BE:
			strText += ";cidn=" "AV_CODEC_ID_PCM_F64BE";
			break;
		case AV_CODEC_ID_PCM_F64LE:
			strText += ";cidn=" "AV_CODEC_ID_PCM_F64LE";
			break;
		case AV_CODEC_ID_PCM_BLURAY:
			strText += ";cidn=" "AV_CODEC_ID_PCM_BLURAY";
			break;
		case AV_CODEC_ID_PCM_LXF:
			strText += ";cidn=" "AV_CODEC_ID_PCM_LXF";
			break;
		case AV_CODEC_ID_S302M:
			strText += ";cidn=" "AV_CODEC_ID_S302M";
			break;
		case AV_CODEC_ID_PCM_S8_PLANAR:
			strText += ";cidn=" "AV_CODEC_ID_PCM_S8_PLANAR";
			break;
		case AV_CODEC_ID_PCM_S24LE_PLANAR:
			strText += ";cidn=" "AV_CODEC_ID_PCM_S24LE_PLANAR";
			break;
		case AV_CODEC_ID_PCM_S32LE_PLANAR:
			strText += ";cidn=" "AV_CODEC_ID_PCM_S32LE_PLANAR";
			break;
		case AV_CODEC_ID_PCM_S16BE_PLANAR:
			strText += ";cidn=" "AV_CODEC_ID_PCM_S16BE_PLANAR";
			break;
		case AV_CODEC_ID_PCM_S64LE:
			strText += ";cidn=" "AV_CODEC_ID_PCM_S64LE";
			break;
		case AV_CODEC_ID_PCM_S64BE:
			strText += ";cidn=" "AV_CODEC_ID_PCM_S64BE";
			break;
		case AV_CODEC_ID_PCM_F16LE:
			strText += ";cidn=" "AV_CODEC_ID_PCM_F16LE";
			break;
		case AV_CODEC_ID_PCM_F24LE:
			strText += ";cidn=" "AV_CODEC_ID_PCM_F24LE";
			break;
		case AV_CODEC_ID_PCM_VIDC:
			strText += ";cidn=" "AV_CODEC_ID_PCM_VIDC";
			break;
		case AV_CODEC_ID_PCM_SGA:
			strText += ";cidn=" "AV_CODEC_ID_PCM_SGA";
			break;
		default:
			break;
		}
	}

	strText += ";ch=" + jvx_size2String(fParams.nChans);
	strText += ";chl=" + fParams.chanLayoutTag;
	strText += ";sr=" + jvx_size2String(fParams.sRate);
	if(fParams.frameSize == 0)
	{
		strText += ";bs=" + jvx_size2String(bSize);
	}
	else
	{
		strText += ";bs=" + jvx_size2String(fParams.frameSize);
	}
	strText += ";fsm=" + jvx_size2String(123);
	return strText;
}

void
CjvxAudioFFMpegReaderDevice::test_set_output_parameters()
{
	// The buffersize as it is derived from the buffersize as set by the audio framing definition
// You can specify the buffersize by setting the standard parameter for the device buffersize
	if (fParams.frameSize == 0)
	{
		_common_set_ldslave.theData_out.con_params.buffersize = (CjvxAudioDevice::properties_active.buffersize.value * fParams.bitsPerCoded * fParams.nChans) / 8;
	}
	else
	{
		_common_set_ldslave.theData_out.con_params.buffersize = JVX_SIZE_UNSELECTED;
	}

	// Type is modified as the data is coded
	_common_set_ldslave.theData_out.con_params.format = JVX_DATAFORMAT_BYTE;
	_common_set_ldslave.theData_out.con_params.format_group = JVX_DATAFORMAT_GROUP_FFMPEG_BUFFER_FWD;
	_common_set_ldslave.theData_out.con_params.segmentation_x = _common_set_ldslave.theData_out.con_params.buffersize;
	_common_set_ldslave.theData_out.con_params.segmentation_y = 1;
	_common_set_ldslave.theData_out.con_params.data_flow = jvxDataflow::JVX_DATAFLOW_PUSH_ON_PULL;
	_common_set_ldslave.theData_out.con_params.format_spec = jvx_ffmpeg_produce_codec_token(fParams, CjvxAudioDevice::properties_active.buffersize.value);

}

jvxErrorType
CjvxAudioFFMpegReaderDevice::prepare_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
    jvxErrorType res = JVX_NO_ERROR;




	AVDictionary* format_opts = nullptr, * codec_opts = nullptr, * filtered_opts = nullptr;
	if (fParams.frameSize == 0)
	{
		/*
		const AVOption* opt = av_opt_find(ic,
			"max_size",
			NULL, AV_OPT_FLAG_DECODING_PARAM,
			AV_OPT_SEARCH_CHILDREN);
		*/
		jvxUByte* max_size_ptr = NULL;
		int ret = av_opt_set(fParams.ic, "max_size", "2048", AV_OPT_SEARCH_CHILDREN);
		ret = av_opt_get(fParams.ic, "max_size", AV_OPT_SEARCH_CHILDREN, &max_size_ptr);
		if (max_size_ptr)
		{
			av_free(max_size_ptr);
			max_size_ptr = NULL;
		}
	}
	fParams.cc = avcodec_alloc_context3(NULL);

	int ret = avcodec_parameters_to_context(fParams.cc, fParams.st->codecpar);
	assert(ret == 0);
	fParams.cc->pkt_timebase = fParams.st->time_base;

	fParams.codec = avcodec_find_decoder(fParams.st->codecpar->codec_id);
	// Alternative call here if we knwo the name
	// avcodec_find_decoder_by_name(forced_codec_name);
	assert(fParams.codec);

	// Make sure..
	fParams.cc->codec_id = fParams.codec->id;

	filtered_opts = filter_codec_opts(codec_opts, fParams.st->codecpar->codec_id, fParams.ic, fParams.st, fParams.codec);
	if (!av_dict_get(filtered_opts, "threads", NULL, 0))
		av_dict_set(&filtered_opts, "threads", "auto", 0);

	ret = avcodec_open2(fParams.cc, fParams.codec, &filtered_opts);
	assert(ret == 0);

	// Set not eof
	// fParams.st->eof = 0;

	// Here we allow input packets
	fParams.st->discard = AVDISCARD_DEFAULT;

	ret = av_read_frame(fParams.ic, fParams.pkt);






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
		involve_read_thread = false;
		if (genFFMpegReader_device::exteded_properties.threaded_read.value == c_true)
		{
#if 0
			jvxSize bitssample = jvx_wav_get_bits_sample(&file_params);
			jvxSize bytesonesample = file_params.nchans * bitssample / 8;
			jvxData numsamples = genFileReader_device::exteded_properties.preuse_bsize_msecs.value * 0.001 * file_params.srate;
			jvxSize numbytes = (jvxSize)numsamples * bytesonesample;
			jvxSize numbuffers = ceil((jvxData)numbytes / (jvxData)_common_set_ldslave.theData_out.con_params.buffersize);

			preuse_buffer_sz = JVX_MAX(1, numbuffers) * _common_set_ldslave.theData_out.con_params.buffersize;
			JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(preuse_buffer_ptr, jvxByte, preuse_buffer_sz);
			readposi = 0;
			fHeight = 0;
			readsize = _common_set_ldslave.theData_out.con_params.buffersize;
			bufstatus = jvxAudioFileReaderBufferStatus::JVX_BUFFER_STATUS_NONE;

			involve_read_thread = true;
#endif
		}

		// All parameters were set before, only very few need update
		_common_set_ldslave.theData_out.con_data.number_buffers = 1;

		// Monitoring feedback
		genFFMpegReader_device::monitor.progress_percent.value = 0;
		genFFMpegReader_device::monitor.num_lost.value = 0;

		// Start the reader
#if 0
		wavFileReader.prepare(JVX_SIZE_UNSELECTED); // No read limit in module
		wavFileReader.start();
#endif
		if (involve_read_thread)
		{
			// Start the secondary thread
			jvxErrorType resL = refThreads.cpPtr->initialize(
				static_cast<IjvxThreads_report*>(this));
			assert(resL == JVX_NO_ERROR);
		}
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

	if (involve_read_thread)
	{

		resL = refThreads.cpPtr->terminate();
		assert(resL == JVX_NO_ERROR);
	}

#if 0
	// Stop the reader
	wavFileReader.stop();
	wavFileReader.postprocess();
#endif
	if (involve_read_thread)
	{
		JVX_DSP_SAFE_DELETE_FIELD(preuse_buffer_ptr);
		preuse_buffer_sz = 0;
		readposi = 0;
		fHeight = 0;
	}

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

	// Start core function

	if (involve_read_thread)
	{
		resL = refThreads.cpPtr->start();
		assert(resL == JVX_NO_ERROR);
	}

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
CjvxAudioFFMpegReaderDevice::stop_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxErrorType resL;

	// Core stop function here
	if (involve_read_thread)
	{
		resL = refThreads.cpPtr->stop(1000);
		assert(resL == JVX_NO_ERROR);
	}

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

	_common_set_ldslave.theData_in->con_params.buffersize = _inproc.buffersize;
	_common_set_ldslave.theData_in->con_params.format = _inproc.format;
	_common_set_ldslave.theData_in->con_data.buffers = NULL;
	_common_set_ldslave.theData_in->con_data.number_buffers = JVX_MAX(_common_set_ldslave.theData_in->con_data.number_buffers, 1);
	_common_set_ldslave.theData_in->con_params.number_channels = _inproc.numOutputs;
	_common_set_ldslave.theData_in->con_params.rate = _inproc.samplerate;

	// Connect the output side and start this link
	res = allocate_pipeline_and_buffers_prepare_to();

	// Do not attach any user hint into backward direction
	_common_set_ldslave.theData_in->con_compat.user_hints = NULL;
	// _common_set_ldslave.theData_in->pipeline.idx_stage = 0;

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
		send_one_buffer();		
		break;
	case JVX_LINKDATA_TRANSFER_COMPLAIN_DATA_SETTINGS:

		ld_cp = (jvxLinkDataDescriptor*)data;
		// Do something to select another codec setting

#if 0
		CjvxAudioDevice_genpcg::properties_active.buffersize.value = 
			jvx_wav_compute_bsize_audio(&file_params, ld_cp->con_params.buffersize);

		file_params.bsize = CjvxAudioDevice_genpcg::properties_active.buffersize.value;
		file_params.fsizemax = jvx_wav_compute_bsize_bytes_pcm(&file_params, file_params.bsize);

		_common_set_ldslave.theData_out.con_params.buffersize = file_params.fsizemax;
		_common_set_ldslave.theData_out.con_params.segmentation_x = _common_set_ldslave.theData_out.con_params.buffersize;
		
		format_descriptor.assign(jvx_wav_produce_codec_token(&file_params));
#endif
		return JVX_NO_ERROR;
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

void
CjvxAudioFFMpegReaderDevice::send_one_buffer()
{
	jvxErrorType res = JVX_NO_ERROR;
	if (_common_set_ldslave.theData_out.con_link.connect_to)
	{
		res = _common_set_ldslave.theData_out.con_link.connect_to->process_start_icon();
		if (res == JVX_NO_ERROR)
		{
			if (involve_read_thread)
			{
				send_buffer_thread();
			}
			else
			{
				send_buffer_direct();
			}

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

void
CjvxAudioFFMpegReaderDevice::send_buffer_thread()
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
				(bufstatus == jvxAudioFileReaderBufferStatus::JVX_BUFFER_STATUS_OPERATION) ||
				(bufstatus == jvxAudioFileReaderBufferStatus::JVX_BUFFER_STATUS_COMPLETE))
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
					if (bufstatus == jvxAudioFileReaderBufferStatus::JVX_BUFFER_STATUS_OPERATION)
					{
						genFFMpegReader_device::monitor.num_lost.value++;
						bufstatus = jvxAudioFileReaderBufferStatus::JVX_BUFFER_STATUS_CHARGING;
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
				genFFMpegReader_device::monitor.num_lost.value++;
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
CjvxAudioFFMpegReaderDevice::send_buffer_direct()
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
		jvxSize progress = 0;
#if 0
		jvxErrorType resRead = wavFileReader.read_one_buf_raw(ptr_to, numcopy, &num_copied);
		_common_set_ldslave.theData_out.con_params.fHeight_x = num_copied;

		
		wavFileReader.current_progress(&progress);
#endif
		genFFMpegReader_device::monitor.progress_percent.value = ((jvxData)progress / (jvxData)fParams.lengthSecs * 100.0);
	}
}

jvxErrorType 
CjvxAudioFFMpegReaderDevice::startup(jvxInt64 timestamp_us)
{
	read_samples_to_buffer();
	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxAudioFFMpegReaderDevice::expired(jvxInt64 timestamp_us, jvxSize* delta_ms)
{
	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxAudioFFMpegReaderDevice::wokeup(jvxInt64 timestamp_us, jvxSize* delta_ms)
{
	read_samples_to_buffer();
	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxAudioFFMpegReaderDevice::stopped(jvxInt64 timestamp_us)
{
	return JVX_NO_ERROR;
}

void
CjvxAudioFFMpegReaderDevice::read_samples_to_buffer()
{
	jvxSize readposil = 0;
	jvxSize fheightl = 0;
	jvxBool stayInLoop = true;

	while (stayInLoop)
	{
		if (bufstatus == jvxAudioFileReaderBufferStatus::JVX_BUFFER_STATUS_COMPLETE)
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
			jvxErrorType resRead = JVX_NO_ERROR;

#if 0
			resRead = wavFileReader.read_one_buf_raw(ptrread, numcopy, &num_copied);
#endif
			JVX_LOCK_MUTEX(safeAccessBuffer);
			fHeight += num_copied;
			switch (bufstatus)
			{
			case jvxAudioFileReaderBufferStatus::JVX_BUFFER_STATUS_NONE:
				bufstatus = jvxAudioFileReaderBufferStatus::JVX_BUFFER_STATUS_CHARGING;
				// No break here to fall through

			case jvxAudioFileReaderBufferStatus::JVX_BUFFER_STATUS_CHARGING:

				if (fHeight > preuse_buffer_sz / 2)
				{
					bufstatus = jvxAudioFileReaderBufferStatus::JVX_BUFFER_STATUS_OPERATION;
				}
				break;
			}
			if (resRead != JVX_NO_ERROR)
			{
				bufstatus = jvxAudioFileReaderBufferStatus::JVX_BUFFER_STATUS_COMPLETE;
			}
			JVX_UNLOCK_MUTEX(safeAccessBuffer);
		}
		JVX_UNLOCK_MUTEX(safeAccessRead);

	}
	jvxSize progress = 0;
#if 0
	wavFileReader.current_progress(&progress);
#endif
	genFFMpegReader_device::monitor.progress_percent.value = ((jvxData)progress / (jvxData)fParams.lengthSecs * 100.0);
}

JVX_PROPERTIES_FORWARD_C_CALLBACK_EXECUTE_FULL(CjvxAudioFFMpegReaderDevice, set_config)
{
	if (JVX_PROPERTY_CHECK_ID_CAT(ident.id, ident.cat, genFFMpegReader_device::command.restart))
	{
#if 0
		wavFileReader.set_loop(genFileReader_device::config.loop.value == c_true);
#endif
	}
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
			JVX_LOCK_MUTEX(safeAccessRead);
#if 0
			wavFileReader.rewind();
#endif
			readposi = 0;
			fHeight = 0;
			bufstatus = jvxAudioFileReaderBufferStatus::JVX_BUFFER_STATUS_NONE;
			JVX_UNLOCK_MUTEX(safeAccessRead);
			refThreads.cpPtr->trigger_wakeup();
		}
		genFFMpegReader_device::command.restart.value = false;
	}
	return JVX_NO_ERROR;
}

