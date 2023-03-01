#include "CjvxAudioFFMpegReaderDevice.h"
#include "CjvxAudioFFMpegReaderTechnology.h"
#include "jvx-helpers-cpp.h"

extern "C"
{
#include "libavutil/opt.h"
#include "libavcodec/avcodec.h"
}

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
	reset_props();
}


CjvxAudioFFMpegReaderDevice::~CjvxAudioFFMpegReaderDevice()
{
	JVX_TERMINATE_MUTEX(safeAccessBuffer);
	JVX_TERMINATE_MUTEX(safeAccessRead);
}

// =============================================================================


void
CjvxAudioFFMpegReaderDevice::reset_props()
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

static int check_stream_specifier(AVFormatContext* s, AVStream* st, const char* spec)
{
	int ret = avformat_match_stream_specifier(s, st, spec);
	if (ret < 0)
		av_log(s, AV_LOG_ERROR, "Invalid stream specifier: %s.\n", spec);
	return ret;
};

static AVDictionary* filter_codec_opts(AVDictionary* opts, enum AVCodecID codec_id,
	AVFormatContext* s, AVStream* st, const AVCodec* codec)
{
	AVDictionary* ret = NULL;
	const AVDictionaryEntry* t = NULL;
	int            flags = s->oformat ? AV_OPT_FLAG_ENCODING_PARAM
		: AV_OPT_FLAG_DECODING_PARAM;
	char          prefix = 0;
	const AVClass* cc = avcodec_get_class();

	if (!codec)
		codec = s->oformat ? avcodec_find_encoder(codec_id)
		: avcodec_find_decoder(codec_id);

	switch (st->codecpar->codec_type) {
	case AVMEDIA_TYPE_VIDEO:
		prefix = 'v';
		flags |= AV_OPT_FLAG_VIDEO_PARAM;
		break;
	case AVMEDIA_TYPE_AUDIO:
		prefix = 'a';
		flags |= AV_OPT_FLAG_AUDIO_PARAM;
		break;
	case AVMEDIA_TYPE_SUBTITLE:
		prefix = 's';
		flags |= AV_OPT_FLAG_SUBTITLE_PARAM;
		break;
	}

	while (t = av_dict_iterate(opts, t)) {
		const AVClass* priv_class;
		char* p = strchr(t->key, ':');

		/* check stream specification in opt name */
		if (p)
			switch (check_stream_specifier(s, st, p + 1)) {
			case  1: *p = 0; break;
			case  0:         continue;
			default:         assert(0);
			}

		if (av_opt_find(&cc, t->key, NULL, flags, AV_OPT_SEARCH_FAKE_OBJ) ||
			!codec ||
			((priv_class = codec->priv_class) &&
				av_opt_find(&priv_class, t->key, NULL, flags,
					AV_OPT_SEARCH_FAKE_OBJ)))
			av_dict_set(&ret, t->key, t->value, 0);
		else if (t->key[0] == prefix &&
			av_opt_find(&cc, t->key + 1, NULL, flags,
				AV_OPT_SEARCH_FAKE_OBJ))
			av_dict_set(&ret, t->key + 1, t->value, 0);

		if (p)
			*p = ':';
	}
	return ret;
}

static AVDictionary** setup_find_stream_info_opts(AVFormatContext* s,
	AVDictionary* codec_opts)
{
	int i;
	AVDictionary** opts = nullptr;

	if (!s->nb_streams)
		return NULL;
	opts = (AVDictionary**)av_calloc(s->nb_streams, sizeof(*opts));
	for (i = 0; i < s->nb_streams; i++)
		opts[i] = filter_codec_opts(codec_opts, s->streams[i]->codecpar->codec_id,
			s, s->streams[i], NULL);
	return opts;
}

#if 0
static int stream_component_open(VideoState* is, int stream_index)
{

	AVFormatContext* ic = is->ic;
	AVCodecContext* avctx;
	const AVCodec* codec;
	const char* forced_codec_name = NULL;
	AVDictionary* opts = NULL;
	const AVDictionaryEntry* t = NULL;
	int sample_rate;
	AVChannelLayout ch_layout = { 0 };
	int ret = 0;
	int stream_lowres = lowres;

	if (stream_index < 0 || stream_index >= ic->nb_streams)
		return -1;

	avctx = avcodec_alloc_context3(NULL);
	if (!avctx)
		return AVERROR(ENOMEM);

	ret = avcodec_parameters_to_context(avctx, ic->streams[stream_index]->codecpar);
	if (ret < 0)
		goto fail;
	avctx->pkt_timebase = ic->streams[stream_index]->time_base;

	codec = avcodec_find_decoder(avctx->codec_id);

	switch (avctx->codec_type) {
	case AVMEDIA_TYPE_AUDIO: is->last_audio_stream = stream_index; forced_codec_name = audio_codec_name; break;
	case AVMEDIA_TYPE_SUBTITLE: is->last_subtitle_stream = stream_index; forced_codec_name = subtitle_codec_name; break;
	case AVMEDIA_TYPE_VIDEO: is->last_video_stream = stream_index; forced_codec_name = video_codec_name; break;
	}
	if (forced_codec_name)
		codec = avcodec_find_decoder_by_name(forced_codec_name);
	if (!codec) {
		if (forced_codec_name) av_log(NULL, AV_LOG_WARNING,
			"No codec could be found with name '%s'\n", forced_codec_name);
		else                   av_log(NULL, AV_LOG_WARNING,
			"No decoder could be found for codec %s\n", avcodec_get_name(avctx->codec_id));
		ret = AVERROR(EINVAL);
		goto fail;
	}

	avctx->codec_id = codec->id;
	if (stream_lowres > codec->max_lowres) {
		av_log(avctx, AV_LOG_WARNING, "The maximum value for lowres supported by the decoder is %d\n",
			codec->max_lowres);
		stream_lowres = codec->max_lowres;
	}
	avctx->lowres = stream_lowres;

	if (fast)
		avctx->flags2 |= AV_CODEC_FLAG2_FAST;

	opts = filter_codec_opts(codec_opts, avctx->codec_id, ic, ic->streams[stream_index], codec);
	if (!av_dict_get(opts, "threads", NULL, 0))
		av_dict_set(&opts, "threads", "auto", 0);
	if (stream_lowres)
		av_dict_set_int(&opts, "lowres", stream_lowres, 0);
	if ((ret = avcodec_open2(avctx, codec, &opts)) < 0) {
		goto fail;
	}
	if ((t = av_dict_get(opts, "", NULL, AV_DICT_IGNORE_SUFFIX))) {
		av_log(NULL, AV_LOG_ERROR, "Option %s not found.\n", t->key);
		ret = AVERROR_OPTION_NOT_FOUND;
		goto fail;
	}

	is->eof = 0;
	ic->streams[stream_index]->discard = AVDISCARD_DEFAULT;
	switch (avctx->codec_type) {
	case AVMEDIA_TYPE_AUDIO:
#if CONFIG_AVFILTER
	{
		AVFilterContext* sink;

		is->audio_filter_src.freq = avctx->sample_rate;
		ret = av_channel_layout_copy(&is->audio_filter_src.ch_layout, &avctx->ch_layout);
		if (ret < 0)
			goto fail;
		is->audio_filter_src.fmt = avctx->sample_fmt;
		if ((ret = configure_audio_filters(is, afilters, 0)) < 0)
			goto fail;
		sink = is->out_audio_filter;
		sample_rate = av_buffersink_get_sample_rate(sink);
		ret = av_buffersink_get_ch_layout(sink, &ch_layout);
		if (ret < 0)
			goto fail;
	}
#else
		sample_rate = avctx->sample_rate;
		ret = av_channel_layout_copy(&ch_layout, &avctx->ch_layout);
		if (ret < 0)
			goto fail;
#endif

		/* prepare audio output */
		if ((ret = audio_open(is, &ch_layout, sample_rate, &is->audio_tgt)) < 0)
			goto fail;
		is->audio_hw_buf_size = ret;
		is->audio_src = is->audio_tgt;
		is->audio_buf_size = 0;
		is->audio_buf_index = 0;

		/* init averaging filter */
		is->audio_diff_avg_coef = exp(log(0.01) / AUDIO_DIFF_AVG_NB);
		is->audio_diff_avg_count = 0;
		/* since we do not have a precise anough audio FIFO fullness,
		   we correct audio sync only if larger than this threshold */
		is->audio_diff_threshold = (double)(is->audio_hw_buf_size) / is->audio_tgt.bytes_per_sec;

		is->audio_stream = stream_index;
		is->audio_st = ic->streams[stream_index];

		if ((ret = decoder_init(&is->auddec, avctx, &is->audioq, is->continue_read_thread)) < 0)
			goto fail;
		if ((is->ic->iformat->flags & (AVFMT_NOBINSEARCH | AVFMT_NOGENSEARCH | AVFMT_NO_BYTE_SEEK)) && !is->ic->iformat->read_seek) {
			is->auddec.start_pts = is->audio_st->start_time;
			is->auddec.start_pts_tb = is->audio_st->time_base;
		}
		if ((ret = decoder_start(&is->auddec, audio_thread, "audio_decoder", is)) < 0)
			goto out;
		SDL_PauseAudioDevice(audio_dev, 0);
		break;
	case AVMEDIA_TYPE_VIDEO:
		is->video_stream = stream_index;
		is->video_st = ic->streams[stream_index];

		if ((ret = decoder_init(&is->viddec, avctx, &is->videoq, is->continue_read_thread)) < 0)
			goto fail;
		if ((ret = decoder_start(&is->viddec, video_thread, "video_decoder", is)) < 0)
			goto out;
		is->queue_attachments_req = 1;
		break;
	case AVMEDIA_TYPE_SUBTITLE:
		is->subtitle_stream = stream_index;
		is->subtitle_st = ic->streams[stream_index];

		if ((ret = decoder_init(&is->subdec, avctx, &is->subtitleq, is->continue_read_thread)) < 0)
			goto fail;
		if ((ret = decoder_start(&is->subdec, subtitle_thread, "subtitle_decoder", is)) < 0)
			goto out;
		break;
	default:
		break;
	}
	goto out;

fail:
	avcodec_free_context(&avctx);
out:
	av_channel_layout_uninit(&ch_layout);
	av_dict_free(&opts);

	return ret;
}
#endif

jvxErrorType
CjvxAudioFFMpegReaderDevice::init_from_filename(const std::string& fnameArg, CjvxAudioFFMpegReaderTechnology* par)
{
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;
	AVDictionary* format_opts = nullptr, * codec_opts = nullptr;
	parentTech = par;
	fname = fnameArg;

	// This code prepares the input from a file. The code has been copied in parts from ffplay app code in ffplay.c

	// Step I: Allocate the packet container
	pkt = av_packet_alloc();

	// Step II: Allocate the context
	ic = avformat_alloc_context();

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
	int err = avformat_open_input(&ic, fnameArg.c_str(), iformat, &format_opts);
	if (err < 0)
	{
		goto failed;
	}
	// Generate all pointers
	ic->flags |= AVFMT_FLAG_GENPTS;

	// ============================================================
	// This code returns the codec options for the detected type
	AVDictionary** opts = setup_find_stream_info_opts(ic, codec_opts);
	int orig_nb_streams = ic->nb_streams;

	err = avformat_find_stream_info(ic, opts);

	for (i = 0; i < orig_nb_streams; i++)
		av_dict_free(&opts[i]);
	av_freep(&opts);
	// ============================================================

	// Seems to be a required hack
	if (ic->pb)
		ic->pb->eof_reached = 0;

	// This code output the information about the input file to console
	av_dump_format(ic, 0, fnameArg.c_str(), 0);

	for (i = 0; i < ic->nb_streams; i++)
	{
		AVStream* st = ic->streams[i];
		enum AVMediaType type = st->codecpar->codec_type;
		st->discard = AVDISCARD_ALL;
		/*
		* I do not know what this is all good for - AVDISCARD_ALL seems to be not corrct though
		if (type >= 0 && wanted_stream_spec[type] && st_index[type] == -1)
			if (avformat_match_stream_specifier(ic, st, wanted_stream_spec[type]) > 0)
				st_index[type] = i;
		*/
	}

	/*
	 *
	 for (i = 0; i < AVMEDIA_TYPE_NB; i++) {
		if (wanted_stream_spec[i] && st_index[i] == -1) {
			av_log(NULL, AV_LOG_ERROR, "Stream specifier %s does not match any %s stream\n", wanted_stream_spec[i], av_get_media_type_string(i));
			st_index[i] = INT_MAX;
		}
	}
	 */

	 // Find the best stream in terms of quality. There is only one here!!
	st_index[AVMEDIA_TYPE_AUDIO] =
		av_find_best_stream(ic, AVMEDIA_TYPE_AUDIO,
			st_index[AVMEDIA_TYPE_AUDIO],
			st_index[AVMEDIA_TYPE_VIDEO],
			NULL, 0);

	// Open the stream
	if (st_index[AVMEDIA_TYPE_AUDIO] >= 0) {
		// stream_component_open(is, st_index[AVMEDIA_TYPE_AUDIO]);
	}

	// In loop:
	int ret = av_read_frame(ic, pkt);
	

#if 0
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
#endif

	return res;

failed:
	if (ic)
	{
		avformat_free_context(ic);
		ic = nullptr;
	}
	if (pkt)
	{
		av_packet_free(&pkt);
	}
	
}

jvxErrorType
CjvxAudioFFMpegReaderDevice::term_file()
{
	jvxErrorType res = JVX_NO_ERROR;
#if 0
	jvxErrorType res = wavFileReader.deactivate();
	assert(res == JVX_NO_ERROR);

	res = wavFileReader.unselect();
	assert(res == JVX_NO_ERROR);

	reset_props();
#endif
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
#if 0
		for (i = 0; i < file_params.nchans; i++)
		{
			CjvxAudioDevice::properties_active.inputchannelselection.value.entries.push_back("In #" + jvx_size2String(i));
			jvx_bitSet(CjvxAudioDevice::properties_active.inputchannelselection.value.selection(), i);
		}
		CjvxAudioDevice::properties_active.samplerate.value = file_params.srate;
#endif

		CjvxAudioDevice::properties_active.outputchannelselection.value.entries.clear();
		jvx_bitFClear(CjvxAudioDevice::properties_active.outputchannelselection.value.selection());

		this->updateDependentVariables_lock(-1, JVX_PROPERTY_CATEGORY_PREDEFINED, true);

		genFileReader_device::wav_properties.file_length.value = l_samples;

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

		CjvxAudioDevice::properties_active.sourceName.value = jvx_shortenStringName(32, fname);
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

void
CjvxAudioFFMpegReaderDevice::test_set_output_parameters()
{
	// _common_set
	
	// We need to make some specific adaptations here which can not be 
	// made in the standard implementation

	// The format text specification to configure the decoder
	_common_set_ldslave.theData_out.con_params.format_spec = &format_descriptor;

	// The buffersize as it is derived from the buffersize as set by the audio framing definition
	// You can specify the buffersize by setting the standard parameter for the device buffersize
#if 0
	_common_set_ldslave.theData_out.con_params.buffersize =
		jvx_wav_compute_bsize_bytes_pcm(&file_params, _common_set_ldslave.theData_out.con_params.buffersize);
#endif

	// Type is modified as the data is coded
	_common_set_ldslave.theData_out.con_params.format = JVX_DATAFORMAT_BYTE;
	_common_set_ldslave.theData_out.con_params.caps.format_group = JVX_DATAFORMAT_GROUP_AUDIO_CODED_GENERIC;
	_common_set_ldslave.theData_out.con_params.segmentation_x = _common_set_ldslave.theData_out.con_params.buffersize;
	_common_set_ldslave.theData_out.con_params.segmentation_y = 1;

#if 0
	format_descriptor.assign(jvx_wav_produce_codec_token(&file_params));
#endif
}

jvxErrorType
CjvxAudioFFMpegReaderDevice::prepare_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
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
		involve_read_thread = false;
		if (genFileReader_device::exteded_properties.threaded_read.value == c_true)
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
		genFileReader_device::monitor.progress_percent.value = 0;
		genFileReader_device::monitor.num_lost.value = 0;

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
	jvxMasterSourceType* trigTp = nullptr;	

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
	case JVX_LINKDATA_TRANSFER_REQUEST_TRIGGER_TYPE:

		// Indicate that master requires a "trigger" to output data
		trigTp = (jvxMasterSourceType*)data;
		if (trigTp)
		{
			*trigTp = jvxMasterSourceType::JVX_MASTER_SOURCE_EXTERNAL_TRIGGER;
		}
		res = JVX_NO_ERROR;
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
						genFileReader_device::monitor.num_lost.value++;
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
				genFileReader_device::monitor.num_lost.value++;
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
		genFileReader_device::monitor.progress_percent.value = ((jvxData)progress / (jvxData)l_samples * 100.0);
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
	genFileReader_device::monitor.progress_percent.value = ((jvxData)progress / (jvxData)l_samples * 100.0);
}

JVX_PROPERTIES_FORWARD_C_CALLBACK_EXECUTE_FULL(CjvxAudioFFMpegReaderDevice, set_config)
{
	if (JVX_PROPERTY_CHECK_ID_CAT(ident.id, ident.cat, genFileReader_device::command.restart))
	{
#if 0
		wavFileReader.set_loop(genFileReader_device::config.loop.value == c_true);
#endif
	}
	return JVX_NO_ERROR;
}

JVX_PROPERTIES_FORWARD_C_CALLBACK_EXECUTE_FULL(CjvxAudioFFMpegReaderDevice, trigger_command)
{
	if (JVX_PROPERTY_CHECK_ID_CAT(ident.id, ident.cat, genFileReader_device::command.restart))
	{
		// Designed for application during application.
		// Most of the functions should silently failed if uused when not online
		if (genFileReader_device::command.restart.value)
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
		genFileReader_device::command.restart.value = false;
	}
	return JVX_NO_ERROR;
}

