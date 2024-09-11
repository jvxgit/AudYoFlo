#include "jvxAudioNodes/CjvxAuN2AudioMixer.h"

void 
CjvxAuN2AudioMixer::getChannelsStorageConfig(IjvxConfigProcessor* processor,
	jvxHandle* sectionToContainAllSubsectionsForMe, const std::string& prefix,
	std::map<std::string, std::list<oneEntryChannel> >& channelsInStorage,
	jvxBool isInput)
{
	jvxConfigData* datSecStorageEntry = nullptr;
	jvxConfigData* datSecStorageEntryTmp = nullptr;
	jvxConfigData* datSecStorageEntryTmpTmp = nullptr;
	jvxSize cnt = 0;
	while (1)
	{
		std::string token = prefix + jvx_size2String(cnt);
		processor->getReferenceEntryCurrentSection_name(sectionToContainAllSubsectionsForMe, &datSecStorageEntry, token.c_str());
		if (datSecStorageEntry)
		{
			jvxApiString astr;
			token = prefix;
			token += "MASTERNAME";
			std::string masName;

			processor->getReferenceEntryCurrentSection_name(datSecStorageEntry, &datSecStorageEntryTmp,
				token.c_str());
			if (datSecStorageEntryTmp)
			{
				processor->getAssignmentString(datSecStorageEntryTmp, &astr);
				masName = astr.std_str();
				datSecStorageEntryTmp = nullptr;

				std::list<oneEntryChannel> lst;
				jvxSize ccnt = 0;
				jvxBool entryOk = true;

				while (1)
				{
					token = prefix;
					token += "CHANNELNAME_";
					token += jvx_size2String(ccnt);

					processor->getReferenceEntryCurrentSection_name(datSecStorageEntry, &datSecStorageEntryTmp, token.c_str());
					if (datSecStorageEntryTmp)
					{
						jvxValue val;
						oneEntryChannel newEntry;

						token = prefix;
						token += "NAME";

						processor->getReferenceEntryCurrentSection_name(datSecStorageEntryTmp, &datSecStorageEntryTmpTmp, token.c_str());
						if (datSecStorageEntryTmpTmp)
						{
							processor->getAssignmentString(datSecStorageEntryTmpTmp, &astr);
							newEntry.name = astr.std_str();
						}
						else
						{
							entryOk = false;
							break;
						}
						datSecStorageEntryTmpTmp = nullptr;

						token = prefix;
						token += "GAIN";

						processor->getReferenceEntryCurrentSection_name(datSecStorageEntryTmp, &datSecStorageEntryTmpTmp, token.c_str());
						if (datSecStorageEntryTmpTmp)
						{
							processor->getAssignmentValue(datSecStorageEntryTmpTmp, &val);
							val.toContent(&newEntry.gain);
						}
						else
						{
							entryOk = false;
							break;
						}
						datSecStorageEntryTmpTmp = nullptr;

						token = prefix;
						token += "MUTE";

						processor->getReferenceEntryCurrentSection_name(datSecStorageEntryTmp, &datSecStorageEntryTmpTmp, token.c_str());
						if (datSecStorageEntryTmpTmp)
						{
							processor->getAssignmentValue(datSecStorageEntryTmpTmp, &val);
							val.toContent(&newEntry.mute);
						}
						else
						{
							entryOk = false;
							break;
						}
						datSecStorageEntryTmpTmp = nullptr;

						if (entryOk)
						{
							if (extender)
							{
								newEntry.attSpecificPtr = extender->allocateAttachChannelSpecific(true);
								extender->putConfiguration_attached(isInput, newEntry.attSpecificPtr, processor, datSecStorageEntryTmp, prefix);
							}

							lst.push_back(newEntry);
#ifdef JVX_AUDIOMIXER_DEBUG_SPECIFIC
							newEntry.attSpecificPtr = nullptr;
#endif
						}
					}
					else
					{
						break;
					}
					ccnt++;
				} // while (1)

				if (entryOk)
				{
					auto elmChans = channelsInStorage.find(masName);
					if (elmChans == channelsInStorage.end())
					{
						channelsInStorage[masName] = lst;
#ifdef JVX_AUDIOMIXER_DEBUG_SPECIFIC
						for (auto& elmL : lst)
						{
							elmL.attSpecificPtr = nullptr;
						}
#endif
					}
				}
			}
		}
		else
		{
			break;
		}
		cnt++;

	}
}

jvxErrorType 
CjvxAuN2AudioMixer::put_configuration(jvxCallManagerConfiguration* callConf, IjvxConfigProcessor* processor,
	jvxHandle* sectionToContainAllSubsectionsForMe,
	const char* filename,
	jvxInt32 lineno)
{
	std::vector<std::string> warns;
	jvxConfigData* datSecStorage = nullptr, *datSecStorageTmp = nullptr;
	jvxSize cnt = 0;
	jvxErrorType res = CjvxNVTasks::put_configuration(callConf, processor, sectionToContainAllSubsectionsForMe,
		filename,  lineno);
	if (res == JVX_NO_ERROR)
	{
		// ==============================================================================
		if (_common_set_min.theState == JVX_STATE_SELECTED)
		{

			// Store all storage to config file
			processor->getReferenceEntryCurrentSection_name(sectionToContainAllSubsectionsForMe, &datSecStorage, "AYF_CHANNELS_STORAGE");
			if (datSecStorage)
			{
				getChannelsStorageConfig(processor, datSecStorage, "AYF_INPUT_CHANNELS_STORAGE_", inputChannelsInStorage, true);
				getChannelsStorageConfig(processor, datSecStorage, "AYF_OUTPUT_CHANNELS_STORAGE_", outputChannelsInStorage, false);
			} // if (datSecStorage)}			
		}

		if (_common_set_min.theState == JVX_STATE_ACTIVE)
		{
			// Profiles active in state ACTIVE only
			while (1)
			{
				std::string token = "AYF_CHANNELS_PROFILE_";
				token += jvx_size2String(cnt);
				processor->getReferenceEntryCurrentSection_name(sectionToContainAllSubsectionsForMe, &datSecStorage, token.c_str());
				if (datSecStorage)
				{
					token = "AYF_CHANNELS_PROFILE_NAME";
					processor->getReferenceEntryCurrentSection_name(datSecStorage, &datSecStorageTmp, token.c_str());
					if (datSecStorageTmp)
					{
						jvxApiString astr;
						std::string name;
						processor->getAssignmentString(datSecStorageTmp, &astr);
						name = astr.std_str();
						if (!name.empty())
						{
							oneProfileParameters newProps;
							getChannelsStorageConfig(processor, datSecStorage, "AYF_INPUT_CHANNELS_STORAGE_", newProps.inputChannelsInStorage, true);
							getChannelsStorageConfig(processor, datSecStorage, "AYF_OUTPUT_CHANNELS_STORAGE_", newProps.outputChannelsInStorage, false);
							if (newProps.inputChannelsInStorage.size() || newProps.outputChannelsInStorage.size())
							{
								profileList[name] = newProps;
							}
						}
					}
					datSecStorage = nullptr;
				}
				else
				{
					break;
				}
				cnt++;
			}
			update_profile_list_properties();

			// gen2AudioMixer_node::put_configuration__profiles(callConf, processor, sectionToContainAllSubsectionsForMe, &warns);
		}
	}
	return(res);
}

void
CjvxAuN2AudioMixer::addChannelsStorageConfig(IjvxConfigProcessor* processor,
	jvxHandle* sectionWhereToAddAllSubsections, const std::string& prefix,
	std::map<std::string, std::list<oneEntryChannel> >& channelsInStorage,
	jvxBool isInput)
{
	jvxConfigData* datSecStorageEntry = nullptr;
	jvxConfigData* datSecStorageEntryTmp = nullptr;
	jvxConfigData* datSecStorageEntryTmpTmp = nullptr;
	jvxValue val;

	std::string token;
	jvxSize cnt = 0;
	for (auto& elmStorage : channelsInStorage)
	{
		auto& lst = elmStorage.second;
		if (lst.size())
		{
			processor->createEmptySection(&datSecStorageEntry, (prefix + jvx_size2String(cnt)).c_str());
			if (datSecStorageEntry)
			{
				token = prefix;
				token += "MASTERNAME";
				processor->createAssignmentString(&datSecStorageEntryTmp, token.c_str(), elmStorage.first.c_str());
				if (datSecStorageEntryTmp)
				{
					processor->addSubsectionToSection(datSecStorageEntry, datSecStorageEntryTmp);
				}
				datSecStorageEntryTmp = nullptr;

				jvxSize ccnt = 0;
				for (auto& lstElm : lst)
				{
					token = prefix;
					token += "CHANNELNAME_";
					token += jvx_size2String(ccnt);
					processor->createEmptySection(&datSecStorageEntryTmp, token.c_str());

					token = prefix;
					token += "NAME";

					processor->createAssignmentString(&datSecStorageEntryTmpTmp, token.c_str(), lstElm.name.c_str());
					processor->addSubsectionToSection(datSecStorageEntryTmp, datSecStorageEntryTmpTmp);
					datSecStorageEntryTmpTmp = nullptr;

					val.assign(lstElm.gain);
					token = prefix;
					token += "GAIN";
					processor->createAssignmentValue(&datSecStorageEntryTmpTmp, token.c_str(), val);
					processor->addSubsectionToSection(datSecStorageEntryTmp, datSecStorageEntryTmpTmp);
					datSecStorageEntryTmpTmp = nullptr;

					val.assign(lstElm.mute);
					token = prefix;
					token += "MUTE";
					processor->createAssignmentValue(&datSecStorageEntryTmpTmp, token.c_str(), val);
					processor->addSubsectionToSection(datSecStorageEntryTmp, datSecStorageEntryTmpTmp);
					datSecStorageEntryTmpTmp = nullptr;

					if (extender)
					{
						extender->getConfiguration_attached(isInput, lstElm.attSpecificPtr, processor, datSecStorageEntryTmp, prefix);
					}
					ccnt++;

					processor->addSubsectionToSection(datSecStorageEntry, datSecStorageEntryTmp);
					datSecStorageEntryTmp = nullptr;
				}
				processor->addSubsectionToSection(sectionWhereToAddAllSubsections, datSecStorageEntry);
				datSecStorageEntry = nullptr;
			} // if (datSecStorageEntry)
			cnt++;
		} // if (lst.size())
	}
}

jvxErrorType 
CjvxAuN2AudioMixer::get_configuration(jvxCallManagerConfiguration* callConf, IjvxConfigProcessor* processor,
	jvxHandle* sectionWhereToAddAllSubsections)
{
	jvxSize cnt = 0;
	jvxConfigData* datSecStorage = nullptr;
	jvxConfigData* datSecStorageName = nullptr;
	
	jvxErrorType res = CjvxNVTasks::get_configuration(callConf, processor, sectionWhereToAddAllSubsections);
	if (res == JVX_NO_ERROR)
	{
		if (_common_set_min.theState >= JVX_STATE_SELECTED)
		{
			// Update the storage at first
			for (auto& set : registeredChannelListInput)
			{
				updateChannelToStorage(inputChannelsInStorage, set.second.masName, set.second.channels, true);
			}
			for (auto& set : registeredChannelListOutput)
			{
				updateChannelToStorage(outputChannelsInStorage, set.second.masName, set.second.channels, false);
			}

			// Store all storage to config file
			processor->createEmptySection(&datSecStorage, "AYF_CHANNELS_STORAGE");
			if (datSecStorage)
			{
				addChannelsStorageConfig(processor, datSecStorage, "AYF_INPUT_CHANNELS_STORAGE_", inputChannelsInStorage, true);
				addChannelsStorageConfig(processor, datSecStorage, "AYF_OUTPUT_CHANNELS_STORAGE_", outputChannelsInStorage, false);
				processor->addSubsectionToSection(sectionWhereToAddAllSubsections, datSecStorage);
				datSecStorage = nullptr;
			}

			for (auto& elmPro : profileList)
			{
				if (elmPro.second.storeInConfigFile)
				{

					// Store all storage to config file
					processor->createEmptySection(&datSecStorage, ("AYF_CHANNELS_PROFILE_" + jvx_size2String(cnt)).c_str());
					processor->createAssignmentString(&datSecStorageName, "AYF_CHANNELS_PROFILE_NAME", elmPro.first.c_str());
					processor->addSubsectionToSection(datSecStorage, datSecStorageName);
					if (datSecStorage)
					{
						addChannelsStorageConfig(processor, datSecStorage, "AYF_INPUT_CHANNELS_STORAGE_", elmPro.second.inputChannelsInStorage, true);
						addChannelsStorageConfig(processor, datSecStorage, "AYF_OUTPUT_CHANNELS_STORAGE_", elmPro.second.outputChannelsInStorage, false);
						processor->addSubsectionToSection(sectionWhereToAddAllSubsections, datSecStorage);
					}
					datSecStorage = nullptr;
					cnt++;
				}
			}
		}
		if (_common_set_min.theState >= JVX_STATE_ACTIVE)
		{
			gen2AudioMixer_node::get_configuration__custom_profiles(callConf,
				processor, sectionWhereToAddAllSubsections);
		}
	}
	return(res);
}

void
CjvxAuN2AudioMixer::updateChannelFromStorage(std::map<std::string, std::list<oneEntryChannel>>& channelsInStorage, oneEntryChannel& newEntry, std::string masName, jvxBool isInput)
{
	auto channels_old = channelsInStorage.find(masName);
	if (channels_old != channelsInStorage.end())
	{
		for (auto& elmOc : channels_old->second)
		{
			if (elmOc.name == newEntry.name)
			{
				newEntry.gain = elmOc.gain;
				newEntry.mute = elmOc.mute;
				if (elmOc.attSpecificPtr && newEntry.attSpecificPtr)
				{
					if (extender)
					{
						extender->updateChannelStorage_attached(isInput, newEntry.attSpecificPtr, elmOc.attSpecificPtr);
					}
				}
			}
		}
	}
}

void
CjvxAuN2AudioMixer::updateChannelToStorage(std::map<std::string, std::list<oneEntryChannel>>& channelsInStorage, std::string masName, 
	const std::list<oneEntryChannel>& channels_old, jvxBool isInput)
{
	auto chanlst_it = channelsInStorage.find(masName);
	if (chanlst_it != channelsInStorage.end())
	{
		for (auto& channel_it : channels_old)
		{
			std::string name = channel_it.name;
			jvxBool foundit = false;
			for (auto& store_it : chanlst_it->second)
			{
				if (name == store_it.name)
				{
					foundit = true;
					store_it.gain = channel_it.gain;
					store_it.mute = channel_it.mute;
					if (store_it.attSpecificPtr && channel_it.attSpecificPtr)
					{
						if (extender)
						{
							extender->updateChannelStorage_attached(isInput,
								store_it.attSpecificPtr, channel_it.attSpecificPtr);
						}
					}
					break;
				}
			}
			if (!foundit)
			{
				oneEntryChannel newEntry;
				if (extender) newEntry.attSpecificPtr = extender->allocateAttachChannelSpecific(isInput);
				newEntry.gain = channel_it.gain;
				newEntry.mute = channel_it.mute;
				newEntry.name = channel_it.name;
				if (newEntry.attSpecificPtr && channel_it.attSpecificPtr)
				{
					if (extender) extender->updateChannelStorage_attached(isInput, newEntry.attSpecificPtr, channel_it.attSpecificPtr);
				}
				chanlst_it->second.push_back(newEntry);
#ifdef JVX_AUDIOMIXER_DEBUG_SPECIFIC
				newEntry.attSpecificPtr = nullptr;
#endif
			}
		}
	}
	else
	{
		std::list < oneEntryChannel> newLst;
		for (auto& channel_it : channels_old)
		{
			oneEntryChannel newEntry;
			if (extender) newEntry.attSpecificPtr = extender->allocateAttachChannelSpecific(isInput);
			newEntry.gain = channel_it.gain;
			newEntry.mute = channel_it.mute;
			newEntry.name = channel_it.name;
			if (newEntry.attSpecificPtr && channel_it.attSpecificPtr)
			{
				if (extender) extender->updateChannelStorage_attached(isInput, newEntry.attSpecificPtr, channel_it.attSpecificPtr);
			}
			newLst.push_back(newEntry);
#ifdef JVX_AUDIOMIXER_DEBUG_SPECIFIC
			newEntry.attSpecificPtr = nullptr;
#endif
		}
		channelsInStorage[masName] = newLst;
	}
}