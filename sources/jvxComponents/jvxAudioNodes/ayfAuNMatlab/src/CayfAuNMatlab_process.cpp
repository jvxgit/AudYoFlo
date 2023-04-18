#include "CayfAuNMatlab.h"
#include "jvx-helpers-cpp.h"

#ifdef JVX_PROJECT_NAMESPACE
namespace JVX_PROJECT_NAMESPACE {
#endif

jvxErrorType
CayfAuNMatlab::prepare_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;

#ifdef JVX_EXTERNAL_CALL_ENABLED
	CjvxMexCalls::prepare_sender_to_receiver(_common_set_icon.theData_in);
#endif

	res = JVX_LOCAL_BASE_CLASS::prepare_connect_icon(JVX_CONNECTION_FEEDBACK_CALL(fdb));

#ifdef JVX_EXTERNAL_CALL_ENABLED
	if (res == JVX_NO_ERROR)
	{
		CjvxMexCalls::prepare_complete_receiver_to_sender(_common_set_icon.theData_in, &_common_set_ocon.theData_out);
	}
	else
	{
		CjvxMexCalls::postprocess_sender_to_receiver(_common_set_icon.theData_in);
	}
#endif
	return res;

}

jvxErrorType
CayfAuNMatlab::postprocess_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;

#ifdef JVX_EXTERNAL_CALL_ENABLED
	CjvxMexCalls::before_postprocess_receiver_to_sender(_common_set_icon.theData_in);
#endif

	res = JVX_LOCAL_BASE_CLASS::postprocess_connect_icon(JVX_CONNECTION_FEEDBACK_CALL(fdb));

#ifdef JVX_EXTERNAL_CALL_ENABLED
	CjvxMexCalls::postprocess_sender_to_receiver(_common_set_icon.theData_in);
#endif
	return res;
}

// ===========================================================================

jvxErrorType
CayfAuNMatlab::process_buffers_icon(jvxSize mt_mask, jvxSize idx_stage)
{
	jvxErrorType res = JVX_NO_ERROR;

#ifdef JVX_EXTERNAL_CALL_ENABLED
	jvxBool isPresent = false;
	jvxBool engaged = false;

	// Pass control to Matlab processing
	CjvxMexCalls::is_extcall_reference_present(&isPresent);
	CjvxMexCalls::is_matlab_processing_engaged(&engaged);
	if (isPresent && engaged)
	{

		// This lets Matlab run one frame of processing
		// ======================================================================================
		res = CjvxMexCalls::process_buffers_icon(_common_set_icon.theData_in, &_common_set_ocon.theData_out);
		// ======================================================================================

		return _process_buffers_icon(mt_mask, idx_stage);
	}
	else
	{

#endif
		// Here let us locate the local c code
		
		// Get the input buffers
		jvxData** buffers_in = jvx_process_icon_extract_input_buffers<jvxData>(_common_set_icon.theData_in, idx_stage);

		// Get the output buffers
		jvxData** buffers_out = jvx_process_icon_extract_output_buffers<jvxData>(_common_set_ocon.theData_out);

#ifdef JVX_EXTERNAL_CALL_ENABLED
	}
#endif

	// Forward calling chain
	return JVX_LOCAL_BASE_CLASS::process_buffers_icon(mt_mask, idx_stage);
}

#ifdef JVX_PROJECT_NAMESPACE
}
#endif
