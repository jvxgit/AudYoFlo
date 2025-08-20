#include "jvx.h"
#include "jvxAudioNodes/CjvxAudioNode.h"

CjvxAudioNode::CjvxAudioNode(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE):
	CjvxAudioNodeBase(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
{
	//_common_set_ldslave.zeroCopyBuffering_cfg = false; <- done in base class
}

CjvxAudioNode::~CjvxAudioNode()
{
}


jvxErrorType
CjvxAudioNode::activate()
{
	jvxErrorType res = CjvxAudioNodeBase::activate();

	if (res == JVX_NO_ERROR)
	{
		// Will be deactivated in base class "CjvxAudioNodeBase"
		JVX_ACTIVATE_DEFAULT_ONE_IN_ONE_OUT_CONNECTORS(static_cast<IjvxDataProcessor*>(this) , 
			static_cast<IjvxObject*>(this), "default", NULL, _common_set.theModuleName.c_str());
	}
	return(res);
};

jvxErrorType
CjvxAudioNode::prepare()
{
	jvxErrorType res = _prepare();
	if(res == JVX_NO_ERROR)
	{
		_common_set_node_params_1io.processing.buffersize = CjvxAudioNode_genpcg::properties_parameters.buffersize.value;
		_common_set_node_params_1io.processing.samplerate = CjvxAudioNode_genpcg::properties_parameters.samplerate.value;
		_common_set_node_params_1io.processing.number_input_channels = CjvxAudioNode_genpcg::properties_parameters.numberinputchannels.value;
		_common_set_node_params_1io.processing.number_output_channels = CjvxAudioNode_genpcg::properties_parameters.numberoutputchannels.value;
		_common_set_node_params_1io.processing.format = (jvxDataFormat)CjvxAudioNode_genpcg::properties_parameters.format.value;

	}
	return(res);
}

jvxErrorType
CjvxAudioNode::prepare_lock()
{
	jvxErrorType res = _prepare_lock( );
	if(res == JVX_NO_ERROR)
	{
		_common_set_node_params_1io.processing.buffersize = CjvxAudioNode_genpcg::properties_parameters.buffersize.value;
		_common_set_node_params_1io.processing.samplerate = CjvxAudioNode_genpcg::properties_parameters.samplerate.value;
		_common_set_node_params_1io.processing.number_input_channels = CjvxAudioNode_genpcg::properties_parameters.numberinputchannels.value;
		_common_set_node_params_1io.processing.number_output_channels = CjvxAudioNode_genpcg::properties_parameters.numberoutputchannels.value;
		_common_set_node_params_1io.processing.format = (jvxDataFormat)CjvxAudioNode_genpcg::properties_parameters.format.value;
	}
	return(res);
}

jvxErrorType
CjvxAudioNode::postprocess()
{
	jvxErrorType res = _postprocess();
	if (res == JVX_NO_ERROR)
	{
		_common_set_node_params_1io.processing.buffersize = JVX_SIZE_UNSELECTED;
		_common_set_node_params_1io.processing.number_input_channels = JVX_SIZE_UNSELECTED;
		_common_set_node_params_1io.processing.number_output_channels = JVX_SIZE_UNSELECTED;
		_common_set_node_params_1io.processing.samplerate = JVX_SIZE_UNSELECTED;
		_common_set_node_params_1io.processing.format = JVX_DATAFORMAT_NONE;

	}
	return(res);
}
jvxErrorType
CjvxAudioNode::postprocess_lock()
{
	jvxErrorType res =_postprocess_lock();
	if(res == JVX_NO_ERROR)
	{
	}
	return res;
};

// ====================================================================================

jvxErrorType
CjvxAudioNode::reference_object(IjvxObject** refObject)
{
	return(_reference_object(refObject));
};


jvxErrorType
CjvxAudioNode::prepare_sender_to_receiver(jvxLinkDataDescriptor* theData)
{
	jvxErrorType res = JVX_ERROR_WRONG_STATE;
	if(_common_set_min.theState == JVX_STATE_PREPARED)
	{
		if(_common_set_node_params_1io.acceptOnlyExactMatchLinkDataInput)
		{
			// Setup has been set in the properties - let us make sure that it really matches
			if(
				!(
				(_common_set_node_params_1io.processing.buffersize == theData->con_params.buffersize) &&
				(_common_set_node_params_1io.processing.number_input_channels == theData->con_params.number_channels) &&
				(_common_set_node_params_1io.processing.samplerate == theData->con_params.rate) &&
				(_common_set_node_params_1io.processing.format == theData->con_params.format)))
			{
				res = JVX_ERROR_INVALID_SETTING;
			}
			else
			{
				// Allocate memory
				res = allocate_pipeline_and_buffers_prepare_to();
			}
		}
		else
		{
			res = allocate_pipeline_and_buffers_prepare_to();
		}
	}
	return(res);
};

jvxErrorType
CjvxAudioNode::prepare_complete_receiver_to_sender(jvxLinkDataDescriptor* theData)
{
	jvxErrorType res = JVX_NO_ERROR;
	if(_common_set_min.theState == JVX_STATE_PREPARED)
	{
		if(_common_set_node_params_1io.acceptOnlyExactMatchLinkDataOutput)
		{
			if(
				!(
				(_common_set_node_params_1io.processing.buffersize == theData->con_compat.buffersize) &&
				(_common_set_node_params_1io.processing.number_output_channels == theData->con_compat.number_channels) &&
				(_common_set_node_params_1io.processing.samplerate == theData->con_compat.rate) &&
				(_common_set_node_params_1io.processing.format == theData->con_compat.format)))
			{
				res = JVX_ERROR_INVALID_SETTING;
			}
			else
			{
				res = JVX_NO_ERROR;
			}
		}
		else
		{
			res = JVX_NO_ERROR;
		}
	}
	else
	{
		res = JVX_ERROR_WRONG_STATE;
	}
	return(res);
};

jvxErrorType
CjvxAudioNode::supports_multithreading(jvxBool* supports)
{
	return(_supports_multithreading(supports));
};

/* TODO: Add member functions for multithreading negotiations */

jvxErrorType
CjvxAudioNode::process_mt(jvxLinkDataDescriptor* theData, jvxSize idx_sender_to_receiver, jvxSize idx_receiver_to_sender, jvxSize* channelSelect , jvxSize numEntriesChannels, jvxInt32 offset_input, jvxInt32 offset_output, jvxInt32 numEntries)
{
	return(_process_mt(theData, idx_sender_to_receiver, idx_receiver_to_sender, channelSelect, numEntriesChannels, offset_input, offset_output, numEntries));
};

jvxErrorType
CjvxAudioNode::reconfigured_receiver_to_sender(jvxLinkDataDescriptor* theData)
{
	return(JVX_NO_ERROR);
};

jvxErrorType
CjvxAudioNode::process_st(jvxLinkDataDescriptor* theData, jvxSize idx_sender_to_receiver, jvxSize idx_receiver_to_sender)
{
	return(_process_st(theData, idx_sender_to_receiver, idx_receiver_to_sender));
};

jvxErrorType
CjvxAudioNode::last_error_process(char* fld_text, jvxSize fldSize, jvxErrorType* err, jvxInt32* id_error, jvxLinkDataDescriptor* theData)
{
	return(_last_error_process(fld_text, fldSize, err, id_error, theData));
};

jvxErrorType
CjvxAudioNode::postprocess_sender_to_receiver(jvxLinkDataDescriptor* theData)
{
	jvxErrorType res = JVX_ERROR_WRONG_STATE;
	if(_common_set_min.theState == JVX_STATE_PREPARED)
	{
		res = deallocate_pipeline_and_buffers_postprocess_to();

		_common_set_node_params_1io.processing.format = JVX_DATAFORMAT_NONE;
		_common_set_node_params_1io.processing.number_input_channels = JVX_SIZE_UNSELECTED;
		_common_set_node_params_1io.processing.buffersize = JVX_SIZE_UNSELECTED;
		_common_set_node_params_1io.processing.samplerate = JVX_SIZE_UNSELECTED;
	}
	return(res);
};

jvxErrorType
CjvxAudioNode::before_postprocess_receiver_to_sender(jvxLinkDataDescriptor* theData)
{
	jvxErrorType res = JVX_NO_ERROR;
	if(_common_set_min.theState == JVX_STATE_PREPARED)
	{
		/*
		_common_set_node_params_1io.processing.format = theData->con_compat.format;
		_common_set_node_params_1io.processing.number_output_channels = theData->con_compat.number_channels;
		_common_set_node_params_1io.processing.buffersize = theData->con_compat.buffersize;
		_common_set_node_params_1io.processing.samplerate = theData->con_compat.rate;
		*/
		_common_set_node_params_1io.processing.number_output_channels = JVX_SIZE_UNSELECTED;
	}
	else
	{
		res = JVX_ERROR_WRONG_STATE;
	}

	return(res);
};

