#include "CjvxGenericWrapperTechnology.h"
#include "CjvxGenericWrapperDevice.h"
#include "configConfiguration.h"
#include "configSetup.h"
#include "jvx_dsp_base.h"

CjvxGenericWrapperTechnology::CjvxGenericWrapperTechnology(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE): 
	JVX_MY_BASE_CLASS_T(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)

{
	_common_set.theComponentType.unselected(JVX_COMPONENT_AUDIO_TECHNOLOGY);
	_common_set.theObjectSpecialization = reinterpret_cast<jvxHandle*>(static_cast<IjvxTechnology*>(this));
	_common_set.thisisme = static_cast<IjvxObject*>(this);
	//_common_spec_proc_set.thisisme = static_cast<IjvxObject*>(this);

	runtime.theSelectedTech = NULL;
	runtime.theToolsHost = NULL;
	runtime.theTechnologies.clear();

	theFiles.allowModification = true;
	theFiles.uniqueIdHandles = 1;

	theExtReferences.allowModification = true;
	theExtReferences.uniqueIdHandles = 1;

	inPropCallId = JVX_SIZE_UNSELECTED;

	this->numberDevicesInit = 0;

	JVX_INITIALIZE_MUTEX(safeFilesReferences);

}

CjvxGenericWrapperTechnology::~CjvxGenericWrapperTechnology()
{
	JVX_TERMINATE_MUTEX(safeFilesReferences);
}

jvxErrorType
CjvxGenericWrapperTechnology::select(IjvxObject* theOwner)
{
	jvxSize i;
	jvxErrorType res = JVX_MY_BASE_CLASS_T::select(theOwner);
	jvxErrorType resL = JVX_NO_ERROR;
	jvxSize num = 0;
	jvxApiString  fldStr;
	jvxApiString strL;

	if(res == JVX_NO_ERROR)
	{
		this->genGenericWrapper_technology::init__select_files();
		this->genGenericWrapper_technology::allocate__select_files();
		this->genGenericWrapper_technology::register__select_files(static_cast<CjvxProperties*>(this));

		this->genGenericWrapper_technology::init__properties_selected();
		this->genGenericWrapper_technology::allocate__properties_selected();
		this->genGenericWrapper_technology::register__properties_selected(static_cast<CjvxProperties*>(this));

		this->genGenericWrapper_technology::init__properties_selected_active();
		this->genGenericWrapper_technology::allocate__properties_selected_active();
		this->genGenericWrapper_technology::register__properties_selected_active(static_cast<CjvxProperties*>(this));

		this->genGenericWrapper_technology::init__properties_selected_input_file();
		this->genGenericWrapper_technology::allocate__properties_selected_input_file();
		this->genGenericWrapper_technology::register__properties_selected_input_file(static_cast<CjvxProperties*>(this));

		this->genGenericWrapper_technology::init__properties_selected_output_file();
		this->genGenericWrapper_technology::allocate__properties_selected_output_file();
		this->genGenericWrapper_technology::register__properties_selected_output_file(static_cast<CjvxProperties*>(this));


		this->genGenericWrapper_technology::register_callbacks(static_cast<CjvxProperties*>(this),
			cb_prop_selected_input_file_set, 
			cb_prop_selected_input_file_set_pre,
			cb_prop_selected_input_file_get,
			cb_prop_selected_output_file_set,
			cb_prop_selected_output_file_set_pre,
			cb_prop_selected_output_file_get,
			reinterpret_cast<jvxHandle*>(this), 
			NULL);

		CjvxProperties::_lock_properties_local();

		genGenericWrapper_technology::properties_selected.technologies.value.entries.clear();
		genGenericWrapper_technology::properties_selected.technologies.value.selection() = 0;

		if(_common_set_min.theHostRef)
		{
			resL = _common_set_min.theHostRef->request_hidden_interface(JVX_INTERFACE_TOOLS_HOST, reinterpret_cast<jvxHandle**>(&runtime.theToolsHost));
			if((resL == JVX_NO_ERROR) && runtime.theToolsHost)
			{
				runtime.theToolsHost->number_tools(_common_set.theComponentType, &num);
				for(i = 0; i < num; i++)
				{
					IjvxObject* theObj = NULL;
					resL = runtime.theToolsHost->reference_tool(_common_set.theComponentType, &theObj, i, NULL);
					if((resL == JVX_NO_ERROR) && theObj)
					{
						if(theObj != static_cast<IjvxObject*>(this))
						{
							oneTechnologyElement oneElm;
							oneElm.theObj = theObj;
							oneElm.id = i;
							theObj->request_specialization(reinterpret_cast<jvxHandle**>(&oneElm.theTech), NULL, NULL);
							
							theObj->description(&strL);
							std::string txt = strL.std_str();
							
							if (txt != _common_set.theName)
							{
								jvxComponentAccessType acTp = JVX_COMPONENT_ACCESS_UNKNOWN;
								theObj->module_reference(NULL, &acTp);
								switch (acTp)
								{
								case JVX_COMPONENT_ACCESS_DYNAMIC_LIB:
									txt += "<d>";
									break;
								case JVX_COMPONENT_ACCESS_STATIC_LIB:
									txt += "<s>";
									break;
								default:
									break;
								}

								oneElm.name = txt;
								genGenericWrapper_technology::properties_selected.technologies.value.entries.push_back(txt.c_str());
								runtime.theTechnologies.push_back(oneElm);
							}
						}
					}
				}
			}


		}
		genGenericWrapper_technology::properties_selected.technologies.value.entries.push_back("No Technology");
		genGenericWrapper_technology::properties_selected.technologies.value.selection() = ((jvxBitField)1 << 0);
		CjvxProperties::_unlock_properties_local();
	}
	return(res);
}

jvxErrorType
CjvxGenericWrapperTechnology::unselect()
{
	jvxSize i;
	jvxErrorType res = JVX_MY_BASE_CLASS_T::unselect();
	jvxErrorType resL = JVX_NO_ERROR;
	std::vector<std::string> warnings;

	if(res == JVX_NO_ERROR)
	{
		// Cleanup files
		CjvxProperties::_lock_properties_local();
		this->lock_files();

		this->genGenericWrapper_technology::select_files.input_file.value.entries.clear();
		this->genGenericWrapper_technology::select_files.output_file.value.entries.clear();

		updatePropertyToFileLists_nolock(48000, warnings);
		this->unlock_files();
		CjvxProperties::_unlock_properties_local();

		if(_common_set_min.theHostRef)
		{
			if(runtime.theToolsHost)
			{
				for(i = 0; i < runtime.theTechnologies.size(); i++)
				{
					resL = runtime.theToolsHost->return_reference_tool(_common_set.theComponentType, runtime.theTechnologies[i].theObj);
				}
				runtime.theTechnologies.clear();
				_common_set_min.theHostRef->return_hidden_interface(JVX_INTERFACE_TOOLS_HOST, reinterpret_cast<jvxHandle*>(runtime.theToolsHost));
				runtime.theToolsHost = NULL;
			}


		}

		this->genGenericWrapper_technology::unregister_callbacks(static_cast<CjvxProperties*>(this), NULL);

		this->genGenericWrapper_technology::unregister__properties_selected_output_file(static_cast<CjvxProperties*>(this));
		this->genGenericWrapper_technology::deallocate__properties_selected_output_file();

		this->genGenericWrapper_technology::unregister__properties_selected_input_file(static_cast<CjvxProperties*>(this));
		this->genGenericWrapper_technology::deallocate__properties_selected_input_file();

		this->genGenericWrapper_technology::unregister__properties_selected(static_cast<CjvxProperties*>(this));
		this->genGenericWrapper_technology::deallocate__properties_selected();

		this->genGenericWrapper_technology::unregister__properties_selected_active(static_cast<CjvxProperties*>(this));
		this->genGenericWrapper_technology::deallocate__properties_selected_active();

		this->genGenericWrapper_technology::unregister__select_files(static_cast<CjvxProperties*>(this));
		this->genGenericWrapper_technology::deallocate__select_files();

		runtime.entrySelect.clear();
		runtime.entrySelectDefault.clear();
		runtime.theSelectedTech = NULL;
		runtime.theSelectedTechProps = NULL;
		runtime.theToolsHost = NULL;
		runtime.theTechnologies.clear();
	}
	return(res);
}

jvxErrorType
CjvxGenericWrapperTechnology::activate()
{
	jvxSize i;
	jvxErrorType res = JVX_MY_BASE_CLASS_T::activate();
		// _activate();
	// JVX_MY_BASE_CLASS_T::activate();

	jvxSize num = 0;
	jvxErrorType resL = JVX_NO_ERROR;
	std::string descrion = "UNKNOWN";
	std::string descror = "UNKNOWN";
	std::string name = "UNKNOWN";
	jvxApiString strL;
	IjvxDevice* theDevice = NULL;

	if(res == JVX_NO_ERROR)
	{
		techHostRelocator.init(this, _common_set_min.theHostRef);

		// ============================================================================
		// Select the chosen technology
		// ============================================================================
		
		runtime.entrySelect.clear();
		runtime.entrySelectDefault.clear();

		for(i = 0; i < genGenericWrapper_technology::properties_selected.technologies.value.entries.size(); i++)
		{
			if (runtime.entrySelectDefault.empty())
			{
				runtime.entrySelectDefault = genGenericWrapper_technology::properties_selected.technologies.value.entries[i];
			}
			if(jvx_bitTest(genGenericWrapper_technology::properties_selected.technologies.value.selection(), i))
			{
				runtime.entrySelect = genGenericWrapper_technology::properties_selected.technologies.value.entries[i];
				break;
			}
		}

		if (runtime.entrySelect.empty())
		{
			runtime.entrySelect = runtime.entrySelectDefault;
		}

		runtime.theSelectedTech = NULL;
		
		for (auto elm : this->runtime.theTechnologies)
		{
			if (elm.name == runtime.entrySelect)
			{
				runtime.theSelectedTech = this->runtime.theTechnologies[i].theTech;
				break;
			}
		}

		if(runtime.theSelectedTech)
		{
			runtime.theSelectedTech->initialize(&techHostRelocator);

				//_common_set_min.theHostRef);
			{
				res = runtime.theSelectedTech->select();
			}
			if(res == JVX_NO_ERROR)
			{
				res = runtime.theSelectedTech->activate();
			}
			if(res == JVX_NO_ERROR)
			{
				res = runtime.theSelectedTech->number_devices(&num);
			}

			runtime.theSelectedTech->feature_class(&_common_set.theFeatureClass);
			runtime.theSelectedTech->request_hidden_interface(JVX_INTERFACE_PROPERTIES, reinterpret_cast<jvxHandle**>(&runtime.theSelectedTechProps));
			if (runtime.theSelectedTechProps)
			{
				resL = this->_register_sub_module(runtime.theSelectedTechProps, 128, "wrapped_tech", "JVX_GENW");
			}
		}

		if(res != JVX_NO_ERROR)
		{
			_report_text_message("Unexpected fail of technology selection/activation", JVX_REPORT_PRIORITY_ERROR);
		}
		else
		{
			for(i = 0; i < num; i++)
			{
				descrion = "UNKNOWN";
				descror = "UNKNOWN";
				name = "UNKNOWN";
				theDevice = NULL;

				runtime.theSelectedTech->name_device(i, &strL);
				strL.assert_valid();
				name = strL.std_str();
				name = "<" + name + ">";

				runtime.theSelectedTech->description_device(i, &strL);
				strL.assert_valid();
				descrion = strL.std_str();
				descrion = "<" + descrion + ">";

				runtime.theSelectedTech->descriptor_device(i, &strL);
				strL.assert_valid();
				descror = strL.std_str();
				descror = "<" + descror + "#" + jvx_size2String(i) + ">";

				runtime.theSelectedTech->request_device(i, &theDevice);

				// Do whatever is required
				CjvxGenericWrapperDevice* newDevice = new CjvxGenericWrapperDevice(descrion.c_str(), false, descror.c_str(), _common_set.theFeatureClass, _common_set.theModuleName.c_str(),
					JVX_COMPONENT_ACCESS_SUB_COMPONENT, JVX_COMPONENT_AUDIO_DEVICE, "", NULL);

				jvxDeviceCapabilities caps;
				theDevice->capabilities_device(caps);
				newDevice->setDeviceReferences(theDevice, this, caps, name);

				
				// Whatever to be done for initialization
				oneDeviceWrapper elm;
				elm.hdlDev= static_cast<IjvxDevice*>(newDevice);

				_common_tech_set.lstDevices.push_back(elm);
			}

			/* Add another device which does not wrap any hardware */
			descrion = "File Only";
			descror = "FileDev";
			name = "File Only";
			theDevice = NULL;

			jvxDeviceCapabilities caps;
			jvx_bitZSet(caps.capsFlags, (int)jvxDeviceCapabilityTypeShift::JVX_DEVICE_CAPABILITY_DUPLEX_SHIFT);
			caps.flow = jvxDeviceDataFlowType::JVX_DEVICE_DATAFLOW_OFFLINE;

			// Do whatever is required
			CjvxGenericWrapperDevice* newDevice = new CjvxGenericWrapperDevice(descrion.c_str(), false, descror.c_str(), _common_set.theFeatureClass,
				_common_set.theModuleName.c_str(), JVX_COMPONENT_ACCESS_SUB_COMPONENT, JVX_COMPONENT_AUDIO_DEVICE, "", NULL);
			newDevice->setDeviceReferences(theDevice, this, caps, name);// , jvxDeviceCapabilities::JVX_DEVICE_OTHER);

			// Whatever to be done for initialization
			oneDeviceWrapper elm;
			elm.hdlDev= static_cast<IjvxDevice*>(newDevice);

			_common_tech_set.lstDevices.push_back(elm);
		}

		// update_capabilities();
	}
	return(res);
}

jvxErrorType
CjvxGenericWrapperTechnology::deactivate()
{
	jvxSize i;
	jvxErrorType res = JVX_MY_BASE_CLASS_T::_pre_check_deactivate();
	jvxErrorType resL = JVX_NO_ERROR;
	if(res == JVX_NO_ERROR)
	{
		auto elm = _common_tech_set.lstDevices.begin();
		for (; elm != _common_tech_set.lstDevices.end(); elm++)
		{
			
			CjvxGenericWrapperDevice* theDev = dynamic_cast<CjvxGenericWrapperDevice*>(elm->hdlDev);

			IjvxDevice* dev = nullptr;
			theDev->deviceReference(&dev);
			if (dev)
			{
				runtime.theSelectedTech->return_device(dev);
			}
			assert(theDev);
			delete(theDev);
		}
		_common_tech_set.lstDevices.clear();

		if(runtime.theSelectedTech)
		{
			if (runtime.theSelectedTechProps)
			{
				resL = this->_unregister_sub_module("wrapped_tech");
				runtime.theSelectedTech->return_hidden_interface(JVX_INTERFACE_PROPERTIES, reinterpret_cast<jvxHandle*>(runtime.theSelectedTechProps));
			}
			runtime.theSelectedTechProps = NULL;

			jvx_bitFClear(_common_set.theFeatureClass);
			resL = runtime.theSelectedTech->deactivate();
			assert(resL == JVX_NO_ERROR);
			{
				resL = runtime.theSelectedTech->unselect();
			}
			assert(resL == JVX_NO_ERROR);
			resL = runtime.theSelectedTech->terminate();
			assert(resL == JVX_NO_ERROR);

			techHostRelocator.terminate();
		}

		JVX_MY_BASE_CLASS_T::deactivate();
	}
	return(res);
}

jvxErrorType
CjvxGenericWrapperTechnology::addInputFile_nolock(std::string fName, jvxInt32& uniqueId, jvxInt16& numChannelsAdd, std::string& descr )
{
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;
	std::string txt;
	std::string tag_art;
	std::string tag_title;
	std::string tag_date;
	jvxBool useTags = false;
	jvxApiString strL;
	jvxErrorType resL = JVX_NO_ERROR;

	if(theFiles.allowModification)
	{
		for(i = 0; i < this->theFiles.theInputFiles.size(); i++)
		{
			if(fName == this->theFiles.theInputFiles[i].common.name)
			{
				return(JVX_ERROR_DUPLICATE_ENTRY);
			}
		}

		for(i = 0; i < this->theFiles.theOutputFiles.size(); i++)
		{
			if(fName == this->theFiles.theOutputFiles[i].common.name)
			{
				return(JVX_ERROR_DUPLICATE_ENTRY);
			}
		}

		oneInputAudioFile newFile;
		newFile.common.name = fName;

		newFile.common.theObj = NULL;
		newFile.theReader = NULL;
		newFile.common.boostPrio = false;
		newFile.common.pauseOnStart = false;
		newFile.common.idxNumberBuffersLookahead = IDX_LOOKAHEAD_START;
		newFile.common.fform_file = JVX_FILEFORMAT_PCM_FIXED;
		newFile.common.samplerate = SAMPLERATE_OUTFILE_START;
		newFile.common.number_channels = 1;
		newFile.common.numBitsSample = 16;
		newFile.common.subformat = (jvxSize)JVX_WAV_UNKNOWN;

		if(runtime.theToolsHost)
		{
			res = runtime.theToolsHost->instance_tool(JVX_COMPONENT_RT_AUDIO_FILE_READER,  &newFile.common.theObj, 0, NULL);
			if((res == JVX_NO_ERROR) && newFile.common.theObj)
			{
				newFile.common.theObj->request_specialization(reinterpret_cast<jvxHandle**>(&newFile.theReader), NULL, NULL);
			}
		}

		if(newFile.theReader)
		{
			newFile.theReader->initialize(_common_set_min.theHostRef);
			jvxFileDescriptionEndpoint_open theEp;
			theEp.numberChannels = -1;
			theEp.samplerate = -1;
			theEp.fFormatFile = JVX_FILEFORMAT_PCM_FIXED;
			theEp.littleEndian = true;
			theEp.subtype = (jvxSize) JVX_WAV_UNKNOWN;
			theEp.numberBitsSample = 16;
				
			res = newFile.theReader->activate(newFile.common.name.c_str(), JVX_FILETYPE_WAV, &theEp);
			if(res == JVX_NO_ERROR)
			{
				newFile.common.boostPrio = false;
				newFile.theReader->properties_file(&newFile.common.number_channels, &newFile.common.samplerate, &newFile.common.fform_file, NULL, &newFile.common.length, &newFile.common.numBitsSample, &newFile.common.subformat);

				newFile.common.idxNumberBuffersLookahead = IDX_LOOKAHEAD_START;
				newFile.ctTp = JVX_FILECONTINUE_LOOP;
				newFile.common.uniqueIdHandles = theFiles.uniqueIdHandles++;

				jvxAssignedFileTags theTags;
				jvx_initTagName(theTags);
			
				for(i = 0; i < JVX_NUMBER_AUDIO_FILE_TAGS; i++)
				{
					resL = newFile.theReader->get_tag((jvxAudioFileTagType) jvx_bitFieldValue32((jvxBitField)1 << (jvxUInt32)i), &strL);
					if (resL == JVX_NO_ERROR)
					{
						jvx_bitSet(theTags.tagsSet, i);
						theTags.tags[i] = strL.std_str();
					}
				}

				jvx_tagStructToTagExpr(newFile.common.friendlyName, theTags, newFile.common.name);
				descr = newFile.common.name;

				for(i = 0; i < this->theFiles.theInputFiles.size(); i++)
				{
					if( this->theFiles.theInputFiles[i].common.friendlyName == newFile.common.friendlyName)
					{
						_report_text_message(("The tag in the file <" + newFile.common.friendlyName + "> is already in use!").c_str(), JVX_REPORT_PRIORITY_WARNING);

						// Undo opening of file: the tags can not be handled as channels if they are completely identical!
						newFile.theReader->deactivate();
						newFile.theReader->terminate();
						runtime.theToolsHost->return_instance_tool(JVX_COMPONENT_RT_AUDIO_FILE_READER, newFile.common.theObj, 0, NULL);
						return(JVX_ERROR_DUPLICATE_ENTRY);
					}
				}

				this->theFiles.theInputFiles.push_back(newFile);

				uniqueId = newFile.common.uniqueIdHandles;
				numChannelsAdd = JVX_SIZE_INT16(newFile.common.number_channels);

				// What else to do here?
			}
			else
			{
				runtime.theToolsHost->return_instance_tool(JVX_COMPONENT_RT_AUDIO_FILE_READER, newFile.common.theObj, 0, NULL);

				res = JVX_ERROR_OPEN_FILE_FAILED;
			}
		}
		else
		{
			res = JVX_ERROR_CALL_SUB_COMPONENT_FAILED;
		}
	}
	else
	{
		res = JVX_ERROR_COMPONENT_BUSY;
	}
	return(res);
}

jvxErrorType
CjvxGenericWrapperTechnology::removeInputFile_nolock(jvxSize idx, jvxInt32& uniqueId)
{
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;
	std::vector<oneInputAudioFile> newList;

	if(theFiles.allowModification)
	{
		for(i = 0; i < this->theFiles.theInputFiles.size(); i++)
		{
			if(i != idx)
			{
				newList.push_back(this->theFiles.theInputFiles[i]);
			}
			else
			{
				uniqueId = theFiles.theInputFiles[i].common.uniqueIdHandles;
				theFiles.theInputFiles[i].theReader->deactivate();
				theFiles.theInputFiles[i].theReader->terminate();
				runtime.theToolsHost->return_instance_tool(JVX_COMPONENT_RT_AUDIO_FILE_READER, theFiles.theInputFiles[i].common.theObj, 0, NULL);
			}
		}
		this->theFiles.theInputFiles = newList;
	}
	else
	{
		res = JVX_ERROR_COMPONENT_BUSY;
	}
	return(res);
}

jvxErrorType
CjvxGenericWrapperTechnology::removeInputFile_nolock(std::string& name)
{
	jvxSize i;
	std::vector<std::string> newEntriesName;
	std::vector<std::string> newEntriesFName;

	for(i = 0; i < genGenericWrapper_technology::select_files.input_file.value.entries.size(); i++)
	{
		if(genGenericWrapper_technology::select_files.input_file.value.entries[i] != name)
		{
			newEntriesName.push_back(genGenericWrapper_technology::select_files.input_file.value.entries[i]);
		}
	}
	genGenericWrapper_technology::select_files.input_file.value.entries = newEntriesName;
	return(JVX_NO_ERROR);
}

jvxErrorType
CjvxGenericWrapperTechnology::addOutputFile_nolock(std::string name, std::string fname, jvxInt32& uniqueId, jvxInt16& numChannelsAdd, jvxInt32 sRate, std::string& descr )
{
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;
	if(theFiles.allowModification)
	{
		for(i = 0; i < this->theFiles.theInputFiles.size(); i++)
		{
			if(name == this->theFiles.theInputFiles[i].common.name)
			{
				return(JVX_ERROR_DUPLICATE_ENTRY);
			}
		}

		for(i = 0; i < this->theFiles.theOutputFiles.size(); i++)
		{
			if(name.empty())
			{
				if(fname == this->theFiles.theOutputFiles[i].common.friendlyName)
				{
					return(JVX_ERROR_DUPLICATE_ENTRY);
				}
			}
			else
			{
				if(name == this->theFiles.theOutputFiles[i].common.name)
				{
					return(JVX_ERROR_DUPLICATE_ENTRY);
				}
			}
			// If fname is empty, filename will be generated and multiple entries are allowed
		}

		oneOutputAudioFile newFile;
		newFile.common.name = name;
		newFile.common.friendlyName = fname;

		newFile.common.theObj = NULL;
		newFile.theWriter = NULL;
		if(runtime.theToolsHost)
		{
			res = runtime.theToolsHost->instance_tool(JVX_COMPONENT_RT_AUDIO_FILE_WRITER,  &newFile.common.theObj, 0, NULL);
			if((res == JVX_NO_ERROR) && newFile.common.theObj)
			{
				newFile.common.theObj->request_specialization(reinterpret_cast<jvxHandle**>(&newFile.theWriter), NULL, NULL);
			}
		}

		if(newFile.theWriter)
		{
			newFile.theWriter->initialize(_common_set_min.theHostRef);
			/*
			jvxFileDescriptionEndpoint_open theEp;
			theEp.numberChannels = -1;
			theEp.samplerate = -1;
			theEp.dFormatFile = JVX_DATAFORMAT_16BIT_LE;
			theEp.fFormatFile = JVX_FILEFORMAT_PCM_FIXED;
			theEp.littleEndian = true;
			*/
			newFile.common.boostPrio = false;
			newFile.common.number_channels = 1;
			newFile.common.samplerate = sRate; //SAMPLERATE_OUTFILE_START;
			newFile.common.idxNumberBuffersLookahead = IDX_LOOKAHEAD_START;
			newFile.common.fform_file = FFORMAT_OUTFILE_START;
			newFile.common.numBitsSample = 16;
			newFile.common.subformat = (jvxSize)JVX_WAV_32;

			newFile.common.uniqueIdHandles = theFiles.uniqueIdHandles++;
			this->theFiles.theOutputFiles.push_back(newFile);

			uniqueId = newFile.common.uniqueIdHandles;
			descr = newFile.common.name;
			if(descr.empty())
			{
				descr = newFile.common.friendlyName;
			}
			numChannelsAdd = JVX_SIZE_INT16(newFile.common.number_channels);
			// What else to do here?
		}
		else
		{
			res = JVX_ERROR_CALL_SUB_COMPONENT_FAILED;
		}
	}
	else
	{
		res = JVX_ERROR_COMPONENT_BUSY;
	}

	return(res);
}

jvxErrorType
CjvxGenericWrapperTechnology::removeOutputFile_nolock(jvxSize idx, jvxInt32& uniqueId)
{
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;
	if(theFiles.allowModification)
	{
		std::vector<oneOutputAudioFile> newList;
		lock_files();
		for(i = 0; i < this->theFiles.theOutputFiles.size(); i++)
		{
			if(i != idx)
			{
				newList.push_back(this->theFiles.theOutputFiles[i]);
			}
			else
			{
				uniqueId = theFiles.theOutputFiles[i].common.uniqueIdHandles;
				theFiles.theOutputFiles[i].theWriter->terminate();
				runtime.theToolsHost->return_instance_tool(JVX_COMPONENT_RT_AUDIO_FILE_WRITER, theFiles.theOutputFiles[i].common.theObj, 0, NULL);
			}
		}
		this->theFiles.theOutputFiles = newList;
		unlock_files();
	}
	else
	{
		res = JVX_ERROR_COMPONENT_BUSY;
	}
	return(res);
}

jvxErrorType
CjvxGenericWrapperTechnology::removeOutputFile_nolock(std::string& name)
{
	jvxSize i;
	std::vector<std::string> newEntriesNames;
	std::vector<std::string> newEntriesFNames;

	for(i = 0; i < genGenericWrapper_technology::select_files.output_file.value.entries.size(); i++)
	{
		if(genGenericWrapper_technology::select_files.output_file.value.entries[i] != name)
		{
			newEntriesNames.push_back(genGenericWrapper_technology::select_files.output_file.value.entries[i]);
		}
	}
	genGenericWrapper_technology::select_files.output_file.value.entries = newEntriesNames;
	return(JVX_NO_ERROR);
}

jvxErrorType
CjvxGenericWrapperTechnology::put_configuration(jvxCallManagerConfiguration* callConf,
	IjvxConfigProcessor* processor,
	jvxHandle* sectionToContainAllSubsectionsForMe, const char* filename, 
	jvxInt32 lineno)
{
	jvxSize i,j;
	std::vector<std::string> warn;
	jvxConfigData* datSubSec = NULL;
	jvxSize idSelTech = JVX_SIZE_UNSELECTED;
	std::string nm_tech;
	const jvx::propertyRawPointerType::IjvxRawPointerType* ptrRaw = nullptr;
    if(this->_common_set_min.theState == JVX_STATE_SELECTED)
    {
		jvxPropertyContainerSingle<jvxSelectionList_cpp> technologies_copy = genGenericWrapper_technology::properties_selected.technologies;

        // Read in the properties in state SELECTED

		// New: select technology based on the name!
        genGenericWrapper_technology::put_configuration__properties_selected(callConf, processor, sectionToContainAllSubsectionsForMe, &warn);
        for(i = 0; i < warn.size(); i++)
        {
            _report_text_message(("put configuration: warning #" + jvx_size2String(i) + ": " + warn[i]).c_str(), JVX_REPORT_PRIORITY_WARNING);
        }
		idSelTech = jvx_bitfieldSelection2Id(genGenericWrapper_technology::properties_selected.technologies.value.selection(),
			genGenericWrapper_technology::properties_selected.technologies.value.entries.size());
		if(JVX_CHECK_SIZE_SELECTED(idSelTech ))
		{
			nm_tech = genGenericWrapper_technology::properties_selected.technologies.value.entries[idSelTech];
		}
		genGenericWrapper_technology::properties_selected.technologies.value.entries = technologies_copy.value.entries;
		if (!nm_tech.empty())
		{
			for (i = 0; i < genGenericWrapper_technology::properties_selected.technologies.value.entries.size(); i++)
			{
				std::string nm_tech_nov = nm_tech;
				std::string nm_entry_nov = genGenericWrapper_technology::properties_selected.technologies.value.entries[i];

				// Here, we need to be careful: static or dynamic lib are encoded into the entries. Mask them out!
				if(nm_tech_nov.size() > 3)
				{ 
					nm_tech_nov = nm_tech_nov.substr(0, nm_tech_nov.size() - 3);
				}
				if (nm_entry_nov.size() > 3)
				{
					nm_entry_nov = nm_entry_nov.substr(0, nm_entry_nov.size() - 3);
				}

				if (nm_tech_nov == nm_entry_nov)
				{
					jvx_bitZSet(genGenericWrapper_technology::properties_selected.technologies.value.selection(), i);
					break;
				}
			}
		}
    }

    if(this->_common_set_min.theState == JVX_STATE_ACTIVE)
    {
		if (runtime.theSelectedTech)
		{
			IjvxConfiguration* cfg = reqInterfaceObj<IjvxConfiguration>(runtime.theSelectedTech);
			if (cfg)
			{
				jvxConfigData* datSection = nullptr;
				jvxErrorType resL = processor->getReferenceEntryCurrentSection_name(sectionToContainAllSubsectionsForMe, &datSection, JVX_GENERIC_WRAPPER_CONFIG_SUBSECTION_TECHNOLOGY);
				if (resL == JVX_NO_ERROR)
				{
					resL = cfg->put_configuration(callConf, processor, datSection, filename, lineno);
				}
				retInterfaceObj<IjvxConfiguration>(runtime.theSelectedTech, cfg);
			}
	   }
        // Read in the properties in state ACTIVE
        genGenericWrapper_technology::put_configuration__properties_selected_active(callConf, processor, sectionToContainAllSubsectionsForMe, &warn);
        for(i = 0; i < warn.size(); i++)
        {
            _report_text_message(("put configuration: warning #" + jvx_size2String(i) + ": " + warn[i]).c_str(), JVX_REPORT_PRIORITY_WARNING);
        }
		warn.clear();

		genGenericWrapper_technology::put_configuration__select_files(callConf, processor, sectionToContainAllSubsectionsForMe, &warn);
		for (i = 0; i < warn.size(); i++)
		{
			_report_text_message(("put configuration: warning #" + jvx_size2String(i) + ": " + warn[i]).c_str(), JVX_REPORT_PRIORITY_WARNING);
		}
		warn.clear();

		// Hold some values in copy to pass one after the other without overwrite
		std::vector<std::string> copyLstIn = genGenericWrapper_technology::select_files.input_file.value.entries;
		std::vector<std::string> copyLstOut = genGenericWrapper_technology::select_files.output_file.value.entries;
		jvxSize numInDummiesCopy = genGenericWrapper_technology::properties_selected_active.numberInDummies.value;
		jvxSize numOutDummiesCopy = genGenericWrapper_technology::properties_selected_active.numberOutDummies.value;
		jvxCallManagerProperties callGate;
		genGenericWrapper_technology::select_files.input_file.value.entries.clear();

		// All parameters for the input files
		for (i = 0; i < copyLstIn.size(); i++)
		{
			std::string command = "addInputFile(" + copyLstIn[i] + ");";
			jvxApiString jstr;

			jstr.assign_const(command.c_str(), command.size());
			jvx_set_property(static_cast<IjvxProperties*>(this), &jstr, 0, 1, JVX_DATAFORMAT_STRING, true, "/JVX_GENW_COMMAND", callGate);
		}

		for (i = 0; i < genGenericWrapper_technology::select_files.input_file.value.entries.size(); i++)
		{
			datSubSec = NULL;
			jvx::propertyDetail::CjvxTranferDetail tune(false, 0);
			jvxBool breakme = false;
			while (1)
			{
				datSubSec = NULL;
				processor->getReferenceEntryCurrentSection_name(sectionToContainAllSubsectionsForMe, &datSubSec, ("JVX_SECTION_ONE_INPUT_FILE_" + jvx_size2String(tune.offsetStart)).c_str());
				if (datSubSec)
				{
					jvxApiString  fldStr;
					jvxConfigData* datSecSubSub = NULL;
					processor->getReferenceEntryCurrentSection_name(datSubSec, &datSecSubSub, "JVX_SECTION_ONE_INPUT_FILE_NAME");
					if (datSecSubSub)
					{
						processor->getAssignmentString(datSecSubSub, &fldStr);

						if (fldStr.std_str() == genGenericWrapper_technology::select_files.input_file.value.entries[i])
						{
							genGenericWrapper_technology::put_configuration__properties_selected_input_file(callConf, processor, datSubSec, &warn);
							for (j = 0; j < warn.size(); j++)
							{
								_report_text_message(("put configuration: warning #" + jvx_size2String(i) + ": " + warn[j]).c_str(), JVX_REPORT_PRIORITY_WARNING);
							}
							warn.clear();

							ic_cb_prop_selected_input_file_set_pre(callGate, 
								ptrRaw,
								jPAGID(genGenericWrapper_technology::properties_selected_input_file.lookahead.globalIdx,
								genGenericWrapper_technology::properties_selected_input_file.lookahead.category),
								tune, JVX_PROPERTY_CALLBACK_DIRECT);
							ic_cb_prop_selected_input_file_set(callGate, 
								ptrRaw,
								jPAGID(genGenericWrapper_technology::properties_selected_input_file.lookahead.globalIdx,
								genGenericWrapper_technology::properties_selected_input_file.lookahead.category),
								tune, JVX_PROPERTY_CALLBACK_DIRECT);
							ic_cb_prop_selected_input_file_set_pre(callGate, 
								ptrRaw,
								jPAGID(genGenericWrapper_technology::properties_selected_input_file.boost_prio.globalIdx,
								genGenericWrapper_technology::properties_selected_input_file.boost_prio.category),
								tune, JVX_PROPERTY_CALLBACK_DIRECT);
							ic_cb_prop_selected_input_file_set(callGate,  
								ptrRaw,
								jPAGID(genGenericWrapper_technology::properties_selected_input_file.boost_prio.globalIdx,
								genGenericWrapper_technology::properties_selected_input_file.boost_prio.category),
								tune, JVX_PROPERTY_CALLBACK_DIRECT);
							ic_cb_prop_selected_input_file_set_pre(callGate,  
								ptrRaw,
								jPAGID(genGenericWrapper_technology::properties_selected_input_file.loop.globalIdx,
								genGenericWrapper_technology::properties_selected_input_file.loop.category),
								tune, JVX_PROPERTY_CALLBACK_DIRECT);
							ic_cb_prop_selected_input_file_set(callGate,
								ptrRaw,
								jPAGID(genGenericWrapper_technology::properties_selected_input_file.loop.globalIdx,
								genGenericWrapper_technology::properties_selected_input_file.loop.category),
								tune, JVX_PROPERTY_CALLBACK_DIRECT);
							ic_cb_prop_selected_input_file_set_pre(callGate,  
								ptrRaw,
								jPAGID(genGenericWrapper_technology::properties_selected_input_file.pause_on_start.globalIdx,
								genGenericWrapper_technology::properties_selected_input_file.pause_on_start.category),
								tune, JVX_PROPERTY_CALLBACK_DIRECT);
							ic_cb_prop_selected_input_file_set(callGate, 
								ptrRaw,
								jPAGID(genGenericWrapper_technology::properties_selected_input_file.pause_on_start.globalIdx,
								genGenericWrapper_technology::properties_selected_input_file.pause_on_start.category),
								tune, JVX_PROPERTY_CALLBACK_DIRECT);
							breakme = true;
						}
					}
					tune.offsetStart++;
					if (breakme)
					{
						break;
					}
				}
				else
				{
					std::string txt = "Failed to find specific properties for file <" + genGenericWrapper_technology::select_files.input_file.value.entries[i] + ">";
					_report_text_message(("put configuration: warning #" + jvx_size2String(i) + ": " + txt).c_str(), JVX_REPORT_PRIORITY_WARNING);
					break;
				}
			}
		}

		// Activate a file if none was selected before
		if (JVX_EVALUATE_BITFIELD(genGenericWrapper_technology::select_files.input_file.value.selection()) == false)
		{
			if(genGenericWrapper_technology::select_files.input_file.value.entries.size() > 0)
			{
				jvx_bitZSet(genGenericWrapper_technology::select_files.input_file.value.selection(), 0);
			}
		}

		// =======================================================================================
		// OUTPUT
		// ======================================================================================
		genGenericWrapper_technology::select_files.output_file.value.entries.clear();

		// All parameters for the input files
		for (i = 0; i < copyLstOut.size(); i++)
		{
			std::string command = "addOutputFile(" + copyLstOut[i] + ");";
			jvxApiString jstr;
			jstr.assign_const(command.c_str(), command.size());
			jvx_set_property(static_cast<IjvxProperties*>(this), &jstr, 0, 1, JVX_DATAFORMAT_STRING, true, "/JVX_GENW_COMMAND", callGate);
		}

		for (i = 0; i < genGenericWrapper_technology::select_files.output_file.value.entries.size(); i++)
		{
			datSubSec = NULL;
			jvx::propertyDetail::CjvxTranferDetail tune(false, 0);
			jvxDataFormat form = JVX_DATAFORMAT_NONE;
			jvxBool breakme = false;
			
			while (1)
			{
				datSubSec = NULL;
				processor->getReferenceEntryCurrentSection_name(sectionToContainAllSubsectionsForMe, &datSubSec, ("JVX_SECTION_ONE_OUTPUT_FILE_" + jvx_size2String(tune.offsetStart)).c_str());
				if (datSubSec)
				{
					jvxApiString  fldStr;
					jvxConfigData* datSecSubSub = NULL;
					processor->getReferenceEntryCurrentSection_name(datSubSec, &datSecSubSub, "JVX_SECTION_ONE_OUTPUT_FILE_NAME");
					if (datSecSubSub)
					{
						processor->getAssignmentString(datSecSubSub, &fldStr);

						if (fldStr.std_str() == genGenericWrapper_technology::select_files.output_file.value.entries[i])
						{
							genGenericWrapper_technology::put_configuration__properties_selected_output_file(callConf, processor, datSubSec, &warn);
							for (j = 0; j < warn.size(); j++)
							{
								_report_text_message(("put configuration: warning #" + jvx_size2String(i) + ": " + warn[j]).c_str(), JVX_REPORT_PRIORITY_WARNING);
							}
							warn.clear();

							ic_cb_prop_selected_output_file_set_pre(callGate,
								ptrRaw,
								jPAGID(genGenericWrapper_technology::properties_selected_output_file.lookahead.globalIdx,
								genGenericWrapper_technology::properties_selected_output_file.lookahead.category),
								tune, JVX_PROPERTY_CALLBACK_DIRECT);
							ic_cb_prop_selected_output_file_set(callGate, 
								ptrRaw,
								jPAGID(genGenericWrapper_technology::properties_selected_output_file.lookahead.globalIdx,
								genGenericWrapper_technology::properties_selected_output_file.lookahead.category),
								tune, JVX_PROPERTY_CALLBACK_DIRECT);
							ic_cb_prop_selected_output_file_set_pre(callGate, 
								ptrRaw,
								jPAGID(genGenericWrapper_technology::properties_selected_output_file.boost_prio.globalIdx,
								genGenericWrapper_technology::properties_selected_output_file.boost_prio.category),
								tune, JVX_PROPERTY_CALLBACK_DIRECT);
							ic_cb_prop_selected_output_file_set(callGate, 
								ptrRaw,
								jPAGID(genGenericWrapper_technology::properties_selected_output_file.boost_prio.globalIdx,
								genGenericWrapper_technology::properties_selected_output_file.boost_prio.category),
								tune,JVX_PROPERTY_CALLBACK_DIRECT);
							ic_cb_prop_selected_output_file_set_pre(callGate,
								ptrRaw,
								jPAGID(genGenericWrapper_technology::properties_selected_output_file.samplerate.globalIdx,
								genGenericWrapper_technology::properties_selected_output_file.samplerate.category),
								tune, JVX_PROPERTY_CALLBACK_DIRECT);
							ic_cb_prop_selected_output_file_set(callGate, 
								ptrRaw,
								jPAGID(genGenericWrapper_technology::properties_selected_output_file.samplerate.globalIdx,
								genGenericWrapper_technology::properties_selected_output_file.samplerate.category),
								tune, JVX_PROPERTY_CALLBACK_DIRECT);
							ic_cb_prop_selected_output_file_set_pre(callGate, 
								ptrRaw,
								jPAGID(genGenericWrapper_technology::properties_selected_output_file.num_channels.globalIdx,
								genGenericWrapper_technology::properties_selected_output_file.num_channels.category),
								tune, JVX_PROPERTY_CALLBACK_DIRECT);
							ic_cb_prop_selected_output_file_set(callGate, 
								ptrRaw,
								jPAGID(genGenericWrapper_technology::properties_selected_output_file.num_channels.globalIdx,
								genGenericWrapper_technology::properties_selected_output_file.num_channels.category),
								tune, JVX_PROPERTY_CALLBACK_DIRECT);
							ic_cb_prop_selected_output_file_set_pre(callGate,  
								ptrRaw,
								jPAGID(genGenericWrapper_technology::properties_selected_output_file.mode.globalIdx,
								genGenericWrapper_technology::properties_selected_output_file.mode.category),
								tune, JVX_PROPERTY_CALLBACK_DIRECT);
							ic_cb_prop_selected_output_file_set(callGate,  
								ptrRaw,
								jPAGID(genGenericWrapper_technology::properties_selected_output_file.mode.globalIdx,
								genGenericWrapper_technology::properties_selected_output_file.mode.category),
								tune, JVX_PROPERTY_CALLBACK_DIRECT);
							breakme = true;
						}
					}
					tune.offsetStart++;
					if (breakme)
					{
						break;
					}
				}
				else
				{
					std::string txt = "Failed to find specific properties for file <" + genGenericWrapper_technology::select_files.output_file.value.entries[i] + ">";
					_report_text_message(("put configuration: warning #" + jvx_size2String(i) + ": " + txt).c_str(), JVX_REPORT_PRIORITY_WARNING);
					break;
				}
			}
		}

		// Activate a file if none was selected before
		if (JVX_EVALUATE_BITFIELD(genGenericWrapper_technology::select_files.output_file.value.selection()) == false)
		{
			if (genGenericWrapper_technology::select_files.output_file.value.entries.size() > 0)
			{
				jvx_bitZSet(genGenericWrapper_technology::select_files.output_file.value.selection(), 0);
			}
		}


        // Lock all aspects
        this->lock_state();
		_lock_properties_local();
        this->lock_files();
        
        // Align the input and output file properties to the file handler structs.
        // This opens all files - warning, opening may fail
        this->updatePropertyToFileLists_nolock(SAMPLERATE_OUTFILE_START, warn);
        for(i = 0; i < warn.size(); i++)
        {
            _report_text_message(("put configuration: warning #" + jvx_size2String(i) + ": " + warn[i]).c_str(), JVX_REPORT_PRIORITY_WARNING);
        }
                
        // Update property list for input and output files
        this->updateInputAdditionalFilenamesPropertyList_nolock();
        this->updateOutputAdditionalFilenamesPropertyList_nolock();
        
        this->unlock_files();
        _unlock_properties_local();
        this->unlock_state();
        
                        
        this->lock_state();
        _lock_properties_local();
        this->lock_files();
        
		// ==================================================================================
		// We need to add the dummy channels here...
		// ==================================================================================
		for (i = 0; i < numInDummiesCopy; i++)
		{
			std::string command = "addDummyInput();";
			jvxApiString jstr;
			jstr.assign_const(command.c_str(), command.size());
			jvx_set_property(static_cast<IjvxProperties*>(this), &jstr, 0, 1, JVX_DATAFORMAT_STRING, true, "/JVX_GENW_COMMAND", callGate);
		}
		for (i = 0; i < numOutDummiesCopy; i++)
		{
			std::string command = "addDummyOutput();";
			jvxApiString jstr;
			jstr.assign_const(command.c_str(), command.size());
			jvx_set_property(static_cast<IjvxProperties*>(this), &jstr, 0, 1, JVX_DATAFORMAT_STRING, true, "/JVX_GENW_COMMAND", callGate);
		}
        this->unlock_files();
        _unlock_properties_local();
        this->unlock_state();
        
        // Finally, inform all connected devices
        this->updateAllDevicesOtherThan_lock(NULL);
    }
	return(JVX_NO_ERROR);
}

jvxErrorType
CjvxGenericWrapperTechnology::get_configuration(jvxCallManagerConfiguration* callConf, IjvxConfigProcessor* processor,
	jvxHandle* sectionWhereToAddAllSubsections)
{
	jvxSize i;
	jvxConfigData* datSubSec = NULL;
	jvxConfigData* datSubSubSec = NULL;
	jvxCallManagerProperties callGate;
	const jvx::propertyRawPointerType::IjvxRawPointerType* ptrRaw = nullptr;

	genGenericWrapper_technology::get_configuration__properties_selected(callConf, processor, sectionWhereToAddAllSubsections);

	genGenericWrapper_technology::get_configuration__select_files(callConf, processor, sectionWhereToAddAllSubsections);
	// All parameters for the input files
	for (i = 0; i < genGenericWrapper_technology::select_files.input_file.value.entries.size(); i++)
	{
		datSubSec = NULL;
		jvx::propertyDetail::CjvxTranferDetail tune(false, i);
		//jvxSize id = i;
		jvxSize num = 1;
		jvxDataFormat form = JVX_DATAFORMAT_16BIT_LE;
		processor->createEmptySection(&datSubSec, ("JVX_SECTION_ONE_INPUT_FILE_" + jvx_size2String(tune.offsetStart)).c_str());
		ic_cb_prop_selected_input_file_get(callGate,
			ptrRaw, 
			jPAGID(genGenericWrapper_technology::properties_selected_input_file.boost_prio.globalIdx,
			genGenericWrapper_technology::properties_selected_input_file.boost_prio.category),
			tune, JVX_PROPERTY_CALLBACK_DIRECT);
		form = JVX_DATAFORMAT_16BIT_LE;
		ic_cb_prop_selected_input_file_get(callGate, 
			ptrRaw,
			jPAGID(genGenericWrapper_technology::properties_selected_input_file.lookahead.globalIdx,
			genGenericWrapper_technology::properties_selected_input_file.lookahead.category),
			tune, JVX_PROPERTY_CALLBACK_DIRECT);
		ic_cb_prop_selected_input_file_get(callGate, 
			ptrRaw,
			jPAGID(genGenericWrapper_technology::properties_selected_input_file.loop.globalIdx,
			genGenericWrapper_technology::properties_selected_input_file.loop.category),
			tune, JVX_PROPERTY_CALLBACK_DIRECT);
		ic_cb_prop_selected_input_file_get(callGate, 
			ptrRaw,
			jPAGID(genGenericWrapper_technology::properties_selected_input_file.pause_on_start.globalIdx,
			genGenericWrapper_technology::properties_selected_input_file.pause_on_start.category),
			tune, JVX_PROPERTY_CALLBACK_DIRECT);

		genGenericWrapper_technology::get_configuration__properties_selected_input_file(callConf, processor, datSubSec);
		datSubSubSec = NULL;
		processor->createAssignmentString(&datSubSubSec, "JVX_SECTION_ONE_INPUT_FILE_NAME", genGenericWrapper_technology::select_files.input_file.value.entries[i].c_str());
		processor->addSubsectionToSection(datSubSec, datSubSubSec);

		processor->addSubsectionToSection(sectionWhereToAddAllSubsections, datSubSec);
	}

	for (i = 0; i < genGenericWrapper_technology::select_files.output_file.value.entries.size(); i++)
	{
		datSubSec = NULL;
		jvx::propertyDetail::CjvxTranferDetail tune(false, i);
		jvxDataFormat form = JVX_DATAFORMAT_16BIT_LE;

		processor->createEmptySection(&datSubSec, ("JVX_SECTION_ONE_OUTPUT_FILE_" + jvx_size2String(tune.offsetStart)).c_str());
		ic_cb_prop_selected_output_file_get(callGate,  
			ptrRaw,
			jPAGID(genGenericWrapper_technology::properties_selected_output_file.boost_prio.globalIdx,
			genGenericWrapper_technology::properties_selected_output_file.boost_prio.category),
			tune, JVX_PROPERTY_CALLBACK_DIRECT);
		ic_cb_prop_selected_output_file_get(callGate,  
			ptrRaw,
			jPAGID(genGenericWrapper_technology::properties_selected_output_file.lookahead.globalIdx,
			genGenericWrapper_technology::properties_selected_output_file.lookahead.category),
			tune, JVX_PROPERTY_CALLBACK_DIRECT);
		ic_cb_prop_selected_output_file_get(callGate, 
			ptrRaw,
			jPAGID(genGenericWrapper_technology::properties_selected_output_file.num_channels.globalIdx,
			genGenericWrapper_technology::properties_selected_input_file.num_channels.category),
			tune, JVX_PROPERTY_CALLBACK_DIRECT);
		ic_cb_prop_selected_output_file_get(callGate,  
			ptrRaw,
			jPAGID(genGenericWrapper_technology::properties_selected_output_file.samplerate.globalIdx,
			genGenericWrapper_technology::properties_selected_input_file.samplerate.category),
			tune, JVX_PROPERTY_CALLBACK_DIRECT);
		ic_cb_prop_selected_output_file_get(callGate, 
			ptrRaw,
			jPAGID(genGenericWrapper_technology::properties_selected_output_file.mode.globalIdx,
			genGenericWrapper_technology::properties_selected_input_file.mode.category),
			tune, JVX_PROPERTY_CALLBACK_DIRECT);

		genGenericWrapper_technology::get_configuration__properties_selected_output_file(callConf, processor, datSubSec);
		datSubSubSec = NULL;
		processor->createAssignmentString(&datSubSubSec, "JVX_SECTION_ONE_OUTPUT_FILE_NAME", genGenericWrapper_technology::select_files.output_file.value.entries[i].c_str());
		processor->addSubsectionToSection(datSubSec, datSubSubSec);

		processor->addSubsectionToSection(sectionWhereToAddAllSubsections, datSubSec);
	}

	if (genGenericWrapper_technology::select_files.input_file.value.entries.size())
	{
		jvx_bitZSet(genGenericWrapper_technology::select_files.input_file.value.selection(), 0);
	}
	if (genGenericWrapper_technology::select_files.output_file.value.entries.size())
	{
		jvx_bitZSet(genGenericWrapper_technology::select_files.output_file.value.selection(), 0);
	}

	genGenericWrapper_technology::get_configuration__properties_selected_active(callConf, processor, sectionWhereToAddAllSubsections);

	if (runtime.theSelectedTech)
	{
		IjvxConfiguration* cfg = reqInterfaceObj<IjvxConfiguration>(runtime.theSelectedTech);
		if (cfg)
		{
			jvxConfigData* datSection = nullptr;
			jvxErrorType resL = processor->createEmptySection(&datSection, JVX_GENERIC_WRAPPER_CONFIG_SUBSECTION_TECHNOLOGY);
			if ((resL == JVX_NO_ERROR) && datSection)
			{
				resL = cfg->get_configuration(callConf, processor, datSection);
				if (resL == JVX_NO_ERROR)
				{
					processor->addSubsectionToSection(sectionWhereToAddAllSubsections, datSection);
				}
			}
			
			retInterfaceObj<IjvxConfiguration>(runtime.theSelectedTech, cfg);
		}
	}

	return(JVX_NO_ERROR);
}

jvxErrorType
CjvxGenericWrapperTechnology::set_property(
	jvxCallManagerProperties& callGate,
	const jvx::propertyRawPointerType::IjvxRawPointerType& rawPtr,
	const jvx::propertyAddress::IjvxPropertyAddress& ident,
	const jvx::propertyDetail::CjvxTranferDetail& trans
)
{
	jvxSize i;
	jvxBool updateWindowDo = false;
	jvxErrorType res = JVX_NO_ERROR;
	jvxBool reportParametersChanged = false;
	std::vector<std::string> warn;

	JVX_TRANSLATE_PROP_ADDRESS_IDX_CAT(ident, propId, category);

	_lock_properties_local();
	this->lock_files();
	if(!theFiles.allowModification)
	{
		// Do not allow any file modification if device is in processing
		if(
			(propId == genGenericWrapper_technology::select_files.input_file.globalIdx) &&
			(category == genGenericWrapper_technology::select_files.input_file.category))
		{
			res = JVX_ERROR_COMPONENT_BUSY;
		}

		if(
			(propId == genGenericWrapper_technology::select_files.output_file.globalIdx) &&
			(category == genGenericWrapper_technology::select_files.output_file.category))
		{
			res = JVX_ERROR_COMPONENT_BUSY;
		}
	}
	else
	{
		res = JVX_MY_BASE_CLASS_T::set_property(callGate, rawPtr, ident, trans);
		if(res == JVX_NO_ERROR)
		{
			if(
				(propId == genGenericWrapper_technology::properties_selected.technologies.globalIdx) &&
				(category == genGenericWrapper_technology::properties_selected.technologies.category))
			{
				updateWindowDo = true;
			}

			if(
				(propId == genGenericWrapper_technology::properties_selected_active.specifyCommand.globalIdx) &&
				(category == genGenericWrapper_technology::properties_selected_active.specifyCommand.category))
			{
				// Process the requested command
				res = processIssuedStringCommand(genGenericWrapper_technology::properties_selected_active.specifyCommand.value);
			}


			if(
				(propId == genGenericWrapper_technology::select_files.input_file.globalIdx) &&
				(category == genGenericWrapper_technology::select_files.input_file.category))
			{
				updatePropertyToFileLists_nolock(SAMPLERATE_OUTFILE_START, warn);
				for(i = 0; i < warn.size(); i++)
				{
					_report_text_message(("set property: warning #" + jvx_size2String(i) + ": " + warn[i]).c_str(), JVX_REPORT_PRIORITY_WARNING);
				}
				this->updateInputAdditionalFilenamesPropertyList_nolock();
			}

			if(
				(propId == genGenericWrapper_technology::select_files.output_file.globalIdx) &&
				(category == genGenericWrapper_technology::select_files.output_file.category))
			{
				updatePropertyToFileLists_nolock(SAMPLERATE_OUTFILE_START, warn);
				for(i = 0; i < warn.size(); i++)
				{
					_report_text_message(("set property: warning #" + jvx_size2String(i) + ": " + warn[i]).c_str(), JVX_REPORT_PRIORITY_WARNING);
				}
				this->updateOutputAdditionalFilenamesPropertyList_nolock();
			}

		}
	}
	this->unlock_files();
	_unlock_properties_local();

	this->updateAllDevicesOtherThan_lock(NULL);

	if(updateWindowDo)
	{
	}

	return(res);
}

jvxErrorType
CjvxGenericWrapperTechnology::updatePropertyToFileLists_nolock(jvxInt32 sRate, std::vector<std::string>& warning)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize i;
	jvxSize j;
	jvxInt32 uniqueId = 0;
	jvxInt16 numChannels = 0;
	int diffDummies = 0;
	typedef struct
	{
		std::string name;
		std::string fname;
		jvxBool foundit;
	} fileAndSelection;
	std::vector<fileAndSelection> fileList;
	std::string descr;
	jvxErrorType resL = JVX_NO_ERROR;
	jvxBool multiEntries = false;

	for(j = 0; j < theFiles.theInputFiles.size(); j++)
	{
		fileAndSelection oneEntry;
		oneEntry.foundit = false;
		oneEntry.name = theFiles.theInputFiles[j].common.name;
		oneEntry.fname = theFiles.theInputFiles[j].common.friendlyName;
		fileList.push_back(oneEntry);
	}

	for(i = 0; i < genGenericWrapper_technology::select_files.input_file.value.entries.size(); i++)
	{
		jvxBool alreadyPresent = false;
		for(j = 0; j < fileList.size(); j++)
		{
			if(genGenericWrapper_technology::select_files.input_file.value.entries[i] == fileList[j].name)
			{
				if(fileList[j].foundit == false)
				{
					fileList[j].foundit = true;
					alreadyPresent = true;
					multiEntries = false;
					break;
				}
				else
				{
					multiEntries = true;
					alreadyPresent = true;
					break;
				}
			}
		}
		if(!alreadyPresent)
		{
			resL = this->addInputFile_nolock(genGenericWrapper_technology::select_files.input_file.value.entries[i], uniqueId, numChannels, descr);
			if(resL != JVX_NO_ERROR)
			{
				if(resL == JVX_ERROR_OPEN_FILE_FAILED)
				{
					warning.push_back("Failed to open file " + genGenericWrapper_technology::select_files.input_file.value.entries[i]);
				}
				else if(resL == JVX_ERROR_DUPLICATE_ENTRY)
				{
					warning.push_back("File " + genGenericWrapper_technology::select_files.input_file.value.entries[i] + " is already in use");
				}
				else
				{
					warning.push_back("Failed to open file " + genGenericWrapper_technology::select_files.input_file.value.entries[i] + " due to unknown reason.");
				}
			}
		}
		else
		{
			if(multiEntries)
			{
				warning.push_back("File " + genGenericWrapper_technology::select_files.input_file.value.entries[i] + " is already in use");
			}
		}
	}

	for(i = 0; i < fileList.size(); i++)
	{
		if(!fileList[i].foundit)
		{
			for(j = 0; j < theFiles.theInputFiles.size(); j++)
			{
				if(fileList[i].name == theFiles.theInputFiles[j].common.name)
				{
					this->removeInputFile_nolock(j, uniqueId);
					break;
				}
			}
		}
	}

	fileList.clear();
	for(j = 0; j < theFiles.theOutputFiles.size(); j++)
	{
		fileAndSelection oneEntry;
		oneEntry.foundit = false;
		oneEntry.name = theFiles.theOutputFiles[j].common.name;
		oneEntry.fname = theFiles.theOutputFiles[j].common.friendlyName;
		fileList.push_back(oneEntry);
	}

	for(i = 0; i < genGenericWrapper_technology::select_files.output_file.value.entries.size(); i++)
	{
		jvxBool alreadyPresent = false;
		for(j = 0; j < fileList.size(); j++)
		{
			if (genGenericWrapper_technology::select_files.output_file.value.entries[i] == fileList[j].name)
			{
				if (fileList[j].foundit == false)
				{
					fileList[j].foundit = true;
					alreadyPresent = true;
					break;
				}
				else
				{
					multiEntries = true;
					alreadyPresent = true;
					break;
				}
			}
		}
		if(!alreadyPresent)
		{
			// If we have only provided a filename so far, generate a dummy tag entry
			resL = this->addOutputFile_nolock(genGenericWrapper_technology::select_files.output_file.value.entries[i], 
				"", uniqueId, numChannels, sRate, descr);
			if(resL != JVX_NO_ERROR)
			{
				if(resL == JVX_ERROR_DUPLICATE_ENTRY)
				{
					warning.push_back("File " + genGenericWrapper_technology::select_files.output_file.value.entries[i] + " is already in use");
				}
				else
				{
					warning.push_back("Failed to open file " + genGenericWrapper_technology::select_files.output_file.value.entries[i] + " due to unknown reason.");
				}
			}
		}
		else
		{
			if(multiEntries)
			{
				warning.push_back("File " + genGenericWrapper_technology::select_files.output_file.value.entries[i] + " is already in use");
			}
		}
	}

	for(i = 0; i < fileList.size(); i++)
	{
		if(!fileList[i].foundit)
		{
			for(j = 0; j < theFiles.theOutputFiles.size(); j++)
			{
				if(fileList[i].name.empty())
				{
					if(fileList[i].fname == theFiles.theOutputFiles[j].common.friendlyName)
					{
						this->removeOutputFile_nolock(j, uniqueId);
						break;
					}
				}
				else
				{
					if(fileList[i].name == theFiles.theOutputFiles[j].common.name)
					{
						this->removeOutputFile_nolock(j, uniqueId);
						break;
					}
				}
			}
		}
	}

	diffDummies = (int)genGenericWrapper_technology::properties_selected_active.numberInDummies.value - (int)theDummyChannels.inputs.size();
	assert(diffDummies >= 0);
	for (i = 0; i < diffDummies; i++)
	{
		oneDummyChan oneChan;
		oneChan.uniqueId = theDummyChannels.idUsed;
		oneChan.buf = NULL;
		oneChan.wasStarted = false;
		theDummyChannels.idUsed++;
		this->theDummyChannels.inputs.push_back(oneChan);
	}
	diffDummies = (int) genGenericWrapper_technology::properties_selected_active.numberOutDummies.value - (int)theDummyChannels.outputs.size();
	assert(diffDummies >= 0);
	for (i = 0; i < diffDummies; i++)
	{
		oneDummyChan oneChan;
		oneChan.uniqueId = theDummyChannels.idUsed;
		oneChan.buf = NULL;
		oneChan.wasStarted = false;
		theDummyChannels.idUsed++;
		this->theDummyChannels.outputs.push_back(oneChan);
	}

	return(res);
}

jvxErrorType
CjvxGenericWrapperTechnology::addFileNameInputProperty_lock(std::string & fname)
{
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;
	_lock_properties_local();
	jvxBool foundit = false;
	for(i = 0; i < genGenericWrapper_technology::select_files.input_file.value.entries.size(); i++)
	{
		if(genGenericWrapper_technology::select_files.input_file.value.entries[i] == fname)
		{
			foundit = false;
		}
	}
	if(!foundit)
	{
		res = JVX_NO_ERROR;
		genGenericWrapper_technology::select_files.input_file.value.entries.push_back(fname);
	}
	else
	{
		res = JVX_ERROR_DUPLICATE_ENTRY;
	}
	_unlock_properties_local();
	return(res);
}

jvxErrorType
CjvxGenericWrapperTechnology::addFileNameInputProperty_nolock(std::string & fname)
{
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;

	jvxBool foundit = false;
	for(i = 0; i < genGenericWrapper_technology::select_files.input_file.value.entries.size(); i++)
	{
		if(genGenericWrapper_technology::select_files.input_file.value.entries[i] == fname)
		{
			foundit = false;
		}
	}
	if(!foundit)
	{
		res = JVX_NO_ERROR;
		genGenericWrapper_technology::select_files.input_file.value.entries.push_back(fname);
	}
	else
	{
		res = JVX_ERROR_DUPLICATE_ENTRY;
	}
	return(res);
}

jvxErrorType
CjvxGenericWrapperTechnology::addFileNameOutputProperty_lock(std::string & fname)
{
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;
	_lock_properties_local();
	jvxBool foundit = false;
	for(i = 0; i < genGenericWrapper_technology::select_files.output_file.value.entries.size(); i++)
	{
		if(genGenericWrapper_technology::select_files.output_file.value.entries[i] == fname)
		{
			foundit = false;
		}
	}
	if(!foundit)
	{
		res = JVX_NO_ERROR;
		genGenericWrapper_technology::select_files.output_file.value.entries.push_back(fname);
	}
	else
	{
		res = JVX_ERROR_DUPLICATE_ENTRY;
	}
	_unlock_properties_local();
	return(res);
}

jvxErrorType
CjvxGenericWrapperTechnology::addFileNameOutputProperty_nolock(std::string & fname)
{
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;

	jvxBool foundit = false;
	for(i = 0; i < genGenericWrapper_technology::select_files.output_file.value.entries.size(); i++)
	{
		if(genGenericWrapper_technology::select_files.output_file.value.entries[i] == fname)
		{
			foundit = true;
		}
	}
	if(!foundit)
	{
		res = JVX_NO_ERROR;
		genGenericWrapper_technology::select_files.output_file.value.entries.push_back(fname);
	}
	else
	{
		res = JVX_ERROR_DUPLICATE_ENTRY;
	}
	return(res);
}

jvxErrorType
CjvxGenericWrapperTechnology::updateInputAdditionalFilenamesPropertyList_nolock()
{
	jvxSize i;
	genGenericWrapper_technology::select_files.input_file.value.entries.clear();
	for(i = 0; i < theFiles.theInputFiles.size(); i++)
	{
		genGenericWrapper_technology::select_files.input_file.value.entries.push_back(theFiles.theInputFiles[i].common.name);
	}

	genGenericWrapper_technology::properties_selected_active.numberInDummies.value = theDummyChannels.inputs.size();
	return(JVX_NO_ERROR);
}
		
jvxErrorType
CjvxGenericWrapperTechnology::updateOutputAdditionalFilenamesPropertyList_nolock()
{
	jvxSize i;
	genGenericWrapper_technology::select_files.output_file.value.entries.clear();
	for(i = 0; i < theFiles.theOutputFiles.size(); i++)
	{
		genGenericWrapper_technology::select_files.output_file.value.entries.push_back(theFiles.theOutputFiles[i].common.name);
	}

	genGenericWrapper_technology::properties_selected_active.numberOutDummies.value = theDummyChannels.outputs.size();
	return(JVX_NO_ERROR);
}

// Update all connected devices to add channels
jvxErrorType
CjvxGenericWrapperTechnology::updateAllDevicesOtherThan_lock(CjvxGenericWrapperDevice* notme)
{
	jvxSize j;

	this->lock_files();
	auto elm = _common_tech_set.lstDevices.begin();
	for (; elm != _common_tech_set.lstDevices.end(); elm++)
	{
		CjvxGenericWrapperDevice* theDevice = dynamic_cast<CjvxGenericWrapperDevice*>(elm->hdlDev);
		if(theDevice != notme)
		{
			theDevice->lock_settings();

			theDevice->updateChannelMapperWithFiles_nolock();
			theDevice->updateChannelExposure_nolock();

			theDevice->unlock_settings();

			jvxState stat = JVX_STATE_NONE;
			theDevice->_state(&stat);
			if(stat >= JVX_STATE_SELECTED)
			{
				theDevice->updateDependentVariables_lock(-1, JVX_PROPERTY_CATEGORY_PREDEFINED, true);
			}
		}
	}
	this->unlock_files();
	return(JVX_NO_ERROR);
}

void
CjvxGenericWrapperTechnology::reportUpdateAllPropertiesOutput()
{
	_report_property_has_changed(
		genGenericWrapper_technology::select_files.output_file.category,
		genGenericWrapper_technology::select_files.output_file.globalIdx,
		false, 0, 1, JVX_COMPONENT_UNKNOWN);
}

jvxErrorType
CjvxGenericWrapperTechnology::request_hidden_interface(jvxInterfaceType tp, jvxHandle** hdl)
{
	jvxErrorType res = JVX_NO_ERROR;

	switch(tp)
	{
	case JVX_INTERFACE_PROPERTIES:
		*hdl = reinterpret_cast<jvxHandle*>(static_cast<IjvxProperties*>(this));
		break;

	case JVX_INTERFACE_CONFIGURATION:
		*hdl = reinterpret_cast<jvxHandle*>(static_cast<IjvxConfiguration*>(this));
		break;

	case JVX_INTERFACE_EXTERNAL_AUDIO_CHANNELS:
		*hdl = reinterpret_cast<jvxHandle*>(static_cast<IjvxExternalAudioChannels*>(this));
		break;

	default:
		res = JVX_MY_BASE_CLASS_T::request_hidden_interface(tp, hdl);
	}
	return(res);
};

jvxErrorType
CjvxGenericWrapperTechnology::return_hidden_interface(jvxInterfaceType tp, jvxHandle* hdl)
{
	jvxErrorType res = JVX_NO_ERROR;

	switch(tp)
	{
	case JVX_INTERFACE_PROPERTIES:
		if(hdl == reinterpret_cast<jvxHandle*>(static_cast<IjvxProperties*>(this)))
		{
			res = JVX_NO_ERROR;
		}
		else
		{
			res = JVX_ERROR_INVALID_ARGUMENT;
		}
		break;


	case JVX_INTERFACE_CONFIGURATION:
		if(hdl == reinterpret_cast<jvxHandle*>(static_cast<IjvxConfiguration*>(this)))
		{
			res = JVX_NO_ERROR;
		}
		else
		{
			res = JVX_ERROR_INVALID_ARGUMENT;
		}
		break;

	case JVX_INTERFACE_EXTERNAL_AUDIO_CHANNELS:
		if(hdl == reinterpret_cast<jvxHandle*>(static_cast<IjvxExternalAudioChannels*>(this)))
		{
			res = JVX_NO_ERROR;
		}
		else
		{
			res = JVX_ERROR_INVALID_ARGUMENT;
		}
		break;

	default:
		res = JVX_MY_BASE_CLASS_T::return_hidden_interface(tp, hdl);
	}
	return(res);
}

jvxErrorType
CjvxGenericWrapperTechnology::register_one_set(jvxBool is_input, jvxSize num_channels, jvxInt32 sRate, jvxSize* register_id, IjvxExternalAudioChannels_data* ref, const char* descr)
{
	jvxErrorType res = JVX_NO_ERROR;
	this->lock_files();
	if(theExtReferences.allowModification)
	{
		oneExternalAudioReference newElm;
		newElm.number_channels = JVX_SIZE_INT16(num_channels);
		newElm.samplerate = sRate;
		newElm.theCallback = ref;
		newElm.uniqueIdHandles = theExtReferences.uniqueIdHandles++;
		newElm.description = descr;

		if(is_input)
		{
			theExtReferences.theInputReferences.push_back(newElm);
		}
		else
		{
			theExtReferences.theOutputReferences.push_back(newElm);
		}

		if(register_id)
		{
			*register_id = newElm.uniqueIdHandles;
		}
	}
	else
	{
		res = JVX_ERROR_COMPONENT_BUSY;
	}

	this->unlock_files();

	this->updateAllDevicesOtherThan_lock(NULL);

	return(JVX_NO_ERROR);
}

jvxErrorType
CjvxGenericWrapperTechnology::unregister_one_set(jvxSize register_id)
{
	jvxErrorType res = JVX_NO_ERROR;
	this->lock_files();

	std::vector<oneExternalAudioReference>::iterator elm;
	if(theExtReferences.allowModification)
	{
		res = JVX_ERROR_ELEMENT_NOT_FOUND;

		for(elm = theExtReferences.theInputReferences.begin(); elm != theExtReferences.theInputReferences.end(); elm++)
		{
			if(elm->uniqueIdHandles == register_id)
			{
				break;
			}
		}
		if( elm != theExtReferences.theInputReferences.end())
		{
			res = JVX_NO_ERROR;
			theExtReferences.theInputReferences.erase(elm);
		}

		if(res == JVX_ERROR_ELEMENT_NOT_FOUND)
		{
			for(elm = theExtReferences.theOutputReferences.begin(); elm != theExtReferences.theOutputReferences.end(); elm++)
			{
				if(elm->uniqueIdHandles == register_id)
				{
					break;
				}
			}
			if( elm != theExtReferences.theOutputReferences.end())
			{
				res = JVX_NO_ERROR;
				theExtReferences.theOutputReferences.erase(elm);
			}
		}
	}
	this->unlock_files();

	if(res == JVX_NO_ERROR)
	{
		this->updateAllDevicesOtherThan_lock(NULL);
	}

	return(JVX_NO_ERROR);
}

jvxErrorType
CjvxGenericWrapperTechnology::processIssuedStringCommand(std::string command)
{
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;
	
	jvxInt32 uId = 0;
	jvxInt16 nChs = 0;
	std::string descr;

	std::string f_expr;
	std::vector<std::string> args;

	res = jvx_parseCommandIntoToken(command, f_expr, args);
	if(res == JVX_NO_ERROR)
	{
		if(f_expr == "addInputFile")
		{
			assert(args.size() == 1);
			for(i = 0; i < args.size(); i++)
			{
				this->lock_files();
				res = this->addInputFile_nolock(args[i], uId, nChs, descr);
				this->unlock_files();
				if(res != JVX_NO_ERROR)
				{
					break;
				}
			}
		}
		else if(f_expr == "remInputFile")
		{
			assert(args.size() == 1);
			for(i = 0; i < args.size(); i++)
			{
				this->lock_files();
				res = this->removeInputFile_nolock(atoi(args[i].c_str()), uId);
				this->unlock_files();
				if(res != JVX_NO_ERROR)
				{
					break;
				}
			}
		}
		else if(f_expr == "addOutputFile")
		{
			if(args.size() == 2)
			{
				this->lock_files();
				res = this->addOutputFile_nolock(args[0], args[1], uId, nChs, SAMPLERATE_OUTFILE_START, descr);
				this->unlock_files();
			}
			else
			{
				this->lock_files();
				res = this->addOutputFile_nolock(args[0], "", uId, nChs, SAMPLERATE_OUTFILE_START, descr);
				this->unlock_files();
			}

		}
		else if (f_expr == "renameOutputFile")
		{
			if (args.size() == 2)
			{
				this->lock_files();
				for (i = 0; i < this->theFiles.theOutputFiles.size(); i++)
				{
					if (theFiles.theOutputFiles[i].common.name == args[0])
					{
						theFiles.theOutputFiles[i].common.name = args[1];
						break;
					}
				}
				this->unlock_files();
			}
		}
        else if(f_expr == "remOutputFile")
        {
            assert(args.size() == 1);
            this->lock_files();
            res = this->removeOutputFile_nolock(atoi(args[0].c_str()), uId);
            this->unlock_files();
        }

		else if(f_expr == "renameOutputFileTags")
		{
			assert(args.size() == 2);
			this->lock_files();
			for(i = 0; i < this->theFiles.theOutputFiles.size(); i++)
			{
				if(i == atoi(args[0].c_str()))
				{
					this->theFiles.theOutputFiles[i].common.friendlyName = args[1];
					break;
				}
			}
			this->unlock_files();
		}
		else if (f_expr == "addDummyInput")
		{
			oneDummyChan oneChan;
			oneChan.uniqueId = theDummyChannels.idUsed;
			oneChan.buf = NULL;
			oneChan.wasStarted = false;
			theDummyChannels.idUsed++;
			this->lock_files();
			this->theDummyChannels.inputs.push_back(oneChan);
			this->unlock_files();
		}
		else if (f_expr == "remDummyInput")
		{
			this->lock_files();
			if (this->theDummyChannels.inputs.size())
			{
				this->theDummyChannels.inputs.pop_back();
			}
			this->unlock_files();
		}
		else if (f_expr == "addDummyOutput")
		{
			oneDummyChan oneChan;
			oneChan.uniqueId = theDummyChannels.idUsed;
			oneChan.buf = NULL;
			oneChan.wasStarted = false;
			theDummyChannels.idUsed++;
			this->lock_files();
			this->theDummyChannels.outputs.push_back(oneChan);
			this->unlock_files();
		}
		else if (f_expr == "remDummyOutput")
		{
			this->lock_files();
			if (this->theDummyChannels.outputs.size())
			{
				this->theDummyChannels.outputs.pop_back();
			}
			this->unlock_files();
		}
		else
		{
			res = JVX_ERROR_UNSUPPORTED;
		}

		this->updateInputAdditionalFilenamesPropertyList_nolock();
		this->updateOutputAdditionalFilenamesPropertyList_nolock();

		// Default case: always activae first entry
		jvx_bitZero(genGenericWrapper_technology::select_files.input_file.value.selection());
		jvx_bitZero(genGenericWrapper_technology::select_files.output_file.value.selection());
		if (genGenericWrapper_technology::select_files.input_file.value.entries.size())
		{
			jvx_bitZSet(genGenericWrapper_technology::select_files.input_file.value.selection(), 0);
		}
		if (genGenericWrapper_technology::select_files.output_file.value.entries.size())
		{
			jvx_bitZSet(genGenericWrapper_technology::select_files.output_file.value.selection(), 0);
		}
	}
	return(res);
}

