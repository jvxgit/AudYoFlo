#include "CjvxAuNBitstreamDecoder.h"
#include "jvx-helpers-cpp.h"

jvxErrorType
CjvxAuNBitstreamDecoder::hook_test_negotiate(jvxLinkDataDescriptor* proposed JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxErrorType res = _common_set_ldslave.theData_in->con_link.connect_from->transfer_backward_ocon(
		JVX_LINKDATA_TRANSFER_COMPLAIN_DATA_SETTINGS, proposed JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
	if (res == JVX_NO_ERROR)
	{
		config_token = _common_set_ldslave.theData_in->con_params.format_spec->std_str();

		// Configure the decoder
		res = theDecoder->set_configure_token(config_token.c_str());
		assert(res == JVX_NO_ERROR);
	}
	return res;
}

jvxErrorType
CjvxAuNBitstreamDecoder::hook_test_accept(jvxLinkDataDescriptor* dataIn JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxErrorType res = hook_test_update(dataIn  JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
	res = JVX_LOCAL_BASE_CLASS::test_connect_ocon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	return res;
}

jvxErrorType
CjvxAuNBitstreamDecoder::hook_test_update(jvxLinkDataDescriptor* dataIn  JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	// We end up here if the microconnection was tested with success
	// Forward the connection parameters to the bitsream output path
	_common_set_ldslave.theData_out.con_params = dataIn->con_params;
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxAuNBitstreamDecoder::hook_forward(jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	return JVX_LOCAL_BASE_CLASS::_transfer_forward_ocon(tp, data JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
}

jvxErrorType
CjvxAuNBitstreamDecoder::hook_check_is_ready(jvxBool* is_ready, jvxApiString* astr)
{
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxAuNBitstreamDecoder::hook_prepare(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{

	// Forward the chain	
	return _prepare_connect_icon(true JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
}

jvxErrorType
CjvxAuNBitstreamDecoder::hook_postprocess(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	return _postprocess_connect_icon(true JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
}

jvxErrorType
CjvxAuNBitstreamDecoder::hook_start(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxSize idRuntime = JVX_SIZE_UNSELECTED;
	_common_set.theUniqueId->obtain_unique_id(&idRuntime, _common_set.theDescriptor.c_str());
	return _start_connect_icon(true, idRuntime JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
}

jvxErrorType
CjvxAuNBitstreamDecoder::hook_stop(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	return _stop_connect_icon(true, nullptr JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
}

jvxErrorType
CjvxAuNBitstreamDecoder::hook_process_start(
	jvxSize pipeline_offset,
	jvxSize* idx_stage,
	jvxSize tobeAccessedByStage,
	callback_process_start_in_lock clbk,
	jvxHandle* priv_ptr)
{
	jvxErrorType res = JVX_NO_ERROR;
	if (_common_set_ldslave.ocon)
	{
		// Input / output connector implementations are never zerocopy
		// We start a regular output connector here
		res = _common_set_ldslave.ocon->process_start_ocon(
			pipeline_offset,
			idx_stage,
			tobeAccessedByStage,
			clbk,
			priv_ptr);
	}
	return res;
}

jvxErrorType
CjvxAuNBitstreamDecoder::hook_process(jvxSize mt_mask, jvxSize idx_stage)
{
	jvxErrorType res = JVX_NO_ERROR;
	if (_common_set_ldslave.ocon)
	{
		return _common_set_ldslave.ocon->process_buffers_ocon(mt_mask, idx_stage);
	}
	return res;
}

jvxErrorType
CjvxAuNBitstreamDecoder::hook_process_stop(
	jvxSize idx_stage,
	jvxBool shift_fwd,
	jvxSize tobeAccessedByStage,
	callback_process_stop_in_lock clbk,
	jvxHandle* priv_ptr)
{
	jvxErrorType res = JVX_NO_ERROR;
	if (_common_set_ldslave.ocon)
	{
		// Input / output connector implementations are never zerocopy
		// We start a regular output connector here
		res = _common_set_ldslave.ocon->process_stop_ocon(
			idx_stage,
			shift_fwd,
			tobeAccessedByStage,
			clbk,
			priv_ptr);
	}
	return res;
}