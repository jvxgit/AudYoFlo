#include "jvx.h"
#include "jvxNodes/CjvxBareNode1io_logtofile.h"

CjvxBareNode1io_logfile::CjvxBareNode1io_logfile(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE):
	CjvxBareNode1ioRearrange(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
{
	_common_set_audio_log_to_file.numFramesLog = 100;
	_common_set_audio_log_to_file.timeoutMsec = 5000;
	idInChannels = JVX_SIZE_UNSELECTED;
	idOutChannels = JVX_SIZE_UNSELECTED;
	//_common_set_ldslave.zeroCopyBuffering_cfg = false; <- done in base class
}

CjvxBareNode1io_logfile::~CjvxBareNode1io_logfile()
{
}

jvxErrorType
CjvxBareNode1io_logfile::activate()
{
	jvxErrorType res = CjvxBareNode1io::activate();
	jvxErrorType resL = JVX_NO_ERROR;

	if (res == JVX_NO_ERROR)
	{
		resL = theMainLogFile.lfc_activate(
			static_cast<IjvxObject*>(this),
			static_cast<CjvxProperties*>(this),
			_common_set_min.theHostRef, 
			_common_set_audio_log_to_file.logger_prop_prefix,
			_common_set_audio_log_to_file.numFramesLog, 
			_common_set_audio_log_to_file.timeoutMsec);
		if (resL != JVX_NO_ERROR)
		{
			std::cout << "Failed to initialize log file writing: " << jvxErrorType_descr(resL) << std::endl;
		}
	}
	return(res);
};

jvxErrorType
CjvxBareNode1io_logfile::deactivate()
{
	jvxErrorType res = CjvxBareNode1io::deactivate();
	if(res == JVX_NO_ERROR)
	{
		theMainLogFile.lfc_deactivate();
	}
	return(res);
}

jvxErrorType
CjvxBareNode1io_logfile::put_configuration(jvxCallManagerConfiguration* callConf,
	IjvxConfigProcessor* processor,
	jvxHandle* sectionToContainAllSubsectionsForMe, 
	const char* filename,
	jvxInt32 lineno)
{
	jvxSize i;
	std::vector<std::string> warns;
	jvxErrorType res = CjvxBareNode1io::put_configuration(callConf, processor,
		sectionToContainAllSubsectionsForMe, filename, lineno);
	
	if (_common_set_min.theState == JVX_STATE_ACTIVE)
	{
		theMainLogFile.lfc_put_configuration_active(callConf,
			processor,
			sectionToContainAllSubsectionsForMe,
			filename,
			lineno,
			warns);
		for (i = 0; i < warns.size(); i++)
		{
			_report_text_message(warns[i].c_str(), JVX_REPORT_PRIORITY_WARNING);
		}
	}
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxBareNode1io_logfile::get_configuration(jvxCallManagerConfiguration* callConf,
	IjvxConfigProcessor* processor,
	jvxHandle* sectionWhereToAddAllSubsections)
{
	jvxErrorType res = CjvxBareNode1io::get_configuration(callConf, processor,sectionWhereToAddAllSubsections);
	theMainLogFile.lfc_get_configuration(callConf, processor, sectionWhereToAddAllSubsections);
	return res;
}

jvxErrorType
CjvxBareNode1io_logfile::log_prepare_all_channels()
{
	jvxErrorType res = JVX_NO_ERROR;

	theMainLogFile.lfc_prepare_one_channel(
		node_inout._common_set_node_params_a_1io.number_channels,
		node_inout._common_set_node_params_a_1io.buffersize,
		node_inout._common_set_node_params_a_1io.samplerate,
		(jvxDataFormat)node_inout._common_set_node_params_a_1io.format,
		"audio/in/channel", idInChannels);

	if (res != JVX_NO_ERROR)
	{
		return res;
	}
	
	theMainLogFile.lfc_prepare_one_channel(
		node_output._common_set_node_params_a_1io.number_channels,
		node_output._common_set_node_params_a_1io.buffersize,
		node_output._common_set_node_params_a_1io.samplerate,
		(jvxDataFormat)node_output._common_set_node_params_a_1io.format,
		"audio/out/channel", idOutChannels);

	return res;

}

jvxErrorType
CjvxBareNode1io_logfile::log_start_all_channels()
{
	jvxErrorType res = JVX_NO_ERROR;
	res = theMainLogFile.lfc_start_all_channels();
	return res;
}

jvxErrorType
CjvxBareNode1io_logfile::log_stop_all_channels()
{
	jvxErrorType res = JVX_NO_ERROR;
	// Stop logfile if active
	res = theMainLogFile.lfc_stop_all_channels();
	return res;
}

jvxErrorType
CjvxBareNode1io_logfile::log_postprocess_all_channels()
{
	jvxErrorType res = JVX_NO_ERROR;
	res = theMainLogFile.lfc_postprocess_all_channels();
	idInChannels = JVX_SIZE_UNSELECTED;
	idOutChannels = JVX_SIZE_UNSELECTED;
	return(res);
}

jvxErrorType
CjvxBareNode1io_logfile::log_write_all_channels(jvxLinkDataDescriptor* theData_in, 
	jvxLinkDataDescriptor* theData_out, jvxData* heightPercent)
{
	jvxErrorType res = JVX_NO_ERROR;
	if (theData_in)
	{
		res = theMainLogFile.lfc_write_all_channels(
			theData_in->con_data.buffers[*theData_in->con_pipeline.idx_stage_ptr],
			theData_in->con_params.number_channels,
			theData_in->con_params.buffersize,
			theData_in->con_params.format,
			idInChannels, heightPercent);
	}
	if (res != JVX_NO_ERROR)
	{
		return(res);
	}
	if (theData_out)
	{
		res = theMainLogFile.lfc_write_all_channels(
			theData_out->con_data.buffers[*theData_in->con_pipeline.idx_stage_ptr],
			theData_out->con_params.number_channels,
			theData_out->con_params.buffersize,
			theData_out->con_params.format,
			idOutChannels, heightPercent);
	}
	return(res);
}

jvxErrorType
CjvxBareNode1io_logfile::set_property(
	jvxCallManagerProperties& callGate,
	const jvx::propertyRawPointerType::IjvxRawPointerType& rawPtr,
	const jvx::propertyAddress::IjvxPropertyAddress& ident,
	const jvx::propertyDetail::CjvxTranferDetail& trans)
{
	jvxErrorType res = JVX_NO_ERROR;
	
	res = _set_property(
		callGate,
		rawPtr,
		ident, trans);	
	if (res == JVX_NO_ERROR)
	{
		theMainLogFile.lfc_set_property(
			callGate, rawPtr,
			ident, trans,
			callGate.call_purpose);
	}
	return res;
}

