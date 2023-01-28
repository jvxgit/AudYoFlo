#include "jvx.h"
#include "CjvxAuNMixMatrix.h"

CjvxAuNMixMatrix::CjvxAuNMixMatrix(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE): 
	CjvxAudioNode(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
{
	_common_set.theComponentType.unselected(JVX_COMPONENT_AUDIO_NODE);
	_common_set.theObjectSpecialization = reinterpret_cast<jvxHandle*>(static_cast<IjvxNode*>(this));
	_common_set.thisisme = static_cast<IjvxObject*>(this);	
	_common_spec_proc_set.thisisme = static_cast<IjvxObject*>(this);
	theMixerCore = NULL;
}

CjvxAuNMixMatrix::~CjvxAuNMixMatrix()
{
}

jvxErrorType 
CjvxAuNMixMatrix::activate()
{
	jvxErrorType res = CjvxAudioNode::activate();
	if(res == JVX_NO_ERROR)
	{
		jvx_factory_allocate_jvxMixMatrix(&theMixerCore);
		assert(theMixerCore);

		theMixerCore->activate(static_cast<CjvxProperties*>(this), "CjvxAuNMixMatrix");

		genMixMatrix_node::init_all();
		genMixMatrix_node::allocate_all();
		genMixMatrix_node::register_all(static_cast<CjvxProperties*>(this));
	}
	return(res);
};

jvxErrorType 
CjvxAuNMixMatrix::deactivate()
{
	jvxErrorType res = CjvxAudioNode::deactivate();
	if(res == JVX_NO_ERROR)
	{
		genMixMatrix_node::unregister_all(static_cast<CjvxProperties*>(this));
		genMixMatrix_node::deallocate_all();

		theMixerCore->deactivate();
		jvx_factory_deallocate_jvxMixMatrix(theMixerCore);
	}
	return(res);
};

jvxErrorType 
CjvxAuNMixMatrix::process_st(jvxLinkDataDescriptor* theData, jvxSize idx_sender_to_receiver, jvxSize idx_receiver_to_sender)
{

	jvxErrorType res = JVX_NO_ERROR;
	
	// This default function does not tolerate a lot of unexpected settings
	assert(theData->con_params.format == theData->con_compat.format);
	assert(theData->con_params.buffersize == theData->con_compat.buffersize);
	
	res = theMixerCore->process(theData->con_data.buffers[idx_sender_to_receiver],
		theData->con_compat.from_receiver_buffer_allocated_by_sender[idx_receiver_to_sender]);

	return(res);
};

jvxErrorType 
CjvxAuNMixMatrix::start()
{
	jvxErrorType res = CjvxAudioNode::start();
	if (res == JVX_NO_ERROR)
	{
		res = theMixerCore->start();
	}
	return(res);
}

jvxErrorType 
CjvxAuNMixMatrix::stop()
{
	jvxErrorType res = CjvxAudioNode::stop();
	if (res == JVX_NO_ERROR)
	{
		res = theMixerCore->stop();
	}
	return(res);
}

jvxErrorType 
CjvxAuNMixMatrix::set_property(jvxCallManagerProperties& callGate, 
	const jvx::propertyRawPointerType::IjvxRawPointerType& rawPtr,
	const jvx::propertyAddress::IjvxPropertyAddress& ident,
	const jvx::propertyDetail::CjvxTranferDetail& trans)
{
	jvxBool updateUi = false;
	jvxErrorType res = CjvxAudioNode::set_property(callGate, 
		rawPtr, ident, trans);
	if (res == JVX_NO_ERROR)
	{
		JVX_TRANSLATE_PROP_ADDRESS_IDX_CAT(ident, propId, category);
		if (
			((category == CjvxAudioNode_genpcg::properties_parameters.numberinputchannels.category) && (propId == CjvxAudioNode_genpcg::properties_parameters.numberinputchannels.globalIdx)) ||
			((category == CjvxAudioNode_genpcg::properties_parameters.numberoutputchannels.category) && (propId == CjvxAudioNode_genpcg::properties_parameters.numberoutputchannels.globalIdx)) ||
			((category == CjvxAudioNode_genpcg::properties_parameters.buffersize.category) && (propId == CjvxAudioNode_genpcg::properties_parameters.buffersize.globalIdx)))
		{
			theMixerCore->setParameters(CjvxAudioNode_genpcg::properties_parameters.buffersize.value,
				CjvxAudioNode_genpcg::properties_parameters.numberinputchannels.value,
				CjvxAudioNode_genpcg::properties_parameters.numberoutputchannels.value,
				&updateUi);
		}
		else
		{
			theMixerCore->set_property_local(rawPtr, propId,
				category, trans.offsetStart, trans.contentOnly, callGate.call_purpose,
				&updateUi);
		}

		if (updateUi)
		{
			jvxCBitField prio = (jvxCBitField)1 << JVX_REPORT_REQUEST_UPDATE_WINDOW_SHIFT;
			this->_report_command_request(prio);
		}
	}
	return(res);
}

jvxErrorType 
CjvxAuNMixMatrix::put_configuration(jvxCallManagerConfiguration* callConf, IjvxConfigProcessor* processor,
	jvxHandle* sectionToContainAllSubsectionsForMe,
	const char* filename,
	jvxInt32 lineno)
{
	jvxErrorType res = CjvxAudioNode::put_configuration(callConf, processor, sectionToContainAllSubsectionsForMe, 
		filename,  lineno);
	if (res == JVX_NO_ERROR)
	{
		if (_common_set_min.theState == JVX_STATE_ACTIVE)
		{
			theMixerCore->put_configuration(callConf, processor, sectionToContainAllSubsectionsForMe,
				filename, lineno);
		}
	}
	return(res);
}

jvxErrorType 
CjvxAuNMixMatrix::get_configuration(jvxCallManagerConfiguration* callConf, IjvxConfigProcessor* processor,
	jvxHandle* sectionWhereToAddAllSubsections)
{
	jvxErrorType res = CjvxAudioNode::get_configuration(callConf, processor, sectionWhereToAddAllSubsections);
	if (res == JVX_NO_ERROR)
	{
		if (_common_set_min.theState >= JVX_STATE_ACTIVE)
		{
			theMixerCore->get_configuration(callConf, processor, sectionWhereToAddAllSubsections);
		}
	}
	return(res);
}

jvxErrorType
CjvxAuNMixMatrix::test_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(var))
{
	jvxErrorType res = JVX_NO_ERROR;

	CjvxAudioNode::test_connect_icon(JVX_CONNECTION_FEEDBACK_CALL(var));
	if (res == JVX_NO_ERROR)
	{
		update_interfaces();
	}
	return res;
}

jvxErrorType
CjvxAuNMixMatrix::transfer_backward_ocon(jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxSize i;

	jvxErrorType res = CjvxAudioNode::transfer_backward_ocon(tp, data JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
	if (res == JVX_NO_ERROR)
	{
		update_interfaces();
	}
	return res;
}

void
CjvxAuNMixMatrix::update_interfaces()
{
	jvxBool updateUi = false;
	theMixerCore->setParameters(CjvxAudioNode_genpcg::properties_parameters.buffersize.value,
		CjvxAudioNode_genpcg::properties_parameters.numberinputchannels.value,
		CjvxAudioNode_genpcg::properties_parameters.numberoutputchannels.value,
		&updateUi);

	// This function is expected to be followed by an UPDATE WINDOW call
}