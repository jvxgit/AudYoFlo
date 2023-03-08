#include "CjvxAudioFFMpegReaderTechnology.h"


// =========================================================================================

CjvxAudioFFMpegReaderTechnology::CjvxAudioFFMpegReaderTechnology(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE):
	CjvxTemplateTechnologyVD<CjvxAudioFFMpegReaderDevice>(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
{
	this->numberDevicesInit = 0;
	this->deviceNamePrefix = "Input File";
	allowMultipleInstances = false;
}

CjvxAudioFFMpegReaderTechnology::~CjvxAudioFFMpegReaderTechnology()
{
}

jvxErrorType
CjvxAudioFFMpegReaderTechnology::activate()
{
	// "Jump" over the base class implementation 
	jvxErrorType res = CjvxTemplateTechnology<CjvxAudioFFMpegReaderDevice>::activate();
	if (res == JVX_NO_ERROR)
	{
		genFileReader_technology::init_all();
		genFileReader_technology::allocate_all();
		genFileReader_technology::register_all(static_cast<CjvxProperties*>(this));
		
		genFileReader_technology::register_callbacks(static_cast<CjvxProperties*>(this),
			modify_fileio, reinterpret_cast<jvxHandle*>(this), nullptr);
	}
	return(res);
};

jvxErrorType
CjvxAudioFFMpegReaderTechnology::deactivate()
{
	jvxErrorType res = CjvxTemplateTechnology<CjvxAudioFFMpegReaderDevice>::_pre_check_deactivate();
	if (res == JVX_NO_ERROR)
	{
		genFileReader_technology::unregister_callbacks(static_cast<CjvxProperties*>(this), nullptr);

		genFileReader_technology::unregister_all(static_cast<CjvxProperties*>(this));
		genFileReader_technology::deallocate_all();

		CjvxTemplateTechnology<CjvxAudioFFMpegReaderDevice>::deactivate();
	}
	return res;
}

jvxErrorType
CjvxAudioFFMpegReaderTechnology::return_device(IjvxDevice* dev)
{
	// If we are here, the device was returned. We ay remove it here!
	jvxErrorType res = CjvxTemplateTechnologyVD<CjvxAudioFFMpegReaderDevice>::return_device(dev);
	if (res == JVX_NO_ERROR)
	{
		auto elm = lstDevType.find(dev);
		if (elm != lstDevType.end())
		{
			trigger_close_file_core(*elm);

			CjvxReportCommandRequest theRequest(
				jvxReportCommandRequest::JVX_REPORT_COMMAND_REQUEST_UPDATE_AVAILABLE_COMPONENT_LIST,
				_common_set.theComponentType);
			_request_command(theRequest);			
		}
	}
	return res;
}

// =======================================================================================

JVX_PROPERTIES_FORWARD_C_CALLBACK_EXECUTE_FULL(CjvxAudioFFMpegReaderTechnology, modify_fileio)
{
	jvxErrorType res = JVX_NO_ERROR;

	if (JVX_PROPERTY_CHECK_ID_CAT(ident.id, ident.cat, genFileReader_technology::file_selection.open_filename))
	{
		jvxErrorType res = JVX_NO_ERROR;
		jvxSize idSelectNew = JVX_SIZE_UNSELECTED;
		std::string devSpec;
		std::string filename_to_be_opened = genFileReader_technology::file_selection.open_filename.value;

		res = activateOneFile(filename_to_be_opened,
			devSpec,
			idSelectNew);
		if (res == JVX_NO_ERROR)
		{
			// Update everything
			update_local_properties(true, idSelectNew);
			report_device_was_born(devSpec);

			genFileReader_technology::file_selection.open_filename.value = "ok";
			if (callGate.on_set.report_set)
			{
				CjvxProperties::add_property_report_collect(genFileReader_technology::file_selection.file_lists.descriptor.std_str(),
					false);
			}
		}
		else
		{
			// Error description already set in activateOneFile
		}

	}
	if (JVX_PROPERTY_CHECK_ID_CAT(ident.id, ident.cat, genFileReader_technology::file_selection.close_file))
	{
		if (genFileReader_technology::file_selection.close_file.value)
		{
			jvxSize idSel = jvx_bitfieldSelection2Id(
				genFileReader_technology::file_selection.file_lists.value.selection(),
				genFileReader_technology::file_selection.file_lists.value.entries.size());
			if (idSel < genFileReader_technology::file_selection.file_lists.value.entries.size())
			{
				jvxApiString astr;
				assert(idSel < lstDevType.size());
				auto elm = lstDevType.begin();
				std::advance(elm, idSel);
				elm->second->description(&astr);
				std::string ident = astr.std_str();				
				report_device_died(ident);
			}
			genFileReader_technology::file_selection.close_file.value = false;
			genFileReader_technology::file_selection.open_filename.value = "ok";			
		}
	}
	return res;
}

void
CjvxAudioFFMpegReaderTechnology::update_local_properties(jvxBool trig_host, jvxSize idSelectNew)
{
	std::string old_sel;
	jvxSize cnt;
	jvxSize idSel = jvx_bitfieldSelection2Id(
		genFileReader_technology::file_selection.file_lists.value.selection(),
		genFileReader_technology::file_selection.file_lists.value.entries.size());
	if (idSel < genFileReader_technology::file_selection.file_lists.value.entries.size())
	{
		old_sel = genFileReader_technology::file_selection.file_lists.value.entries[idSel];
	}

	idSel = JVX_SIZE_UNSELECTED;
	cnt = 0;
	genFileReader_technology::file_selection.file_lists.value.entries.clear();
	for (std::pair<IjvxDevice*, CjvxAudioFFMpegReaderDevice*> elm : lstDevType)
	{
		genFileReader_technology::file_selection.file_lists.value.entries.push_back(elm.second->fParams.fName);
		if (elm.second->fParams.fName == old_sel)
		{
			idSel = cnt;
		}
		cnt++;
	}
	
	if (idSel >= genFileReader_technology::file_selection.file_lists.value.entries.size())
	{
		if (genFileReader_technology::file_selection.file_lists.value.entries.size())
		{
			idSel = 0;
		}
		else
		{
			idSel = JVX_SIZE_UNSELECTED;
		}
	}

	if (JVX_CHECK_SIZE_SELECTED(idSel))
	{
		jvx_bitZSet(
			genFileReader_technology::file_selection.file_lists.value.selection(),
			idSel);
	}

	if (
		JVX_CHECK_SIZE_SELECTED(idSelectNew) &&
		(idSelectNew < genFileReader_technology::file_selection.file_lists.value.entries.size()))
	{
		jvx_bitZSet(
			genFileReader_technology::file_selection.file_lists.value.selection(),
			idSelectNew);
	}

	if (trig_host)
	{
		this->_report_command_request((jvxCBitField)1 << JVX_REPORT_REQUEST_UPDATE_WINDOW_SHIFT);
		this->_report_property_has_changed(
			genFileReader_technology::file_selection.file_lists.category,
			genFileReader_technology::file_selection.file_lists.globalIdx,
			false, 0, 1, JVX_COMPONENT_UNKNOWN);
	}
}

void
CjvxAudioFFMpegReaderTechnology::trigger_close_file_core(std::pair<IjvxDevice*, CjvxAudioFFMpegReaderDevice*> elm)
{
	jvxErrorType res = JVX_NO_ERROR;
	IjvxDevice* devPtr = elm.first;
	auto elmL = _common_tech_set.lstDevices.begin();
	for (; elmL != _common_tech_set.lstDevices.end(); elmL++)
	{
		if (elmL->hdlDev == devPtr)
		{
			jvxState stat = JVX_STATE_NONE;
			elm.second->state(&stat);
			assert(stat == JVX_STATE_NONE);

			res = elm.second->term_file();
			assert(res == JVX_NO_ERROR);

			_common_tech_set.lstDevices.erase(elmL);
			break;
		}
	}
	delete(elm.second);
	lstDevType.erase(elm.first);

	// Update everything
	update_local_properties(true, JVX_SIZE_UNSELECTED);
	
	// Always report change of property
	CjvxProperties::add_property_report_collect(genFileReader_technology::file_selection.file_lists.descriptor.std_str(),
		false);
}

jvxErrorType
CjvxAudioFFMpegReaderTechnology::put_configuration(jvxCallManagerConfiguration* callMan, IjvxConfigProcessor* processor,
	jvxHandle* sectionToContainAllSubsectionsForMe, const char* filename, jvxInt32 lineno )
{
	jvxErrorType res = JVX_NO_ERROR;
	std::vector<std::string> warns;
	if (_common_set_min.theState == JVX_STATE_ACTIVE)
	{
		jvxSize i;
		jvxSize selId = JVX_SIZE_UNSELECTED;
		genFileReader_technology::put_configuration_all(callMan,
			processor, sectionToContainAllSubsectionsForMe,
			&warns);

		selId = jvx_bitfieldSelection2Id(genFileReader_technology::file_selection.file_lists.value.selection(),
			genFileReader_technology::file_selection.file_lists.value.entries.size());
		if (selId < genFileReader_technology::file_selection.file_lists.value.entries.size())
		{
			selectedFile = genFileReader_technology::file_selection.file_lists.value.entries[selId];
		}

		for (i = 0; i < genFileReader_technology::file_selection.file_lists.value.entries.size(); i++)
		{
			fNamesOnConfig.push_back(genFileReader_technology::file_selection.file_lists.value.entries[i]);
		}
		genFileReader_technology::file_selection.file_lists.value.entries.clear();
		jvx_bitFClear(genFileReader_technology::file_selection.file_lists.value.selection());
	}
	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxAudioFFMpegReaderTechnology::get_configuration(jvxCallManagerConfiguration* callMan,
	IjvxConfigProcessor* processor, jvxHandle* sectionWhereToAddAllSubsections)
{
	
	genFileReader_technology::get_configuration_all(callMan,
		processor, sectionWhereToAddAllSubsections);
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxAudioFFMpegReaderTechnology::activateOneFile(
	const std::string& filename_to_be_opened,
	std::string& devSpec,
	jvxSize& idSelectNew )
{
	CjvxAudioFFMpegReaderDevice* newDevice = nullptr;
	IjvxDevice* newDevicePtr = nullptr;
	oneDeviceWrapper elm;
	std::string errTxt;
	jvxErrorType resL = JVX_NO_ERROR;
	jvxErrorType res = JVX_NO_ERROR;

	std::cout << "Trying to open file <" << filename_to_be_opened << ">" << std::endl;

	// Avoid file opened multiple times
	for (std::pair<IjvxDevice*, CjvxAudioFFMpegReaderDevice*> elm : lstDevType)
	{
		if (elm.second->fParams.fName == filename_to_be_opened)
		{
			errTxt = "File <";
			errTxt += filename_to_be_opened;
			errTxt += "> is already in use.";

			res = JVX_ERROR_DUPLICATE_ENTRY;
			goto error_file_failed;
		}
	}

	devSpec = deviceNamePrefix;
	devSpec += "#" + jvx_size2String(uId);

	newDevice = new CjvxAudioFFMpegReaderDevice(
		devSpec.c_str(), false,
		_common_set.theDescriptor.c_str(),
		_common_set.theFeatureClass,
		_common_set.theModuleName.c_str(),
		JVX_COMPONENT_ACCESS_SUB_COMPONENT,
		(jvxComponentType)(_common_set.theComponentType.tp + 1),
		"", NULL);
	newDevicePtr = static_cast<IjvxDevice*>(newDevice);

	// Initialize input file from filename
	resL = newDevice->init_from_filename(filename_to_be_opened, this);
	if (resL != JVX_NO_ERROR)
	{
		errTxt = "Failed to open file <";
		errTxt += filename_to_be_opened;
		errTxt += ">.";

		goto error_file_failed_delete;
	}

	uId++;
	lstDevType[newDevicePtr] = newDevice;

	// Store it in the original list as well	
	elm.hdlDev = newDevicePtr;

	idSelectNew = _common_tech_set.lstDevices.size();
	_common_tech_set.lstDevices.push_back(elm);

	return res;

error_file_failed_delete:
	if (newDevice)
	{		
		// File has not been opened, the newly created instance must be removed again
		delete newDevice;
		res = JVX_ERROR_OPEN_FILE_FAILED;
	}

error_file_failed:
	genFileReader_technology::file_selection.open_filename.value = errTxt;
	return res;
}

jvxErrorType 
CjvxAudioFFMpegReaderTechnology::done_configuration()
{
	jvxErrorType res = JVX_NO_ERROR;

	// Configuration all done, activate the given filenames
	jvxSize selId = JVX_SIZE_UNSELECTED;
	jvxSize cnt = 0;
	for (std::string& elm : fNamesOnConfig)
	{
		std::string devSpec;
		jvxSize idSelect;
		
		res = this->activateOneFile(elm, devSpec, idSelect);
		if (res == JVX_NO_ERROR)
		{
			if (elm == selectedFile)
			{
				selId = cnt;
			}
			cnt++;
			report_device_was_born(devSpec);
		}
	}
	update_local_properties(true, selId);

	// Always report change of property
	CjvxProperties::add_property_report_collect(genFileReader_technology::file_selection.file_lists.descriptor.std_str(),
		false);

	return JVX_NO_ERROR;
}
