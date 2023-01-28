#include "jvx.h"
#include "CjvxSignalProcessingNode_mexCall.h"
//#include "mcg_exports_matlab.h"

CjvxSignalProcessingNode_mexCall::CjvxSignalProcessingNode_mexCall(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE):
	CjvxSignalProcessingNode(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
{
	_common_set.theComponentType.unselected(JVX_COMPONENT_AUDIO_NODE);
	_common_set.theObjectSpecialization = reinterpret_cast<jvxHandle*>(static_cast<IjvxNode*>(this));
	_common_set.thisisme = static_cast<IjvxObject*>(this);
	_common_spec_proc_set.thisisme = static_cast<IjvxObject*>(this);
}

CjvxSignalProcessingNode_mexCall::~CjvxSignalProcessingNode_mexCall()
{
}

jvxErrorType
CjvxSignalProcessingNode_mexCall::select(IjvxObject* theOwner)
{
	jvxErrorType res = JVX_NO_ERROR;
	res = CjvxSignalProcessingNode::select(theOwner);
	if(res == JVX_NO_ERROR)
	{
		res = CjvxMexCalls::select(_common_set_min.theHostRef, static_cast<CjvxProperties*>(this));
	}
	return(res);
}

jvxErrorType
CjvxSignalProcessingNode_mexCall::unselect()
{
	jvxErrorType res = JVX_NO_ERROR;
	res = CjvxSignalProcessingNode::unselect();
	if(res == JVX_NO_ERROR)
	{
		res = CjvxMexCalls::unselect();
	}
	return(res);
}

jvxErrorType
CjvxSignalProcessingNode_mexCall::activate()
{
	jvxErrorType res = CjvxSignalProcessingNode::activate();
	if(res == JVX_NO_ERROR)
	{
		res = CjvxMexCalls::activate();
	}
	return(res);
};

jvxErrorType
CjvxSignalProcessingNode_mexCall::deactivate()
{
	jvxErrorType res = CjvxSignalProcessingNode::deactivate();
	if(res == JVX_NO_ERROR)
	{
		res = CjvxMexCalls::deactivate();
	}
	return(res);
};


jvxErrorType
CjvxSignalProcessingNode_mexCall::prepare()
{
	jvxErrorType res = CjvxSignalProcessingNode::prepare();
	if(res == JVX_NO_ERROR)
	{
		res = CjvxMexCalls::prepare();
	}
	return(res);
}

jvxErrorType
CjvxSignalProcessingNode_mexCall::prepare_sender_to_receiver(jvxLinkDataDescriptor* theData)
{
	jvxErrorType res = CjvxSignalProcessingNode::prepare_sender_to_receiver(theData);
	if(res == JVX_NO_ERROR)
	{
		res = CjvxMexCalls::prepare_sender_to_receiver(theData);
	}
	return(res);
}

jvxErrorType
CjvxSignalProcessingNode_mexCall::prepare_complete_receiver_to_sender(jvxLinkDataDescriptor* theData)
{
	jvxErrorType res = CjvxSignalProcessingNode::prepare_complete_receiver_to_sender(theData);
	if(res == JVX_NO_ERROR)
	{
		assert(0);
		res = CjvxMexCalls::prepare_complete_receiver_to_sender(theData, NULL);
	}
	return(res);
}

jvxErrorType
CjvxSignalProcessingNode_mexCall::postprocess_sender_to_receiver(jvxLinkDataDescriptor* theData)
{
	jvxErrorType res = CjvxSignalProcessingNode::postprocess_sender_to_receiver(theData);
	if(res == JVX_NO_ERROR)
	{
		res = CjvxMexCalls::postprocess_sender_to_receiver(theData);
	}
	return(res);
}

jvxErrorType
CjvxSignalProcessingNode_mexCall::before_postprocess_receiver_to_sender(jvxLinkDataDescriptor* theData)
{
	jvxErrorType res = CjvxSignalProcessingNode::before_postprocess_receiver_to_sender(theData);
	if(res == JVX_NO_ERROR)
	{
		res = CjvxMexCalls::before_postprocess_receiver_to_sender(theData);
	}
	return(res);
}

jvxErrorType
CjvxSignalProcessingNode_mexCall::postprocess()
{
	jvxErrorType res = CjvxSignalProcessingNode::postprocess();
	if(res == JVX_NO_ERROR)
	{
		res = CjvxMexCalls::postprocess();
	}
	return(res);
}


jvxErrorType
CjvxSignalProcessingNode_mexCall::process_st(jvxLinkDataDescriptor* theData, jvxSize idx_sender_to_receiver, jvxSize idx_receiver_to_sender)
{
	return(CjvxMexCalls::process_st(theData, idx_sender_to_receiver, idx_receiver_to_sender));
};

jvxErrorType
CjvxSignalProcessingNode_mexCall::put_configuration(jvxCallManagerConfiguration* callConf, IjvxConfigProcessor* processor,
	jvxHandle* sectionToContainAllSubsectionsForMe, const char* filename, jvxInt32 lineno)
{
	return(CjvxMexCalls::put_configuration(callConf, processor, this->_common_set_min.theState, sectionToContainAllSubsectionsForMe, filename, lineno));
}

jvxErrorType
CjvxSignalProcessingNode_mexCall::get_configuration(jvxCallManagerConfiguration* callConf, IjvxConfigProcessor* processor,
	jvxHandle* sectionWhereToAddAllSubsections)
{
	return(CjvxMexCalls::get_configuration(callConf, processor, sectionWhereToAddAllSubsections));
}
