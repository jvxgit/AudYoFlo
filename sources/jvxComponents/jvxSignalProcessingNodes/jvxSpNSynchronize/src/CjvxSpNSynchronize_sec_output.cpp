#include "CjvxSpNSynchronize_sec.h"
#include "CjvxSpNSynchronize.h"

// ================================================================================
// Callbacks for input connector
// ================================================================================

jvxErrorType
CjvxSpNSynchronize_sec::report_selected_connector(CjvxSingleOutputConnector* iconn)
{
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxSpNSynchronize_sec::report_unselected_connector(CjvxSingleOutputConnector* iconn)
{
	return JVX_NO_ERROR;
}

// =================================================================================

void
CjvxSpNSynchronize_sec::request_unique_id_start(CjvxSingleOutputConnector* iconn, jvxSize* uId)
{
	std::string nmCon = referencePtr->_common_set_min.theDescription + "-" + CjvxSingleInputConnector::_common_set_io_common.descriptor;

	if (referencePtr->_common_set.theUniqueId)
	{
		referencePtr->_common_set.theUniqueId->obtain_unique_id(uId, nmCon.c_str());
	}
}

void
CjvxSpNSynchronize_sec::release_unique_id_stop(CjvxSingleOutputConnector* iconn, jvxSize uId)
{
	if (referencePtr->_common_set.theUniqueId)
	{
		referencePtr->_common_set.theUniqueId->release_unique_id(uId);
	}
}

// ============================================================================================

jvxErrorType
CjvxSpNSynchronize_sec::report_started_connector(CjvxSingleOutputConnector* iconn)
{
	this->try_allocate_runtime_sync_data(2);
	return JVX_NO_ERROR;
}


jvxErrorType
CjvxSpNSynchronize_sec::report_stopped_connector(CjvxSingleOutputConnector* iconn)
{
	this->try_deallocate_runtime_sync_data(1);
	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxSpNSynchronize_sec::report_test_connector(CjvxSingleOutputConnector* iconn  JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	return JVX_NO_ERROR;
}

void 
CjvxSpNSynchronize_sec::report_process_buffers(CjvxSingleOutputConnector* iconn, jvxHandle** bufferPtrs, const jvxLinkDataDescriptor_con_params& params)
{

}