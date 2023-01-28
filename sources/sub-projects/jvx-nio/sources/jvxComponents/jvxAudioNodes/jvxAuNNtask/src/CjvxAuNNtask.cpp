#include "jvx.h"
#include "CjvxAuNNtask.h"
#define ALPHA_SMOOTH 0.7

// ===========================================================================================
// ===========================================================================================

jvxErrorType
CjvxAuNNtaskLog::activate()
{
	jvxErrorType res = CjvxAuNTasks::activate();

	if (res == JVX_NO_ERROR)
	{
		jvxErrorType resL = logWriter_main_signal.lfc_activate(
			static_cast<IjvxObject*>(this),
			static_cast<CjvxProperties*>(this), 
			_common_set_min.theHostRef, "main_log", 100, 500);
		if (resL != JVX_NO_ERROR)
		{
			_report_text_message("Failed to activate logfile writer!", JVX_REPORT_PRIORITY_WARNING);
	}

	}
	return res;
}

// =====================================================================================

jvxErrorType
CjvxAuNNtaskLog::deactivate()
{
	jvxErrorType res = CjvxAuNTasks::_pre_check_deactivate();

	if (res == JVX_NO_ERROR)
	{
		jvxErrorType resL = logWriter_main_signal.lfc_deactivate();
		res = CjvxAuNTasks::deactivate();
	}
	return res;
}

// ===========================================================================================
// ===========================================================================================

jvxErrorType
CjvxAuNNtaskLog::prepare_connect_icon_ntask(jvxLinkDataDescriptor* theData_in, jvxLinkDataDescriptor* theData_out, jvxSize idCtxt,
	CjvxOutputConnectorNtask* refto JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxErrorType resL = JVX_NO_ERROR;
	jvxErrorType res = JVX_ERROR_UNSUPPORTED;

	res = CjvxAuNTasks::prepare_connect_icon_ntask(theData_in, theData_out, idCtxt, refto JVX_CONNECTION_FEEDBACK_CALL_A(fdb));

	if (res != JVX_NO_ERROR)
	{
		return res;
	}

	if (idCtxt == JVX_DEFAULT_CON_ID)
	{
		if (res == JVX_NO_ERROR)
		{
			resL = logWriter_main_signal.lfc_prepare_one_channel(theData_in->con_params.number_channels,
				theData_in->con_params.buffersize, theData_in->con_params.rate,
				theData_in->con_params.format, "audio_in_main", idMainIn);
			resL = logWriter_main_signal.lfc_prepare_one_channel(theData_out->con_params.number_channels,
				theData_out->con_params.buffersize, theData_out->con_params.rate,
				theData_out->con_params.format, "audio_in_main", idMainOut);
			resL = logWriter_main_signal.lfc_start_all_channels();
		}
		// We may put the other logfile lanes here..
		/*
		JVX_AUDIONODE_CLASSNAME::theLogger.add_one_lane(1,
		_common_set_node_params_1io.processing.buffersize,
		_common_set_node_params_1io.processing.format,
		"Whatever", myDataDescriptor, uniqueMyId);
		*/

	}
	return res;
}
// =============================================================================================

jvxErrorType
CjvxAuNNtaskLog::postprocess_connect_icon_ntask(jvxLinkDataDescriptor* theData_in, jvxLinkDataDescriptor* theData_out, jvxSize idCtxt,
	CjvxOutputConnectorNtask* refto JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxErrorType res = JVX_ERROR_UNSUPPORTED;
	jvxErrorType resL = JVX_NO_ERROR;

	if (idCtxt == JVX_DEFAULT_CON_ID)
	{
		resL = logWriter_main_signal.lfc_stop_all_channels();
		resL = logWriter_main_signal.lfc_postprocess_all_channels();
	}

	res = CjvxAuNTasks::postprocess_connect_icon_ntask(theData_in, theData_out, idCtxt, refto JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
	assert(res == JVX_NO_ERROR);

	return res;
}

// ===================================================================================================

jvxErrorType
CjvxAuNNtaskLog::process_buffers_icon_ntask(jvxLinkDataDescriptor* theData_in,
	jvxLinkDataDescriptor* theData_out, jvxSize idCtxt,
	CjvxOutputConnectorNtask* refto,
	jvxSize mt_mask, jvxSize idx_stage)
{
	jvxErrorType resL = JVX_NO_ERROR;

	jvxErrorType res = JVX_NO_ERROR;
	jvxSize maxChans = 0;
	jvxSize minChans = 0;

	static jvxLinkDataDescriptor* theData_out_cc = NULL;
	static jvxLinkDataDescriptor* theData_in_cc = NULL;

	if (idCtxt == JVX_DEFAULT_CON_ID)
	{
		resL = logWriter_main_signal.lfc_write_all_channels(
			theData_in->con_data.buffers[*theData_in->con_pipeline.idx_stage_ptr],
			theData_in->con_params.number_channels, theData_in->con_params.buffersize,
			theData_in->con_params.format, idMainIn);
	}

	res = CjvxAuNTasks::process_buffers_icon_ntask(
		theData_in,
		theData_out,
		idCtxt,
		refto,
		mt_mask,
		idx_stage);

	if (idCtxt == JVX_DEFAULT_CON_ID)
	{
		resL = logWriter_main_signal.lfc_write_all_channels(
			theData_out->con_data.buffers[*theData_out->con_pipeline.idx_stage_ptr],
			theData_out->con_params.number_channels, theData_out->con_params.buffersize,
			theData_out->con_params.format, idMainOut);
	}

	return res;
}

// ======================================================================================

jvxErrorType 
CjvxAuNNtaskLog::put_configuration(jvxCallManagerConfiguration* callConf, IjvxConfigProcessor* processor,
	jvxHandle* sectionToContainAllSubsectionsForMe, 
	const char* filename,
	jvxInt32 lineno)
{
	jvxSize i;
	std::vector<std::string> warns;
	jvxErrorType res = CjvxAuNTasks::put_configuration(callConf, processor,
		sectionToContainAllSubsectionsForMe, 
		filename,
		lineno);
	if (res == JVX_NO_ERROR)
	{
		if (_common_set_min.theState == JVX_STATE_ACTIVE)
		{
			logWriter_main_signal.lfc_put_configuration_active(callConf, processor,
				sectionToContainAllSubsectionsForMe,
				filename,
				lineno,
				warns);
			for (i = 0; i < warns.size(); i++)
			{
				_report_text_message(warns[i].c_str(), JVX_REPORT_PRIORITY_WARNING);
			}
		}
	}
	return res;
}


// ==================================================================================================

jvxErrorType 
CjvxAuNNtaskLog::get_configuration(jvxCallManagerConfiguration* callConf, IjvxConfigProcessor* processor,
	jvxHandle* sectionWhereToAddAllSubsections)
{
	jvxErrorType res = CjvxBareNtask::get_configuration(callConf, processor, sectionWhereToAddAllSubsections);
	logWriter_main_signal.lfc_get_configuration(callConf, processor, sectionWhereToAddAllSubsections);
	return res;
}

// =============================================================================================

jvxErrorType 
CjvxAuNNtaskLog::set_property(
	jvxCallManagerProperties& callGate,
	const jvx::propertyRawPointerType::IjvxRawPointerType& rawPtr,
	const jvx::propertyAddress::IjvxPropertyAddress& ident,
	const jvx::propertyDetail::CjvxTranferDetail& trans)
{
	jvxErrorType res = CjvxBareNtask::set_property(
		callGate, 
		rawPtr,
		ident, trans);

	if (res == JVX_NO_ERROR)
	{
		res = logWriter_main_signal.lfc_set_property(
			callGate,
			rawPtr,
			ident, trans,
			callGate.call_purpose);
	}
	return res;
}

CjvxAuNNtaskLog::CjvxAuNNtaskLog(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE) :
	CjvxAuNTasks(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
{
	idMainIn = JVX_SIZE_UNSELECTED;
	idMainOut = JVX_SIZE_UNSELECTED;
	_append_compile_option("JVX_NTASK_ENABLE_LOGFILE");
}

CjvxAuNNtaskLog::~CjvxAuNNtaskLog()
{
}

// =======================================================================
// =======================================================================
// =======================================================================
// =======================================================================
// =======================================================================
// =======================================================================

#ifdef JVX_EXTERNAL_CALL_ENABLED

#include "mcg_export_project.h"

JVX_MEXCALL_SELECT_REGISTER(CjvxAuNNtaskm, CjvxAuNTasks, _theExtCallObjectName)
JVX_MEXCALL_UNSELECT_UNREGISTER(CjvxAuNNtaskm, CjvxAuNTasks, _theExtCallObjectName)

CjvxAuNNtaskm::CjvxAuNNtaskm(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE) :
	CjvxAuNTasks(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
{
	_append_compile_option("JVX_EXTERNAL_CALL_ENABLED");
	jvxDspBaseErrorType resL = jvx_audio_stack_sample_dispenser_cont_initCfg(&myAudioDispenser);
	assert(resL == JVX_DSP_NO_ERROR);
	fHeightEstimator = NULL;

	work_buffers_fromExt = NULL;
	tstamp_buffer_fromExt = NULL;

	work_buffers_toExt = NULL;
	tstamp_buffer_toExt = NULL;
}

CjvxAuNNtaskm::~CjvxAuNNtaskm()
{
}

jvxErrorType
CjvxAuNNtaskm::activate()
{
	jvxErrorType res = CjvxAuNTasks::activate();

	if (res == JVX_NO_ERROR)
	{
		res = CjvxMexCalls::activate();
	}
	return res;
}

jvxErrorType
CjvxAuNNtaskm::deactivate()
{
	jvxErrorType res = CjvxAuNTasks::_pre_check_deactivate();

	if (res == JVX_NO_ERROR)
	{
		res = CjvxMexCalls::deactivate();
		res = CjvxAuNTasks::deactivate();
	}
	return res;
}

jvxErrorType
CjvxAuNNtaskm::prepare_connect_icon_ntask(jvxLinkDataDescriptor* theData_in, jvxLinkDataDescriptor* theData_out, jvxSize idCtxt,
	CjvxOutputConnectorNtask* refto JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxErrorType res = JVX_ERROR_UNSUPPORTED;

	if (idCtxt == JVX_DEFAULT_CON_ID)
	{
		res = CjvxMexCalls::prepare_connect_icon_enter(theData_in);
	}

	res = CjvxAuNTasks::prepare_connect_icon_ntask(theData_in, theData_out, idCtxt, refto JVX_CONNECTION_FEEDBACK_CALL_A(fdb));

	if (res != JVX_NO_ERROR)
	{
		res = CjvxMexCalls::postprocess_connect_icon_leave(theData_in);
		assert(res == JVX_NO_ERROR);
		return res;
	}

	if (idCtxt == JVX_DEFAULT_CON_ID)
	{
		res = CjvxMexCalls::prepare_connect_icon_leave(theData_in, theData_out);
	}
	return res;
}

jvxErrorType
CjvxAuNNtaskm::postprocess_connect_icon_ntask(jvxLinkDataDescriptor* theData_in, jvxLinkDataDescriptor* theData_out, jvxSize idCtxt,
	CjvxOutputConnectorNtask* refto JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxErrorType res = JVX_ERROR_UNSUPPORTED;

	if (idCtxt == JVX_DEFAULT_CON_ID)
	{
		res = CjvxMexCalls::postprocess_connect_icon_enter(theData_in);
		assert(res == JVX_NO_ERROR);
	}

	res = CjvxAuNTasks::postprocess_connect_icon_ntask(theData_in, theData_out, idCtxt, refto JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
	assert(res == JVX_NO_ERROR);

	if (idCtxt == JVX_DEFAULT_CON_ID)
	{
		res = CjvxMexCalls::postprocess_connect_icon_leave(theData_in);
		assert(res == JVX_NO_ERROR);
	}
	return res;
}

jvxErrorType
CjvxAuNNtaskm::put_configuration(jvxCallManagerConfiguration* callConf, IjvxConfigProcessor* processor,
	jvxHandle* sectionToContainAllSubsectionsForMe,
	const char* filename,
	jvxInt32 lineno)
{
	jvxSize i;
	std::vector<std::string> warns;
	jvxErrorType res = CjvxAuNTasks::put_configuration(callConf, processor,
		sectionToContainAllSubsectionsForMe,
		filename,
		lineno);
	if (res == JVX_NO_ERROR)
	{
		res = CjvxMexCalls::put_configuration(callConf, processor, this->_common_set_min.theState, sectionToContainAllSubsectionsForMe,
			filename, lineno);
	}
	return res;
}

jvxErrorType
CjvxAuNNtaskm::get_configuration(jvxCallManagerConfiguration* callConf, IjvxConfigProcessor* processor,
	jvxHandle* sectionWhereToAddAllSubsections)
{
	jvxErrorType res = CjvxBareNtask::get_configuration(callConf, processor, sectionWhereToAddAllSubsections);
	res = CjvxMexCalls::get_configuration(callConf, processor, sectionWhereToAddAllSubsections);
	return res;
}

// ================================================================================
// ================================================================================

// Generate prototype function for function >start_audiosample_pri<
/*
jvxErrorType 
JVX_AUNTASK_CLASSNAME::start_audiosample_pri( /* Buffersize primary side->* / jvxSize paramIn0)
{
	return JVX_NO_ERROR;
}
*/
static jvxDspBaseErrorType loc_lock(jvxHandle* prv)
{
	return JVX_DSP_NO_ERROR;
}

static jvxDspBaseErrorType loc_unlock(jvxHandle* prv)
{
	return JVX_DSP_NO_ERROR;
}

// Generate prototype function for function >start_audiosample_sec<
jvxErrorType 
CjvxAuNNtaskm::start_audiosample_dispenser( /* Buffersize primary side->*/ jvxSize paramIn0,
	/* Samplerate->*/ jvxSize paramIn1, /* Size of Store Buffer->*/ jvxSize paramIn2,
	/* Start Threshold->*/ jvxSize paramIn3)
{
	jvxSize i;
	jvxSize buffersize = paramIn0;
	jvxSize srate = paramIn1;
	jvxDataFormat form = JVX_DATAFORMAT_DATA;
	jvxSize numChannels = 1;
	jvxSize num_MinMaxSections = 5;
	jvxData recSmoothFactor = 0.95;
	jvxSize numOperations = 0;
	jvxDspBaseErrorType resL = JVX_DSP_NO_ERROR;
	jvxInt16 numberEventsConsidered_perMinMaxSection = JVX_DATA2INT16(srate / buffersize / num_MinMaxSections);

	if (fHeightEstimator != NULL)
	{
		stop_audiosample_dispenser();
	}
	if (fHeightEstimator == NULL)
	{
		resL = jvx_estimate_buffer_fillheight_init(
			&fHeightEstimator,
			numberEventsConsidered_perMinMaxSection,
			num_MinMaxSections,
			recSmoothFactor,
			numOperations);

		jvx_estimate_buffer_fillheight_restart(fHeightEstimator);

		myAudioDispenser.buffersize = paramIn2;
		myAudioDispenser.form = form;
		myAudioDispenser.numChannelsFromExternal = numChannels;
		myAudioDispenser.numChannelsToExternal = numChannels;

		myAudioDispenser.callbacks.prv_callbacks = reinterpret_cast<jvxHandle*>(this);
		myAudioDispenser.callbacks.lock_callback = loc_lock;
		myAudioDispenser.callbacks.unlock_callback = loc_unlock;

		myAudioDispenser.start_threshold = paramIn3;

		JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(work_buffers_fromExt, jvxHandle*, myAudioDispenser.numChannelsFromExternal);
		for (i = 0; i < myAudioDispenser.numChannelsFromExternal; i++)
		{
			JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(work_buffers_fromExt[i],
				jvxData, myAudioDispenser.buffersize);
		}
		JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(tstamp_buffer_fromExt, jvxData, myAudioDispenser.buffersize);
		for (i = 0; i < myAudioDispenser.buffersize; i++)
		{
			tstamp_buffer_fromExt[i] = -1;
		}

		JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(work_buffers_toExt, jvxHandle*, myAudioDispenser.numChannelsToExternal);
		for (i = 0; i < myAudioDispenser.numChannelsToExternal; i++)
		{
			JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(work_buffers_toExt[i],
				jvxData, myAudioDispenser.buffersize);
		}
		JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(tstamp_buffer_toExt, jvxData, myAudioDispenser.buffersize);
		for (i = 0; i < myAudioDispenser.buffersize; i++)
		{
			tstamp_buffer_toExt[i] = -1;
		}

		resL = jvx_audio_stack_sample_dispenser_cont_init(&myAudioDispenser, work_buffers_fromExt, 
			work_buffers_toExt, tstamp_buffer_fromExt, tstamp_buffer_toExt);
		assert(resL == JVX_DSP_NO_ERROR);

		jvx_audio_stack_sample_dispenser_cont_prepare(&myAudioDispenser, fHeightEstimator);
		assert(resL == JVX_DSP_NO_ERROR);

		fillheight_avrg = 0;
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_WRONG_STATE;
}

jvxErrorType
CjvxAuNNtaskm::stop_audiosample_dispenser()
{
	jvxDspBaseErrorType resL = JVX_DSP_NO_ERROR;
	jvxSize i;
	if(fHeightEstimator)
	{
		resL = jvx_audio_stack_sample_dispenser_cont_postprocess(&myAudioDispenser);
		assert(resL == JVX_DSP_NO_ERROR);

		resL = jvx_audio_stack_sample_dispenser_cont_terminate(&myAudioDispenser);
		assert(resL == JVX_DSP_NO_ERROR);

		for (i = 0; i < myAudioDispenser.numChannelsFromExternal; i++)
		{
			JVX_DSP_SAFE_DELETE_FIELD_TYPE(work_buffers_fromExt[i], jvxData);
		}
		JVX_DSP_SAFE_DELETE_FIELD(work_buffers_fromExt);
		work_buffers_fromExt = NULL;
		JVX_DSP_SAFE_DELETE_FIELD(tstamp_buffer_fromExt);
		tstamp_buffer_fromExt = NULL;

		for (i = 0; i < myAudioDispenser.numChannelsToExternal; i++)
		{
			JVX_DSP_SAFE_DELETE_FIELD_TYPE(work_buffers_toExt[i], jvxData);
		}
		JVX_DSP_SAFE_DELETE_FIELD(work_buffers_toExt);
		work_buffers_toExt = NULL;
		JVX_DSP_SAFE_DELETE_FIELD(tstamp_buffer_toExt);
		tstamp_buffer_toExt = NULL;

		resL = jvx_estimate_buffer_fillheight_terminate(fHeightEstimator);
		assert(resL == JVX_DSP_NO_ERROR);

		fHeightEstimator = NULL;
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_WRONG_STATE;
}

jvxErrorType
CjvxAuNNtaskm::process_audiosample_dispenser_pri(  /* Input buffer primary side->*/ jvxData* paramIn0, jvxInt32 dimInX0,
	/* Timestamps buffer pri->*/ jvxData* paramIn1, jvxInt32 dimInX1,
	/* Timestamp->*/ jvxInt64 paramIn2,
	/* Output buffer pri side->*/ jvxData* paramOut0, jvxInt32 dimOutX0,
	/* Output timestamps pri->*/ jvxData* paramOut1, jvxInt32 dimOutX1,
	/* FHeight on enter->*/ jvxSize* paramOut2,
	/* FHeight on leave->*/ jvxSize* paramOut3,
	/* State on enter->*/ jvxSize* paramOut4,
	/* State on leave->*/ jvxSize* paramOut5,
	/* Average fheight->*/ jvxData* paramOut6,
	/* Fail reason->*/ jvxSize* paramOut7)
{
	jvxSize i;
	jvxSize posi = 0;
	jvxErrorType res = JVX_ERROR_INTERNAL;
	jvxDspBaseErrorType resl = JVX_DSP_NO_ERROR;
	jvxAudioStackDispenserProfile prof;
	jvxData out_average = 0;
	jvxData out_min = 0;
	jvxData out_max = 0;
	prof.fHeightOnEnter = 0;
	prof.fHeightOnLeave = 0;
	prof.state_onEnter = 0;
	prof.state_onLeave = 0;
	prof.tStamp = 0;

	if(paramOut7)
		*paramOut7 = 0;

	resl = jvx_audio_stack_sample_dispenser_cont_internal_copy(&myAudioDispenser,
		(jvxHandle**)&paramOut0, 0, (jvxHandle**)&paramIn0, 0, dimOutX0, 0, &prof, &paramIn2, paramIn1, paramOut1);
	switch (resl)
	{
	case JVX_DSP_NO_ERROR:
		jvx_audio_stack_sample_dispenser_update_fillheight(
			&myAudioDispenser, 0, &out_average,
			&out_min, &out_max, NULL, NULL, NULL);

		fillheight_avrg = fillheight_avrg * ALPHA_SMOOTH + out_average * (1 - ALPHA_SMOOTH);
		res = JVX_NO_ERROR;
		break;
	case JVX_DSP_ERROR_ABORT:
		memset(paramOut0, 0, sizeof(jvxData) * dimOutX0);
		for (i = 0; i < dimOutX1; i++)
		{
			paramOut1[i] = -1;
		}
		
		if (paramOut7)
			*paramOut7 = 1;
		res = JVX_NO_ERROR;
		break;
	case JVX_DSP_ERROR_BUFFER_OVERFLOW:
		memset(paramOut0, 0, sizeof(jvxData) * dimOutX0);
		for (i = 0; i < dimOutX1; i++)
		{
			paramOut1[i] = -1;
		}
		if (paramOut7)
			*paramOut7 = 2;
		res = JVX_NO_ERROR;
		break;
	default:
		break;
	}

	if (res == JVX_NO_ERROR)
	{
		if (paramOut2)
			*paramOut2 = prof.fHeightOnEnter;
		if (paramOut3)
			*paramOut3 = prof.fHeightOnLeave;
		if (paramOut4)
			*paramOut4 = prof.state_onEnter;
		if (paramOut5)
			*paramOut5 = prof.state_onLeave;
		if (paramOut6)
			*paramOut6 = fillheight_avrg;
	}
	return res;
}

jvxErrorType
CjvxAuNNtaskm::process_audiosample_dispenser_sec(  /* Input buffer sec side->*/ jvxData* paramIn0, jvxInt32 dimInX0,
	/* Timestamps buffer sec->*/ jvxData* paramIn1, jvxInt32 dimInX1,
	/* Timestamp->*/ jvxInt64 paramIn2,
	/* Output buffer sec side->*/ jvxData* paramOut0, jvxInt32 dimOutX0,
	/* Output timestamps sec->*/ jvxData* paramOut1, jvxInt32 dimOutX1,
	/* FHeight on enter->*/ jvxSize* paramOut2,
	/* FHeight on leave->*/ jvxSize* paramOut3,
	/* State on enter->*/ jvxSize* paramOut4,
	/* State on leave->*/ jvxSize* paramOut5,
	/* Average fheight->*/ jvxData* paramOut6,
	/* Fail reason->*/ jvxSize* paramOut7)
{
	jvxErrorType res = JVX_ERROR_INTERNAL;
	jvxSize i;
	jvxSize posi = 0;
	jvxDspBaseErrorType resl = JVX_DSP_NO_ERROR;
	jvxData out_average = 0;
	jvxData out_min = 0;
	jvxData out_max = 0;
	jvxAudioStackDispenserProfile prof;
	prof.fHeightOnEnter = 0;
	prof.fHeightOnLeave = 0;
	prof.state_onEnter = 0;
	prof.state_onLeave = 0;
	prof.tStamp = 0;

	if (paramOut7)
		*paramOut7 = 0;

	resl = jvx_audio_stack_sample_dispenser_cont_external_copy(&myAudioDispenser,
		(jvxHandle**)&paramIn0, 0, (jvxHandle**)&paramOut0, 0, dimInX0, 0, &prof, &paramIn2, paramIn1, paramOut1);

	switch (resl)
	{
	case JVX_DSP_NO_ERROR:
		jvx_audio_stack_sample_dispenser_update_fillheight(
			&myAudioDispenser, 0, &out_average,
			&out_min, &out_max, NULL, NULL, NULL);

		fillheight_avrg = fillheight_avrg* ALPHA_SMOOTH + out_average * (1 - ALPHA_SMOOTH);
		// genSPAsyncio_node::rt_info.output.fillheight_min.value = out_min;
		// genSPAsyncio_node::rt_info.output.fillheight_max.value = out_max;
		res = JVX_NO_ERROR;
		break;
	case JVX_DSP_ERROR_ABORT:
		if (paramOut7)
			*paramOut7 = 1;
		res = JVX_NO_ERROR;
		break;
	case JVX_DSP_ERROR_BUFFER_OVERFLOW:
		if (paramOut7)
			*paramOut7 = 2;
		res = JVX_NO_ERROR;
		break;
	default:
		break;
	}

	if (res == JVX_NO_ERROR)
	{
		if (paramOut2)
			*paramOut2 = prof.fHeightOnEnter;
		if (paramOut3)
			*paramOut3 = prof.fHeightOnLeave;
		if (paramOut4)
			*paramOut4 = prof.state_onEnter;
		if (paramOut5)
			*paramOut5 = prof.state_onLeave;
		if (paramOut6)
			*paramOut6 = fillheight_avrg;
	}
	return JVX_NO_ERROR;
}
#endif
