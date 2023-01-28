#include "jvx.h"
#include "jvxAudioNodes/CjvxAudioNode_logfile.h"

CjvxAudioNode_logfile::CjvxAudioNode_logfile(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE):
	CjvxAudioNode(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
{
	_common_set_audio_log_to_file.numFramesLog = 100;
	_common_set_audio_log_to_file.timeoutMsec = 5000;
	
	//_common_set_ldslave.zeroCopyBuffering_cfg = false; <- done in base class
}

CjvxAudioNode_logfile::~CjvxAudioNode_logfile()
{
}

jvxErrorType
CjvxAudioNode_logfile::activate()
{
	jvxErrorType res = CjvxAudioNode::activate();
	jvxErrorType resL = JVX_NO_ERROR;

	if (res == JVX_NO_ERROR)
	{
		CjvxAudioNode_logfile_genpcg::init__audiologtofile__properties_logtofile();
		CjvxAudioNode_logfile_genpcg::allocate__audiologtofile__properties_logtofile();
		CjvxAudioNode_logfile_genpcg::register__audiologtofile__properties_logtofile(static_cast<CjvxProperties*>(this), logger_prop_prefix);

		// Initialize logfile writer
		resL = theLogger.init_logging(_common_set_min.theHostRef, _common_set_audio_log_to_file.numFramesLog, _common_set_audio_log_to_file.timeoutMsec);
		if (resL != JVX_NO_ERROR)
		{
			std::cout << "Failed to initialize log file writing: " << jvxErrorType_descr(resL) << std::endl;
		}
	}
	return(res);
};

jvxErrorType
CjvxAudioNode_logfile::deactivate()
{
	jvxErrorType res = CjvxAudioNode::deactivate();
	if(res == JVX_NO_ERROR)
	{
		theLogger.terminate_logging();

		CjvxAudioNode_logfile_genpcg::unregister__audiologtofile__properties_logtofile(static_cast<CjvxProperties*>(this));
		CjvxAudioNode_logfile_genpcg::deallocate__audiologtofile__properties_logtofile();

	}
	return(res);
}

jvxErrorType
CjvxAudioNode_logfile::put_configuration(jvxCallManagerConfiguration* callConf,
	IjvxConfigProcessor* processor,
	jvxHandle* sectionToContainAllSubsectionsForMe, 
	const char* filename,
	jvxInt32 lineno)
{
	jvxSize i;
	std::vector<std::string> warns;
	jvxErrorType res = CjvxAudioNode::put_configuration(callConf, processor,
		sectionToContainAllSubsectionsForMe, filename, lineno);
	
	if (_common_set_min.theState == JVX_STATE_ACTIVE)
	{
		CjvxAudioNode_logfile_genpcg::put_configuration_all(callConf, processor, sectionToContainAllSubsectionsForMe, &warns);
		for (i = 0; i < warns.size(); i++)
		{
			_report_text_message(warns[i].c_str(), JVX_REPORT_PRIORITY_WARNING);
		}
		
		if (CjvxAudioNode_logfile_genpcg::audiologtofile.properties_logtofile.activateLogToFile.value)
		{
			theLogger.activate_logging(CjvxAudioNode_logfile_genpcg::audiologtofile.properties_logtofile.logFileFolder.value, 
			CjvxAudioNode_logfile_genpcg::audiologtofile.properties_logtofile.logFilePrefix.value);
		}
	}
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxAudioNode_logfile::get_configuration(jvxCallManagerConfiguration* callConf,
	IjvxConfigProcessor* processor,
	jvxHandle* sectionWhereToAddAllSubsections)
{
	jvxErrorType res = CjvxAudioNode::get_configuration(callConf, processor,sectionWhereToAddAllSubsections);

	CjvxAudioNode_logfile_genpcg::get_configuration_all(callConf, processor, sectionWhereToAddAllSubsections);

	return JVX_NO_ERROR;
}

jvxErrorType
CjvxAudioNode_logfile::log_prepare_all_channels()
{
	jvxErrorType res = JVX_NO_ERROR;
	res = theLogger.add_one_lane(_common_set_node_params_1io.processing.number_input_channels,
		_common_set_node_params_1io.processing.buffersize,
		_common_set_node_params_1io.processing.format,
		"audio/in/channel", 0, idInChannels);

	theLogger.add_tag("/audio/in/samplerate", jvx_size2String(_common_set_node_params_1io.processing.samplerate).c_str());
	theLogger.add_tag("/audio/in/buffersize", jvx_size2String(_common_set_node_params_1io.processing.buffersize).c_str());
	theLogger.add_tag("/audio/in/format", jvxDataFormat_txt(_common_set_node_params_1io.processing.format));
	theLogger.add_tag("audio/in/channels", jvx_size2String(_common_set_node_params_1io.processing.number_input_channels).c_str());

	if (res != JVX_NO_ERROR)
	{
		return res;
	}
	res = theLogger.add_one_lane(_common_set_node_params_1io.processing.number_output_channels,
		_common_set_node_params_1io.processing.buffersize,
		_common_set_node_params_1io.processing.format,
		"audio/out/channel", 0, idOutChannels);
	theLogger.add_tag("/audio/out/samplerate", jvx_size2String(_common_set_node_params_1io.processing.samplerate).c_str());
	theLogger.add_tag("/audio/out/buffersize", jvx_size2String(_common_set_node_params_1io.processing.buffersize).c_str());
	theLogger.add_tag("/audio/out/format", jvxDataFormat_txt(_common_set_node_params_1io.processing.format));
	theLogger.add_tag("/audio/out/channels", jvx_size2String(_common_set_node_params_1io.processing.number_output_channels).c_str());

	return res;

}

jvxErrorType
CjvxAudioNode_logfile::log_start_all_channels()
{
	jvxErrorType res = JVX_NO_ERROR;

	res = theLogger.start_processing();
	CjvxAudioNode_logfile_genpcg::audiologtofile.properties_logtofile.fillHeightBuffer.value.val() = 0;
	return res;
}

jvxErrorType
CjvxAudioNode_logfile::log_stop_all_channels()
{
	jvxErrorType res = JVX_NO_ERROR;
	// Stop logfile if active
	res = theLogger.stop_processing();
	if (res != JVX_NO_ERROR)
	{
		return res;
	}

	theLogger.remove_all_tags();
	theLogger.reactivate_logging();

	return res;
}

jvxErrorType
CjvxAudioNode_logfile::log_postprocess_all_channels()
{
	jvxErrorType res = JVX_NO_ERROR;
	res = theLogger.remove_all_lanes();

	if (res != JVX_NO_ERROR)
	{
		return res;
	}
	idInChannels = 0;
	idOutChannels = 0;
	return(res);
}

jvxErrorType
CjvxAudioNode_logfile::log_write_all_channels(jvxLinkDataDescriptor* theData, jvxSize idx_sender_to_receiver, jvxSize idx_receiver_to_sender)
{
	jvxErrorType res = JVX_NO_ERROR;
	res = theLogger.process_logging_try(idInChannels, theData->con_data.buffers[idx_sender_to_receiver],
		theData->con_params.number_channels,
		theData->con_params.format,
		theData->con_params.buffersize, NULL);
	if (res != JVX_NO_ERROR)
	{
		return(res);
	}

	res = theLogger.process_logging_try(idOutChannels, theData->con_compat.from_receiver_buffer_allocated_by_sender[idx_receiver_to_sender],
		theData->con_compat.number_channels,
		theData->con_compat.format,
		theData->con_compat.buffersize,
		&CjvxAudioNode_logfile_genpcg::audiologtofile.properties_logtofile.fillHeightBuffer.value.val());

	return(res);
}

jvxErrorType
CjvxAudioNode_logfile::set_property(
	jvxCallManagerProperties& callGate,
	const jvx::propertyRawPointerType::IjvxRawPointerType& rawPtr,
	const jvx::propertyAddress::IjvxPropertyAddress& ident,
	const jvx::propertyDetail::CjvxTranferDetail& detail)
{
	jvxErrorType res = JVX_NO_ERROR;
	res = _set_property(
		callGate,
		rawPtr,
		ident, detail);
	if (res == JVX_NO_ERROR)
	{
		JVX_TRANSLATE_PROP_ADDRESS_IDX_CAT(ident, propId, category);

		if (
			JVX_PROPERTY_CHECK_ID_CAT(propId, category, CjvxAudioNode_logfile_genpcg::audiologtofile.properties_logtofile.activateLogToFile))
		{
			if (CjvxAudioNode_logfile_genpcg::audiologtofile.properties_logtofile.activateLogToFile.value)
			{
				theLogger.activate_logging(CjvxAudioNode_logfile_genpcg::audiologtofile.properties_logtofile.logFileFolder.value, 
					CjvxAudioNode_logfile_genpcg::audiologtofile.properties_logtofile.logFilePrefix.value);
			}
			else
			{
				theLogger.deactivate_logging();
			}
		}

	}
	return res;
}

