#include "jvx.h"
#include "jvxSignalProcessingNodes/CjvxSignalProcessingNode.h"

#ifdef JVX_PROJECT_NAMESPACE
namespace JVX_PROJECT_NAMESPACE {
#endif

JVX_PROCESSINGNODE_CLASSNAME::JVX_PROCESSINGNODE_CLASSNAME(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE):
	CjvxObject(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL), CjvxProperties(module_name, *this)
{
	_common_set.theComponentType.unselected(JVX_COMPONENT_SIGNAL_PROCESSING_NODE);
	_common_set.theObjectSpecialization = reinterpret_cast<jvxHandle*>(static_cast<IjvxNode*>(this));
	_common_set.thisisme = static_cast<IjvxObject*>(this);
	_common_spec_proc_set.thisisme = static_cast<IjvxObject*>(this);

	_common_set_node_params_1io.acceptOnlyExactMatchLinkDataInput = false;
	_common_set_node_params_1io.acceptOnlyExactMatchLinkDataOutput = false;

	_common_set_node_params_1io.preferred.buffersize.min = -1;
	_common_set_node_params_1io.preferred.buffersize.max = -1;
	_common_set_node_params_1io.preferred.number_input_channels.min = -1;
	_common_set_node_params_1io.preferred.number_input_channels.max = -1;
	_common_set_node_params_1io.preferred.number_output_channels.min = -1;
	_common_set_node_params_1io.preferred.number_output_channels.max = -1;
	_common_set_node_params_1io.preferred.samplerate.min = -1;
	_common_set_node_params_1io.preferred.samplerate.max = -1;
	_common_set_node_params_1io.preferred.format.min = JVX_DATAFORMAT_NONE;
	_common_set_node_params_1io.preferred.format.max = JVX_DATAFORMAT_NONE;

	_common_set_node_params_1io.processing.buffersize = -1;
	_common_set_node_params_1io.processing.number_input_channels = -1;
	_common_set_node_params_1io.processing.number_output_channels = -1;
	_common_set_node_params_1io.processing.samplerate = -1;
	_common_set_node_params_1io.processing.format = JVX_DATAFORMAT_NONE;

	_common_set.theComponentSubTypeDescriptor = "signal_processing_node/audio_node";

#ifdef JVX_ALL_AUDIO_TO_LOGFILE
	_common_set_audio_log_to_file.numFramesLog = 100;
	_common_set_audio_log_to_file.timeoutMsec = 5000;
#endif


}

JVX_PROCESSINGNODE_CLASSNAME::~JVX_PROCESSINGNODE_CLASSNAME()
{
}

jvxErrorType
JVX_PROCESSINGNODE_CLASSNAME::activate()
{
	jvxErrorType res = _activate();
	if(res == JVX_NO_ERROR)
	{
		JVX_PROCESSINGNODE_CLASSNAME_GEN::init__properties_parameters();
		JVX_PROCESSINGNODE_CLASSNAME_GEN::allocate__properties_parameters();
		JVX_PROCESSINGNODE_CLASSNAME_GEN::register__properties_parameters(static_cast<CjvxProperties*>(this));

		JVX_PROCESSINGNODE_CLASSNAME_GEN::init__properties_active_higher();
		JVX_PROCESSINGNODE_CLASSNAME_GEN::allocate__properties_active_higher();
		JVX_PROCESSINGNODE_CLASSNAME_GEN::register__properties_active_higher(static_cast<CjvxProperties*>(this));

#ifdef JVX_ALL_AUDIO_TO_LOGFILE
		JVX_PROCESSINGNODE_CLASSNAME_GEN::init__audiologtofile__properties_logtofile();
		JVX_PROCESSINGNODE_CLASSNAME_GEN::allocate__audiologtofile__properties_logtofile();
		JVX_PROCESSINGNODE_CLASSNAME_GEN::register__audiologtofile__properties_logtofile(static_cast<CjvxProperties*>(this), logger_prop_prefix);

		// Initialize logfile writer
		theLogger.init_logging(_common_set_min.theHostRef, _common_set_audio_log_to_file.numFramesLog, _common_set_audio_log_to_file.timeoutMsec);
#endif

	}
	return(res);
};

jvxErrorType
JVX_PROCESSINGNODE_CLASSNAME::deactivate()
{
	jvxErrorType res = _deactivate();
	if(res == JVX_NO_ERROR)
	{
#ifdef JVX_ALL_AUDIO_TO_LOGFILE
		theLogger.terminate_logging();

		JVX_PROCESSINGNODE_CLASSNAME_GEN::unregister__audiologtofile__properties_logtofile(static_cast<CjvxProperties*>(this));
		JVX_PROCESSINGNODE_CLASSNAME_GEN::deallocate__audiologtofile__properties_logtofile();
#endif


		JVX_PROCESSINGNODE_CLASSNAME_GEN::unregister__properties_active_higher(static_cast<CjvxProperties*>(this));
		JVX_PROCESSINGNODE_CLASSNAME_GEN::deallocate__properties_active_higher();

		JVX_PROCESSINGNODE_CLASSNAME_GEN::unregister__properties_parameters(static_cast<CjvxProperties*>(this));
		JVX_PROCESSINGNODE_CLASSNAME_GEN::deallocate__properties_parameters();
	}
	return(res);
};

jvxErrorType
JVX_PROCESSINGNODE_CLASSNAME::prepare()
{
	jvxErrorType res = _prepare( );
	if(res == JVX_NO_ERROR)
	{
		_common_set_node_params_1io.processing.buffersize = JVX_PROCESSINGNODE_CLASSNAME_GEN::properties_parameters.buffersize.value;
		_common_set_node_params_1io.processing.samplerate = JVX_PROCESSINGNODE_CLASSNAME_GEN::properties_parameters.samplerate.value;
		_common_set_node_params_1io.processing.number_input_channels = JVX_PROCESSINGNODE_CLASSNAME_GEN::properties_parameters.numberinputchannels.value;
		_common_set_node_params_1io.processing.number_output_channels = JVX_PROCESSINGNODE_CLASSNAME_GEN::properties_parameters.numberoutputchannels.value;
		_common_set_node_params_1io.processing.format = (jvxDataFormat)JVX_PROCESSINGNODE_CLASSNAME_GEN::properties_parameters.format.value;

	}
	return(res);
}

jvxErrorType
JVX_PROCESSINGNODE_CLASSNAME::prepare_lock()
{
	jvxErrorType res = _prepare_lock();
	if(res == JVX_NO_ERROR)
	{
		_common_set_node_params_1io.processing.buffersize = JVX_PROCESSINGNODE_CLASSNAME_GEN::properties_parameters.buffersize.value;
		_common_set_node_params_1io.processing.samplerate = JVX_PROCESSINGNODE_CLASSNAME_GEN::properties_parameters.samplerate.value;
		_common_set_node_params_1io.processing.number_input_channels = JVX_PROCESSINGNODE_CLASSNAME_GEN::properties_parameters.numberinputchannels.value;
		_common_set_node_params_1io.processing.number_output_channels = JVX_PROCESSINGNODE_CLASSNAME_GEN::properties_parameters.numberoutputchannels.value;
		_common_set_node_params_1io.processing.format = (jvxDataFormat)JVX_PROCESSINGNODE_CLASSNAME_GEN::properties_parameters.format.value;
	}
	return(res);
}

jvxErrorType
JVX_PROCESSINGNODE_CLASSNAME::reportPreferredParameters(jvxPropertyCategoryType cat, jvxSize propId)
{
	jvxInt32 tmp32;
	jvxInt16 tmp16;
	jvxErrorType resL = JVX_NO_ERROR;
	jvxBool requiresCorrection = false;

	/* Use preferred buffersize if possible */
	if(JVX_PROPERTY_CHECK_ID_CAT(propId, cat, JVX_PROCESSINGNODE_CLASSNAME_GEN::properties_parameters.buffersize))
	{
		requiresCorrection = false;

		tmp32 = -1;
		if(_common_set_node_params_1io.preferred.buffersize.max >= 0)
		{
			if(JVX_PROCESSINGNODE_CLASSNAME_GEN::properties_parameters.buffersize.value > _common_set_node_params_1io.preferred.buffersize.max)
			{
				tmp32 = JVX_SIZE_INT32(_common_set_node_params_1io.preferred.buffersize.max);
				requiresCorrection = true;
			}
		}
		if(_common_set_node_params_1io.preferred.buffersize.min >= 0)
		{
			if(JVX_PROCESSINGNODE_CLASSNAME_GEN::properties_parameters.buffersize.value < _common_set_node_params_1io.preferred.buffersize.min)
			{
				tmp32 = JVX_SIZE_INT32(_common_set_node_params_1io.preferred.buffersize.min);
				requiresCorrection = true;
			}
		}

		if(requiresCorrection)
		{
			// indirect set via technology - if possible
			resL = this->_report_take_over_property(&tmp32, 1, JVX_DATAFORMAT_32BIT_LE, 0, true,
				JVX_PROCESSINGNODE_CLASSNAME_GEN::properties_parameters.buffersize.category,
				JVX_PROCESSINGNODE_CLASSNAME_GEN::properties_parameters.buffersize.globalIdx,
				JVX_COMPONENT_AUDIO_DEVICE);
		}
	}

	/* Use preferred samplerate if possible */
	if(JVX_PROPERTY_CHECK_ID_CAT(propId, cat, JVX_PROCESSINGNODE_CLASSNAME_GEN::properties_parameters.samplerate))
	{
		requiresCorrection = false;

		tmp32 = -1;
		if(_common_set_node_params_1io.preferred.samplerate.max >= 0)
		{
			if(JVX_PROCESSINGNODE_CLASSNAME_GEN::properties_parameters.samplerate.value > _common_set_node_params_1io.preferred.samplerate.max)
			{
				tmp32 = JVX_SIZE_INT32(_common_set_node_params_1io.preferred.samplerate.max);
				requiresCorrection = true;
			}
		}
		if(_common_set_node_params_1io.preferred.samplerate.min >= 0)
		{
			if(JVX_PROCESSINGNODE_CLASSNAME_GEN::properties_parameters.samplerate.value < _common_set_node_params_1io.preferred.samplerate.min)
			{
				tmp32 = JVX_SIZE_INT32(_common_set_node_params_1io.preferred.samplerate.min);
				requiresCorrection = true;
			}
		}

		if(requiresCorrection)
		{
			// indirect set via technology - if possible
			resL = this->_report_take_over_property(&tmp32, 1, JVX_DATAFORMAT_32BIT_LE, 0, true,
				JVX_PROCESSINGNODE_CLASSNAME_GEN::properties_parameters.samplerate.category,
				JVX_PROCESSINGNODE_CLASSNAME_GEN::properties_parameters.samplerate.globalIdx,
				JVX_COMPONENT_AUDIO_DEVICE);
		}
	}

	/* Use preferred number input channels if possible */
	if(JVX_PROPERTY_CHECK_ID_CAT(propId, cat, JVX_PROCESSINGNODE_CLASSNAME_GEN::properties_parameters.numberinputchannels))
	{
		requiresCorrection = false;

		tmp32 = -1;
		if(_common_set_node_params_1io.preferred.number_input_channels.max >= 0)
		{
			if(JVX_PROCESSINGNODE_CLASSNAME_GEN::properties_parameters.numberinputchannels.value > _common_set_node_params_1io.preferred.number_input_channels.max)
			{
				tmp32 = JVX_SIZE_INT32(_common_set_node_params_1io.preferred.number_input_channels.max);
				requiresCorrection = true;
			}
		}
		if(_common_set_node_params_1io.preferred.number_input_channels.min >= 0)
		{
			if(JVX_PROCESSINGNODE_CLASSNAME_GEN::properties_parameters.numberinputchannels.value < _common_set_node_params_1io.preferred.number_input_channels.min)
			{
				tmp32 = JVX_SIZE_INT32(_common_set_node_params_1io.preferred.number_input_channels.min);
				requiresCorrection = true;
			}
		}


		if(requiresCorrection)
		{
			// indirect set via technology - if possible
			resL = this->_report_take_over_property(&tmp32, 1, JVX_DATAFORMAT_32BIT_LE, 0, true,
				JVX_PROCESSINGNODE_CLASSNAME_GEN::properties_parameters.numberinputchannels.category,
				JVX_PROCESSINGNODE_CLASSNAME_GEN::properties_parameters.numberinputchannels.globalIdx,
				JVX_COMPONENT_AUDIO_DEVICE);
		}
	}

	/* Use preferred number input channels if possible */
	if(JVX_PROPERTY_CHECK_ID_CAT(propId, cat, JVX_PROCESSINGNODE_CLASSNAME_GEN::properties_parameters.numberoutputchannels))
	{
		requiresCorrection = false;

		tmp32 = -1;
		if(_common_set_node_params_1io.preferred.number_output_channels.max >= 0)
		{
			if(JVX_PROCESSINGNODE_CLASSNAME_GEN::properties_parameters.numberoutputchannels.value > _common_set_node_params_1io.preferred.number_output_channels.max)
			{
				tmp32 = JVX_SIZE_INT32(_common_set_node_params_1io.preferred.number_output_channels.max);
				requiresCorrection = true;
			}
		}
		if(_common_set_node_params_1io.preferred.number_output_channels.min >= 0)
		{
			if(JVX_PROCESSINGNODE_CLASSNAME_GEN::properties_parameters.numberoutputchannels.value < _common_set_node_params_1io.preferred.number_output_channels.min)
			{
				tmp32 = JVX_SIZE_INT32(_common_set_node_params_1io.preferred.number_output_channels.min);
				requiresCorrection = true;
			}
		}


		if(requiresCorrection)
		{
			// indirect set via technology - if possible
			resL = this->_report_take_over_property(&tmp32, 1, JVX_DATAFORMAT_32BIT_LE, 0, true,
				JVX_PROCESSINGNODE_CLASSNAME_GEN::properties_parameters.numberoutputchannels.category,
				JVX_PROCESSINGNODE_CLASSNAME_GEN::properties_parameters.numberoutputchannels.globalIdx,
				JVX_COMPONENT_AUDIO_DEVICE);
		}
	}



	if(JVX_PROPERTY_CHECK_ID_CAT(propId, cat, JVX_PROCESSINGNODE_CLASSNAME_GEN::properties_parameters.format))
	{
		requiresCorrection = false;

		tmp16 = JVX_DATAFORMAT_NONE;
		if(_common_set_node_params_1io.preferred.format.max > JVX_DATAFORMAT_NONE)
		{
			if(JVX_PROCESSINGNODE_CLASSNAME_GEN::properties_parameters.format.value > _common_set_node_params_1io.preferred.format.max)
			{
				tmp16 = _common_set_node_params_1io.preferred.format.max;
				requiresCorrection = true;
			}
		}
		if(_common_set_node_params_1io.preferred.format.min > JVX_DATAFORMAT_NONE)
		{
			if(JVX_PROCESSINGNODE_CLASSNAME_GEN::properties_parameters.format.value < _common_set_node_params_1io.preferred.format.min)
			{
				tmp16 = _common_set_node_params_1io.preferred.format.min;
				requiresCorrection = true;
			}
		}

		if(requiresCorrection)
		{
			// indirect set via technology - if possible
			resL = this->_report_take_over_property(&tmp16, 1, JVX_DATAFORMAT_16BIT_LE, 0, true,
				JVX_PROCESSINGNODE_CLASSNAME_GEN::properties_parameters.format.category,
				JVX_PROCESSINGNODE_CLASSNAME_GEN::properties_parameters.format.globalIdx,
				JVX_COMPONENT_AUDIO_DEVICE);
		}
	}
	return(resL);
}

jvxErrorType
JVX_PROCESSINGNODE_CLASSNAME::is_ready(jvxBool* suc, jvxApiString * reasonIfNot)
{
	std::string txt;
	jvxErrorType res = _is_ready( suc, reasonIfNot);
	jvxBool Iamready = true;
	jvxBool isAProblem = false;

	if(res == JVX_NO_ERROR)
	{
		/*
		 * CHECK BUFFERSIZE
		 */
		isAProblem = false;
		if(_common_set_node_params_1io.preferred.buffersize.min >= 0)
		{
			if(JVX_PROCESSINGNODE_CLASSNAME_GEN::properties_parameters.buffersize.value < _common_set_node_params_1io.preferred.buffersize.min)
			{
				isAProblem = true;
			}
		}
		if(_common_set_node_params_1io.preferred.buffersize.max >= 0)
		{
			if(JVX_PROCESSINGNODE_CLASSNAME_GEN::properties_parameters.buffersize.value > _common_set_node_params_1io.preferred.buffersize.max)
			{
				isAProblem = true;
			}
		}
		if(isAProblem)
		{
			Iamready  = false;
			txt = "Buffersize mismatch, constraint: ";
			txt += jvx_size2String(_common_set_node_params_1io.preferred.buffersize.min);
			txt += "<=";
			txt += jvx_int2String(JVX_PROCESSINGNODE_CLASSNAME_GEN::properties_parameters.buffersize.value);
			txt+= "<=";
			txt += jvx_size2String(_common_set_node_params_1io.preferred.buffersize.max);
		}

		/*
		 * CHECK SAMPLERATE
		 */
		isAProblem = false;
		if(_common_set_node_params_1io.preferred.samplerate.min >= 0)
		{
			if(JVX_PROCESSINGNODE_CLASSNAME_GEN::properties_parameters.samplerate.value < _common_set_node_params_1io.preferred.samplerate.min)
			{
				isAProblem = true;
			}
		}
		if(_common_set_node_params_1io.preferred.samplerate.max >= 0)
		{
			if(JVX_PROCESSINGNODE_CLASSNAME_GEN::properties_parameters.samplerate.value > _common_set_node_params_1io.preferred.samplerate.max)
			{
				isAProblem = true;
			}
		}
		if(isAProblem)
		{
			Iamready  = false;
			txt = "Samplerate mismatch, constraint: ";
			txt += jvx_size2String(_common_set_node_params_1io.preferred.samplerate.min);
			txt += "<=";
			txt += jvx_int2String(JVX_PROCESSINGNODE_CLASSNAME_GEN::properties_parameters.samplerate.value);
			txt+= "<=";
			txt += jvx_size2String(_common_set_node_params_1io.preferred.samplerate.max);
		}

		/*
		 * CHECK NUMBER INPUT CHANNELS
		 */
		isAProblem = false;
		if(_common_set_node_params_1io.preferred.number_input_channels.min >= 0)
		{
			if(JVX_PROCESSINGNODE_CLASSNAME_GEN::properties_parameters.numberinputchannels.value < _common_set_node_params_1io.preferred.number_input_channels.min)
			{
				isAProblem = true;
			}
		}
		if(_common_set_node_params_1io.preferred.number_input_channels.max >= 0)
		{
			if(JVX_PROCESSINGNODE_CLASSNAME_GEN::properties_parameters.numberinputchannels.value > _common_set_node_params_1io.preferred.number_input_channels.max)
			{
				isAProblem = true;
			}
		}
		if(isAProblem)
		{
			Iamready  = false;
			txt = "Input Channel mismatch, constraint: ";
			txt += jvx_size2String(_common_set_node_params_1io.preferred.number_input_channels.min);
			txt += "<=";
			txt += jvx_int2String(JVX_PROCESSINGNODE_CLASSNAME_GEN::properties_parameters.numberinputchannels.value);
			txt+= "<=";
			txt += jvx_size2String(_common_set_node_params_1io.preferred.number_input_channels.max);
		}


		/*
		 * CHECK NUMBER OUTPUT CHANNELS
		 */
		isAProblem = false;

		if(_common_set_node_params_1io.preferred.number_output_channels.min >= 0)
		{
			if(JVX_PROCESSINGNODE_CLASSNAME_GEN::properties_parameters.numberoutputchannels.value < _common_set_node_params_1io.preferred.number_output_channels.min)
			{
				isAProblem = true;
			}
		}
		if(_common_set_node_params_1io.preferred.number_output_channels.max >= 0)
		{
			if(JVX_PROCESSINGNODE_CLASSNAME_GEN::properties_parameters.numberoutputchannels.value > _common_set_node_params_1io.preferred.number_output_channels.max)
			{
				isAProblem = true;
			}
		}


		if(isAProblem)
		{
			Iamready  = false;
			txt = "Output Channel mismatch, constraint: ";
			txt += jvx_size2String(_common_set_node_params_1io.preferred.number_output_channels.min);
			txt += "<=";
			txt += jvx_int2String(JVX_PROCESSINGNODE_CLASSNAME_GEN::properties_parameters.numberoutputchannels.value);
			txt+= "<=";
			txt += jvx_size2String(_common_set_node_params_1io.preferred.number_output_channels.max);
		}

		/*
		 * CHECK PROCESSING FORMAT
		 */
		isAProblem = false;

		if(_common_set_node_params_1io.preferred.format.min > JVX_DATAFORMAT_NONE)
		{
			if(JVX_PROCESSINGNODE_CLASSNAME_GEN::properties_parameters.format.value < _common_set_node_params_1io.preferred.format.min)
			{
				isAProblem = true;
			}
		}
		if(_common_set_node_params_1io.preferred.format.max > JVX_DATAFORMAT_NONE)
		{
			if(JVX_PROCESSINGNODE_CLASSNAME_GEN::properties_parameters.format.value > _common_set_node_params_1io.preferred.format.max)
			{
				isAProblem = true;
			}
		}


		if(isAProblem)
		{
			Iamready  = false;
			txt = "Processing Format mismatch, constraint: ";
			txt += jvxDataFormat_txt(_common_set_node_params_1io.preferred.format.min);
			txt += "<=";
			txt += jvxDataFormat_txt(JVX_PROCESSINGNODE_CLASSNAME_GEN::properties_parameters.format.value);
			txt+= "<=";
			txt += jvxDataFormat_txt(_common_set_node_params_1io.preferred.format.max);
		}


		if(!txt.empty())
		{
			if (reasonIfNot)
			{
				reasonIfNot->assign(txt);
			}
		}
		if(suc)
		{
			*suc = Iamready;
		}
	}

	return(res);
};

jvxErrorType
JVX_PROCESSINGNODE_CLASSNAME::start()
{
	return(_start());
};

jvxErrorType
JVX_PROCESSINGNODE_CLASSNAME::stop()
{
	return(_stop());
};

jvxErrorType
JVX_PROCESSINGNODE_CLASSNAME::postprocess()
{
	jvxErrorType res =_postprocess();
	if(res == JVX_NO_ERROR)
	{

	}
	return res;
};

jvxErrorType
JVX_PROCESSINGNODE_CLASSNAME::postprocess_lock()
{
	jvxErrorType res =_postprocess_lock();
	if(res == JVX_NO_ERROR)
	{
	}
	return res;
};

jvxErrorType
JVX_PROCESSINGNODE_CLASSNAME::reference_object(IjvxObject** refObject)
{
	return(_reference_object(refObject));
};

jvxErrorType
JVX_PROCESSINGNODE_CLASSNAME::prepare_sender_to_receiver(jvxLinkDataDescriptor* theData)
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
				res = jvx_allocateDataLinkDescriptor(theData, true);
			}
		}
		else
		{
			res = jvx_allocateDataLinkDescriptor(theData, true);
		}
	}
	return(res);
};

jvxErrorType
JVX_PROCESSINGNODE_CLASSNAME::prepare_complete_receiver_to_sender(jvxLinkDataDescriptor* theData)
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
JVX_PROCESSINGNODE_CLASSNAME::supports_multithreading(jvxBool* supports)
{
	return(_supports_multithreading(supports));
};

/* TODO: Add member functions for multithreading negotiations */

jvxErrorType
JVX_PROCESSINGNODE_CLASSNAME::process_mt(jvxLinkDataDescriptor* theData, jvxSize idx_sender_to_receiver, jvxSize idx_receiver_to_sender, jvxSize* channelSelect , jvxSize numEntriesChannels, jvxInt32 offset_input, jvxInt32 offset_output, jvxInt32 numEntries)
{
	return(_process_mt(theData, idx_sender_to_receiver, idx_receiver_to_sender, channelSelect, numEntriesChannels, offset_input, offset_output, numEntries));
};

jvxErrorType
JVX_PROCESSINGNODE_CLASSNAME::reconfigured_receiver_to_sender(jvxLinkDataDescriptor* theData)
{
	return(JVX_NO_ERROR);
};

jvxErrorType
JVX_PROCESSINGNODE_CLASSNAME::process_st(jvxLinkDataDescriptor* theData, jvxSize idx_sender_to_receiver, jvxSize idx_receiver_to_sender)
{
	return(_process_st(theData, idx_sender_to_receiver, idx_receiver_to_sender));
};

jvxErrorType
JVX_PROCESSINGNODE_CLASSNAME::last_error_process(char* fld_text, jvxSize fldSize, jvxErrorType* err, jvxInt32* id_error, jvxLinkDataDescriptor* theData)
{
	return(_last_error_process(fld_text, fldSize, err, id_error, theData));
};

jvxErrorType
JVX_PROCESSINGNODE_CLASSNAME::postprocess_sender_to_receiver(jvxLinkDataDescriptor* theData)
{
	jvxErrorType res = JVX_ERROR_WRONG_STATE;
	if(_common_set_min.theState == JVX_STATE_PREPARED)
	{
		// Deallocate memory
		res = jvx_deallocateDataLinkDescriptor(theData, true);
		_common_set_node_params_1io.processing.format = JVX_DATAFORMAT_NONE;
		_common_set_node_params_1io.processing.number_input_channels = -1;
		_common_set_node_params_1io.processing.buffersize = -1;
		_common_set_node_params_1io.processing.samplerate = -1;
	}
	return(res);
};

jvxErrorType
JVX_PROCESSINGNODE_CLASSNAME::before_postprocess_receiver_to_sender(jvxLinkDataDescriptor* theData)
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
		_common_set_node_params_1io.processing.number_output_channels = -1;
	}
	else
	{
		res = JVX_ERROR_WRONG_STATE;
	}

	return(res);
};

jvxErrorType
JVX_PROCESSINGNODE_CLASSNAME::put_configuration(jvxCallManagerConfiguration* callConf,
	IjvxConfigProcessor* processor,
	jvxHandle* sectionToContainAllSubsectionsForMe,
	const char* filename,
	jvxInt32 lineno)
{
	if(_common_set_min.theState == JVX_STATE_ACTIVE)
	{
#ifdef JVX_ALL_AUDIO_TO_LOGFILE
		if(JVX_PROCESSINGNODE_CLASSNAME_GEN::audiologtofile.properties_logtofile.activateLogToFile.value)
		{
			theLogger.activate_logging(JVX_PROCESSINGNODE_CLASSNAME_GEN::audiologtofile.properties_logtofile.logFileFolder.value, JVX_PROCESSINGNODE_CLASSNAME_GEN::audiologtofile.properties_logtofile.logFilePrefix.value);
		}
#endif
	}
     return JVX_NO_ERROR;
}

#ifdef JVX_ALL_AUDIO_TO_LOGFILE
jvxErrorType
JVX_PROCESSINGNODE_CLASSNAME::set_property(jvxHandle* fld,
							jvxSize numElements,
							jvxDataFormat format,
							jvxSize propId,
							jvxPropertyCategoryType category,
							jvxSize offsetStart,
							jvxBool contentOnly,
							jvxPropertyCallPurpose callPurpose)
{
	jvxErrorType res = _set_property(fld,
			     numElements,
			     format,
			     propId,
			     category,
			     offsetStart,
			     contentOnly,
			     callPurpose,
			     static_cast<IjvxObject*>(this));
	if(res == JVX_NO_ERROR)
	{

		if(
			(propId == JVX_PROCESSINGNODE_CLASSNAME_GEN::audiologtofile.properties_logtofile.activateLogToFile.id) &&
			(category == JVX_PROCESSINGNODE_CLASSNAME_GEN::audiologtofile.properties_logtofile.activateLogToFile.cat))
		{
			if(JVX_PROCESSINGNODE_CLASSNAME_GEN::audiologtofile.properties_logtofile.activateLogToFile.value)
			{
				theLogger.activate_logging(JVX_PROCESSINGNODE_CLASSNAME_GEN::audiologtofile.properties_logtofile.logFileFolder.value, JVX_PROCESSINGNODE_CLASSNAME_GEN::audiologtofile.properties_logtofile.logFilePrefix.value);
			}
			else
			{
				theLogger.deactivate_logging();
			}
		}

	}
	return res;
}
#endif

jvxErrorType
JVX_PROCESSINGNODE_CLASSNAME::get_configuration(jvxCallManagerConfiguration* callConf, IjvxConfigProcessor* processor,
				 jvxHandle* sectionWhereToAddAllSubsections)
{
     return JVX_NO_ERROR;
}

#ifdef JVX_ALL_AUDIO_TO_LOGFILE

jvxErrorType 
JVX_PROCESSINGNODE_CLASSNAME::log_prepare_all_channels()
{
	jvxErrorType res = JVX_NO_ERROR;
	res = theLogger.add_one_lane(_common_set_node_params_1io.processing.number_input_channels,
		_common_set_node_params_1io.processing.buffersize,
		_common_set_node_params_1io.processing.format,
		"audio/in/channel", 0, idInChannels);
	
	theLogger.add_tag("/audio/in/samplerate", jvx_int2String(_common_set_node_params_1io.processing.samplerate).c_str());
	theLogger.add_tag("/audio/in/buffersize", jvx_size2String(_common_set_node_params_1io.processing.buffersize).c_str());
	theLogger.add_tag("/audio/in/format", jvxDataFormat_txt(_common_set_node_params_1io.processing.format));
	theLogger.add_tag("audio/in/channels", jvx_int2String(_common_set_node_params_1io.processing.number_input_channels).c_str());

	if(res != JVX_NO_ERROR)
	{
		return res;
	}
	res = theLogger.add_one_lane(_common_set_node_params_1io.processing.number_output_channels,
		_common_set_node_params_1io.processing.buffersize,
		_common_set_node_params_1io.processing.format,
		"audio/out/channel", 0, idOutChannels);
	theLogger.add_tag("/audio/out/samplerate", jvx_int2String(_common_set_node_params_1io.processing.samplerate).c_str());
	theLogger.add_tag("/audio/out/buffersize", jvx_size2String(_common_set_node_params_1io.processing.buffersize).c_str());
	theLogger.add_tag("/audio/out/format", jvxDataFormat_txt(_common_set_node_params_1io.processing.format));
	theLogger.add_tag("/audio/out/channels", jvx_int2String(_common_set_node_params_1io.processing.number_output_channels).c_str());

	return res;

}

jvxErrorType
JVX_PROCESSINGNODE_CLASSNAME::log_start_all_channels()
{
	jvxErrorType res = JVX_NO_ERROR;
	
	res = theLogger.start_processing();
	JVX_PROCESSINGNODE_CLASSNAME_GEN::audiologtofile.properties_logtofile.fillHeightBuffer.value.val = 0;
	return res;
}

jvxErrorType 
JVX_PROCESSINGNODE_CLASSNAME::log_stop_all_channels()
{
	jvxErrorType res = JVX_NO_ERROR;
	// Stop logfile if active
	res = theLogger.stop_processing();
	if(res != JVX_NO_ERROR)
	{
		return res;
	}

	theLogger.remove_all_tags();
	theLogger.reactivate_logging();
	
	return res;
}

jvxErrorType 
JVX_PROCESSINGNODE_CLASSNAME::log_postprocess_all_channels()
{
	jvxErrorType res = JVX_NO_ERROR;
	res = theLogger.remove_all_lanes();
	
	if(res != JVX_NO_ERROR)
	{
		return res;
	}
	idInChannels = 0;
	idOutChannels = 0;
	return(res);
}

jvxErrorType
JVX_PROCESSINGNODE_CLASSNAME::log_write_all_channels(jvxLinkDataDescriptor* theData, jvxSize idx_sender_to_receiver, jvxSize idx_receiver_to_sender)
{
	jvxErrorType res = JVX_NO_ERROR;
	res = theLogger.process_logging_try(idInChannels, theData->con_data.buffers[idx_sender_to_receiver], 
		theData->con_params.number_channels, 
		theData->con_params.format, 
		theData->con_params.buffersize, NULL);
	if(res != JVX_NO_ERROR)
	{
		return(res);
	}

	res = theLogger.process_logging_try(idOutChannels, theData->con_compat.from_receiver_buffer_allocated_by_sender[idx_receiver_to_sender], 
		theData->con_compat.number_channels,
		theData->con_compat.format, 
		theData->con_compat.buffersize, 
		&JVX_PROCESSINGNODE_CLASSNAME_GEN::audiologtofile.properties_logtofile.fillHeightBuffer.value.val);
	
	return(res);
}
#endif

#ifdef JVX_PROJECT_NAMESPACE
}
#endif
