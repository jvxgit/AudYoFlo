#include "CjvxGenericWrapperDevice.h"

void
CjvxGenericWrapperDevice::printout_config(IjvxConfigProcessor* processor, jvxHandle* sectionToContainAllSubsectionsForMe, std::string prefix)
{
    jvxSize i;
    jvxSize num, num2 = 0;
    jvxApiString fldStr;
    jvxData valD;
	jvxValue val;
    processor->getNumberEntriesCurrentSection(sectionToContainAllSubsectionsForMe, &num);
    {
        for(i = 0; i < num; i++)
        {
            jvxHandle* theDat = NULL;
            jvxConfigSectionTypes tp = JVX_CONFIG_SECTION_TYPE_UNKNOWN;
            processor->getReferenceEntryCurrentSection_id(sectionToContainAllSubsectionsForMe, &theDat, i);
            if(theDat)
            {
                processor->getNameCurrentEntry(theDat, &fldStr);
                _report_text_message((prefix + fldStr.std_str()).c_str(), JVX_REPORT_PRIORITY_INFO);
              
                processor->getTypeCurrentEntry(theDat,&tp);
                switch(tp)
                {
                    case JVX_CONFIG_SECTION_TYPE_SECTION:
                        printout_config(processor, theDat, "--" + prefix);
                        break;
                    case JVX_CONFIG_SECTION_TYPE_ASSIGNMENTVALUE:
                        processor->getAssignmentValue(theDat, &val);
						val.toContent(&valD);
                        _report_text_message((prefix + "Value=" + jvx_data2String(valD, 1)).c_str(), JVX_REPORT_PRIORITY_INFO);
                }
            }
        }
    }
}

jvxErrorType
CjvxGenericWrapperDevice::put_configuration(jvxCallManagerConfiguration* callConf, IjvxConfigProcessor* processor,
	jvxHandle* sectionToContainAllSubsectionsForMe, const char* filename, jvxInt32 lineno)
{
	jvxSize i;

	// Provide some of the config parameters for the base class
	jvxErrorType res = JVX_MY_BASE_CLASS_D::put_configuration(callConf, processor, sectionToContainAllSubsectionsForMe, filename , lineno);
	jvxErrorType resL = JVX_NO_ERROR;
	IjvxConfiguration* theConfigHandle = NULL;
	jvxHandle* datSection = NULL;
	std::vector<std::string> warn;

    // In cases, the base class parameters need to be taken over by this class before continuing
	this->lock_settings();
	this->rearrangeChannelMapper_noLock();
	this->updateChannelExposure_nolock();
	this->unlock_settings();

    // Read some other variables
	if(this->_common_set_min.theState >= JVX_STATE_ACTIVE)
	{
		this->lock_settings();
		// Transfer channel selection also to the connected device
		pass_channelsetup_nolock();
		this->unlock_settings();

        // printout_config(processor, sectionToContainAllSubsectionsForMe,"-->");
		genGenericWrapper_device::put_configuration__properties_active_higher(callConf, processor, sectionToContainAllSubsectionsForMe,  &warn);
		for(i = 0; i < warn.size(); i++)
		{
			_report_text_message(("put configuration: warning #" + jvx_size2String(i) + ": " + warn[i]).c_str(), JVX_REPORT_PRIORITY_INFO);
		}

		warn.clear();
		this->genGenericWrapper_device::put_configuration__properties_active(callConf, processor, sectionToContainAllSubsectionsForMe, &warn);
		for(i = 0; i < warn.size(); i++)
		{
			_report_text_message(("put configuration: warning #" + jvx_size2String(i) + ": " + warn[i]).c_str(), JVX_REPORT_PRIORITY_INFO);
		}

		// The channel selection may break during load of connected device. We store it here and use it later
		jvxSelectionList_cpp sel_in = CjvxAudioDevice_genpcg::properties_active.inputchannelselection.value;
		jvxSelectionList_cpp sel_out = CjvxAudioDevice_genpcg::properties_active.outputchannelselection.value;

        if(onInit.connectedDevice)
		{
			resL = processor->getReferenceEntryCurrentSection_name(sectionToContainAllSubsectionsForMe, &datSection, JVX_GENERIC_WRAPPER_CONFIG_SUBSECTION_DEVICE);
			if((resL == JVX_NO_ERROR) && datSection)
			{
				resL = onInit.connectedDevice->request_hidden_interface(JVX_INTERFACE_CONFIGURATION, reinterpret_cast<jvxHandle**>(&theConfigHandle));
				if((resL == JVX_NO_ERROR) && theConfigHandle)
				{
					theConfigHandle->put_configuration(callConf, processor, datSection, filename, lineno);
					lock_settings();
					this->rebuildPropertiesSubDevice_noLock();
					unlock_settings();
				}
			}
		}

        //_report_text_message(("1) Input channel selection flags: " + jvx_int2String(CjvxAudioDevice_genpcg::properties_active.inputchannelselection.value.selection)).c_str(),JVX_REPORT_PRIORITY_INFO);

		// The channel selection may break during load of connected device. We stored it earlier and use it now
		CjvxAudioDevice_genpcg::properties_active.inputchannelselection.value = sel_in;
		CjvxAudioDevice_genpcg::properties_active.outputchannelselection.value = sel_out;

		this->lock_settings();
		this->rearrangeChannelMapper_noLock();
		this->updateChannelExposure_nolock();
		updateChannelInternal_nolock();
		pass_channelsetup_nolock();
		this->updateSWSamplerateAndBuffersize_nolock(NULL, NULL JVX_CONNECTION_FEEDBACK_CALL_A_NULL);// ALlow default config
		this->unlock_settings();

		this->updateDependentVariables_lock(-1, JVX_PROPERTY_CATEGORY_PREDEFINED, true);

		// Last step: read in values
		jvxConfigData* secInputGains = nullptr;
		resL = processor->getReferenceEntryCurrentSection_name(sectionToContainAllSubsectionsForMe, &secInputGains, "JVX_AUDIO_INPUT_GAINS");
		if(secInputGains)
		{
			for (jvxSize i = 0; i < runtime.channelMappings.inputChannelMapper.size(); i++)
			{
				jvxConfigData* secInputOneGain = nullptr;
				resL = processor->getReferenceEntryCurrentSection_name(secInputGains, &secInputOneGain, ("JVX_GAIN_" + jvx_size2String(i)).c_str());
				if (secInputOneGain)
				{
					jvxValue val;
					resL = processor->getAssignmentValue(secInputOneGain, &val);
					val.toContent(&runtime.channelMappings.inputChannelMapper[i].gain);
				}
			}
		}

		jvxConfigData* secOutputGains = nullptr;
		resL = processor->getReferenceEntryCurrentSection_name(sectionToContainAllSubsectionsForMe, &secOutputGains, "JVX_AUDIO_OUTPUT_GAINS");
		if (secOutputGains)
		{
			for (jvxSize i = 0; i < runtime.channelMappings.outputChannelMapper.size(); i++)
			{
				jvxConfigData* secOutputOneGain = nullptr;
				resL = processor->getReferenceEntryCurrentSection_name(secOutputGains, &secOutputOneGain, ("JVX_GAIN_" + jvx_size2String(i)).c_str());
				if (secOutputOneGain)
				{
					jvxValue val;
					resL = processor->getAssignmentValue(secOutputOneGain, &val);
					val.toContent(&runtime.channelMappings.outputChannelMapper[i].gain);
				}
			}
		}
	}
	return(res);
}

jvxErrorType
CjvxGenericWrapperDevice::get_configuration(jvxCallManagerConfiguration* callConf, IjvxConfigProcessor* processor,
	jvxHandle* sectionWhereToAddAllSubsections)
{
	std::vector<std::string> warning;
	jvxErrorType resL = JVX_NO_ERROR;
	jvxHandle* datSection = NULL;
	IjvxConfiguration* theConfigHandle = NULL;
	jvxErrorType res = JVX_MY_BASE_CLASS_D::get_configuration(callConf, processor, sectionWhereToAddAllSubsections);
	if(this->_common_set_min.theState >= JVX_STATE_ACTIVE)
	{
        //_report_text_message(("3) Input channel selection flags: " + jvx_int2String(CjvxAudioDevice_genpcg::properties_active.inputchannelselection.value.selection)).c_str(),JVX_REPORT_PRIORITY_WARNING);

		genGenericWrapper_device::get_configuration__properties_active_higher(callConf, processor, sectionWhereToAddAllSubsections);
		genGenericWrapper_device::get_configuration__properties_active(callConf, processor, sectionWhereToAddAllSubsections);
		if(onInit.connectedDevice)
		{
			resL = onInit.connectedDevice->request_hidden_interface(JVX_INTERFACE_CONFIGURATION, reinterpret_cast<jvxHandle**>(&theConfigHandle));
			if((resL == JVX_NO_ERROR) && theConfigHandle)
			{
				resL = processor->createEmptySection(&datSection, JVX_GENERIC_WRAPPER_CONFIG_SUBSECTION_DEVICE);
				if((resL == JVX_NO_ERROR) && datSection)
				{
					resL = theConfigHandle->get_configuration(callConf, processor, datSection);
					if(resL == JVX_NO_ERROR)
					{
						processor->addSubsectionToSection(sectionWhereToAddAllSubsections, datSection);
					}
				}
			}
		}

		jvxConfigData* secInputGains = nullptr;
		res = processor->createEmptySection(&secInputGains, "JVX_AUDIO_INPUT_GAINS");
		for (jvxSize i = 0; i < runtime.channelMappings.inputChannelMapper.size(); i++)
		{
			jvxConfigData* secInputOneGain = nullptr;
			res = processor->createAssignmentValue(&secInputOneGain, ("JVX_GAIN_" + jvx_size2String(i)).c_str(),
				runtime.channelMappings.inputChannelMapper[i].gain);
			processor->addSubsectionToSection(secInputGains, secInputOneGain);
		}
		processor->addSubsectionToSection(sectionWhereToAddAllSubsections, secInputGains);

		jvxConfigData* secOutputGains = nullptr;
		res = processor->createEmptySection(&secOutputGains, "JVX_AUDIO_OUTPUT_GAINS");
		for (jvxSize i = 0; i < runtime.channelMappings.outputChannelMapper.size(); i++)
		{
			jvxConfigData* secOutputOneGain = nullptr;
			res = processor->createAssignmentValue(&secOutputOneGain, ("JVX_GAIN_" + jvx_size2String(i)).c_str(),
				runtime.channelMappings.outputChannelMapper[i].gain);
			processor->addSubsectionToSection(secOutputGains, secOutputOneGain);
		}
		processor->addSubsectionToSection(sectionWhereToAddAllSubsections, secOutputGains);
	}
	return(res);
}
