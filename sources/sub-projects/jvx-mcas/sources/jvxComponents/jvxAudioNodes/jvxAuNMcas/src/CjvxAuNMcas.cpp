#include "jvx.h"
#include "CjvxAuNMcas.h"
#include "mcg_export_project.h"

CjvxAuNMcas::CjvxAuNMcas(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE) :
	CjvxAudioNode(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)

{
	_common_set.theComponentType.unselected(JVX_COMPONENT_AUDIO_NODE);
	_common_set.theObjectSpecialization = reinterpret_cast<jvxHandle*>(static_cast<IjvxNode*>(this));
	_common_set.thisisme = static_cast<IjvxObject*>(this);	
	_common_spec_proc_set.thisisme = static_cast<IjvxObject*>(this);
}

CjvxAuNMcas::~CjvxAuNMcas()
{
}

#ifdef JVX_EXTERNAL_CALL_ENABLED
JVX_MEXCALL_SELECT(CjvxAuNMcas, CjvxAudioNode, _theExtCallObjectName)
JVX_MEXCALL_UNSELECT(CjvxAuNMcas, CjvxAudioNode, _theExtCallObjectName)
JVX_MEXCALL_PREPARE(CjvxAuNMcas, CjvxAudioNode)
JVX_MEXCALL_POSTPROCESS(CjvxAuNMcas, CjvxAudioNode)
// JVX_MEXCALL_PREPARE_SENDER_TO_RECEIVER(CjvxAuNMcas, CjvxAudioNode)
JVX_MEXCALL_PREPARE_COMPLETE_RECEIVER_TO_SENDER(CjvxAuNMcas, CjvxAudioNode, &_common_set_ldslave.theData_out)
JVX_MEXCALL_POSTPROCESS_SENDER_TO_RECEIVER(CjvxAuNMcas, CjvxAudioNode)
JVX_MEXCALL_BEFORE_POSTPROCESS_RECEIVER_TO_SENDER(CjvxAuNMcas, CjvxAudioNode)
JVX_MEXCALL_PUT_CONFIGURATION(CjvxAuNMcas, CjvxAudioNode)
JVX_MEXCALL_GET_CONFIGURATION(CjvxAuNMcas, CjvxAudioNode)
#endif

jvxErrorType 
CjvxAuNMcas::activate()
{
	jvxErrorType res = CjvxAudioNode::activate();
	if(res == JVX_NO_ERROR)
	{
		this->myPlayControl = new playControl();
		
#ifdef JVX_EXTERNAL_CALL_ENABLED
		res = CjvxMexCalls::activate();
#endif
		genSimple_node::init__properties_active();
		genSimple_node::allocate__properties_active();
		genSimple_node::register__properties_active(static_cast<CjvxProperties*>(this));

		associate__properties_active(static_cast<CjvxProperties*>(this),
			&(this->myPlayControl->playProps.volume), 1,
			&(this->myPlayControl->playProps.numberOfChannels), 1,
		    &(this->myPlayControl->playProps.inputChannelOffset), 1,
			&(this->myPlayControl->playProps.monoToStereoPlayback), 1,
			&(this->myPlayControl->playProps.dirtyFlag),1,
			&(this->myPlayControl->clippingIndicator),1
			);

			
//		genSimple_node::register_callbacks(static_cast<CjvxProperties*>(this), set_properties_callback_pre, set_properties_callback_post, reinterpret_cast<jvxHandle*>(this),NULL);

	}
	return(res);
};

jvxErrorType 
CjvxAuNMcas::deactivate()
{
	jvxErrorType res = CjvxAudioNode::deactivate();
	if(res == JVX_NO_ERROR)
	{
		genSimple_node::unregister__properties_active(static_cast<CjvxProperties*>(this));
		genSimple_node::deallocate__properties_active();
#ifdef JVX_EXTERNAL_CALL_ENABLED
		res = CjvxMexCalls::deactivate();
#endif
	}
	return(res);
};

jvxErrorType 
CjvxAuNMcas::process_st(jvxLinkDataDescriptor* theData, jvxSize idx_sender_to_receiver, jvxSize idx_receiver_to_sender)
{

	jvxErrorType res = JVX_NO_ERROR;
	jvxBool extCallPres = false;
	jvxBool engageMatlab = false;

#ifdef JVX_EXTERNAL_CALL_ENABLED

	res = CjvxMexCalls::is_extcall_reference_present(&extCallPres);
	res = CjvxMexCalls::is_matlab_processing_engaged(&engageMatlab);
	if(extCallPres && engageMatlab)
	{
		// Pass control to Matlab processing
		res = CjvxMexCalls::process_st(theData, idx_sender_to_receiver, idx_receiver_to_sender);
	}
	else
	{
#endif
		//res = CjvxAudioNode::process_st(theData, idx_sender_to_receiver, idx_receiver_to_sender);
		
		// This default function does not tolerate a lot of unexpected settings
		assert(theData->con_params.format == theData->con_compat.format);
		assert(theData->con_params.buffersize == theData->con_compat.buffersize);

		res = process_buffer(
			theData->con_data.buffers[idx_sender_to_receiver],
			theData->con_compat.from_receiver_buffer_allocated_by_sender[idx_receiver_to_sender],
			theData->con_params.buffersize,
			theData->con_params.number_channels,
			theData->con_compat.number_channels,
			theData->con_params.format);


#ifdef JVX_EXTERNAL_CALL_ENABLED
	}
#endif

	return(res);
};


jvxErrorType  CjvxAuNMcas::set_property(
	jvxCallManagerProperties& callGate, 
	const jvx::propertyRawPointerType::IjvxRawPointerType& rawPtr,
	const jvx::propertyAddress::IjvxPropertyAddress& ident,
	const jvx::propertyDetail::CjvxTranferDetail& trans)
{

	jvxErrorType res = CjvxAudioNode::set_property(callGate, rawPtr, ident, trans);

	return res;
}


jvxErrorType
CjvxAuNMcas::process_buffer(jvxHandle** buffers_input, jvxHandle** buffers_output, jvxSize bSize,
	jvxSize numChans_input, jvxSize numChans_output, jvxDataFormat format)
{

	jvxDspBaseErrorType resD = myPlayControl->process((jvxData**)buffers_input, (jvxData**)buffers_output, numChans_input, numChans_output, bSize);

	if (resD != JVX_DSP_NO_ERROR)
	{
		return JVX_ERROR_CALL_SUB_COMPONENT_FAILED;
	}
	return JVX_NO_ERROR;
}


#ifdef JVX_EXTERNAL_CALL_ENABLED

jvxErrorType 
CjvxAuNMcas::myFirstMatlabEntry()
{
	return(JVX_NO_ERROR);
}

#endif

jvxErrorType JVX_CALLINGCONVENTION CjvxAuNMcas::prepare_sender_to_receiver(jvxLinkDataDescriptor * theData)
{
	jvxErrorType res = CjvxAudioNode::prepare_sender_to_receiver(theData);
	if (res) return res;

	res = CjvxMexCalls::prepare_sender_to_receiver(theData);
	if (res) return res;

	int sampleRate = theData->con_params.rate;
	int bufferSize = theData->con_params.buffersize;
	
	this->myPlayControl->prepare(sampleRate, bufferSize);

	return JVX_NO_ERROR;
}
