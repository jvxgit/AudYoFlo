#include "CjvxAudioFileWriterTechnology.h"

struct oneEntryFileSubtypeWav
{
	const char* descr = nullptr;
	jvxEndpointSubType_wav tp = JVX_WAV_32;
  oneEntryFileSubtypeWav(const char* descrArg, jvxEndpointSubType_wav tpArg): descr(descrArg), tp(tpArg) {};
};
#define JVX_WAV_DIFFERENT_TYPES 3
oneEntryFileSubtypeWav jvxWavDifferentSubTypes[JVX_WAV_DIFFERENT_TYPES] =
{
	{"WAV32", JVX_WAV_32},
	{"WAV64SONY", JVX_WAV_64_SONY},
	{"WAV64", JVX_WAV_64}
};

// =========================================================================================

CjvxAudioFileWriterTechnology::CjvxAudioFileWriterTechnology(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE):
	CjvxTemplateTechnologyVD<CjvxAudioFileWriterDevice>(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
{
	this->numberDevicesInit = 0;
	this->deviceNamePrefix = "Output File";	
}

CjvxAudioFileWriterTechnology::~CjvxAudioFileWriterTechnology()
{
}

jvxErrorType
CjvxAudioFileWriterTechnology::activate()
{
	// "Jump" over the base class implementation 
	jvxErrorType res = CjvxTemplateTechnology<CjvxAudioFileWriterDevice>::activate();
	if (res == JVX_NO_ERROR)
	{
		genFileWriter_technology::init_all();
		genFileWriter_technology::allocate_all();
		genFileWriter_technology::register_all(static_cast<CjvxProperties*>(this));
		
		genFileWriter_technology::register_callbacks(static_cast<CjvxProperties*>(this),
			control_fileio, modify_wav_file_params, reinterpret_cast<jvxHandle*>(this), nullptr);
	}
	return(res);
};

jvxErrorType
CjvxAudioFileWriterTechnology::deactivate()
{
	jvxErrorType res = CjvxTemplateTechnology<CjvxAudioFileWriterDevice>::_pre_check_deactivate();
	if (res == JVX_NO_ERROR)
	{
		genFileWriter_technology::unregister_callbacks(static_cast<CjvxProperties*>(this), nullptr);

		genFileWriter_technology::unregister_all(static_cast<CjvxProperties*>(this));
		genFileWriter_technology::deallocate_all();

		CjvxTemplateTechnology<CjvxAudioFileWriterDevice>::deactivate();
	}
	return res;
}

jvxErrorType
CjvxAudioFileWriterTechnology::return_device(IjvxDevice* dev)
{
	// If we are here, the device was returned. We ay remove it here!
	jvxErrorType res = CjvxTemplateTechnologyVD<CjvxAudioFileWriterDevice>::return_device(dev);
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


void
CjvxAudioFileWriterTechnology::update_local_properties(jvxBool trig_host, jvxSize idSelectNew)
{
	std::string old_sel;
	jvxSize cnt;
	jvxSize i;
	jvxSize idSel = jvx_bitfieldSelection2Id(
		genFileWriter_technology::file_params.device_lists.value.selection(),
		genFileWriter_technology::file_params.device_lists.value.entries.size());
	if (idSel < genFileWriter_technology::file_params.device_lists.value.entries.size())
	{
		old_sel = genFileWriter_technology::file_params.device_lists.value.entries[idSel];
	}

	idSel = JVX_SIZE_UNSELECTED;
	cnt = 0;
	genFileWriter_technology::file_params.device_lists.value.entries.clear();
	for (std::pair<IjvxDevice*, CjvxAudioFileWriterDevice*> elm : lstDevType)
	{
		genFileWriter_technology::file_params.device_lists.value.entries.push_back(
			elm.second->_common_set_min.theDescription);
		if (elm.second->_common_set_min.theDescription == old_sel)
		{
			idSel = cnt;
		}
		cnt++;
	}
	
	if (idSel >= genFileWriter_technology::file_params.device_lists.value.entries.size())
	{
		if (genFileWriter_technology::file_params.device_lists.value.entries.size())
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
			genFileWriter_technology::file_params.device_lists.value.selection(),
			idSel);
	}

	if (
		JVX_CHECK_SIZE_SELECTED(idSelectNew) &&
		(idSelectNew < genFileWriter_technology::file_params.device_lists.value.entries.size()))
	{
		jvx_bitZSet(
			genFileWriter_technology::file_params.device_lists.value.selection(),
			idSelectNew);
	}

	// Update the wav file subtypes
	std::string old_entry;
	jvxSize id = JVX_BITFIELD_SELECTION_ID(genFileWriter_technology::file_params.file_sub_type);
	if(JVX_CHECK_SIZE_SELECTED(id))		
	{
		old_entry = genFileWriter_technology::file_params.file_sub_type.value.entries[id];
	}
	id = JVX_SIZE_UNSELECTED;
	genFileWriter_technology::file_params.file_sub_type.value.entries.clear();
	jvxAudioFileWriteFiletype fileTp = genFileWriter_technology::translate__file_params__file_type_from();
	switch (fileTp)
	{
	case jvxAudioFileWriteFiletype::JVX_FILEWRITER_WAV:
		for (i = 0; i < JVX_WAV_DIFFERENT_TYPES; i++)
		{
			std::string txt = jvxWavDifferentSubTypes[i].descr;
			genFileWriter_technology::file_params.file_sub_type.value.entries.push_back(txt);
			if (txt == old_entry)
			{
				id = i;
			}
		}
		break;
	}

	if (JVX_CHECK_SIZE_UNSELECTED(id))
	{
		if (genFileWriter_technology::file_params.file_sub_type.value.entries.size())
		{
			id = 0;
		}
	}
	if (JVX_CHECK_SIZE_SELECTED(id))
	{
		jvx_bitZSet(genFileWriter_technology::file_params.file_sub_type.value.selection(), id);
	}
	// jvxWavDifferentSubTypes

	if (trig_host)
	{
		this->_report_command_request((jvxCBitField)1 << JVX_REPORT_REQUEST_UPDATE_WINDOW_SHIFT);
		this->_report_property_has_changed(
			genFileWriter_technology::file_params.device_lists.category,
			genFileWriter_technology::file_params.device_lists.globalIdx,
			false, 0, 1, JVX_COMPONENT_UNKNOWN);
	}
}

void
CjvxAudioFileWriterTechnology::trigger_close_file_core(std::pair<IjvxDevice*, CjvxAudioFileWriterDevice*> elm)
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
}


jvxErrorType
CjvxAudioFileWriterTechnology::activateOneFile(
	const std::string& folder_store,
	const std::string& filename_prefix,
	std::string& devSpec,
	jvxSize& idSelectNew,
	const std::string& cfg_compact)
{
	CjvxAudioFileWriterDevice* newDevice = nullptr;
	oneDeviceWrapper elm;
	std::string errTxt;
	jvxErrorType res = JVX_NO_ERROR;

	fileprops_wav_base props;

	std::cout << "Reserving device to open file with prefix description <" << filename_prefix << "> in folder <" << folder_store << ">." << std::endl;

#if 0
	// Avoid file opened multiple times
	for (std::pair<IjvxDevice*, CjvxAudioFileWriterDevice*> elm : lstDevType)
	{
		if (elm.second->fname == filename_to_be_opened)
		{
			errTxt = "File prefix <";
			errTxt += filename_to_be_opened;
			errTxt += "> is already in use.";

			res = JVX_ERROR_DUPLICATE_ENTRY;
			goto error_file_failed;
		}
	}
#endif
	if (devSpec.empty())
	{
		devSpec = deviceNamePrefix;
		devSpec += "#" + jvx_size2String(uId);
		uId++;
	}

	IjvxDevice* newDevicePtr = nullptr;

	newDevice = new CjvxAudioFileWriterDevice(
		devSpec.c_str(), false,
		_common_set.theDescriptor.c_str(),
		_common_set.theFeatureClass,
		_common_set.theModuleName.c_str(),
		JVX_COMPONENT_ACCESS_SUB_COMPONENT,
		(jvxComponentType)(_common_set.theComponentType.tp + 1),
		"", NULL);
	newDevicePtr = static_cast<IjvxDevice*>(newDevice);

	jvxAudioFileWriteFiletype fType = jvxAudioFileWriteFiletype::JVX_FILEWRITER_WAV;
	jvxSize selFSType = 0;
	jvxSize fSType = 0;
	jvxErrorType resL = JVX_NO_ERROR;
		
	if(jvx_bitTest(genFileWriter_technology::file_params.file_type.value.selection(), 0))
	{
		fType = jvxAudioFileWriteFiletype::JVX_FILEWRITER_WAV;
	}

	selFSType = JVX_BITFIELD_SELECTION_ID(
		genFileWriter_technology::file_params.file_sub_type);
	assert(JVX_CHECK_SIZE_SELECTED(selFSType));

	if(fType == jvxAudioFileWriteFiletype::JVX_FILEWRITER_WAV)
	{ 
		fSType = jvxWavDifferentSubTypes[selFSType].tp;
	}
	
	// Initialize input file from filename
	resL = newDevice->init_parameters(
		folder_store,
		filename_prefix, 
		fType, 
		fSType,
		&wav_start_params,
		cfg_compact,
		this);

	if (resL != JVX_NO_ERROR)
	{
		errTxt = "Failed to open file <";
		errTxt += filename_prefix;
		errTxt += ">.";

		goto error_file_failed_delete;
	}

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

	return res;
}

