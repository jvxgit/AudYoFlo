#include "CjvxAudioFileWriterTechnology.h"

jvxErrorType
CjvxAudioFileWriterTechnology::put_configuration(jvxCallManagerConfiguration* callMan, IjvxConfigProcessor* processor,
	jvxHandle* sectionToContainAllSubsectionsForMe, const char* filename, jvxInt32 lineno )
{
	jvxErrorType res = JVX_NO_ERROR;
	std::vector<std::string> warns;
	if (_common_set_min.theState == JVX_STATE_ACTIVE)
	{
		jvxSize i;
		jvxSize selId = JVX_SIZE_UNSELECTED;
		genFileWriter_technology::put_configuration_all(callMan,
			processor, sectionToContainAllSubsectionsForMe,
			&warns);

		/*
		selId = jvx_bitfieldSelection2Id(genFileWriter_technology::file_params.device_lists.value.selection,
			genFileWriter_technology::file_params.device_lists.value.entries.size());
		if (selId < genFileWriter_technology::file_params.device_lists.value.entries.size())
		{
			selectedFile = genFileWriter_technology::file_params.device_lists.value.entries[selId];
		}

		for (i = 0; i < genFileWriter_technology::file_params.device_lists.value.entries.size(); i++)
		{
			fNamesOnConfig.push_back(genFileWriter_technology::file_params.device_lists.value.entries[i]);
		}
		genFileWriter_technology::file_params.device_lists.value.entries.clear();
		jvx_bitFClear(genFileWriter_technology::file_params.device_lists.value.selection);
		*/
		genFileWriter_technology::file_params.device_lists.value.entries.clear();

		std::string token_devs = "JVX_FILEWRITER_DEVICE";
		jvxConfigData* datSec = nullptr;
		processor->getReferenceEntryCurrentSection_name(sectionToContainAllSubsectionsForMe,
			&datSec, "DEVICES");
		if (datSec)
		{
			jvxSize cnt = 0;
			while (1)
			{
				jvxErrorType resL = JVX_ERROR_ELEMENT_NOT_FOUND;
				std::string nm = token_devs + "_" + jvx_size2String(cnt);
				jvxConfigData* datSubSub = nullptr;
				oneFileDeviceOnConfig newElm;

				processor->getReferenceEntryCurrentSection_name(datSec, &datSubSub, nm.c_str());
				if (datSubSub)
				{
					jvxConfigData* secPrefix = nullptr;
					jvxConfigData* secFolder = nullptr;
					jvxConfigData* secDevSpec = nullptr;
					jvxConfigData* secProps = nullptr;
					processor->getReferenceEntryCurrentSection_name(datSubSub, &secFolder, "DEV_FOLDER_NAME");
					processor->getReferenceEntryCurrentSection_name(datSubSub, &secPrefix, "DEV_FILE_PREFIX");
					processor->getReferenceEntryCurrentSection_name(datSubSub, &secDevSpec, "DEV_SPEC");
					processor->getReferenceEntryCurrentSection_name(datSubSub, &secProps, "DEV_PROPS");
					if (secFolder && secPrefix && secDevSpec && secProps)
					{
						jvxApiString astr;
						resL = processor->getAssignmentString(secFolder, &astr);
						if (resL == JVX_NO_ERROR)
						{
							newElm.folder = astr.std_str();
							resL = processor->getAssignmentString(secPrefix, &astr);
							if (resL == JVX_NO_ERROR)
							{
								newElm.fNamePrefix = astr.std_str();
								resL = processor->getAssignmentString(secDevSpec, &astr);
								if (resL == JVX_NO_ERROR)
								{
									newElm.devSpec = astr.std_str();

									processor->printConfiguration(secProps, &astr, true);
									newElm.cfg_compact = astr.std_str();
								}
							}
						}
					}
				}
				if(resL == JVX_NO_ERROR)
				{
					devsOnConfig.push_back(newElm);					
					cnt++;
				}
				else
				{
					break;
				}
			} // while (1)			
		}
	
	}
	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxAudioFileWriterTechnology::get_configuration(jvxCallManagerConfiguration* callMan,
	IjvxConfigProcessor* processor, jvxHandle* sectionWhereToAddAllSubsections)
{
	
	genFileWriter_technology::get_configuration_all(callMan,
		processor, sectionWhereToAddAllSubsections);

	std::string token_devs = "JVX_FILEWRITER_DEVICE";
	jvxConfigData* datSec = nullptr;
	jvxSize cnt = 0;
	processor->createEmptySection(&datSec, "DEVICES");
	if (datSec)
	{
		for (auto oneElm : lstDevType)
		{
			std::string nm = token_devs + "_" + jvx_size2String(cnt);
			jvxConfigData* locSec = nullptr;
			processor->createEmptySection(&locSec, nm.c_str());
			if (locSec)
			{
				jvxConfigData* subSec = nullptr;
				processor->createAssignmentString(&subSec, "DEV_FOLDER_NAME", oneElm.second->foldername.c_str());
				if (subSec)
				{
					processor->addSubsectionToSection(locSec, subSec);
				}

				subSec = nullptr;
				processor->createAssignmentString(&subSec, "DEV_FILE_PREFIX", oneElm.second->fileprefix.c_str());
				if (subSec)
				{
					processor->addSubsectionToSection(locSec, subSec);
				}

				subSec = nullptr;
				processor->createAssignmentString(&subSec, "DEV_SPEC", oneElm.second->_common_set_min.theDescription.c_str());
				if (subSec)
				{
					processor->addSubsectionToSection(locSec, subSec);
				}

				IjvxConfiguration* cfgs = static_cast<IjvxConfiguration*>(oneElm.second);
				if (cfgs)
				{
					// Store local configuration as part of the technology config section
					subSec = nullptr;
					processor->createEmptySection(&subSec, "DEV_PROPS");
					if (subSec)
					{
						cfgs->get_configuration(callMan, processor, subSec);					
						processor->addSubsectionToSection(locSec, subSec);
					}
				}

				processor->addSubsectionToSection(datSec, locSec);
			}
		}
		
		processor->addSubsectionToSection(
			sectionWhereToAddAllSubsections,
			datSec);
	}

	return JVX_NO_ERROR;
}

jvxErrorType
CjvxAudioFileWriterTechnology::done_configuration()
{
	jvxErrorType res = JVX_NO_ERROR;

	// Configuration all done, activate the given filenames
	jvxSize cnt = 0;
	jvxSize idSelect = JVX_SIZE_UNSELECTED;

	update_wav_params_from_props();

	for (auto elm : devsOnConfig)
	{
		std::string devSpec;
		activateOneFile(
			elm.folder, elm.fNamePrefix,
			devSpec, idSelect, 
			elm.cfg_compact);
		report_device_was_born(devSpec);
	}
	update_local_properties(true, idSelect);
	return JVX_NO_ERROR;
}