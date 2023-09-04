#include "CjvxAppHost.h"
#include "CjvxAppHost_config.h"

#include "jvx-helpers.h"

#define REPORT_ERROR(a, b) if(report) {report->report_simple_text_message(a,b);}

#include "templates/jvxTemplates_configuration.h"

// =================================================================
// =================================================================
// =================================================================
// =================================================================

jvxErrorType
CjvxAppHost::put_configuration(jvxCallManagerConfiguration* callConf,
	IjvxConfigProcessor* processor,
	jvxHandle* sectionToContainAllSubsectionsForMe, const char* filename, jvxInt32 lineno)
{
	jvxConfigData* datTmp = NULL, *datTmpLoc = NULL, *datTmpOut = NULL;
	jvxApiValueList datLst;
	IjvxConfiguration* config_local = NULL;
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize targetStateIdx = 0;
	jvxApiString fldStr;
	jvxApiString fldStrL1;
	jvxApiString fldStrL2;
	jvxInt32 valI32 = 0;
	std::string entry;
	std::string desc;
	jvxBool errorDetected = false;
	jvxInt16 selectme = -1;
	jvxSize num = 0, numL1, numL2;
	jvxSize i,j,k;
	jvxValue valD;
	jvxConfigSectionTypes tp = JVX_CONFIG_SECTION_TYPE_UNKNOWN;
	jvxSize h;
	jvxSize slotid;jvxSize slotsubid;
	std::string slottoken;
	jvxSize numSlots = 1;
	jvxValue val;
	jvxConfigData* theSlotNum = NULL;
	IjvxBootSteps* theBootSteps = NULL;

	// First, deactivate all components which have been previously selected
	auto elm = jvx_findItemSelectorInList_one<oneInterface, jvxInterfaceType>(_common_set_host.externalInterfaces, JVX_INTERFACE_BOOT_STEPS, 0);
	if (elm != _common_set_host.externalInterfaces.end())
	{
		theBootSteps = reinterpret_cast<IjvxBootSteps*>(elm->theHdl);
	}
	if (callConf->configModeFlags & JVX_CONFIG_MODE_FULL)
	{
		// A little bit of complicated since the list of slots is resized on every unselect!!
		unselectAllComponents();
	}
	if(processor && sectionToContainAllSubsectionsForMe)
	{
		// Backward reference for additional operations
		if (theBootSteps)
		{
			theBootSteps->on_components_before_configure();
		}

		if (callConf->configModeFlags & JVX_CONFIG_MODE_FULL)
		{
			std::vector<std::string> warnMess;
			// Store sequencer data
			jvx_put_configuration_dataproc_rules(callConf, processor, sectionToContainAllSubsectionsForMe, filename, lineno,
				static_cast<IjvxDataConnections*>(this), warnMess);

			for (i = 0; i < warnMess.size(); i++)
			{
				std::string txt = "On loading configuration file ";
				txt += filename;
				txt += ", loading data connection rules : " + warnMess[i];
				this->_common_set_host.reportUnit->report_simple_text_message(txt.c_str(), JVX_REPORT_PRIORITY_WARNING);
			}
		}

		if (theBootSteps)
		{
			theBootSteps->on_connection_rules_started();
		}

		// Get the section for this object
		processor->getReferenceEntryCurrentSection_name(sectionToContainAllSubsectionsForMe, &datTmp, SECTIONNAME_ALL_SUBSECTIONS);
		if(datTmp)
		{
			if (callConf->configModeFlags & JVX_CONFIG_MODE_FULL)
			{
				processor->getReferenceEntryCurrentSection_name(datTmp, &datTmpLoc, SECTIONNAME_ALL_EXTERNAL_ENTRIES);
				if (datTmpLoc)
				{
					externalConfigEntries.clear();
					processor->getNumberEntriesCurrentSection(datTmpLoc, &numL1);
					for (i = 0; i < numL1; i++)
					{
						datTmpOut = NULL;
						processor->getReferenceEntryCurrentSection_id(datTmpLoc, &datTmpOut, i);
						if (datTmpOut)
						{
							tp = JVX_CONFIG_SECTION_TYPE_UNKNOWN;
							processor->getTypeCurrentEntry(datTmpOut, &tp);
							processor->getNameCurrentEntry(datTmpOut, &fldStrL1);

							switch (tp)
							{
							case JVX_CONFIG_SECTION_TYPE_ASSIGNMENTSTRING:

								processor->getAssignmentString(datTmpOut, &fldStrL2);

								this->set_configuration_entry(fldStrL1.c_str(), (jvxHandle*)&fldStrL2, JVX_CONFIG_SECTION_TYPE_ASSIGNMENTSTRING, -1);
								break;
							case JVX_CONFIG_SECTION_TYPE_ASSIGNMENTVALUE:
								processor->getAssignmentValue(datTmpOut, &valD);
								this->set_configuration_entry(fldStrL1.c_str(), (jvxHandle*)&valD, JVX_CONFIG_SECTION_TYPE_ASSIGNMENTVALUE, -1);
								break;
							case JVX_CONFIG_SECTION_TYPE_STRINGLIST:
							{
								oneExternalConfigEntry newEntry;
								newEntry.tp = JVX_CONFIG_SECTION_TYPE_STRINGLIST;
								newEntry.selector = fldStrL1.std_str();
								processor->getNumberStrings(datTmpOut, &numL2);
								for (j = 0; j < numL2; j++)
								{
									processor->getString_id(datTmpOut, &fldStrL2, j);
									newEntry.assignmentStringList.push_back(fldStrL2.std_str());
								}
								externalConfigEntries.push_back(newEntry);
							}
							break;
							case JVX_CONFIG_SECTION_TYPE_VALUELIST:
								processor->getNumberValueLists(datTmpOut, &numL2);
								for (j = 0; j < numL2; j++)
								{
									processor->getValueList_id(datTmpOut, &datLst, j);
									this->set_configuration_entry(fldStrL1.c_str(), (jvxHandle*)&datLst, JVX_CONFIG_SECTION_TYPE_VALUELIST, (jvxInt32)j);
								}
								break;
							}

						}
					}
				}// if(datTmpLoc)

				datTmpLoc = nullptr;
				processor->getReferenceEntryCurrentSection_name(datTmp, &datTmpLoc, SECTIONNAME_ALL_EXTERNAL_MODULE_ENTRIES);
				if (datTmpLoc)
				{
					jvxSize numModEntries = 0;
					processor->getNumberSubsectionsCurrentSection(datTmpLoc, &numModEntries);
					for (i = 0; i < numModEntries; i++)
					{
						jvxConfigData* datTmpOut = nullptr;
						processor->getReferenceSubsectionCurrentSection_id(datTmpLoc, &datTmpOut, i);
						if (datTmpOut)
						{
							jvxApiString modAstr;
							jvxApiString cfgStr;
							jvxApiString fNameAstr;
							int lineNoCfg = 0;
							processor->getNameCurrentEntry(datTmpOut, &modAstr);
							processor->printConfiguration(datTmpOut, &cfgStr, true);
							processor->getOriginSection(datTmpLoc, &fNameAstr, &lineNoCfg);

							oneDynExtCfg newElm;
							newElm.cfgString = cfgStr.std_str();
							newElm.moduleName = modAstr.std_str();
							newElm.fName = fNameAstr.std_str();
							newElm.lineNo = lineNoCfg;
							auto exElm = extModulesConfigs.find(newElm.moduleName);
							if (exElm == extModulesConfigs.end())
							{
								extModulesConfigs[newElm.moduleName] = newElm;
							}
							else
							{
								std::cout << "Warning: Module <" << newElm.moduleName << "> stored multiple times in configuration for external modules." << std::endl;
							}
						}
					}
				}
			}

			if (jvx_bitTest(CjvxHost_genpcg::properties_selected.configure_parts.value.selection(), 0))
			{
				// Initial assumption: All modules in unselected state
				for (i = 0; i < _common_set_types.registeredTechnologyTypes.size(); i++)
				{
					numSlots = 1;

					processor->getReferenceEntryCurrentSection_name(datTmp, &theSlotNum,
						(SECTIONNAME_NUMSLOTS_TECHNOLOGY_PREFIX + _common_set_types.registeredTechnologyTypes[i].tokenInConfigFile).c_str());

					if (theSlotNum)
					{
						processor->getAssignmentValue(theSlotNum, &val);
						val.toContent(&numSlots);
					}
					
					slottoken = "";
					for (h = 0; h < numSlots; h++)
					{
						if (numSlots > 1)
						{
							// Default case for multiple slots: add a slot postfix 
							slottoken = "_slot_" + jvx_size2String(h);
						}
						slotid = h;
						slotsubid = 0;
						jvxComponentIdentification idTp(_common_set_types.registeredTechnologyTypes[i].selector[0], slotid, slotsubid);
						put_configuration_technology<technologiesT, std::vector<oneTechnology> >(callConf,
							_common_set_types.registeredTechnologyTypes[i].technologyInstances,
							_common_set_types.registeredTechnologyTypes[i].technologyInstances.availableTechnologies,
							processor, datTmp, filename, lineno,
							SECTIONNAME_STATE_TECHNOLOGY_PREFIX + _common_set_types.registeredTechnologyTypes[i].tokenInConfigFile + slottoken,
							SECTIONNAME_STATE_DEVICE_PREFIX + _common_set_types.registeredTechnologyTypes[i].tokenInConfigFile + slottoken,
							SECTIONNAME_NAME_TECHNOLOGY_PREFIX + _common_set_types.registeredTechnologyTypes[i].tokenInConfigFile + slottoken,
							SECTIONNAME_NAME_DEVICE_PREFIX + _common_set_types.registeredTechnologyTypes[i].tokenInConfigFile + slottoken,
							SECTIONNAME_SUBSECTION_TECHNOLOGY_PREFIX + _common_set_types.registeredTechnologyTypes[i].tokenInConfigFile + slottoken,
							SECTIONNAME_SUBSECTION_DEVICE_PREFIX + _common_set_types.registeredTechnologyTypes[i].tokenInConfigFile + slottoken,
							idTp,
							_common_set_types.registeredTechnologyTypes[i].selector[1],
							"[" + _common_set_types.registeredTechnologyTypes[i].description + "]<technology>",
							"[" + _common_set_types.registeredTechnologyTypes[i].description + "]<device>",
							this->_common_set_host.reportUnit, this->_common_set_host.reportOnConfig, static_cast<IjvxHost*>(this));
					}
				}
			}

			if (jvx_bitTest(CjvxHost_genpcg::properties_selected.configure_parts.value.selection(), 1))
			{
				for (i = 0; i < _common_set_types.registeredNodeTypes.size(); i++)
				{
					numSlots = 1;

					processor->getReferenceEntryCurrentSection_name(datTmp, &theSlotNum,
						(SECTIONNAME_NUMSLOTS_ENDPOINT_PREFIX + _common_set_types.registeredNodeTypes[i].tokenInConfigFile).c_str());

					if (theSlotNum)
					{
						processor->getAssignmentValue(theSlotNum, &val);
						val.toContent(&numSlots);
					}
						
					slottoken = "";
					for (h = 0; h < numSlots; h++)
					{
						if (numSlots > 1)
						{
							// Default case for multiple slots: add a slot postfix 
							slottoken = "_slot_" + jvx_size2String(h);
						}
						slotid = h;
						slotsubid = 0;
						jvxComponentIdentification idTp(_common_set_types.registeredNodeTypes[i].selector[0], slotid, slotsubid);
						jvxCBitField whatToDo = 0;
						jvxSize l;
						for (l = 0; l < JVX_PUT_CONFIGURATION_COMPONENT_NUMBER_ITERATIONS_NODES; l++)
						{
							jvx_bitZSet(whatToDo, l);
							put_configuration_component_node<objT<IjvxNode>, std::vector<oneObj<IjvxNode>> >(callConf,
								_common_set_types.registeredNodeTypes[i].instances,
								_common_set_types.registeredNodeTypes[i].instances.availableEndpoints,
								processor, datTmp, filename, lineno,
								SECTIONNAME_STATE_ENDPOINT_PREFIX + _common_set_types.registeredNodeTypes[i].tokenInConfigFile + slottoken,
								SECTIONNAME_NAME_ENDPOINT_PREFIX + _common_set_types.registeredNodeTypes[i].tokenInConfigFile + slottoken,
								SECTIONNAME_SUBSECTION_ENDPOINT_PREFIX + _common_set_types.registeredNodeTypes[i].tokenInConfigFile + slottoken,
								idTp,
								"[" + _common_set_types.registeredNodeTypes[i].description + "]<endpoint>",
								this->_common_set_host.reportUnit, this->_common_set_host.reportOnConfig, static_cast<IjvxHost*>(this),
								whatToDo);
						}
					}
				}
			}

			/*
			put_configuration_technology<technologiesT, std::vector<oneTechnology>>(_common_set_host.scannerTechnologies,
				_common_set_host.scannerTechnologies.availableTechnologies, processor,
				datTmp, filename, lineno, SECTIONNAME_STATE_SCANNER_TECHNOLOGY, SECTIONNAME_STATE_SCANNER_DEVICE,
				SECTIONNAME_NAME_SCANNER_TECHNOLOGY, SECTIONNAME_NAME_SCANNER_DEVICE,
				SECTIONNAME_SUBSECTION_SCANNER_TECHNOLOGY, SECTIONNAME_SUBSECTION_SCANNER_DEVICE,
				JVX_COMPONENT_SCANNER_TECHNOLOGY, JVX_COMPONENT_SCANNER_DEVICE, "scanner technology", "scanner device",
				this->_common_set_host.reportUnit, static_cast<IjvxHost*>(this));

			put_configuration_technology<technologiesT, std::vector<oneTechnology>>(_common_set_host.spectrumProcessorTechnologies,
				_common_set_host.spectrumProcessorTechnologies.availableTechnologies, processor,
				datTmp, filename, lineno, SECTIONNAME_STATE_SPECPROC_TECHNOLOGY, SECTIONNAME_STATE_SPECPROC_DEVICE,
				SECTIONNAME_NAME_SPECPROC_TECHNOLOGY, SECTIONNAME_NAME_SPECPROC_DEVICE,
				SECTIONNAME_SUBSECTION_SPECPROC_TECHNOLOGY, SECTIONNAME_SUBSECTION_SPECPROC_DEVICE,
				JVX_COMPONENT_SPECTRUM_PROCESSOR_TECHNOLOGY, JVX_COMPONENT_SPECTRUM_PROCESSOR_DEVICE, "spectrum processor technology", "spectrum processor device",
				this->_common_set_host.reportUnit, static_cast<IjvxHost*>(this));

			put_configuration_technology<technologiesT, std::vector<oneTechnology>>(_common_set_host.lampTechnologies,
				_common_set_host.lampTechnologies.availableTechnologies, processor,
				datTmp, filename, lineno, SECTIONNAME_STATE_LAMP_TECHNOLOGY, SECTIONNAME_STATE_LAMP_DEVICE,
				SECTIONNAME_NAME_LAMP_TECHNOLOGY, SECTIONNAME_NAME_LAMP_DEVICE,
				SECTIONNAME_SUBSECTION_LAMP_TECHNOLOGY, SECTIONNAME_SUBSECTION_LAMP_DEVICE,
				JVX_COMPONENT_LAMP_TECHNOLOGY, JVX_COMPONENT_LAMP_DEVICE, "lamp technology", "lamp device",
				this->_common_set_host.reportUnit, static_cast<IjvxHost*>(this));

			put_configuration_technology<technologiesT, std::vector<oneTechnology>>(_common_set_host.cameraTechnologies,
				_common_set_host.cameraTechnologies.availableTechnologies, processor,
				datTmp, filename, lineno, SECTIONNAME_STATE_CAMERA_TECHNOLOGY, SECTIONNAME_STATE_CAMERA_DEVICE,
				SECTIONNAME_NAME_CAMERA_TECHNOLOGY, SECTIONNAME_NAME_CAMERA_DEVICE,
				SECTIONNAME_SUBSECTION_CAMERA_TECHNOLOGY, SECTIONNAME_SUBSECTION_CAMERA_DEVICE,
				JVX_COMPONENT_CAMERA_TECHNOLOGY, JVX_COMPONENT_CAMERA_DEVICE, "camera technology", "camera device",
				this->_common_set_host.reportUnit, static_cast<IjvxHost*>(this));

			// Components
			put_configuration_component<nodesT, std::vector<oneNode>>(_common_set_host.ascanEndpoints,
				_common_set_host.ascanEndpoints.availableEndpoints, processor,
				datTmp, filename, lineno, SECTIONNAME_STATE_ASCANENDPOINT, SECTIONNAME_STATE_ASCANENDPOINT,
				SECTIONNAME_SUBSECTION_ASCANENDPOINT,
				JVX_COMPONENT_ASCAN_ENDPOINT, "ascan endpoint",
				this->_common_set_host.reportUnit, static_cast<IjvxHost*>(this));
			*/

			for (i = 0; i < externalConfigHooks.size(); i++)
			{
				processor->getReferenceEntryCurrentSection_name(datTmp, &datTmpLoc, externalConfigHooks[i].selector.c_str());
				if (datTmpLoc)
				{
					if (externalConfigHooks[i].ref)
					{
						processor->getOriginSection(datTmpLoc, &fldStr, &valI32);
						externalConfigHooks[i].ref->put_configuration_ext(callConf, processor, datTmpLoc, fldStr.c_str(), valI32);
					}
				}
			}

			// =======================================================================
			// Store all registered submodules
			// =======================================================================
			for (auto& elm : registeredConfigSubmodules)
			{
				std::string sectionName = jvx_makePathExpr("submodules", elm.prefix);
				processor->getReferenceEntryCurrentSection_name(sectionToContainAllSubsectionsForMe, &datTmp, sectionName.c_str());
				if (datTmp)
				{
					elm.cfgRef->put_configuration(callConf, processor, datTmp, filename, lineno);
				}
			}
		} // if(datTmp)

		// Backward reference for additional operations
		if (theBootSteps)
		{
			theBootSteps->on_components_configured();
		}
		

		if (callConf->configModeFlags & JVX_CONFIG_MODE_FULL)
		{
			// Store sequencer data
			this->_put_configuration_seq(callConf, processor, sectionToContainAllSubsectionsForMe, filename, lineno);
		}
		
		if (theBootSteps)
		{
			theBootSteps->on_sequencer_started();
		}

		if (callConf->configModeFlags & JVX_CONFIG_MODE_FULL)
		{
			std::vector<std::string> warnMess;
			// Store sequencer data
			jvx_put_configuration_dataproc(callConf, processor, sectionToContainAllSubsectionsForMe, filename, lineno, 
				static_cast<IjvxDataConnections*>(this), warnMess);

			for (i = 0; i < warnMess.size(); i++)
			{
				std::string txt = "On loading configuration file ";
				txt += filename;
				txt += ", loading data connections : " + warnMess[i];
				this->_common_set_host.reportUnit->report_simple_text_message(txt.c_str(), JVX_REPORT_PRIORITY_WARNING);
			}
		}

		if (theBootSteps)
		{
			theBootSteps->on_connections_started();
		}		
		
	}// if(processor && sectionToContainAllSubsectionsForMe)
	else
	{
		res = JVX_ERROR_INVALID_ARGUMENT;
	}
	return(res);
}

jvxErrorType
CjvxAppHost::get_configuration(jvxCallManagerConfiguration* callConf,
	IjvxConfigProcessor* processor, jvxHandle* sectionWhereToAddAllSubsections)
{
	jvxSize i,j,k,h;
	jvxConfigData* datTmp = NULL;
	jvxConfigData* datTmp_add = NULL;
	IjvxConfiguration* config_local = NULL;
	jvxApiString fldStr;
	jvxBool errorDetected = false;
	std::string entry;
	jvxErrorType res = JVX_NO_ERROR;
	jvxData valD = 0;
	jvxConfigData* datTmpLoc = NULL, *datAdd = NULL;
	std::string slottoken;
	jvxConfigData* theSlotNum = NULL;
	jvxSize numSlots = 0;
	if(processor && sectionWhereToAddAllSubsections)
	{

		processor->createEmptySection(&datTmp, SECTIONNAME_ALL_SUBSECTIONS);

		if (jvx_bitTest(CjvxHost_genpcg::properties_selected.configure_parts.value.selection(), 0))
		{
			for (i = 0; i < _common_set_types.registeredTechnologyTypes.size(); i++)
			{
				numSlots = _common_set_types.registeredTechnologyTypes[i].technologyInstances.selTech.size();
				if (numSlots > 1)
				{
					//Write te config section specifically for the slots
					processor->createAssignmentValue(&theSlotNum,
						(SECTIONNAME_NUMSLOTS_TECHNOLOGY_PREFIX + _common_set_types.registeredTechnologyTypes[i].tokenInConfigFile).c_str(),
						numSlots);
					processor->addSubsectionToSection(datTmp, theSlotNum);
				}

				slottoken = "";
				for (h = 0; h < numSlots; h++)
				{
					slottoken = "";
					if (numSlots > 1)
					{
						slottoken = "_slot_" + jvx_size2String(h);
					}
					get_configuration_technology<technologiesT>(callConf, 
						_common_set_types.registeredTechnologyTypes[i].technologyInstances,
						processor, datTmp,
						SECTIONNAME_STATE_TECHNOLOGY_PREFIX + _common_set_types.registeredTechnologyTypes[i].tokenInConfigFile + slottoken,
						SECTIONNAME_STATE_DEVICE_PREFIX + _common_set_types.registeredTechnologyTypes[i].tokenInConfigFile + slottoken,
						SECTIONNAME_NAME_TECHNOLOGY_PREFIX + _common_set_types.registeredTechnologyTypes[i].tokenInConfigFile + slottoken,
						SECTIONNAME_NAME_DEVICE_PREFIX + _common_set_types.registeredTechnologyTypes[i].tokenInConfigFile + slottoken,
						SECTIONNAME_SUBSECTION_TECHNOLOGY_PREFIX + _common_set_types.registeredTechnologyTypes[i].tokenInConfigFile + slottoken,
						SECTIONNAME_SUBSECTION_DEVICE_PREFIX + _common_set_types.registeredTechnologyTypes[i].tokenInConfigFile + slottoken,
						h);
				}
			}
		}

		if (jvx_bitTest(CjvxHost_genpcg::properties_selected.configure_parts.value.selection(), 1))
		{
			for (i = 0; i < _common_set_types.registeredNodeTypes.size(); i++)
			{
				numSlots = _common_set_types.registeredNodeTypes[i].instances.theHandle_shortcut.size();
				if (numSlots > 1)
				{
					//Write te config section specifically for the slots
					processor->createAssignmentValue(&theSlotNum,
						(SECTIONNAME_NUMSLOTS_ENDPOINT_PREFIX + _common_set_types.registeredNodeTypes[i].tokenInConfigFile).c_str(),
						numSlots);
					processor->addSubsectionToSection(datTmp, theSlotNum);
				}
				for (h = 0; h < _common_set_types.registeredNodeTypes[i].instances.theHandle_shortcut.size(); h++)
				{
					slottoken = "";
					if (numSlots > 1)
					{
						slottoken = "_slot_" + jvx_size2String(h);
					}
					// Components
					get_configuration_component_node<objT<IjvxNode>>(callConf,
						_common_set_types.registeredNodeTypes[i].instances,
						processor,
						datTmp,
						SECTIONNAME_STATE_ENDPOINT_PREFIX + _common_set_types.registeredNodeTypes[i].tokenInConfigFile + slottoken,
						SECTIONNAME_NAME_ENDPOINT_PREFIX + _common_set_types.registeredNodeTypes[i].tokenInConfigFile + slottoken,
						SECTIONNAME_SUBSECTION_ENDPOINT_PREFIX + _common_set_types.registeredNodeTypes[i].tokenInConfigFile + slottoken,
						h);
				}
			}
		}
/*		// Technologies
		get_configuration_technology<technologiesT>(_common_set_host.scannerTechnologies, processor,
			datTmp, SECTIONNAME_STATE_SCANNER_TECHNOLOGY, SECTIONNAME_STATE_SCANNER_DEVICE,
			SECTIONNAME_NAME_SCANNER_TECHNOLOGY, SECTIONNAME_NAME_SCANNER_DEVICE,
			SECTIONNAME_SUBSECTION_SCANNER_TECHNOLOGY, SECTIONNAME_SUBSECTION_SCANNER_DEVICE);

		get_configuration_technology<technologiesT>(_common_set_host.spectrumProcessorTechnologies, processor,
			datTmp, SECTIONNAME_STATE_SPECPROC_TECHNOLOGY, SECTIONNAME_STATE_SPECPROC_DEVICE,
			SECTIONNAME_NAME_SPECPROC_TECHNOLOGY, SECTIONNAME_NAME_SPECPROC_DEVICE,
			SECTIONNAME_SUBSECTION_SPECPROC_TECHNOLOGY, SECTIONNAME_SUBSECTION_SPECPROC_DEVICE);

		get_configuration_technology<technologiesT>(_common_set_host.lampTechnologies, processor,
			datTmp, SECTIONNAME_STATE_LAMP_TECHNOLOGY, SECTIONNAME_STATE_LAMP_DEVICE,
			SECTIONNAME_NAME_LAMP_TECHNOLOGY, SECTIONNAME_NAME_LAMP_DEVICE,
			SECTIONNAME_SUBSECTION_LAMP_TECHNOLOGY, SECTIONNAME_SUBSECTION_LAMP_DEVICE);

		get_configuration_technology<technologiesT>(_common_set_host.cameraTechnologies, processor,
			datTmp, SECTIONNAME_STATE_CAMERA_TECHNOLOGY, SECTIONNAME_STATE_CAMERA_DEVICE,
			SECTIONNAME_NAME_CAMERA_TECHNOLOGY, SECTIONNAME_NAME_CAMERA_DEVICE,
			SECTIONNAME_SUBSECTION_CAMERA_TECHNOLOGY, SECTIONNAME_SUBSECTION_CAMERA_DEVICE);

		// Components
		get_configuration_component<nodesT>(_common_set_host.ascanEndpoints, processor,
			datTmp, SECTIONNAME_STATE_ASCANENDPOINT,
			SECTIONNAME_NAME_ASCANENDPOINT,
			SECTIONNAME_SUBSECTION_ASCANENDPOINT);
*/
		if (callConf->configModeFlags & JVX_CONFIG_MODE_FULL)
		{
			if (externalConfigEntries.size())
			{
				processor->createEmptySection(&datTmpLoc, SECTIONNAME_ALL_EXTERNAL_ENTRIES);
				if (datTmpLoc)
				{
					for (i = 0; i < externalConfigEntries.size(); i++)
					{
						datAdd = NULL;
						switch (externalConfigEntries[i].tp)
						{
						case JVX_CONFIG_SECTION_TYPE_ASSIGNMENTSTRING:
							processor->createAssignmentString(&datAdd, externalConfigEntries[i].selector.c_str(),
								externalConfigEntries[i].assignmentString.c_str());
							break;

						case JVX_CONFIG_SECTION_TYPE_ASSIGNMENTVALUE:
							processor->createAssignmentValue(&datAdd, externalConfigEntries[i].selector.c_str(),
								externalConfigEntries[i].assignmentValue);
							break;
						case JVX_CONFIG_SECTION_TYPE_STRINGLIST:
							processor->createAssignmentStringList(&datAdd, externalConfigEntries[i].selector.c_str());

							for (j = 0; j < externalConfigEntries[i].assignmentStringList.size(); j++)
							{
								processor->addAssignmentStringToList(datAdd, externalConfigEntries[i].assignmentStringList[j].c_str());
							}
							break;
						case JVX_CONFIG_SECTION_TYPE_VALUELIST:
							processor->createAssignmentValueList(&datAdd, externalConfigEntries[i].selector.c_str());
							for (j = 0; j < externalConfigEntries[i].assignmentValueMatrix.size(); j++)
							{
								for (k = 0; k < externalConfigEntries[i].assignmentValueMatrix[j].size(); k++)
								{
									processor->addAssignmentValueToList(datAdd, JVX_SIZE_INT(j), externalConfigEntries[i].assignmentValueMatrix[j][k]);
								}
							}
							break;
						}
						if (datAdd)
						{
							processor->addSubsectionToSection(datTmpLoc, datAdd);
						}
					}
					processor->addSubsectionToSection(datTmp, datTmpLoc);
				}
			}
		
			if (extModuleDefinitions.size())
			{
				datTmpLoc = nullptr;
				datAdd = nullptr;
				jvxConfigData* datAddAdd = nullptr;
				processor->createEmptySection(&datTmpLoc, SECTIONNAME_ALL_EXTERNAL_MODULE_ENTRIES);
				for (auto& elm : extModuleDefinitions)
				{
					processor->createEmptySection(&datAdd, elm.second.moduleName.c_str());
					for (auto& elmI : elm.second.associatedExternalComponents)
					{			
						jvxApiString modAstr;
						elmI->module_reference(&modAstr, nullptr);
						auto ifElm = reqInterfaceObj<IjvxConfiguration>(elmI);
						if(ifElm)
						{
							processor->createEmptySection(&datTmp_add, modAstr.c_str());
							ifElm->get_configuration(callConf, processor, datTmp_add);
						}
						else
						{
							std::string commStr = "No configuration for module reference <" + modAstr.std_str();
							commStr += ">.";
							processor->createComment(&datTmp_add, commStr.c_str());
						}
						processor->addSubsectionToSection(datAdd, datTmp_add);
						retInterfaceObj<IjvxConfiguration>(elmI, ifElm);
					}
					processor->addSubsectionToSection(datTmpLoc, datAdd);
				}
				processor->addSubsectionToSection(datTmp, datTmpLoc);
			}
		}

		// Add config sections by hook callbacks
		for (i = 0; i < externalConfigHooks.size(); i++)
		{
			processor->createEmptySection(&datTmpLoc, externalConfigHooks[i].selector.c_str());
			if (externalConfigHooks[i].ref)
			{
				externalConfigHooks[i].ref->get_configuration_ext(callConf, processor, datTmpLoc);
			}
			processor->addSubsectionToSection(datTmp, datTmpLoc);
		}

		processor->addSubsectionToSection(sectionWhereToAddAllSubsections, datTmp);

		// =======================================================================
		// Store all registered submodules
		// =======================================================================
		for (auto& elm : registeredConfigSubmodules)
		{
			std::string sectionName = jvx_makePathExpr("submodules", elm.prefix);
			processor->createEmptySection(&datTmp, sectionName.c_str()); 			
			if (datTmp)
			{
				elm.cfgRef->get_configuration(callConf, processor, datTmp);
				processor->addSubsectionToSection(sectionWhereToAddAllSubsections, datTmp);
				datTmp = nullptr;
			}
		}

		if (callConf->configModeFlags & JVX_CONFIG_MODE_FULL)
		{
			// Store sequencer data
			this->_get_configuration_seq(callConf, processor, sectionWhereToAddAllSubsections, static_cast<IjvxObject*>(this));
		}

		if (callConf->configModeFlags & JVX_CONFIG_MODE_FULL)
		{
			// Store connection rule data
			CjvxDataConnections::get_configuration_dataproc_rules(callConf, processor, sectionWhereToAddAllSubsections);
		}

		if (callConf->configModeFlags & JVX_CONFIG_MODE_FULL)
		{
			// Store connection data
			CjvxDataConnections::get_configuration_dataproc(callConf, processor, sectionWhereToAddAllSubsections);
		}
		return(JVX_NO_ERROR);
	}
	return(JVX_ERROR_INVALID_ARGUMENT);
}

jvxErrorType
CjvxAppHost::done_configuration()
{
	// =======================================================================
	// Run the post-configuration step
	auto reportDone = [&](IjvxObject* theOwner, IjvxHiddenInterface* iface)
	{
		IjvxConfigurationDone* conf = nullptr;
		if (theOwner == nullptr)
		{
			if (iface)
			{
				conf = reqInterface<IjvxConfigurationDone>(iface);
				if (conf)
				{
					conf->done_configuration();
					retInterface<IjvxConfigurationDone>(iface, conf);
				}
			}
		}
	};

	// Actually trigger it - involve the lambda from previous definition
	jvx_runAllActiveMainComponents(static_cast<IjvxHost*>(this), reportDone);
	// =======================================================================

	if (_common_set_host.reportUnit)
	{
		CjvxReportCommandRequest req_config_done(jvxReportCommandRequest::JVX_REPORT_COMMAND_REQUEST_REPORT_CONFIGURATION_COMPLETE,
			_common_set.theComponentType,
			jvxReportCommandBroadcastType::JVX_REPORT_COMMAND_BROADCAST_AUTOMATION,
			nullptr,
			true);
		_common_set_host.reportUnit->request_command(req_config_done);
	}
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxAppHost::register_extension(IjvxConfigurationExtender_report* bwd, const char* sectionName)
{
	std::string entryStr = sectionName;
	std::vector<oneExternalConfigHook>::iterator elm = jvx_findItemSelectorInList<oneExternalConfigHook, std::string>(externalConfigHooks, entryStr);
	if (elm == externalConfigHooks.end())
	{
		oneExternalConfigHook newElm;
		newElm.selector = entryStr;
		newElm.ref = bwd;
		externalConfigHooks.push_back(newElm);
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_DUPLICATE_ENTRY;
}

jvxErrorType
CjvxAppHost::unregister_extension(const char* sectionName)
{
	std::string entryStr = sectionName;
	std::vector<oneExternalConfigHook>::iterator elm = jvx_findItemSelectorInList<oneExternalConfigHook, std::string>(externalConfigHooks, entryStr);
	if (elm != externalConfigHooks.end())
	{
		externalConfigHooks.erase(elm);
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_ELEMENT_NOT_FOUND;
}

jvxErrorType 
CjvxAppHost::clear_configuration_entries()
{
	externalConfigEntries.clear();
	return(JVX_NO_ERROR);
}

jvxErrorType 
CjvxAppHost::set_configuration_entry(const char* entryname, jvxHandle* fld, jvxConfigSectionTypes tp, jvxSize id)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxApiStringList* lstStr;
	jvxApiValueList* lstData;
	std::vector<jvxValue> newLst;
	jvxSize i;
	std::string entryStr = entryname;
	if(fld)
	{
		std::vector<oneExternalConfigEntry>::iterator elm = jvx_findItemSelectorInList<oneExternalConfigEntry, std::string>(externalConfigEntries, entryStr);
		if(elm == externalConfigEntries.end())
		{
			oneExternalConfigEntry newEntry;
			newEntry.selector = entryname;
			newEntry.tp = tp;
			switch(newEntry.tp)
			{
			case JVX_CONFIG_SECTION_TYPE_ASSIGNMENTSTRING:
				newEntry.assignmentString = ((jvxApiString*)fld)->std_str();
				break;
			case JVX_CONFIG_SECTION_TYPE_ASSIGNMENTVALUE:
				newEntry.assignmentValue = *((jvxValue*)fld);
				break;
			case JVX_CONFIG_SECTION_TYPE_STRINGLIST:
				newEntry.assignmentStringList.clear();
				lstStr = (jvxApiStringList*)fld;
				for(i = 0; i < lstStr->ll(); i++)
				{
					newEntry.assignmentStringList.push_back(lstStr->std_str_at(i));
				}
				break;
			case JVX_CONFIG_SECTION_TYPE_VALUELIST:
				if(JVX_CHECK_SIZE_UNSELECTED(id))
				{
					newEntry.assignmentValueMatrix.clear();
				}
				else
				{
					lstData = (jvxApiValueList*)fld;
					newLst.clear();
					for(i = 0; i < lstData->ll(); i++)
					{
						newLst.push_back(lstData->elm_at(i));
					}
                    
                    if(id < newEntry.assignmentValueMatrix.size())
                    {
                        newEntry.assignmentValueMatrix[id] = newLst;
                    }
                    else
                    {
                        for(i = 0; i < (id - newEntry.assignmentValueMatrix.size());i++)
                        {
                            std::vector<jvxValue> llst;
                            newEntry.assignmentValueMatrix.push_back(llst);
                        }
                        newEntry.assignmentValueMatrix.push_back(newLst);
                    }
				}
				break;
			default:
				res = JVX_ERROR_INVALID_ARGUMENT;
			}
			if(res == JVX_NO_ERROR)
			{
				externalConfigEntries.push_back(newEntry);
			}
		}
		else
		{
			elm->tp = tp;
			switch(elm->tp)
			{
			case JVX_CONFIG_SECTION_TYPE_ASSIGNMENTSTRING:
				elm->assignmentString = ((jvxApiString*)fld)->std_str();
				break;
			case JVX_CONFIG_SECTION_TYPE_ASSIGNMENTVALUE:
				elm->assignmentValue = *((jvxValue*)fld);
				break;
			case JVX_CONFIG_SECTION_TYPE_STRINGLIST:
				elm->assignmentStringList.clear();
				lstStr = (jvxApiStringList*)fld;
				for(i = 0; i < lstStr->ll(); i++)
				{
					elm->assignmentStringList.push_back(lstStr->std_str_at(i));
				}
				break;
			case JVX_CONFIG_SECTION_TYPE_VALUELIST:
				if(JVX_CHECK_SIZE_UNSELECTED(id))
				{
					elm->assignmentValueMatrix.clear();
				}
				else
				{
					lstData = (jvxApiValueList*)fld;
					newLst.clear();
					for(i = 0; i < lstData->ll(); i++)
					{
						newLst.push_back(lstData->elm_at(i));
					}	
					elm->assignmentValueMatrix[id] = newLst;
				}
				break;
			default:
				res = JVX_ERROR_INVALID_ARGUMENT;
			}
		}
	}
	else
	{
		res = JVX_ERROR_INVALID_ARGUMENT;
	}
	return(res);
}

jvxErrorType 
CjvxAppHost::get_configuration_entry(const char* entryname, jvxHandle* fld, jvxConfigSectionTypes* tp, jvxSize id)
{
	jvxErrorType res = JVX_NO_ERROR;
	std::string entryStr = entryname;
	jvxBool allowCopyTypeSafe = false;
	std::vector<oneExternalConfigEntry>::iterator elm = jvx_findItemSelectorInList<oneExternalConfigEntry, std::string>(externalConfigEntries, entryStr);
	if(elm != externalConfigEntries.end())
	{
		if(tp)
		{
			if (*tp == JVX_CONFIG_SECTION_TYPE_UNKNOWN)
			{
				*tp = elm->tp;
			}
			else
			{
				allowCopyTypeSafe = (elm->tp == *tp);
			}
		}
		if(fld)
		{
			if (allowCopyTypeSafe)
			{
				switch (elm->tp)
				{
				case JVX_CONFIG_SECTION_TYPE_ASSIGNMENTSTRING:
					((jvxApiString*)fld)->assign( elm->assignmentString);
					break;
				case JVX_CONFIG_SECTION_TYPE_ASSIGNMENTVALUE:
					*((jvxValue*)fld) = elm->assignmentValue;
					break;
				case JVX_CONFIG_SECTION_TYPE_STRINGLIST:
					((jvxApiStringList*)fld)->assign(elm->assignmentStringList);
					break;
				case JVX_CONFIG_SECTION_TYPE_VALUELIST:
					if (id < elm->assignmentValueMatrix.size())
					{
						((jvxApiValueList*)fld)->assign(elm->assignmentValueMatrix[id]);
					}
					else
					{
						res = JVX_ERROR_ID_OUT_OF_BOUNDS;
					}
					break;
				}
			}
			else
			{
				res = JVX_ERROR_INVALID_SETTING;
			}
		}
	}
	else
	{
		if(tp)
		{
			*tp = JVX_CONFIG_SECTION_TYPE_UNKNOWN;
		}
		res = JVX_ERROR_ELEMENT_NOT_FOUND;
	}
	return(res);
}


