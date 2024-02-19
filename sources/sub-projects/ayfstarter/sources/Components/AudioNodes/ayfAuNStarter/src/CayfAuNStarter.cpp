#include "jvx.h"
#include "CayfAuNStarter.h"

CayfAuNStarter::CayfAuNStarter(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE):
	AYF_AUDIO_NODE_BASE_CLASS(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
{
	_common_set.theComponentType.unselected(JVX_COMPONENT_AUDIO_NODE);
	_common_set.theObjectSpecialization = reinterpret_cast<jvxHandle*>(static_cast<IjvxNode*>(this));
	_common_set.thisisme = static_cast<IjvxObject*>(this);	

	memset(&processing_lib, 0, sizeof(processing_lib));
}

CayfAuNStarter::~CayfAuNStarter()
{
}

//! ================================ STEP II ============================================
jvxErrorType 
CayfAuNStarter::activate()
{
	jvxErrorType res = AYF_AUDIO_NODE_BASE_CLASS::activate();
	if (res == JVX_NO_ERROR)
	{
		ayf_starter_init(&processing_lib);
	}
	return res;
}

jvxErrorType 
CayfAuNStarter::deactivate()
{
	jvxErrorType res = AYF_AUDIO_NODE_BASE_CLASS::_pre_check_deactivate();
	if (res == JVX_NO_ERROR)
	{
		// 
		res = AYF_AUDIO_NODE_BASE_CLASS::deactivate();
	}
	return res;
}

/** 
 * Prepare processing, that is, allocate all buffers and required structures. We will use the allocated memory
 * during processing until postprocessing is run.
 */
jvxErrorType
CayfAuNStarter::prepare_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = AYF_AUDIO_NODE_BASE_CLASS::prepare_connect_icon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	if (res != JVX_NO_ERROR) return res;
	ayf_starter_prepare(&processing_lib);
	return res;
}

/**
 * Process frame-by-frame the inout signal into an output signal. We copy the input data to the output buffer.
 * Once, local processing is done, we forward processing towards the next component. Note that processing may be 
 * in-place - in that case we need to make sure that we do not destroy the data since input may be overriden while
 * producing the output data.
 */
jvxErrorType
CayfAuNStarter::process_buffers_icon(jvxSize mt_mask, jvxSize idx_stage)
{
	jvxData** buffers_in = jvx_process_icon_extract_input_buffers<jvxData>(_common_set_icon.theData_in, idx_stage);
	jvxData** buffers_out = jvx_process_icon_extract_output_buffers<jvxData>(_common_set_ocon.theData_out);

	// Involve the c-library for processing
	ayf_starter_process(&processing_lib, buffers_in, buffers_out, 
		_common_set_icon.theData_in->con_params.number_channels, 
		_common_set_ocon.theData_out.con_params.number_channels,
		_common_set_icon.theData_in->con_params.buffersize);

	return fwd_process_buffers_icon(mt_mask, idx_stage);
}

/**
 * Run the post processing, that is, deallocate all buffers and required structures which we allocated on prepare.
 */
jvxErrorType
CayfAuNStarter::postprocess_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = AYF_AUDIO_NODE_BASE_CLASS::postprocess_connect_icon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	if (res != JVX_NO_ERROR) return res;
	ayf_starter_postprocess(&processing_lib);
	return res;
}

