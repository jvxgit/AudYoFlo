#include "jvx.h"
#include "CjvxAuNSynthesizer.h"

CjvxAuNSynthesizer::CjvxAuNSynthesizer(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE): 
	CjvxAudioNode(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
{
	_common_set.theComponentType.unselected(JVX_COMPONENT_AUDIO_NODE);
	_common_set.theObjectSpecialization = reinterpret_cast<jvxHandle*>(static_cast<IjvxNode*>(this));
	_common_set.thisisme = static_cast<IjvxObject*>(this);	
	_common_spec_proc_set.thisisme = static_cast<IjvxObject*>(this);
	myWaveGenerators = NULL;
}

CjvxAuNSynthesizer::~CjvxAuNSynthesizer()
{
}

jvxErrorType 
CjvxAuNSynthesizer::activate()
{
	jvxBool updateUi1 = false;
	jvxErrorType res = CjvxAudioNode::activate();
	if(res == JVX_NO_ERROR)
	{
		jvx_factory_allocate_jvxIntegrateWaveGenerators(&myWaveGenerators);
		res = myWaveGenerators->activate(static_cast<CjvxProperties*>(this), "WaveGenerators");
		myWaveGenerators->setParameters(
				CjvxAudioNode_genpcg::properties_parameters.samplerate.value,
				CjvxAudioNode_genpcg::properties_parameters.buffersize.value,
				CjvxAudioNode_genpcg::properties_parameters.numberoutputchannels.value,
				(jvxDataFormat)CjvxAudioNode_genpcg::properties_parameters.format.value,
				&updateUi1
				);

		if(updateUi1)
		{
			_report_command_request((jvxCBitField)1 << JVX_REPORT_REQUEST_UPDATE_PROPERTY_VIEWER_FULL_SHIFT);
		}
	}
	return(res);
};

jvxErrorType 
CjvxAuNSynthesizer::deactivate()
{
	jvxErrorType res = CjvxAudioNode::deactivate();
	if(res == JVX_NO_ERROR)
	{
		res = myWaveGenerators->deactivate();
		jvx_factory_deallocate_jvxIntegrateWaveGenerators(myWaveGenerators);
	}
	return(res);
};

jvxErrorType 
CjvxAuNSynthesizer::set_property(jvxCallManagerProperties& callGate,
	const jvx::propertyRawPointerType::IjvxRawPointerType& rawPtr,
	const jvx::propertyAddress::IjvxPropertyAddress& ident,
	const jvx::propertyDetail::CjvxTranferDetail& trans)
{
	jvxBool updateUi1 = false;
	jvxBool updateUi2 = false;
	jvxErrorType res = CjvxAudioNode::set_property(callGate,
		rawPtr, ident, trans);

	if(res == JVX_NO_ERROR)
	{
		JVX_TRANSLATE_PROP_ADDRESS_IDX_CAT(ident, propId, category);
		/* If property for processing have changed */
		if(
			((propId == CjvxAudioNode_genpcg::properties_parameters.samplerate.globalIdx) &&
			(category == CjvxAudioNode_genpcg::properties_parameters.samplerate.category)) ||
			((propId == CjvxAudioNode_genpcg::properties_parameters.buffersize.globalIdx) &&
			(category == CjvxAudioNode_genpcg::properties_parameters.buffersize.category)) ||
			((propId == CjvxAudioNode_genpcg::properties_parameters.numberoutputchannels.globalIdx) &&
			(category == CjvxAudioNode_genpcg::properties_parameters.numberoutputchannels.category)))
		{
			myWaveGenerators->setParameters(
				CjvxAudioNode_genpcg::properties_parameters.samplerate.value,
				CjvxAudioNode_genpcg::properties_parameters.buffersize.value,
				CjvxAudioNode_genpcg::properties_parameters.numberoutputchannels.value,
				(jvxDataFormat)CjvxAudioNode_genpcg::properties_parameters.format.value,
				&updateUi1
				);
		}

		// Pass it on to submodule
		res = myWaveGenerators->set_property_local(rawPtr,
							propId,category,
							trans.offsetStart, trans.contentOnly, callGate.call_purpose,
							&updateUi2);

		if(updateUi1 || updateUi2)
		{
			_report_command_request((jvxCBitField)1 << JVX_REPORT_REQUEST_UPDATE_PROPERTY_VIEWER_FULL_SHIFT);
		}
	}
	return(res);
}

jvxErrorType 
CjvxAuNSynthesizer::process_st(jvxLinkDataDescriptor* theData, jvxSize idx_sender_to_receiver, jvxSize idx_receiver_to_sender)
{

	jvxErrorType res = JVX_NO_ERROR;
	jvxSize numFilled = 0;
	jvxBool allStopped = false;
	jvxSize i;

	// This default function does not tolerate a lot of unexpected settings
	assert(theData->con_params.format == theData->con_compat.format);
	assert(theData->con_params.buffersize == theData->con_compat.buffersize);
	
	for(i = 0; i < theData->con_compat.number_channels; i++)
	{
		memset(theData->con_compat.from_receiver_buffer_allocated_by_sender[idx_receiver_to_sender][i], 
			0, jvxDataFormat_size[theData->con_compat.format]*theData->con_compat.buffersize);
	}

	res = myWaveGenerators->process(theData->con_compat.from_receiver_buffer_allocated_by_sender[idx_receiver_to_sender], numFilled, allStopped);

	if(allStopped)
	{
		if(!runtime.endProcessingTriggered)
		{
			_report_command_request((jvxCBitField)1 << JVX_REPORT_REQUEST_TRY_TRIGGER_STOP_SEQUENCER_PROCESS_SHIFT);
			runtime.endProcessingTriggered = true;
		}
	}
	return(res);
};

jvxErrorType 
CjvxAuNSynthesizer::prepare_sender_to_receiver(jvxLinkDataDescriptor* theData JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;
	res = CjvxAudioNode::prepare_sender_to_receiver(theData JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
	if(res == JVX_NO_ERROR)
	{
		res = myWaveGenerators->startAll();
		runtime.endProcessingTriggered = false;
	}
	return(res);
}

jvxErrorType 
CjvxAuNSynthesizer::postprocess_sender_to_receiver(jvxLinkDataDescriptor* theData JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;
	res = CjvxAudioNode::postprocess_sender_to_receiver(theData JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
	if(res == JVX_NO_ERROR)
	{
		res = myWaveGenerators->stopAll();
	}
	return(res);
}

jvxErrorType 
CjvxAuNSynthesizer::put_configuration(jvxCallManagerConfiguration* callConf, IjvxConfigProcessor* processor,
								     jvxHandle* sectionToContainAllSubsectionsForMe, 
								     const char* filename,
								     jvxInt32 lineno)
{
	jvxErrorType res = JVX_NO_ERROR;

	if(_common_set_min.theState == JVX_STATE_ACTIVE)
	{
		res = myWaveGenerators->put_configuration(callConf, processor,
			sectionToContainAllSubsectionsForMe, 
			filename,
			lineno);
	}
	return(res);}

jvxErrorType 
CjvxAuNSynthesizer::get_configuration(jvxCallManagerConfiguration* callConf, IjvxConfigProcessor* processor,
								     jvxHandle* sectionWhereToAddAllSubsections)
{
	jvxErrorType res = JVX_NO_ERROR;

	if(_common_set_min.theState == JVX_STATE_ACTIVE)
	{
		res = myWaveGenerators->get_configuration(callConf, processor, sectionWhereToAddAllSubsections);
	}
	return(res);
}
