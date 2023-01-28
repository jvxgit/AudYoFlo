#include "jvx.h"
#include "jvxVideoNodes/CjvxVideoNode.h"

#ifdef JVX_PROJECT_NAMESPACE
namespace JVX_PROJECT_NAMESPACE {
#endif

JVX_VIDEONODE_CLASSNAME::JVX_VIDEONODE_CLASSNAME(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE):
	CjvxObject(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL), CjvxProperties(module_name, *this)
{
	_common_set.theComponentType.unselected(JVX_COMPONENT_VIDEO_NODE);
	_common_set.theObjectSpecialization = reinterpret_cast<jvxHandle*>(static_cast<IjvxNode*>(this));
	_common_set.thisisme = static_cast<IjvxObject*>(this);
	_common_spec_proc_set.thisisme = static_cast<IjvxObject*>(this);

	_common_set_video_params.acceptOnlyExactMatchLinkDataInput = false;
	_common_set_video_params.acceptOnlyExactMatchLinkDataOutput = false;

	_common_set_video_params.preferred.format.min = JVX_DATAFORMAT_NONE;
	_common_set_video_params.preferred.format.max = JVX_DATAFORMAT_NONE;
	_common_set_video_params.preferred.seg_x.min = JVX_SIZE_UNSELECTED;
	_common_set_video_params.preferred.seg_x.max = JVX_SIZE_UNSELECTED;
	_common_set_video_params.preferred.seg_y.min = JVX_SIZE_UNSELECTED;
	_common_set_video_params.preferred.seg_y.max = JVX_SIZE_UNSELECTED;
	_common_set_video_params.preferred.subformat.min = JVX_DATAFORMAT_GROUP_NONE;
	_common_set_video_params.preferred.subformat.max = JVX_DATAFORMAT_GROUP_NONE;

	_common_set_video_params.processing.format= JVX_DATAFORMAT_NONE;
	_common_set_video_params.processing.seg_x = JVX_SIZE_UNSELECTED;
	_common_set_video_params.processing.seg_y = JVX_SIZE_UNSELECTED;
	_common_set_video_params.processing.subformat = JVX_DATAFORMAT_GROUP_NONE;

	_common_set.theComponentSubTypeDescriptor = "signal_processing_node/video_node";

	// Avoid infinite loops
	_common_set_video_params.report_take_over_started = false;

#ifdef JVX_ALL_AUDIO_TO_LOGFILE
	_common_set_audio_log_to_file.numFramesLog = 100;
	_common_set_audio_log_to_file.timeoutMsec = 5000;
#endif
	_common_set_video_params.settingReadyToStart = true;

	/* TODO
	CjvxInputOutputConnector::lds_activate(static_cast<IjvxDataProcessor*>(this), static_cast<IjvxObject*>(this), static_cast<IjvxInputConnector*>(this));
	*/
}

JVX_VIDEONODE_CLASSNAME::~JVX_VIDEONODE_CLASSNAME()
{
}

jvxErrorType
JVX_VIDEONODE_CLASSNAME::activate()
{
	jvxErrorType res = _activate();
	if(res == JVX_NO_ERROR)
	{
		JVX_ACTIVATE_DEFAULT_ONE_IN_ONE_OUT_CONNECTORS(static_cast<IjvxDataProcessor*>(this), 
			static_cast<IjvxObject*>(this), "default", NULL, _common_set.theModuleName.c_str());

		JVX_VIDEONODE_CLASSNAME_GEN::init_all();
		JVX_VIDEONODE_CLASSNAME_GEN::allocate_all();
		JVX_VIDEONODE_CLASSNAME_GEN::register_all(static_cast<CjvxProperties*>(this));

#ifdef JVX_ALL_AUDIO_TO_LOGFILE
		JVX_VIDEONODE_CLASSNAME_GEN::init__audiologtofile__properties_logtofile();
		JVX_VIDEONODE_CLASSNAME_GEN::allocate__audiologtofile__properties_logtofile();
		JVX_VIDEONODE_CLASSNAME_GEN::register__audiologtofile__properties_logtofile(static_cast<CjvxProperties*>(this), logger_prop_prefix);

		// Initialize logfile writer
		theLogger.init_logging(_common_set_min.theHostRef, _common_set_audio_log_to_file.numFramesLog, _common_set_audio_log_to_file.timeoutMsec);
#endif

	}
	return(res);
};

jvxErrorType
JVX_VIDEONODE_CLASSNAME::deactivate()
{
	jvxErrorType res = _deactivate();
	if(res == JVX_NO_ERROR)
	{
#ifdef JVX_ALL_AUDIO_TO_LOGFILE
		theLogger.terminate_logging();

		JVX_VIDEONODE_CLASSNAME_GEN::unregister__audiologtofile__properties_logtofile(static_cast<CjvxProperties*>(this));
		JVX_VIDEONODE_CLASSNAME_GEN::deallocate__audiologtofile__properties_logtofile();
#endif


		JVX_VIDEONODE_CLASSNAME_GEN::unregister_all(static_cast<CjvxProperties*>(this));
		JVX_VIDEONODE_CLASSNAME_GEN::deallocate_all();

		JVX_DEACTIVATE_DEFAULT_ONE_IN_ONE_OUT_CONNECTORS();

	}
	return(res);
};

jvxErrorType
JVX_VIDEONODE_CLASSNAME::prepare()
{
	jvxErrorType res = _prepare( );
	if(res == JVX_NO_ERROR)
	{

	}
	return(res);
}

jvxErrorType
JVX_VIDEONODE_CLASSNAME::prepare_lock()
{
	jvxErrorType res = _prepare_lock();
	if(res == JVX_NO_ERROR)
	{
	}
	return(res);
}

jvxErrorType
JVX_VIDEONODE_CLASSNAME::reportPreferredParameters(jvxPropertyCategoryType cat, jvxSize propId)
{
	jvxInt32 tmp32;
	jvxInt16 tmp16;
	jvxErrorType resL = JVX_NO_ERROR;
	jvxBool requiresCorrection = false;
	jvxBool transferAll = false;

	if (JVX_CHECK_SIZE_UNSELECTED(propId))
	{
		transferAll = true;
	}

	if(transferAll || 
		(
			JVX_PROPERTY_CHECK_ID_CAT(propId, cat,  JVX_VIDEONODE_CLASSNAME_GEN::video.segmentsize_x)))
	{
		requiresCorrection = false;

		tmp32 = -1;
		if(JVX_CHECK_SIZE_SELECTED(_common_set_video_params.preferred.seg_x.max))
		{
			if(JVX_VIDEONODE_CLASSNAME_GEN::video.segmentsize_x.value > _common_set_video_params.preferred.seg_x.max)
			{
			  tmp32 = JVX_SIZE_INT32(_common_set_video_params.preferred.seg_x.max);
				requiresCorrection = true;
			}
		}
		if(JVX_CHECK_SIZE_SELECTED(_common_set_video_params.preferred.seg_x.min))
		{
			if(JVX_VIDEONODE_CLASSNAME_GEN::video.segmentsize_x.value < _common_set_video_params.preferred.seg_x.min)
			{
			  tmp32 = JVX_SIZE_INT32(_common_set_video_params.preferred.seg_x.min);
				requiresCorrection = true;
			}
		}

		if(requiresCorrection && !_common_set_video_params.report_take_over_started )
		{
			_common_set_video_params.report_take_over_started = true;

			// indirect set via technology - if possible
			resL = this->_report_take_over_property(&tmp32, 1, JVX_DATAFORMAT_32BIT_LE, 0, true,
				JVX_VIDEONODE_CLASSNAME_GEN::video.segmentsize_x.category,
				JVX_VIDEONODE_CLASSNAME_GEN::video.segmentsize_x.globalIdx,
				JVX_COMPONENT_VIDEO_DEVICE);

			_common_set_video_params.report_take_over_started = false;
		}
	}

	if (transferAll ||
		(
			JVX_PROPERTY_CHECK_ID_CAT(propId, cat, JVX_VIDEONODE_CLASSNAME_GEN::video.segmentsize_y)))
	{
		requiresCorrection = false;

		tmp32 = -1;
		if (JVX_CHECK_SIZE_SELECTED(_common_set_video_params.preferred.seg_y.max))
		{
			if (JVX_VIDEONODE_CLASSNAME_GEN::video.segmentsize_y.value > _common_set_video_params.preferred.seg_y.max)
			{
			  tmp32 = JVX_SIZE_INT32(_common_set_video_params.preferred.seg_y.max);
				requiresCorrection = true;
			}
		}
		if (JVX_CHECK_SIZE_SELECTED(_common_set_video_params.preferred.seg_y.min))
		{
			if (JVX_VIDEONODE_CLASSNAME_GEN::video.segmentsize_y.value < _common_set_video_params.preferred.seg_y.min)
			{
			  tmp32 = JVX_SIZE_INT32(_common_set_video_params.preferred.seg_y.min);
				requiresCorrection = true;
			}
		}

		if (requiresCorrection && !_common_set_video_params.report_take_over_started)
		{
			_common_set_video_params.report_take_over_started = true;

			// indirect set via technology - if possible
			resL = this->_report_take_over_property(&tmp32, 1, JVX_DATAFORMAT_32BIT_LE, 0, true,
				JVX_VIDEONODE_CLASSNAME_GEN::video.segmentsize_y.category,
				JVX_VIDEONODE_CLASSNAME_GEN::video.segmentsize_y.globalIdx,
				JVX_COMPONENT_VIDEO_DEVICE);

			_common_set_video_params.report_take_over_started = false;
		}
	}

	if (transferAll ||
		(
			JVX_PROPERTY_CHECK_ID_CAT(propId, cat, JVX_VIDEONODE_CLASSNAME_GEN::video.segmentsize_y)))
	{
		requiresCorrection = false;

		tmp32 = -1;
		if (_common_set_video_params.preferred.format.max != JVX_DATAFORMAT_NONE)
		{
			if (JVX_VIDEONODE_CLASSNAME_GEN::video.format.value > _common_set_video_params.preferred.format.max)
			{
				tmp16 = _common_set_video_params.preferred.format.max;
				requiresCorrection = true;
			}
		}
		if (_common_set_video_params.preferred.format.min != JVX_DATAFORMAT_NONE)
		{
			if (JVX_VIDEONODE_CLASSNAME_GEN::video.format.value < _common_set_video_params.preferred.format.min)
			{
				tmp16 = _common_set_video_params.preferred.format.min;
				requiresCorrection = true;
			}
		}

		if (requiresCorrection && !_common_set_video_params.report_take_over_started)
		{
			_common_set_video_params.report_take_over_started = true;

			// indirect set via technology - if possible
			resL = this->_report_take_over_property(&tmp16, 1, JVX_DATAFORMAT_16BIT_LE, 0, true,
				JVX_VIDEONODE_CLASSNAME_GEN::video.format.category,
				JVX_VIDEONODE_CLASSNAME_GEN::video.format.globalIdx,
				JVX_COMPONENT_VIDEO_DEVICE);

			_common_set_video_params.report_take_over_started = false;
		}
	}

	if (transferAll ||
		(
			JVX_PROPERTY_CHECK_ID_CAT(propId, cat, JVX_VIDEONODE_CLASSNAME_GEN::video.subformat)))
	{
		requiresCorrection = false;

		tmp32 = -1;
		if (_common_set_video_params.preferred.subformat.max != JVX_DATAFORMAT_GROUP_NONE)
		{
			if (JVX_VIDEONODE_CLASSNAME_GEN::video.subformat.value > _common_set_video_params.preferred.subformat.max)
			{
				tmp16 = _common_set_video_params.preferred.subformat.max;
				requiresCorrection = true;
			}
		}
		if (_common_set_video_params.preferred.subformat.min != JVX_DATAFORMAT_GROUP_NONE)
		{
			if (JVX_VIDEONODE_CLASSNAME_GEN::video.subformat.value < _common_set_video_params.preferred.subformat.min)
			{
				tmp16 = _common_set_video_params.preferred.subformat.min;
				requiresCorrection = true;
			}
		}

		if (requiresCorrection && !_common_set_video_params.report_take_over_started)
		{
			_common_set_video_params.report_take_over_started = true;

			// indirect set via technology - if possible
			resL = this->_report_take_over_property(&tmp16, 1, JVX_DATAFORMAT_16BIT_LE, 0, true,
				JVX_VIDEONODE_CLASSNAME_GEN::video.subformat.category,
				JVX_VIDEONODE_CLASSNAME_GEN::video.subformat.globalIdx,
				JVX_COMPONENT_VIDEO_DEVICE);

			_common_set_video_params.report_take_over_started = false;
		}
	}

	return(resL);
}

jvxErrorType
JVX_VIDEONODE_CLASSNAME::is_ready(jvxBool* suc, jvxApiString* reasonIfNot)
{
	std::string txt;
	jvxErrorType res = _is_ready( suc, reasonIfNot);
	jvxBool Iamready = true;
	jvxBool isAProblem = false;

	if(res == JVX_NO_ERROR)
	{
		
		isAProblem = false;
		if(JVX_CHECK_SIZE_SELECTED(_common_set_video_params.preferred.seg_x.min))
		{
			if(JVX_VIDEONODE_CLASSNAME_GEN::video.segmentsize_x.value < _common_set_video_params.preferred.seg_x.min)
			{
				isAProblem = true;
			}
		}
		if(JVX_CHECK_SIZE_SELECTED(_common_set_video_params.preferred.seg_x.max))
		{
			if(JVX_VIDEONODE_CLASSNAME_GEN::video.segmentsize_x.value > _common_set_video_params.preferred.seg_x.max)
			{
				isAProblem = true;
			}
		}
		if(isAProblem)
		{
			Iamready  = false;
			txt = "Video size mismatch (x), constraint: ";
			if (JVX_CHECK_SIZE_SELECTED(_common_set_video_params.preferred.seg_x.min))
			{
				txt += jvx_size2String(_common_set_video_params.preferred.seg_x.min);
				txt += "<=";
			}
			txt += jvx_int2String(JVX_VIDEONODE_CLASSNAME_GEN::video.segmentsize_x.value);
			if (JVX_CHECK_SIZE_SELECTED(_common_set_video_params.preferred.seg_x.max))
			{
				txt += "<=";
				txt += jvx_size2String(_common_set_video_params.preferred.seg_x.max);
			}
		}

		isAProblem = false;
		if (JVX_CHECK_SIZE_SELECTED(_common_set_video_params.preferred.seg_x.min))
		{
			if (JVX_VIDEONODE_CLASSNAME_GEN::video.segmentsize_x.value < _common_set_video_params.preferred.seg_x.min)
			{
				isAProblem = true;
			}
		}
		if (JVX_CHECK_SIZE_SELECTED(_common_set_video_params.preferred.seg_x.max))
		{
			if (JVX_VIDEONODE_CLASSNAME_GEN::video.segmentsize_x.value > _common_set_video_params.preferred.seg_x.max)
			{
				isAProblem = true;
			}
		}
		if (isAProblem)
		{
			Iamready = false;
			txt = "Video size mismatch (y), constraint: ";
			if (JVX_CHECK_SIZE_SELECTED(_common_set_video_params.preferred.seg_y.min))
			{
				txt += jvx_size2String(_common_set_video_params.preferred.seg_y.min);
				txt += "<=";
			}
			txt += jvx_int2String(JVX_VIDEONODE_CLASSNAME_GEN::video.segmentsize_y.value);
			if (JVX_CHECK_SIZE_SELECTED(_common_set_video_params.preferred.seg_y.max))
			{
				txt += "<=";
				txt += jvx_size2String(_common_set_video_params.preferred.seg_y.max);
			}
		}

		isAProblem = false;
		if (_common_set_video_params.preferred.format.min != JVX_DATAFORMAT_NONE)
		{
			if (JVX_VIDEONODE_CLASSNAME_GEN::video.format.value < _common_set_video_params.preferred.format.min)
			{
				isAProblem = true;
			}
		}
		if (_common_set_video_params.preferred.format.max != JVX_DATAFORMAT_NONE)
		{
			if (JVX_VIDEONODE_CLASSNAME_GEN::video.format.value > _common_set_video_params.preferred.format.max)
			{
				isAProblem = true;
			}
		}
		if (isAProblem)
		{
			Iamready = false;
			txt = "Video format mismatch, constraint: ";
			if (_common_set_video_params.preferred.format.min != JVX_DATAFORMAT_NONE)
			{
				txt += jvxDataFormat_txt(_common_set_video_params.preferred.format.min);
				txt += "<=";
			}
			txt += jvx_int2String(JVX_VIDEONODE_CLASSNAME_GEN::video.format.value);
			if (_common_set_video_params.preferred.format.max != JVX_DATAFORMAT_NONE)
			{
				txt += "<=";
				txt += jvx_int2String(_common_set_video_params.preferred.format.max);
			}
		}

		isAProblem = false;
		if (_common_set_video_params.preferred.subformat.min != JVX_DATAFORMAT_GROUP_NONE)
		{
			if (JVX_VIDEONODE_CLASSNAME_GEN::video.subformat.value < _common_set_video_params.preferred.subformat.min)
			{
				isAProblem = true;
			}
		}
		if (_common_set_video_params.preferred.subformat.max != JVX_DATAFORMAT_GROUP_NONE)
		{
			if (JVX_VIDEONODE_CLASSNAME_GEN::video.subformat.value > _common_set_video_params.preferred.subformat.max)
			{
				isAProblem = true;
			}
		}
		if (isAProblem)
		{
			Iamready = false;
			txt = "Video subformat mismatch, constraint: ";
			if (_common_set_video_params.preferred.subformat.min != JVX_DATAFORMAT_GROUP_NONE)
			{
				txt += jvxDataFormat_txt(_common_set_video_params.preferred.subformat.min);
				txt += "<=";
			}
			txt += jvx_int2String(JVX_VIDEONODE_CLASSNAME_GEN::video.subformat.value);
			if (_common_set_video_params.preferred.subformat.max != JVX_DATAFORMAT_GROUP_NONE)
			{
				txt += "<=";
				txt += jvx_int2String(_common_set_video_params.preferred.subformat.max);
			}
		}
		// =========================================================================

		if (!_common_set_video_params.settingReadyToStart)
		{
			Iamready = false;
			txt = "Algorithm is not propperly configured: ";
			txt += _common_set_video_params.settingReadyFailReason;
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
JVX_VIDEONODE_CLASSNAME::start()
{
	return(_start());
};

jvxErrorType
JVX_VIDEONODE_CLASSNAME::stop()
{
	return(_stop());
};

jvxErrorType
JVX_VIDEONODE_CLASSNAME::postprocess()
{
	jvxErrorType res =_postprocess();
	if(res == JVX_NO_ERROR)
	{

	}
	return res;
};

jvxErrorType
JVX_VIDEONODE_CLASSNAME::postprocess_lock()
{
	jvxErrorType res =_postprocess_lock();
	if(res == JVX_NO_ERROR)
	{
	}
	return res;
};

jvxErrorType
JVX_VIDEONODE_CLASSNAME::reference_object(IjvxObject** refObject)
{
	return(_reference_object(refObject));
};

jvxErrorType
JVX_VIDEONODE_CLASSNAME::prepare_sender_to_receiver(jvxLinkDataDescriptor* theData)
{
	jvxErrorType res = JVX_ERROR_WRONG_STATE;
	return(JVX_ERROR_UNSUPPORTED);
};

jvxErrorType
JVX_VIDEONODE_CLASSNAME::prepare_complete_receiver_to_sender(jvxLinkDataDescriptor* theData)
{
	jvxErrorType res = JVX_NO_ERROR;
	return(JVX_ERROR_UNSUPPORTED);
};

jvxErrorType
JVX_VIDEONODE_CLASSNAME::supports_multithreading(jvxBool* supports)
{
	return(_supports_multithreading(supports));
};

/* TODO: Add member functions for multithreading negotiations */

jvxErrorType
JVX_VIDEONODE_CLASSNAME::process_mt(jvxLinkDataDescriptor* theData, jvxSize idx_sender_to_receiver, jvxSize idx_receiver_to_sender, jvxSize* channelSelect , jvxSize numEntriesChannels, jvxInt32 offset_input, jvxInt32 offset_output, jvxInt32 numEntries)
{
	return(_process_mt(theData, idx_sender_to_receiver, idx_receiver_to_sender, channelSelect, numEntriesChannels, offset_input, offset_output, numEntries));
};

jvxErrorType
JVX_VIDEONODE_CLASSNAME::reconfigured_receiver_to_sender(jvxLinkDataDescriptor* theData)
{
	return(JVX_NO_ERROR);
};

jvxErrorType
JVX_VIDEONODE_CLASSNAME::process_st(jvxLinkDataDescriptor* theData, jvxSize idx_sender_to_receiver, jvxSize idx_receiver_to_sender)
{
	return(_process_st(theData, idx_sender_to_receiver, idx_receiver_to_sender));
};

jvxErrorType
JVX_VIDEONODE_CLASSNAME::last_error_process(char* fld_text, jvxSize fldSize, jvxErrorType* err, jvxInt32* id_error, jvxLinkDataDescriptor* theData)
{
	return(_last_error_process(fld_text, fldSize, err, id_error, theData));
};

jvxErrorType
JVX_VIDEONODE_CLASSNAME::postprocess_sender_to_receiver(jvxLinkDataDescriptor* theData)
{
	jvxErrorType res = JVX_ERROR_WRONG_STATE;
	return(JVX_ERROR_UNSUPPORTED);
};

jvxErrorType
JVX_VIDEONODE_CLASSNAME::before_postprocess_receiver_to_sender(jvxLinkDataDescriptor* theData)
{
	jvxErrorType res = JVX_NO_ERROR;
	return(JVX_ERROR_UNSUPPORTED);
};

jvxErrorType
JVX_VIDEONODE_CLASSNAME::put_configuration(jvxCallManagerConfiguration* callConf, IjvxConfigProcessor* processor,
				 jvxHandle* sectionToContainAllSubsectionsForMe, 
				 const char* filename,
				 jvxInt32 lineno)
{
	if(_common_set_min.theState == JVX_STATE_ACTIVE)
	{
#ifdef JVX_ALL_AUDIO_TO_LOGFILE
		if(JVX_VIDEONODE_CLASSNAME_GEN::audiologtofile.properties_logtofile.activateLogToFile.value)
		{
			theLogger.activate_logging(JVX_VIDEONODE_CLASSNAME_GEN::audiologtofile.properties_logtofile.logFileFolder.value, JVX_VIDEONODE_CLASSNAME_GEN::audiologtofile.properties_logtofile.logFilePrefix.value);
		}
#endif
	}
     return JVX_NO_ERROR;
}

#ifdef JVX_ALL_AUDIO_TO_LOGFILE
jvxErrorType
JVX_VIDEONODE_CLASSNAME::set_property(jvxHandle* fld,
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
			(propId == JVX_VIDEONODE_CLASSNAME_GEN::audiologtofile.properties_logtofile.activateLogToFile.id) &&
			(category == JVX_VIDEONODE_CLASSNAME_GEN::audiologtofile.properties_logtofile.activateLogToFile.cat))
		{
			if(JVX_VIDEONODE_CLASSNAME_GEN::audiologtofile.properties_logtofile.activateLogToFile.value)
			{
				theLogger.activate_logging(JVX_VIDEONODE_CLASSNAME_GEN::audiologtofile.properties_logtofile.logFileFolder.value, JVX_VIDEONODE_CLASSNAME_GEN::audiologtofile.properties_logtofile.logFilePrefix.value);
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
JVX_VIDEONODE_CLASSNAME::get_configuration(jvxCallManagerConfiguration* callConf, IjvxConfigProcessor* processor,
				 jvxHandle* sectionWhereToAddAllSubsections)
{
     return JVX_NO_ERROR;
}

#if 0
jvxErrorType
JVX_VIDEONODE_CLASSNAME::prepare_connect_icon(jvxLinkDataDescriptor* theData)
{
	jvxErrorType res = JVX_NO_ERROR;
	if (theData->link.next)
	{
		jvx_initDataLinkDescriptor(&myData4Next);
		myData4Next.sender_to_receiver = theData->sender_to_receiver;
		myData4Next.con_link.master = theData->con_link.master;
		myData4Next.link.next = NULL;

		res = theData->link.next->prepare_connect_icon(&myData4Next);
		if (res == JVX_NO_ERROR)
		{
			theData->con_data.buffers =
				myData4Next.con_data.buffers;
			theData->pipeline.idx_stage = myData4Next.pipeline.idx_stage;
		}
	}
	else
	{
		res = jvx_allocateDataLinkDescriptor(theData, true);
	}
	return res;
};

jvxErrorType
JVX_VIDEONODE_CLASSNAME::postprocess_connect_icon(jvxLinkDataDescriptor* theData)
{
	jvxErrorType res = JVX_NO_ERROR;
	if (theData->link.next)
	{
		theData->con_data.buffers = NULL;
		res = theData->link.next->postprocess_connect_icon(&myData4Next);

		//myData4Next.sender_to_receiver = theData->sender_to_receiver;
		myData4Next.con_link.master = NULL;
		myData4Next.link.next = NULL;

		assert(res == JVX_NO_ERROR);

		// Reset this struct
		jvx_initDataLinkDescriptor(&myData4Next);
	}
	else
	{
		res = jvx_deallocateDataLinkDescriptor(theData, true);
	}
	return res;
};

jvxErrorType
JVX_VIDEONODE_CLASSNAME::start_connect_icon(jvxLinkDataDescriptor* theData)
{
	jvxErrorType res = JVX_NO_ERROR;
	if (theData->link.next)
	{
		theData->link.next->start_connect_icon(&myData4Next);
	}
	return res;
}

jvxErrorType
JVX_VIDEONODE_CLASSNAME::process_start_icon(jvxLinkDataDescriptor* theData)
{
	jvxErrorType res = JVX_NO_ERROR;
	if (theData->link.next)
	{
		res = theData->link.next->process_start_icon(&myData4Next);
	}
	return res;
};

jvxErrorType
JVX_VIDEONODE_CLASSNAME::process_buffers_icon(jvxLinkDataDescriptor* theData)
{
	jvxErrorType res = JVX_NO_ERROR;
	if (theData->link.next)
	{
		theData->link.next->process_buffers_icon(&myData4Next);
		theData->pipeline.idx_stage = myData4Next.pipeline.idx_stage;
	}
	return res;
};

jvxErrorType
JVX_VIDEONODE_CLASSNAME::process_stop_icon(jvxLinkDataDescriptor* theData)
{
	jvxErrorType res = JVX_NO_ERROR;
	if (theData->link.next)
	{
		theData->link.next->process_stop_icon(&myData4Next);
	}
	return res;
};

jvxErrorType
JVX_VIDEONODE_CLASSNAME::stop_connect_icon(jvxLinkDataDescriptor* theData)
{
	jvxErrorType res = JVX_NO_ERROR;
	if (theData->link.next)
	{
		theData->link.next->stop_connect_icon(&myData4Next);
	}
	return res;
};

jvxErrorType
JVX_VIDEONODE_CLASSNAME::link_request_reference_object(IjvxObject** obj)
{
	jvxErrorType res = JVX_NO_ERROR;
	if (obj)
	{
		*obj = static_cast<IjvxObject*>(this);
	}
	return res;
};

jvxErrorType
JVX_VIDEONODE_CLASSNAME::link_return_reference_object(IjvxObject* obj)
{
	jvxErrorType res = JVX_NO_ERROR;
	if (obj != static_cast<IjvxObject*>(this))
	{
		res = JVX_ERROR_ELEMENT_NOT_FOUND;
	}
	return res;
};
#endif

#ifdef JVX_ALL_AUDIO_TO_LOGFILE

jvxErrorType 
JVX_VIDEONODE_CLASSNAME::log_prepare_all_channels()
{
	jvxErrorType res = JVX_NO_ERROR;
	res = theLogger.add_one_lane(_common_set_video_params.processing.number_input_channels,
		_common_set_video_params.processing.buffersize,
		_common_set_video_params.processing.format,
		"audio/in/channel", 0, idInChannels);
	
	theLogger.add_tag("/audio/in/samplerate", jvx_int2String(_common_set_video_params.processing.samplerate).c_str());
	theLogger.add_tag("/audio/in/buffersize", jvx_size2String(_common_set_video_params.processing.buffersize).c_str());
	theLogger.add_tag("/audio/in/format", jvxDataFormat_txt(_common_set_video_params.processing.format));
	theLogger.add_tag("audio/in/channels", jvx_int2String(_common_set_video_params.processing.number_input_channels).c_str());

	if(res != JVX_NO_ERROR)
	{
		return res;
	}
	res = theLogger.add_one_lane(_common_set_video_params.processing.number_output_channels,
		_common_set_video_params.processing.buffersize,
		_common_set_video_params.processing.format,
		"audio/out/channel", 0, idOutChannels);
	theLogger.add_tag("/audio/out/samplerate", jvx_int2String(_common_set_video_params.processing.samplerate).c_str());
	theLogger.add_tag("/audio/out/buffersize", jvx_size2String(_common_set_video_params.processing.buffersize).c_str());
	theLogger.add_tag("/audio/out/format", jvxDataFormat_txt(_common_set_video_params.processing.format));
	theLogger.add_tag("/audio/out/channels", jvx_int2String(_common_set_video_params.processing.number_output_channels).c_str());

	return res;

}

jvxErrorType
JVX_VIDEONODE_CLASSNAME::log_start_all_channels()
{
	jvxErrorType res = JVX_NO_ERROR;
	
	res = theLogger.start_processing();
	JVX_VIDEONODE_CLASSNAME_GEN::audiologtofile.properties_logtofile.fillHeightBuffer.value.val = 0;
	return res;
}

jvxErrorType 
JVX_VIDEONODE_CLASSNAME::log_stop_all_channels()
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
JVX_VIDEONODE_CLASSNAME::log_postprocess_all_channels()
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
JVX_VIDEONODE_CLASSNAME::log_write_all_channels(jvxLinkDataDescriptor* theData, jvxSize idx_sender_to_receiver, jvxSize idx_receiver_to_sender)
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
		&JVX_VIDEONODE_CLASSNAME_GEN::audiologtofile.properties_logtofile.fillHeightBuffer.value.val);
	
	return(res);
}
#endif

#ifdef JVX_PROJECT_NAMESPACE
}
#endif
