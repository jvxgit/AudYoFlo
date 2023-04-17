#include "jvx.h"
#include "CjvxAudioNode_mexCall.h"
//#include "mcg_exports_matlab.h"

CjvxAudioNode_mexCall::CjvxAudioNode_mexCall(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE):
	CjvxAudioNode(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
{
	_common_set.theComponentType.unselected(JVX_COMPONENT_AUDIO_NODE);
	_common_set.theObjectSpecialization = reinterpret_cast<jvxHandle*>(static_cast<IjvxNode*>(this));
	_common_set.thisisme = static_cast<IjvxObject*>(this);
	_common_spec_proc_set.thisisme = static_cast<IjvxObject*>(this);
	_theExtCallObjectName = "jvxAuNmexCall";
}

CjvxAudioNode_mexCall::~CjvxAudioNode_mexCall()
{
}

/*
JVX_MEXCALL_UNSELECT_UNREGISTER(CjvxPmexCallDevice, CjvxSpectrumProcessorDevice, _theExtCallObjectName)
JVX_MEXCALL_ACTIVATE(CjvxPmexCallDevice, CjvxSpectrumProcessorDevice)
JVX_MEXCALL_DEACTIVATE(CjvxPmexCallDevice, CjvxSpectrumProcessorDevice)
JVX_MEXCALL_PREPARE(CjvxPmexCallDevice, CjvxSpectrumProcessorDevice)
JVX_MEXCALL_POSTPROCESS(CjvxPmexCallDevice, CjvxSpectrumProcessorDevice)
//JVX_MEXCALL_PREPARE_COMPLETE_RECEIVER_TO_SENDER(CjvxPmexCallDevice, CjvxSpectrumProcessorDevice)
JVX_MEXCALL_POSTPROCESS_SENDER_TO_RECEIVER(CjvxPmexCallDevice, CjvxSpectrumProcessorDevice)
JVX_MEXCALL_BEFORE_POSTPROCESS_RECEIVER_TO_SENDER(CjvxPmexCallDevice, CjvxSpectrumProcessorDevice)
JVX_MEXCALL_PROCESS(CjvxPmexCallDevice, CjvxSpectrumProcessorDevice)
JVX_MEXCALL_PUT_CONFIGURATION_NO_BASE(CjvxPmexCallDevice)
JVX_MEXCALL_GET_CONFIGURATION_NO_BASE(CjvxPmexCallDevice)
*/

JVX_MEXCALL_SELECT(CjvxAudioNode_mexCall, CjvxAudioNode, _theExtCallObjectName)
/*
jvxErrorType
CjvxAudioNode_mexCall::select()
{
	jvxErrorType res = JVX_NO_ERROR;
	res = CjvxAudioNode::select();
	if(res == JVX_NO_ERROR)
	{
		res = CjvxMexCalls::select(_common_set_min.theHostRef, static_cast<CjvxProperties*>(this));
	}
	return(res);
}
*/

JVX_MEXCALL_UNSELECT(CjvxAudioNode_mexCall, CjvxAudioNode, _theExtCallObjectName)
/*
jvxErrorType
CjvxAudioNode_mexCall::unselect()
{
	jvxErrorType res = JVX_NO_ERROR;
	res = CjvxAudioNode::unselect();
	if(res == JVX_NO_ERROR)
	{
		res = CjvxMexCalls::unselect();
	}
	return(res);
}
*/

JVX_MEXCALL_ACTIVATE(CjvxAudioNode_mexCall, CjvxAudioNode)
/*
jvxErrorType
CjvxAudioNode_mexCall::activate()
{
	jvxErrorType res = CjvxAudioNode::activate();
	if(res == JVX_NO_ERROR)
	{
		res = CjvxMexCalls::activate();
	}
	return(res);
};
*/

JVX_MEXCALL_DEACTIVATE(CjvxAudioNode_mexCall, CjvxAudioNode)
/*
jvxErrorType
CjvxAudioNode_mexCall::deactivate()
{
	jvxErrorType res = CjvxAudioNode::deactivate();
	if(res == JVX_NO_ERROR)
	{
		res = CjvxMexCalls::deactivate();
	}
	return(res);
};
*/

JVX_MEXCALL_PREPARE(CjvxAudioNode_mexCall, CjvxAudioNode)
/*
jvxErrorType
CjvxAudioNode_mexCall::prepare(IjvxDataProcessor** theEndpoint, jvxSize numEndpoints)
{
	jvxErrorType res = CjvxAudioNode::prepare(theEndpoint, numEndpoints);
	if(res == JVX_NO_ERROR)
	{
		res = CjvxMexCalls::prepare(theEndpoint, numEndpoints);
	}
	return(res);
}
*/

JVX_MEXCALL_PREPARE_SENDER_TO_RECEIVER(CjvxAudioNode_mexCall, CjvxAudioNode)
/*
jvxErrorType
CjvxAudioNode_mexCall::prepare_sender_to_receiver(jvxLinkDataDescriptor* theData)
{
	jvxErrorType res = CjvxAudioNode::prepare_sender_to_receiver(theData);
	if(res == JVX_NO_ERROR)
	{
		res = CjvxMexCalls::prepare_sender_to_receiver(theData);
	}
	return(res);
}
*/

JVX_MEXCALL_PREPARE_COMPLETE_RECEIVER_TO_SENDER(CjvxAudioNode_mexCall, CjvxAudioNode, &_common_set_ocon.theData_out)
/*
jvxErrorType
CjvxAudioNode_mexCall::prepare_complete_receiver_to_sender(jvxLinkDataDescriptor* theData)
{
	jvxErrorType res = CjvxAudioNode::prepare_complete_receiver_to_sender(theData);
	if(res == JVX_NO_ERROR)
	{
		res = CjvxMexCalls::prepare_complete_receiver_to_sender(theData);
	}
	return(res);
}
*/

JVX_MEXCALL_POSTPROCESS_SENDER_TO_RECEIVER(CjvxAudioNode_mexCall, CjvxAudioNode)
/*
jvxErrorType
CjvxAudioNode_mexCall::postprocess_sender_to_receiver(jvxLinkDataDescriptor* theData)
{
	jvxErrorType res = CjvxAudioNode::postprocess_sender_to_receiver(theData);
	if(res == JVX_NO_ERROR)
	{
		res = CjvxMexCalls::postprocess_sender_to_receiver(theData);
	}
	return(res);
}
*/


JVX_MEXCALL_BEFORE_POSTPROCESS_RECEIVER_TO_SENDER(CjvxAudioNode_mexCall, CjvxAudioNode)
/*
jvxErrorType
CjvxAudioNode_mexCall::before_postprocess_receiver_to_sender(jvxLinkDataDescriptor* theData)
{
	jvxErrorType res = CjvxAudioNode::before_postprocess_receiver_to_sender(theData);
	if(res == JVX_NO_ERROR)
	{
		res = CjvxMexCalls::before_postprocess_receiver_to_sender(theData);
	}
	return(res);
}
*/

JVX_MEXCALL_POSTPROCESS(CjvxAudioNode_mexCall, CjvxAudioNode)
/*
jvxErrorType
CjvxAudioNode_mexCall::postprocess()
{
	jvxErrorType res = CjvxAudioNode::postprocess();
	if(res == JVX_NO_ERROR)
	{
		res = CjvxMexCalls::postprocess();
	}
	return(res);
}
*/

JVX_MEXCALL_PROCESS(CjvxAudioNode_mexCall, CjvxAudioNode)
/*
jvxErrorType
CjvxAudioNode_mexCall::process_st(jvxLinkDataDescriptor* theData, jvxSize idx_sender_to_receiver, jvxSize idx_receiver_to_sender)
{
	return(CjvxMexCalls::process_st(theData, idx_sender_to_receiver, idx_receiver_to_sender));
};
*/

JVX_MEXCALL_PUT_CONFIGURATION_NO_BASE(CjvxAudioNode_mexCall)
JVX_MEXCALL_GET_CONFIGURATION_NO_BASE(CjvxAudioNode_mexCall)
/*
jvxErrorType
CjvxAudioNode_mexCall::put_configuration(IjvxConfigProcessor* processor, jvxHandle* sectionToContainAllSubsectionsForMe, jvxFlagTag flagtag, const char* filename, jvxInt32 lineno )
{
	return(CjvxMexCalls::put_configuration(processor, this->_common_set_min.theState, sectionToContainAllSubsectionsForMe, flagtag, filename, lineno));
}

jvxErrorType
CjvxAudioNode_mexCall::get_configuration(IjvxConfigProcessor* processor, jvxHandle* sectionWhereToAddAllSubsections, jvxFlagTag flagtag)
{
	return(CjvxMexCalls::get_configuration(processor, sectionWhereToAddAllSubsections, flagtag));
}
*/