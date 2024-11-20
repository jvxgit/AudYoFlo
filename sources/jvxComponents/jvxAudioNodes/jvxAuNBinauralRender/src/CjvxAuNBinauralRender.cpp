#include "CjvxAuNBinauralRender.h"
#include "jvx-helpers-cpp.h"
#include "jvx_fft_tools/jvx_firfft.h"

CjvxAuNBinauralRender::CjvxAuNBinauralRender(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE) :
	CjvxBareNode1ioRearrange(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
{
	// Following already set in componentEntry
	//_common_set.theComponentType.tp = JVX_NODE_TYPE_SPECIFIER_TYPE;
	//_common_set.theComponentSubTypeDescriptor = JVX_NODE_TYPE_SPECIFIER_DESCRIPTOR;

	// Set the processing parameters that will be accepted in the input connector.
	// Note: if no constraint is set the input and output format entries must be 
	// identical.

	neg_input._set_parameters_fixed(
		1 /* 1 input channel required*/,
		JVX_SIZE_UNSELECTED, /* buffersize is not constrained */
		JVX_SIZE_UNSELECTED, /* samplerate is not constrained */
		JVX_DATAFORMAT_DATA, /* data format */
		JVX_DATAFORMAT_GROUP_AUDIO_PCM_DEINTERLEAVED); /* data format group */

	neg_output._set_parameters_fixed(
		2 /* 2 output channel required*/,
		JVX_SIZE_UNSELECTED, /* buffersize is not constrained */
		JVX_SIZE_UNSELECTED, /* samplerate is not constrained */
		JVX_DATAFORMAT_DATA, /* data format */
		JVX_DATAFORMAT_GROUP_AUDIO_PCM_DEINTERLEAVED); /* data format group */

	//outputArgsFromOutputParams = true;
	JVX_INITIALIZE_MUTEX(safeAccessUpdateBgrd);
}

CjvxAuNBinauralRender::~CjvxAuNBinauralRender()
{
	JVX_TERMINATE_MUTEX(safeAccessUpdateBgrd);
}

// ===================================================================

jvxErrorType
CjvxAuNBinauralRender::select(IjvxObject* owner)
{
	jvxErrorType res = CjvxBareNode1ioRearrange::select(owner);
	if (res == JVX_NO_ERROR)
	{
		genBinauralRender_node::init__global__if_ext();
		genBinauralRender_node::allocate__global__if_ext();
		genBinauralRender_node::register__global__if_ext(static_cast<CjvxProperties*>(this));

		genBinauralRender_node::init__local__hrtf_rendering();
		genBinauralRender_node::allocate__local__hrtf_rendering();
		genBinauralRender_node::register__local__hrtf_rendering(static_cast<CjvxProperties*>(this));

		genBinauralRender_node::register_callbacks(static_cast<CjvxProperties*>(this),
			set_extend_ifx_reference, update_source_direction, update_active_slot,
			reinterpret_cast<jvxHandle*>(this), NULL);

		genBinauralRender_node::global.if_ext.interf_position_direct.ptr = static_cast<IjvxPropertyExtender*>(this);
	}
	return res;
}

jvxErrorType
CjvxAuNBinauralRender::unselect()
{
	jvxErrorType res = CjvxBareNode1ioRearrange::_pre_check_unselect();
	if (res == JVX_NO_ERROR)
	{
		genBinauralRender_node::unregister_callbacks(static_cast<CjvxProperties*>(this), nullptr);
		
		genBinauralRender_node::unregister__local__hrtf_rendering(static_cast<CjvxProperties*>(this));
		genBinauralRender_node::deallocate__local__hrtf_rendering();

		genBinauralRender_node::unregister__global__if_ext(static_cast<CjvxProperties*>(this));
		genBinauralRender_node::deallocate__global__if_ext();

		res = CjvxBareNode1ioRearrange::unselect();
	}
	return res;
}

// ===================================================================

jvxErrorType
CjvxAuNBinauralRender::activate()
{
	jvxErrorType res = CjvxBareNode1ioRearrange::activate();
	if (res == JVX_NO_ERROR)
	{
		genBinauralRender_node::init__local__source_direction();
		genBinauralRender_node::allocate__local__source_direction();
		genBinauralRender_node::register__local__source_direction(static_cast<CjvxProperties*>(this));

		// register property
		genBinauralRender_node::associate__local__source_direction(
			static_cast<CjvxProperties*>(this),
			source_direction_angles_deg_set, 2,
			source_direction_angles_deg_inuse, 2);

		threads = reqInstTool<IjvxThreads>(_common_set.theToolsHost, JVX_COMPONENT_THREADS);
		if (threads.cpPtr)
		{
			threads.cpPtr->initialize(this);
		}

	}
	return res;
}

jvxErrorType
CjvxAuNBinauralRender::deactivate()
{
	jvxErrorType res = CjvxBareNode1ioRearrange::_pre_check_deactivate();
	if (res == JVX_NO_ERROR)
	{
		if (threads.cpPtr)
		{
			threads.cpPtr->terminate();
		}
		retInstTool<IjvxThreads>(_common_set.theToolsHost, JVX_COMPONENT_THREADS, threads);
		
		genBinauralRender_node::deassociate__local__source_direction(static_cast<CjvxProperties*>(this));
		genBinauralRender_node::unregister__local__source_direction(static_cast<CjvxProperties*>(this));
		genBinauralRender_node::deallocate__local__source_direction();

		res = CjvxBareNode1ioRearrange::deactivate();
	}
	return res;
}

/*
void
CjvxAuNBinauralRender::from_input_to_output()
{	
	// Copy output properties from input properties
	update_output_params_from_input_params();

	// Copy output params (properties) to dataOut field - channels excluded
	update_ldesc_from_output_params_on_test();

	// Apply the neg_output constraints
	constrain_ldesc_from_neg_params(neg_output);
}
*/
// ===================================================================

jvxErrorType
CjvxAuNBinauralRender::prepare_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) 
{
	jvxErrorType res = CjvxBareNode1io::prepare_connect_icon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	jvxErrorType resL = JVX_NO_ERROR;
	if (res != JVX_NO_ERROR) return res;

	// From this point the processing parameters are fixed. You need to allocate all buffers
	// for your algorithm now. The processing parameters can be found here:
	// _common_set_icon.theData_in->con_params

	jvxSize samplerate = _common_set_icon.theData_in->con_params.rate;
	theHrtfDispenser->bind(&samplerate); 

	// Allocate renderer
	render_pri = allocate_renderer(_common_set_icon.theData_in->con_params.buffersize,
		this->source_direction_angles_deg_inuse[0], this->source_direction_angles_deg_inuse[1]);

	// This can be outside the critical section since at this position, we have only the main thread!!
	updateHRir = jvxRenderingUpdateStatus::JVX_RENDERING_UPDATE_ACCEPT_NEW_TASK;
	updateDBase = jvxRenderingUpdateStatus::JVX_RENDERING_UPDATE_ACCEPT_NEW_TASK;

	genBinauralRender_node::local.feedback.num_success_update_hrir.value = 0;
	genBinauralRender_node::local.feedback.num_failed_update_hrir.value = 0;

	threads.cpPtr->start();

	return res;
}

// ===================================================================

jvxErrorType JVX_CALLINGCONVENTION CjvxAuNBinauralRender::start_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = CjvxBareNode1io::start_connect_icon(fdb);

	return res;
}

jvxErrorType JVX_CALLINGCONVENTION CjvxAuNBinauralRender::stop_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	return CjvxBareNode1io::stop_connect_icon(fdb);
}

jvxErrorType
CjvxAuNBinauralRender::postprocess_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType resL = JVX_NO_ERROR;
	jvxErrorType res = CjvxBareNode1io::postprocess_connect_icon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	if (res != JVX_NO_ERROR) return res;

	threads.cpPtr->stop(5000);
	updateHRir = jvxRenderingUpdateStatus::JVX_RENDERING_UPDATE_OFF;
	updateDBase = jvxRenderingUpdateStatus::JVX_RENDERING_UPDATE_OFF;

	deallocate_renderer(render_pri);
	if (render_sec)
	{
		deallocate_renderer(render_sec);
	}

	// At this point the processing is completely done. You need to deallocate all buffers
	// for your algorithm now. The processing parameters can be found here:
	// _common_set_icon.theData_in->con_params

	return res;
}

jvxErrorType
CjvxAuNBinauralRender::process_buffers_icon(jvxSize mt_mask, jvxSize idx_stage)
{
	jvxErrorType res = JVX_NO_ERROR;

	// Extract the input buffers. You will find all processing parameters at
	// _common_set_icon.theData_in->con_params
	jvxData** buffers_in = jvx_process_icon_extract_input_buffers<jvxData>(_common_set_icon.theData_in, idx_stage);
	jvxData* in = buffers_in[0];

	// Extract the output buffers. You will find the processing parameters at
	// _common_set_ocon.theData_out.con_params
	jvxData** buffers_out = jvx_process_icon_extract_output_buffers<jvxData>(_common_set_ocon.theData_out);
	jvxData* out_left = buffers_out[0];
	jvxData* out_right = buffers_out[1];

	const jvxSize num_channels_in = _common_set_icon.theData_in->con_params.number_channels;
	const jvxSize num_channels_out = _common_set_ocon.theData_out.con_params.number_channels;

	jvxBool runWithUpdate = false;

	JVX_TRY_LOCK_MUTEX_RESULT_TYPE resM = JVX_TRY_LOCK_MUTEX_NO_SUCCESS;
	JVX_TRY_LOCK_MUTEX(resM, safeAccessUpdateBgrd);
	if (resM == JVX_TRY_LOCK_MUTEX_SUCCESS)
	{
		if (updateDBase == jvxRenderingUpdateStatus::JVX_RENDERING_UPDATE_READY)
		{
			// Include the new renderer
			if (render_sec)
			{
				jvxOneRenderCore* store = render_pri;
				render_pri = render_sec;
				render_sec = store;
			}
			updateDBase = jvxRenderingUpdateStatus::JVX_RENDERING_UPDATE_ACCEPT_NEW_TASK;

			// Unblock the rir updates
			updateHRir = jvxRenderingUpdateStatus::JVX_RENDERING_UPDATE_ACCEPT_NEW_TASK;
		}

		if (updateHRir == jvxRenderingUpdateStatus::JVX_RENDERING_UPDATE_READY)
		{
			runWithUpdate = true;
			jvx_firfft_cf_process_update_weights(&render_pri->firfftcf_left, in, out_left, render_pri->updatedWeightsLeft, false);
			jvx_firfft_cf_process_update_weights(&render_pri->firfftcf_right, in, out_right, render_pri->updatedWeightsRight, false);
			updateHRir = jvxRenderingUpdateStatus::JVX_RENDERING_UPDATE_ACCEPT_NEW_TASK;
		}
		JVX_UNLOCK_MUTEX(safeAccessUpdateBgrd);
	}

	if (!runWithUpdate)
	{
		jvx_firfft_cf_process(&render_pri->firfftcf_left, in, out_left, false);
		jvx_firfft_cf_process(&render_pri->firfftcf_right, in, out_right, false);
	}

	
	// Once you are done forward the processing flow to the ext stage. We can do this as follows:
	// 1) Only pass the flow forward to next stage without any functionality
	return _process_buffers_icon(mt_mask, idx_stage);


	// 2) Run the default implementation: Do a circular buffer copy. Deactivate this option if you do your own processing
	// res = CjvxBareNode1io::process_buffers_icon(mt_mask, idx_stage);
	// return res;
}

void
CjvxAuNBinauralRender::update_hrirs(jvxOneRenderCore* renderer, jvxData azimuth_deg, jvxData inclination_deg) 
{
	jvxSize slotIdHrtfs = jvx_bitfieldSelection2Id(genBinauralRender_node::local.hrtf_rendering.active_slot_hrtf);

	jvxSize loadId = JVX_SIZE_UNSELECTED;
	// std::cout << "New angles: azimuth: " << azimuth_deg << ", inclination: " << inclination_deg << std::endl;

	// Update state array.
	this->source_direction_angles_deg_inuse[0] = azimuth_deg;
	this->source_direction_angles_deg_inuse[1] = inclination_deg;

	if (JVX_CHECK_SIZE_SELECTED(slotIdHrtfs))
	{
		jvxSize length_hrir;

		// If the update is ongoing, there might be a moment 
		this->theHrtfDispenser->get_length_hrir(length_hrir, &loadId, slotIdHrtfs);
		if (renderer->loadId == loadId)
		{
			jvxErrorType res = this->theHrtfDispenser->copy_closest_hrir_pair(azimuth_deg, inclination_deg,
				renderer->buffer_hrir_left, renderer->buffer_hrir_right, renderer->length_buffer_hrir, slotIdHrtfs);
			if (res == JVX_NO_ERROR)
			{
				genBinauralRender_node::local.feedback.num_success_update_hrir.value++;
			}
			else
			{
				genBinauralRender_node::local.feedback.num_failed_update_hrir.value++;
				/*
				std::cout << __FUNCTION__ << " Warning: on update of position, the request to return hrir buffers failed with error <" <<
					jvxErrorType_descr(res) << ">." << std::endl;
					*/
			}
		}
		else
		{
			std::cout << __FUNCTION__ << " Warning: on update of position, the renderer is linked to database id #" <<
				renderer->loadId << " whereas the currently active database id is #" << loadId << std::endl;
		}
	}
	else
	{ 
		std::cout << __FUNCTION__ << " Warning: No valid slot configured for HRTF request." << std::endl;
	}
}

void 
CjvxAuNBinauralRender::allocate_hrir_buffers(jvxOneRenderCore* renderer)
{
	JVX_SAFE_ALLOCATE_FIELD_CPP_Z(renderer->buffer_hrir_left, jvxData, renderer->length_buffer_hrir);
	JVX_SAFE_ALLOCATE_FIELD_CPP_Z(renderer->buffer_hrir_right, jvxData, renderer->length_buffer_hrir);
}

void 
CjvxAuNBinauralRender::deallocate_hrir_buffers(jvxOneRenderCore* renderer)
{
	JVX_SAFE_DELETE_FIELD(renderer->buffer_hrir_left);
	JVX_SAFE_DELETE_FIELD(renderer->buffer_hrir_right);
}

/*
void
CjvxAuNPlayChannelId::test_set_output_parameters()
{

}
*/

jvxErrorType 
CjvxAuNBinauralRender::supports_prop_extender_type(jvxPropertyExtenderType tp)
{
	if (tp == jvxPropertyExtenderType::JVX_PROPERTY_EXTENDER_SPATIAL_POSITION_DIRECT)
	{
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_UNSUPPORTED;
}

jvxErrorType 
CjvxAuNBinauralRender::prop_extender_specialization(jvxHandle** prop_extender, jvxPropertyExtenderType tp)
{
	switch (tp)
	{
	case jvxPropertyExtenderType::JVX_PROPERTY_EXTENDER_SPATIAL_POSITION_DIRECT:
		if (prop_extender)
		{
			*prop_extender = reinterpret_cast<jvxHandle*>(static_cast<IjvxPropertyExtenderSpatialDirectionDirect*>(this));
		}
		return JVX_NO_ERROR;
		break;
	default:
		break;
	}
	return JVX_ERROR_UNSUPPORTED;
}

jvxErrorType
CjvxAuNBinauralRender::set_spatial_position(jvxData valueAzimuth, jvxData valueInclination)
{
	try_trigger_update_position(valueAzimuth, valueInclination);
	return JVX_NO_ERROR;
}


// ==================================================================

jvxErrorType 
CjvxAuNBinauralRender::startup(jvxInt64 timestamp_us)
{
	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxAuNBinauralRender::expired(jvxInt64 timestamp_us, jvxSize* delta_ms) 
{
	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxAuNBinauralRender::wokeup(jvxInt64 timestamp_us, jvxSize* delta_ms)
{
	JVX_LOCK_MUTEX(safeAccessUpdateBgrd);
	if (updateHRir == jvxRenderingUpdateStatus::JVX_RENDERING_UPDATE_IN_OPERATION)
	{
		this->update_hrirs(render_pri, newAzimuth, newInclination);

		// These updates may and should occur in this third thread since the new position may
		// be triggered from the main thread as well as from the processing thread.
		// If triggered from within the process thread, it is dangerous to run the update in
		// the given thread since it might block the processing for a too long time. Therefore the third thread.
		jvx_firfft_cf_compute_weights(&render_pri->firfftcf_left, render_pri->buffer_hrir_left, render_pri->length_buffer_hrir);
		jvx_firfft_cf_copy_weights(&render_pri->firfftcf_left, render_pri->updatedWeightsLeft, render_pri->lUpdatedWeights);

		jvx_firfft_cf_compute_weights(&render_pri->firfftcf_right, render_pri->buffer_hrir_right, render_pri->length_buffer_hrir);
		jvx_firfft_cf_copy_weights(&render_pri->firfftcf_right, render_pri->updatedWeightsRight, render_pri->lUpdatedWeights);

		updateHRir = jvxRenderingUpdateStatus::JVX_RENDERING_UPDATE_READY;
	}
	JVX_UNLOCK_MUTEX(safeAccessUpdateBgrd);
	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxAuNBinauralRender::stopped(jvxInt64 timestamp_us) 
{
	return JVX_NO_ERROR;
}

// ===================================================================
// ===================================================================
jvxOneRenderCore*
CjvxAuNBinauralRender::allocate_renderer(jvxSize bsize, jvxData startAz, jvxData startInc)
{
	jvxOneRenderCore* render_inst = nullptr;
	JVX_SAFE_ALLOCATE_OBJECT(render_inst, jvxOneRenderCore);

	jvxSize slotIdHrtfs = jvx_bitfieldSelection2Id(genBinauralRender_node::local.hrtf_rendering.active_slot_hrtf);
	assert(JVX_CHECK_SIZE_SELECTED(slotIdHrtfs));

	jvxErrorType res = theHrtfDispenser->get_length_hrir(render_inst->length_buffer_hrir, &render_inst->loadId, slotIdHrtfs);
	assert(res == JVX_NO_ERROR);

	// Allocate the hrir buffers
	allocate_hrir_buffers(render_inst);
	this->update_hrirs(render_inst, startAz, startInc);

	jvx_firfft_initCfg(&render_inst->firfftcf_left);
	jvx_firfft_initCfg(&render_inst->firfftcf_right);

	render_inst->firfftcf_left.init.bsize = bsize;
	render_inst->firfftcf_left.init.fir = render_inst->buffer_hrir_left;
	render_inst->firfftcf_left.init.lFir = render_inst->length_buffer_hrir;
	render_inst->firfftcf_left.init.type = JVX_FIRFFT_FLEXIBLE_FIR;

	render_inst->firfftcf_right.init.bsize = bsize;
	render_inst->firfftcf_right.init.fir = render_inst->buffer_hrir_right;
	render_inst->firfftcf_right.init.lFir = render_inst->length_buffer_hrir;
	render_inst->firfftcf_right.init.type = JVX_FIRFFT_FLEXIBLE_FIR;

	jvx_firfft_cf_init(&render_inst->firfftcf_left);
	jvx_firfft_cf_init(&render_inst->firfftcf_right);

	render_inst->lUpdatedWeights = render_inst->firfftcf_left.derived.lFirW;
	JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(render_inst->updatedWeightsLeft, jvxDataCplx, render_inst->lUpdatedWeights);
	JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(render_inst->updatedWeightsRight, jvxDataCplx, render_inst->lUpdatedWeights);
	return render_inst;
}

void
CjvxAuNBinauralRender::deallocate_renderer(jvxOneRenderCore*& render_inst)
{
	JVX_DSP_SAFE_DELETE_FIELD(render_inst->updatedWeightsLeft);
	JVX_DSP_SAFE_DELETE_FIELD(render_inst->updatedWeightsRight);
	render_inst->lUpdatedWeights = 0;

	jvx_firfft_cf_terminate(&render_inst->firfftcf_left);
	jvx_firfft_cf_terminate(&render_inst->firfftcf_right);

	deallocate_hrir_buffers(render_inst);

	JVX_SAFE_DELETE_OBJECT(render_inst);
}
