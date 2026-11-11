#include "CjvxSpNDemultiplexer.h"

CjvxSpNDemultiplexerInputConnector::CjvxSpNDemultiplexerInputConnector(CjvxSpNDemultiplexer* parentArg, jvxSize idxArg) :
	CjvxSingleInputConnector(false), parent(parentArg), idx(idxArg)
{
	// Each connector can be connected exactly once
	allowSingleConnect = true;
}

jvxErrorType
CjvxSpNDemultiplexerInputConnector::available_to_connect_icon()
{
	jvxErrorType res = parent->check_input_connector_available(this, nullptr);
	if (res == JVX_NO_ERROR)
	{
		res = CjvxSingleInputConnector::available_to_connect_icon();
	}
	return res;
}

jvxErrorType
CjvxSpNDemultiplexerInputConnector::select_connect_icon(IjvxConnectorBridge* obj, IjvxConnectionMaster* master,
	IjvxDataConnectionCommon* ass_connection_common, IjvxInputConnector** replace_connector)
{
	jvxErrorType res = parent->check_input_connector_available(this, ass_connection_common);
	if (res == JVX_NO_ERROR)
	{
		res = CjvxSingleInputConnector::select_connect_icon(obj, master, ass_connection_common, replace_connector);
	}
	return res;
}

// =====================================================================================
// All connection calls are handled in the demultiplexer
// =====================================================================================

jvxErrorType
CjvxSpNDemultiplexerInputConnector::connect_connect_icon(jvxLinkDataDescriptor* theData JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	return parent->icon_connect(this, theData JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
}

jvxErrorType
CjvxSpNDemultiplexerInputConnector::disconnect_connect_icon(jvxLinkDataDescriptor* theData JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	return parent->icon_disconnect(this, theData JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
}

jvxErrorType
CjvxSpNDemultiplexerInputConnector::test_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	return parent->icon_test(this JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
}

jvxErrorType
CjvxSpNDemultiplexerInputConnector::prepare_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	return parent->icon_prepare(this JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
}

jvxErrorType
CjvxSpNDemultiplexerInputConnector::postprocess_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	return parent->icon_postprocess(this JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
}

jvxErrorType
CjvxSpNDemultiplexerInputConnector::start_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	return parent->icon_start(this JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
}

jvxErrorType
CjvxSpNDemultiplexerInputConnector::stop_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	return parent->icon_stop(this JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
}

jvxErrorType
CjvxSpNDemultiplexerInputConnector::process_start_icon(
	jvxSize pipeline_offset,
	jvxSize* idx_stage,
	jvxSize tobeAccessedByStage,
	callback_process_start_in_lock clbk,
	jvxHandle* priv_ptr)
{
	return parent->icon_process_start(this, pipeline_offset, idx_stage, tobeAccessedByStage, clbk, priv_ptr);
}

jvxErrorType
CjvxSpNDemultiplexerInputConnector::process_buffers_icon(jvxSize mt_mask, jvxSize idx_stage)
{
	return parent->icon_process_buffers(this, mt_mask, idx_stage);
}

jvxErrorType
CjvxSpNDemultiplexerInputConnector::process_stop_icon(jvxSize idx_stage,
	jvxBool shift_fwd,
	jvxSize tobeAccessedByStage,
	callback_process_stop_in_lock clbk,
	jvxHandle* priv_ptr)
{
	return parent->icon_process_stop(this, idx_stage, shift_fwd, tobeAccessedByStage, clbk, priv_ptr);
}

// =====================================================================================

jvxErrorType
CjvxSpNDemultiplexerInputConnector::number_next(jvxSize* num)
{
	if (num)
	{
		*num = 1;
	}
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxSpNDemultiplexerInputConnector::reference_next_handle(jvxSize idx, IjvxConnectionIterator** next, jvxApiString* nmOcon, jvxApiString* nmIcon)
{
	return parent->icon_reference_next_handle(idx, next, nmOcon, nmIcon);
}
