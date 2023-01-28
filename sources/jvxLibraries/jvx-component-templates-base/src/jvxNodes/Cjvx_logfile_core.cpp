#include "jvx.h"
#include "jvxNodes/CjvxBareNode1io_logtofile.h"

Cjvx_logfile_core::Cjvx_logfile_core()
{
	propRef = NULL;
	theHostRef = NULL;
}

Cjvx_logfile_core::~Cjvx_logfile_core()
{
}

jvxErrorType
Cjvx_logfile_core::lfc_activate(IjvxObject* objPtr, 
	CjvxProperties* pRef, IjvxHiddenInterface* hRef, const std::string& logger_prop_prefix,
	jvxSize numFramesLog, jvxSize timeoutMsec, const char* gTitle_spec, const char* fPrefix_spec,
	const char* textActivate_spec, const char* textLogfilePrefix_spec)
{
	jvxErrorType res = JVX_NO_ERROR;
	objRef = objPtr;
	propRef = pRef;
	theHostRef = hRef;

	CjvxNode_logfile_genpcg::init__logtofile__properties_logtofile();
	CjvxNode_logfile_genpcg::allocate__logtofile__properties_logtofile();

	// Change the defaults
	if (gTitle_spec)
	{
		CjvxNode_logfile_genpcg::logtofile.properties_logtofile.groupTitle.value = gTitle_spec;
	}
	if (fPrefix_spec)
	{
		CjvxNode_logfile_genpcg::logtofile.properties_logtofile.logFilePrefix.value = fPrefix_spec;
	}
	if (textActivate_spec)
	{
		CjvxNode_logfile_genpcg::logtofile.properties_logtofile.activateLogToFile.description = textActivate_spec;
	}
	if(textLogfilePrefix_spec)
	{
		CjvxNode_logfile_genpcg::logtofile.properties_logtofile.logFilePrefix.description = textLogfilePrefix_spec;
	}
	CjvxNode_logfile_genpcg::register__logtofile__properties_logtofile(propRef, logger_prop_prefix);

	// Initialize logfile writer
	res = theLogger.init_logging(theHostRef, numFramesLog, timeoutMsec);
	return(res);
};

jvxErrorType
Cjvx_logfile_core::lfc_deactivate()
{
	theLogger.terminate_logging();

	CjvxNode_logfile_genpcg::unregister__logtofile__properties_logtofile(propRef);
	CjvxNode_logfile_genpcg::deallocate__logtofile__properties_logtofile();

	return(JVX_NO_ERROR);
}

jvxErrorType
Cjvx_logfile_core::lfc_put_configuration_active(jvxCallManagerConfiguration* callConf,
	IjvxConfigProcessor* processor,
	jvxHandle* sectionToContainAllSubsectionsForMe, 
	const char* filename,
	jvxInt32 lineno,
	std::vector<std::string>& warns)
{
	jvxSize i;

	CjvxNode_logfile_genpcg::put_configuration_all(callConf, processor, sectionToContainAllSubsectionsForMe, &warns);
		
	if (CjvxNode_logfile_genpcg::logtofile.properties_logtofile.activateLogToFile.value)
	{
		theLogger.activate_logging(CjvxNode_logfile_genpcg::logtofile.properties_logtofile.logFileFolder.value,
			CjvxNode_logfile_genpcg::logtofile.properties_logtofile.logFilePrefix.value);
	}
	return JVX_NO_ERROR;
}

jvxErrorType
Cjvx_logfile_core::lfc_get_configuration(jvxCallManagerConfiguration* callConf,
	IjvxConfigProcessor* processor,
	jvxHandle* sectionWhereToAddAllSubsections)
{
	CjvxNode_logfile_genpcg::get_configuration_all(callConf, processor, sectionWhereToAddAllSubsections);
	return JVX_NO_ERROR;
}

jvxErrorType
Cjvx_logfile_core::lfc_prepare_one_channel(jvxSize number_channels, jvxSize buffersize, jvxSize samplerate,
			jvxDataFormat format, const std::string& description, jvxSize& uId)
{
	jvxSize idChans;
	jvxErrorType res = JVX_NO_ERROR;

	std::map<jvxSize, oneEntry>::iterator elm = uIds.begin();
	for (; elm != uIds.end(); elm++)
	{
		if (elm->second.description == description)
		{
			break;
		}
	}

	if(elm == uIds.end())
	{
		oneEntry newEnt;
		
		res = theLogger.add_one_lane(number_channels,
			buffersize,
			format,
			description, 0, idChans);

		newEnt.bSize = buffersize;
		newEnt.description = jvx_makePathExpr(description, "");
		newEnt.format = format;
		newEnt.numChannels = number_channels;
		newEnt.uId = idChans;
		uIds[idChans] = newEnt;
		uId = idChans;

		if (JVX_CHECK_SIZE_SELECTED(samplerate))
		{
			theLogger.add_tag(jvx_makePathExpr(newEnt.description, "samplerate").c_str(), jvx_size2String(samplerate).c_str());
		}
		theLogger.add_tag(jvx_makePathExpr(newEnt.description, "buffersize").c_str(), jvx_size2String(buffersize).c_str());
		theLogger.add_tag(jvx_makePathExpr(newEnt.description, "format").c_str(), jvxDataFormat_txt(format));
		theLogger.add_tag(jvx_makePathExpr(newEnt.description, "channels").c_str(), jvx_size2String(number_channels).c_str());

		return res;
	}

	return JVX_ERROR_DUPLICATE_ENTRY;
}

jvxErrorType
Cjvx_logfile_core::lfc_start_all_channels()
{
	jvxErrorType res = JVX_NO_ERROR;

	res = theLogger.start_processing();
	CjvxNode_logfile_genpcg::logtofile.properties_logtofile.fillHeightBuffer.value.val() = 0;
	return res;
}

jvxErrorType
Cjvx_logfile_core::lfc_stop_all_channels()
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
Cjvx_logfile_core::lfc_postprocess_all_channels()
{
	jvxErrorType res = JVX_NO_ERROR;
	res = theLogger.remove_all_lanes();

	if (res != JVX_NO_ERROR)
	{
		return res;
	}

	uIds.clear();
	return(res);
}

jvxErrorType
Cjvx_logfile_core::lfc_write_all_channels(jvxHandle** theData, jvxSize nChans, jvxSize nElements, jvxDataFormat format, jvxSize uId, jvxData* fHeightPerc)
{
	jvxErrorType res = JVX_NO_ERROR;
	res = theLogger.process_logging_try(uId, theData, nChans, format, nElements, fHeightPerc);
	return(res);
}

jvxErrorType
Cjvx_logfile_core::lfc_set_property(
	jvxCallManagerProperties& callGate,
	const jvx::propertyRawPointerType::IjvxRawPointerType& rawPtr,
	const jvx::propertyAddress::IjvxPropertyAddress& ident,
	const jvx::propertyDetail::CjvxTranferDetail& trans,
	jvxPropertyCallPurpose callPurpose)
{
	jvxSize propId = JVX_SIZE_UNSELECTED; 
	jvxPropertyCategoryType category = JVX_PROPERTY_CATEGORY_UNKNOWN;
	propRef->_translate_address_idx(callGate, ident, propId, category);

	if (JVX_PROPERTY_CHECK_ID_CAT(propId, category, CjvxNode_logfile_genpcg::logtofile.properties_logtofile.activateLogToFile))
	{
		if (CjvxNode_logfile_genpcg::logtofile.properties_logtofile.activateLogToFile.value)
		{
			theLogger.activate_logging(CjvxNode_logfile_genpcg::logtofile.properties_logtofile.logFileFolder.value,
				CjvxNode_logfile_genpcg::logtofile.properties_logtofile.logFilePrefix.value);
		}
		else
		{
			theLogger.deactivate_logging();
		}
	}
	return JVX_NO_ERROR;
}

