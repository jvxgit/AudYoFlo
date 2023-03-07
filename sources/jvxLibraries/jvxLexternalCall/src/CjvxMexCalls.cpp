#include "jvx.h"
#include "CjvxMexCalls.h"
#include "CjvxMexCalls_prv.h"


CjvxMexCalls::CjvxMexCalls()
{
	theSubclass = reinterpret_cast<jvxHandle*>(new CjvxMexCalls_prv(*this));
}

CjvxMexCalls::~CjvxMexCalls()
{
	CjvxMexCalls_prv* prv = reinterpret_cast<CjvxMexCalls_prv*>(theSubclass);
	delete(prv);
}

jvxErrorType
CjvxMexCalls::select(IjvxHiddenInterface* theHost, CjvxProperties* theProps, const char* c_postfix, const char* componentName)
{
	CjvxMexCalls_prv* prv = reinterpret_cast<CjvxMexCalls_prv*>(theSubclass);

	return prv->select(theHost, theProps,  c_postfix,componentName);
}

jvxErrorType
CjvxMexCalls::unselect()
{
	CjvxMexCalls_prv* prv = reinterpret_cast<CjvxMexCalls_prv*>(theSubclass);
	return prv->unselect();
}

jvxErrorType
CjvxMexCalls::activate()
{
	CjvxMexCalls_prv* prv = reinterpret_cast<CjvxMexCalls_prv*>(theSubclass);
	return prv->activate();
}

jvxErrorType
CjvxMexCalls::deactivate()
{
	CjvxMexCalls_prv* prv = reinterpret_cast<CjvxMexCalls_prv*>(theSubclass);
	return prv->deactivate();
};

jvxErrorType
CjvxMexCalls::put_configuration(jvxCallManagerConfiguration* callConf, IjvxConfigProcessor* processor,
				jvxState theState,
				jvxHandle* sectionToContainAllSubsectionsForMe,
				const char* filename,
				jvxInt32 lineno)
{
	CjvxMexCalls_prv* prv = reinterpret_cast<CjvxMexCalls_prv*>(theSubclass);
	return prv->put_configuration(callConf, processor,
				theState, sectionToContainAllSubsectionsForMe,
				filename, lineno);
}

jvxErrorType
CjvxMexCalls::get_configuration(jvxCallManagerConfiguration* callConf, IjvxConfigProcessor* processor,
				jvxHandle* sectionWhereToAddAllSubsections)
{
	CjvxMexCalls_prv* prv = reinterpret_cast<CjvxMexCalls_prv*>(theSubclass);
	return prv->get_configuration(callConf,  processor,
				 sectionWhereToAddAllSubsections);
}

jvxErrorType
CjvxMexCalls::prepare()
{
	CjvxMexCalls_prv* prv = reinterpret_cast<CjvxMexCalls_prv*>(theSubclass);
	return prv->prepare();
}

jvxErrorType
CjvxMexCalls::prepare_sender_to_receiver(jvxLinkDataDescriptor* theData)
{
	CjvxMexCalls_prv* prv = reinterpret_cast<CjvxMexCalls_prv*>(theSubclass);
	return prv->prepare_sender_to_receiver( theData);
}

jvxErrorType
CjvxMexCalls::prepare_connect_icon_enter(jvxLinkDataDescriptor* theData)
{
	CjvxMexCalls_prv* prv = reinterpret_cast<CjvxMexCalls_prv*>(theSubclass);
	return prv->prepare_sender_to_receiver(theData);
}

jvxErrorType
CjvxMexCalls::prepare_connect_icon_leave(jvxLinkDataDescriptor* theData_in, jvxLinkDataDescriptor* theData_out, const char** hintsTags, const char** hintsValues, jvxSize numHints)
{
	CjvxMexCalls_prv* prv = reinterpret_cast<CjvxMexCalls_prv*>(theSubclass);
	return prv->prepare_complete_receiver_to_sender( theData_in, theData_out, hintsTags, hintsValues,  numHints);
}

jvxErrorType
CjvxMexCalls::prepare_complete_receiver_to_sender(jvxLinkDataDescriptor* theData_in, jvxLinkDataDescriptor* theData_out, const char** hintsTags, const char** hintsValues, jvxSize numHints)
{
	CjvxMexCalls_prv* prv = reinterpret_cast<CjvxMexCalls_prv*>(theSubclass);
	return prv->prepare_complete_receiver_to_sender(theData_in, theData_out, hintsTags, hintsValues, numHints);
}

jvxErrorType
CjvxMexCalls::process_st(jvxLinkDataDescriptor* theData, jvxSize idx_sender_to_receiver, jvxSize idx_receiver_to_sender, jvxLinkDataDescriptor* theData_out)
{
	CjvxMexCalls_prv* prv = reinterpret_cast<CjvxMexCalls_prv*>(theSubclass);
	return prv->process_st(theData, idx_sender_to_receiver, idx_receiver_to_sender, theData_out);
}

jvxErrorType
CjvxMexCalls::process_buffers_icon(jvxLinkDataDescriptor* theData_in, jvxLinkDataDescriptor* theData_out)
{
	jvxErrorType res = JVX_NO_ERROR;
	CjvxMexCalls_prv* prv = reinterpret_cast<CjvxMexCalls_prv*>(theSubclass);

	// This is for convenience: support old type of processing principle
	//theData_in->con_compat.user_hints = theData_out->con_data.user_hints;
	theData_in->con_compat.buffersize = theData_out->con_params.buffersize;
	theData_in->con_compat.format = theData_out->con_params.format;
	theData_in->con_compat.from_receiver_buffer_allocated_by_sender = theData_out->con_data.buffers;
	theData_in->con_compat.number_buffers = theData_out->con_data.number_buffers;
	theData_in->con_compat.number_channels = theData_out->con_params.number_channels;
	theData_in->con_compat.rate = theData_out->con_params.rate;

	theData_in->con_compat.ext.segmentation_x = theData_out->con_params.segmentation_x;
	theData_in->con_compat.ext.segmentation_y = theData_out->con_params.segmentation_y;
	theData_in->con_compat.ext.subformat = theData_out->con_params.format_group;
	theData_in->con_compat.ext.hints = theData_out->con_params.hints;

	res = prv->process_st(theData_in, *theData_in->con_pipeline.idx_stage_ptr, *theData_out->con_pipeline.idx_stage_ptr, theData_out);

	theData_in->con_compat.user_hints = NULL;
	theData_in->con_compat.buffersize = 0;
	theData_in->con_compat.format = JVX_DATAFORMAT_NONE;
	theData_in->con_compat.from_receiver_buffer_allocated_by_sender = NULL;
	theData_in->con_compat.number_buffers = 0;
	theData_in->con_compat.number_channels = 0;
	theData_in->con_compat.rate = 0;
	theData_in->con_compat.ext.hints = 0;
	theData_in->con_compat.ext.segmentation_x = 0;
	theData_in->con_compat.ext.segmentation_y = 0;
	theData_in->con_compat.ext.subformat = JVX_DATAFORMAT_GROUP_NONE;

	return res;
}

jvxErrorType
CjvxMexCalls::is_extcall_reference_present(jvxBool* isPresent)
{
	CjvxMexCalls_prv* prv = reinterpret_cast<CjvxMexCalls_prv*>(theSubclass);
	return prv->is_extcall_reference_present(isPresent);
}

jvxErrorType
CjvxMexCalls::is_matlab_processing_engaged(jvxBool* isEngaged)
{
	CjvxMexCalls_prv* prv = reinterpret_cast<CjvxMexCalls_prv*>(theSubclass);
	return prv->is_matlab_processing_engaged(isEngaged);
}

jvxErrorType
CjvxMexCalls::before_postprocess_receiver_to_sender(jvxLinkDataDescriptor* theData)
{
	CjvxMexCalls_prv* prv = reinterpret_cast<CjvxMexCalls_prv*>(theSubclass);
	return prv->before_postprocess_receiver_to_sender( theData);
}

jvxErrorType
CjvxMexCalls::postprocess_connect_icon_enter(jvxLinkDataDescriptor* theData)
{
	CjvxMexCalls_prv* prv = reinterpret_cast<CjvxMexCalls_prv*>(theSubclass);
	return prv->before_postprocess_receiver_to_sender(theData);
}

jvxErrorType
CjvxMexCalls::postprocess_sender_to_receiver(jvxLinkDataDescriptor* theData)
{
	CjvxMexCalls_prv* prv = reinterpret_cast<CjvxMexCalls_prv*>(theSubclass);
	return prv->postprocess_sender_to_receiver(theData);
}

jvxErrorType
CjvxMexCalls::postprocess_connect_icon_leave(jvxLinkDataDescriptor* theData)
{
	CjvxMexCalls_prv* prv = reinterpret_cast<CjvxMexCalls_prv*>(theSubclass);
	return prv->postprocess_sender_to_receiver(theData);
}

jvxErrorType 
CjvxMexCalls::postprocess()
{
	CjvxMexCalls_prv* prv = reinterpret_cast<CjvxMexCalls_prv*>(theSubclass);
	return prv->postprocess();
}

jvxErrorType
CjvxMexCalls::extcall_lasterror(jvxApiError* theErr)
{
	CjvxMexCalls_prv* prv = reinterpret_cast<CjvxMexCalls_prv*>(theSubclass);
	return prv->extcall_lasterror(theErr);
}


jvxErrorType 
CjvxMexCalls::extcall_set(const char* descr, int level, int id)
{
	CjvxMexCalls_prv* prv = reinterpret_cast<CjvxMexCalls_prv*>(theSubclass);
	return prv->extcall_set(descr, level, id);
}


