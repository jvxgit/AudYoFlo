#ifndef __JVXTEMPLATES_CONFIGURATION_H__
#define __JVXTEMPLATES_CONFIGURATION_H__

/*
 Wrapping typedefs for technologies must be like:

typedef struct
{
	oneComponentWrap common;
	IjvxCameraTechnology* theHandle;
} oneCameraTechnology;

typedef struct
{
	std::vector<oneCameraTechnology> availableCameraTechnologies;
	IjvxCameraTechnology* selTech.theHandle_shortcut_tech;
	IjvxCameraDevice* selTech.theHandle_shortcut_dev;
} cameraTechnologiesT;

*/

template<typename T1, typename T2> static void put_configuration_technology(jvxCallManagerConfiguration* callConf,
	T1& theStruct, T2& theList,
	IjvxConfigProcessor* processor,
	jvxHandle* datTmp, 
	const char* filename, jvxInt32 lineno,
	std::string tokena,
	std::string tokenb,
	std::string token1,
	std::string token2,
	std::string tokenss1,
	std::string tokenss2,
	jvxComponentIdentification& tp1,
	jvxComponentType tp2,
	std::string cType1,
	std::string cType2,
	IjvxReport* report,
	IjvxReportOnConfig* reportOnConfig,
	IjvxHost* this_pointer)
{
	jvxSize i,h;
	IjvxConfiguration* config_local = NULL;
	jvxSize valS;
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize targetStateIdx = 0;
	jvxApiString strL;
	std::string entry;
	std::string desc;
	jvxBool wantToContinue = true;
	jvxSize selectme = JVX_SIZE_UNSELECTED;
	jvxSize num = 0;
	// Step I: Get the desired state from configuration
	targetStateIdx = 0;
	jvxHandle* datTmp_ss = NULL;
	jvxState theStat = JVX_STATE_NONE;
	jvxBool unselectTechnology = false;
	jvxBool unselectDevice = false;
	
	if (callConf->configModeFlags & JVX_CONFIG_MODE_OVERLAY)
	{

		// If a technology has been selected before
		res = HjvxConfigProcessor_readEntry_assignmentString(processor, datTmp, token1, &entry);
		if (res == JVX_NO_ERROR)
		{
			if (tp1.slotid < theStruct.selTech.size())
			{
				if (theStruct.selTech[tp1.slotid].theHandle_shortcut_tech)
				{
					jvxApiString strL;
					theStruct.selTech[tp1.slotid].theHandle_shortcut_tech->name(&strL);
					strL.assert_valid();
					desc = strL.std_str();

					if (entry == desc)
					{
						// Correct entry, pass configuration
						// Get configuration interface and pass configuration
						theStruct.selTech[tp1.slotid].theHandle_shortcut_tech->state(&theStat);
						datTmp_ss = NULL;
						config_local = NULL;
						theStruct.selTech[tp1.slotid].theHandle_shortcut_tech->request_hidden_interface(JVX_INTERFACE_CONFIGURATION, (jvxHandle**)&config_local);

						if (config_local)
						{
							res = processor->getReferenceSubsectionCurrentSection_name(datTmp, &datTmp_ss, tokenss1.c_str());
							if ((res == JVX_NO_ERROR) && datTmp_ss)
							{
								res = config_local->put_configuration(callConf, processor, datTmp_ss, filename, lineno);
								if (res != JVX_NO_ERROR)
								{
									if (res == JVX_ERROR_ELEMENT_NOT_FOUND)
									{
										REPORT_ERROR(((std::string)"--> One or more configuration entries not found in object type " + cType1 + " <" + entry + "> - state selected - in config file.").c_str(), JVX_REPORT_PRIORITY_WARNING);
									}
									else
									{
										REPORT_ERROR(((std::string)"Failed to read configuration in object type " + cType1 + " <" + entry + "> - state selected - in config file.").c_str(), JVX_REPORT_PRIORITY_WARNING);
									}
								}
							}
							else
							{
								REPORT_ERROR(((std::string)"Failed to read entry " + tokenss1 + ", object type " + cType1 + " <" + entry + ">, from config file.").c_str(), JVX_REPORT_PRIORITY_WARNING);
							}
							datTmp_ss = NULL; // It was only a reference, no deallocate required
							theStruct.selTech[tp1.slotid].theHandle_shortcut_tech->return_hidden_interface(JVX_INTERFACE_CONFIGURATION, (jvxHandle**)&config_local);
						}
					}
					else
					{
						// error
						REPORT_ERROR(((std::string)"--> Component description mismatch in object type " + cType1 + " <" + entry + "> - " + entry + " vs. " + desc + "-.").c_str(), JVX_REPORT_PRIORITY_WARNING);
					}
				}
				else
				{
					REPORT_ERROR(((std::string)"--> Component configuration mismatch in object type " + cType1 + " <" + entry + "> - No component has been selected for slot id " + jvx_size2String(tp1.slotid) + ".").c_str(), JVX_REPORT_PRIORITY_WARNING);
				}
			}
			else
			{
				REPORT_ERROR(((std::string)"--> Component configuration mismatch in object type " + cType1 + " <" + entry + "> - Slot id " + jvx_size2String(tp1.slotid) + ".").c_str(), JVX_REPORT_PRIORITY_WARNING);
			}
		}


		jvxSize numSlots = 1;
		jvxValue val;
		jvxConfigData* theSlotNum = NULL;

		processor->getReferenceEntryCurrentSection_name(datTmp, &theSlotNum, (SECTIONNAME_NUMSLOTS_DEVICE_PREFIX + token2).c_str());

		if (tp1.slotid < theStruct.selTech.size())
		{
			for (h = 0; h < numSlots; h++)
			{
				std::string myToken = token2;
				if (numSlots > 1)
				{
					myToken += "_slot_" + jvx_size2String(h);
				}
				res = HjvxConfigProcessor_readEntry_assignmentString(processor, datTmp, myToken, &entry);
				if (res == JVX_NO_ERROR)
				{
					if (h < theStruct.selTech[tp1.slotid].theHandle_shortcut_dev.size())
					{
						if (theStruct.selTech[tp1.slotid].theHandle_shortcut_dev[h].dev)
						{
							// If a technology has been selected before
							theStruct.selTech[tp1.slotid].theHandle_shortcut_dev[h].dev->name(&strL);
							desc = strL.std_str();
							if (entry == desc)
							{
								// Correct entry, pass configuration
								// Get configuration interface and pass configuration
								theStruct.selTech[tp1.slotid].theHandle_shortcut_dev[h].dev->state(&theStat);
								datTmp_ss = NULL;
								config_local = NULL;
								theStruct.selTech[tp1.slotid].theHandle_shortcut_dev[h].dev->request_hidden_interface(JVX_INTERFACE_CONFIGURATION, (jvxHandle**)&config_local);

								if (config_local)
								{
									res = processor->getReferenceSubsectionCurrentSection_name(datTmp, &datTmp_ss, tokenss2.c_str());
									if ((res == JVX_NO_ERROR) && datTmp_ss)
									{
										res = config_local->put_configuration(callConf, processor, datTmp_ss, filename, lineno);
										if (res != JVX_NO_ERROR)
										{
											if (res == JVX_ERROR_ELEMENT_NOT_FOUND)
											{
												REPORT_ERROR(((std::string)"--> One or more configuration entries not found in object type " + cType2 + " <" + entry + "> - state selected - in config file.").c_str(), JVX_REPORT_PRIORITY_WARNING);
											}
											else
											{
												REPORT_ERROR(((std::string)"Failed to read configuration in object type " + cType2 + " <" + entry + "> - state selected - in config file.").c_str(), JVX_REPORT_PRIORITY_WARNING);
											}
										}
									}
									else
									{
										REPORT_ERROR(((std::string)"Failed to read entry " + tokenss2 + ", object type " + cType2 + " <" + entry + ">, from config file.").c_str(), JVX_REPORT_PRIORITY_WARNING);
									}
									datTmp_ss = NULL; // It was only a reference, no deallocate required
									theStruct.selTech[tp1.slotid].theHandle_shortcut_dev[h].dev->return_hidden_interface(JVX_INTERFACE_CONFIGURATION, (jvxHandle**)&config_local);
								}
							}
							else
							{
								// error
								REPORT_ERROR(((std::string)"--> Component description mismatch in object type " + cType2 + " <" + entry + "> - " + entry + " vs. " + desc + "-.").c_str(), JVX_REPORT_PRIORITY_WARNING);
							}
						}
						else
						{
							REPORT_ERROR(((std::string)"--> Component configuration mismatch in object type " + cType2 + " <" + entry + "> - No component has been selected.").c_str(), JVX_REPORT_PRIORITY_WARNING);
						}
					}
					else
					{
						REPORT_ERROR(((std::string)"--> Component configuration mismatch in object type " + cType2 + " <" + entry + "> - invalid component.").c_str(), JVX_REPORT_PRIORITY_WARNING);
					}
				}
			}
		}
	}
	else
	{
		res = HjvxConfigProcessor_readEntry_assignment<jvxSize>(processor, datTmp, tokena, &valS);
		if (res == JVX_NO_ERROR)
		{
			targetStateIdx = valS;
		}
		else
		{
			//REPORT_ERROR(((std::string)"Failed to obtain entry " + token1 + " from config file " + filename + ".").c_str(), JVX_REPORT_PRIORITY_WARNING);
			wantToContinue = false;
		}

		if (wantToContinue)
		{
			// Check state to see what needs to be done.
			jvxSize idxC = jvx_stateToIndex(JVX_STATE_SELECTED);
			assert(JVX_CHECK_SIZE_SELECTED(idxC));
			if (targetStateIdx >= idxC)
			{
				// if selection is desired, go and get the name of the selected component
				res = HjvxConfigProcessor_readEntry_assignmentString(processor, datTmp, token1, &entry);
				if (res == JVX_NO_ERROR)
				{
					// Find the selection id for the desired component
					selectme = JVX_SIZE_UNSELECTED;
					for (i = 0; i < theList.size(); i++)
					{
						desc = "";
						theList[i].common.hObject->name(&strL);
						
						desc = strL.std_str();

						if (jvx_compareStringsWildcard(entry, desc))
						{
							selectme = i;
							break;
						}
					}

					// Step II: If selection is valid, select technology
					if (JVX_CHECK_SIZE_SELECTED(selectme))
					{
						res = this_pointer->select_component(tp1, selectme);
						if (res != JVX_NO_ERROR)
						{
							REPORT_ERROR(((std::string)"Unexpected error when selecting component " + cType1 + ".").c_str(), JVX_REPORT_PRIORITY_WARNING);
							wantToContinue = false;
						}
						else
						{
							if (reportOnConfig)
							{
								reportOnConfig->report_component_select_on_config(tp1);
							}
						}
					}
					else
					{
						REPORT_ERROR(((std::string)"Failed to find component with description " + entry + " on target machine.").c_str(), JVX_REPORT_PRIORITY_WARNING);
						wantToContinue = false;
					}
				}
				else
				{
					REPORT_ERROR(((std::string)"Failed to find component description entry for symbol " + token1 + " in config file.").c_str(), JVX_REPORT_PRIORITY_WARNING);
					wantToContinue = false;
				}
			}
			else
			{
				// No selection is desired
				wantToContinue = false;
			}

			// If selection was successful, next, pass config entries to component to be read in state selected. Afterwards, activate
			if (wantToContinue)
			{
				// Get configuration interface and pass configuration
				datTmp_ss = NULL;
				config_local = NULL;
				if (tp1.slotid < theStruct.selTech.size())
				{
					theStruct.selTech[tp1.slotid].theHandle_shortcut_tech->request_hidden_interface(JVX_INTERFACE_CONFIGURATION, (jvxHandle**)&config_local);

					if (config_local)
					{
						res = processor->getReferenceSubsectionCurrentSection_name(datTmp, &datTmp_ss, tokenss1.c_str());
						if ((res == JVX_NO_ERROR) && datTmp_ss)
						{
							res = config_local->put_configuration(callConf, processor, datTmp_ss, filename, lineno);
							if (res != JVX_NO_ERROR)
							{
								if (res == JVX_ERROR_ELEMENT_NOT_FOUND)
								{
									REPORT_ERROR(((std::string)"--> One or more configuration entries not found in object type " + cType1 + " <" + entry + "> - state selected - in config file.").c_str(), JVX_REPORT_PRIORITY_WARNING);
								}
								else
								{
									REPORT_ERROR(((std::string)"Failed to read configuration in object type " + cType1 + " <" + entry + "> - state selected - in config file.").c_str(), JVX_REPORT_PRIORITY_WARNING);
								}
							}
						}
						else
						{
							REPORT_ERROR(((std::string)"Failed to read entry " + tokenss1 + ", object type " + cType1 + " <" + entry + ">, from config file.").c_str(), JVX_REPORT_PRIORITY_WARNING);
							datTmp_ss = NULL;
						}
					}
					/* Do not return handle yet, we may need it lateron */

					// Step III: Check state to see what needs to be done.
					jvxSize idxC = jvx_stateToIndex(JVX_STATE_ACTIVE);
					assert(JVX_CHECK_SIZE_SELECTED(idxC));
					if (targetStateIdx >= idxC)
					{
						// Seems that activation is desired
						res = this_pointer->activate_selected_component(tp1);
						if (res != JVX_NO_ERROR)
						{
							jvxBitField myFeatureClass;
							REPORT_ERROR(((std::string)"Error when activating component " + cType1 + ".").c_str(), JVX_REPORT_PRIORITY_WARNING);
							wantToContinue = false;
							this_pointer->feature_class_selected_component(tp1, &myFeatureClass);
							if (jvx_bitTest(myFeatureClass, JVX_FEATURE_CLASS_NON_ESSENTIAL_SHIFT))
							{
								unselectTechnology = true;
							}
						}
						else
						{
							if (reportOnConfig)
							{
								reportOnConfig->report_component_active_on_config(tp1);
							}
						}
					}
					else
					{
						wantToContinue = false;
					}

					if (wantToContinue)
					{
						// Activation successful, next, configure component specific. Handle should still be "alive"
						if (config_local && datTmp_ss)
						{
							res = config_local->put_configuration(callConf, processor, datTmp_ss, filename, lineno);
							if (res != JVX_NO_ERROR)
							{
								if (res == JVX_ERROR_ELEMENT_NOT_FOUND)
								{
									REPORT_ERROR(((std::string)"--> One or more configuration entries not found in object type " + cType1 + " <" + entry + "> - state active - in config file.").c_str(), JVX_REPORT_PRIORITY_WARNING);
								}
								else
								{
									REPORT_ERROR(((std::string)"Failed to read configuration in object type " + cType1 + " <" + entry + "> - state active - in config file.").c_str(), JVX_REPORT_PRIORITY_WARNING);
								}
							}
						}
					}

					// Return handle for ocmponent configuration
					if (config_local)
					{
						theStruct.selTech[tp1.slotid].theHandle_shortcut_tech->return_hidden_interface(JVX_INTERFACE_CONFIGURATION, config_local);
					}
					config_local = NULL;
					datTmp_ss = NULL;
				}
			}

			// If the component is not essential, we can completely remove it
			if (!wantToContinue && unselectTechnology)
			{
				if (reportOnConfig)
				{
					reportOnConfig->report_component_unselect_on_config(tp1);
				}
				REPORT_ERROR(((std::string)"--> Removing non-essential object type " + cType1 + " <" + entry + "> due to failed activation.").c_str(), JVX_REPORT_PRIORITY_WARNING);
				res = this_pointer->unselect_selected_component(tp1);
				assert(res == JVX_NO_ERROR);
			}

			// ##############################  From here, device! ####################################

			if (wantToContinue)
			{
				jvxSize numSlots = 1;
				jvxValue val;
				jvxConfigData* theSlotNum = NULL;

				processor->getReferenceEntryCurrentSection_name(datTmp, &theSlotNum, (SECTIONNAME_NUMSLOTS_DEVICE_PREFIX + token2).c_str());
				if (theSlotNum)
				{
					processor->getAssignmentValue(theSlotNum, &val);
					val.toContent(&numSlots);
				}

				if (tp1.slotid < theStruct.selTech.size())
				{
					for (h = 0; h < numSlots; h++)
					{
						std::string myTokenState = tokenb;
						std::string myToken = token2;
						std::string myTokenField = tokenss2;
						if (numSlots > 1)
						{
							myTokenState += "_slot_" + jvx_size2String(h);
							myToken += "_slot_" + jvx_size2String(h);
							myTokenField += "_slot_" + jvx_size2String(h);
						}

						jvxComponentIdentification idTp(tp2, tp1.slotid, h);

						// Check the desired target state
						targetStateIdx = 0;
						res = HjvxConfigProcessor_readEntry_assignment<jvxSize>(processor, datTmp, myTokenState, &valS);
						if (res == JVX_NO_ERROR)
						{
							targetStateIdx = valS;
						}
						else
						{
							REPORT_ERROR(((std::string)"Failed to obtain entry " + token2 + " from config file " + filename + ".").c_str(), JVX_REPORT_PRIORITY_WARNING);
							wantToContinue = false;
							break;
						}

						jvxSize idxC = jvx_stateToIndex(JVX_STATE_SELECTED);
						assert(JVX_CHECK_SIZE_SELECTED(idxC));
						if (targetStateIdx >= idxC)
						{
							// If selection is desired find out what to select
							res = HjvxConfigProcessor_readEntry_assignmentString(processor, datTmp, myToken, &entry);
							if (res == JVX_NO_ERROR)
							{
								// Compare value to all descriptions of devices
								selectme = JVX_SIZE_UNSELECTED;
								num = 0;
								this_pointer->number_components_system(idTp, &num);
								for (i = 0; i < num; i++)
								{
									jvxApiString strL;
									desc = "";
									this_pointer->name_component_system(idTp, i, &strL);
									
									desc = strL.std_str();

									if (jvx_compareStringsWildcard(entry, desc))
									{
										selectme = (jvxInt16)i;
										break;
									}
								}

								if (JVX_CHECK_SIZE_SELECTED(selectme))
								{
									res = this_pointer->select_component(idTp, selectme);
									if (res != JVX_NO_ERROR)
									{
										REPORT_ERROR(((std::string)"Unexpected error when selecting component " + cType2 + ".").c_str(), JVX_REPORT_PRIORITY_ERROR);
										wantToContinue = false;
										break;
									}
									else
									{
										if (reportOnConfig)
										{
											reportOnConfig->report_component_select_on_config(idTp);
										}
									}
								}
								else
								{
									REPORT_ERROR(((std::string)"Failed to find component with description " + entry + " on target machine.").c_str(), JVX_REPORT_PRIORITY_ERROR);
									wantToContinue = false;
									break;
								}
							}
							else
							{
								REPORT_ERROR(((std::string)"Failed to find component description entry for symbol " + token2 + " in config file.").c_str(), JVX_REPORT_PRIORITY_ERROR);
								wantToContinue = false;
								break;
							}

							// Get access to configuration handle for component
							datTmp_ss = NULL;
							config_local = NULL;
							theStruct.selTech[idTp.slotid].theHandle_shortcut_dev[idTp.slotsubid].dev->request_hidden_interface(JVX_INTERFACE_CONFIGURATION, (jvxHandle**)&config_local);
							if (config_local)
							{
								res = processor->getReferenceSubsectionCurrentSection_name(datTmp, &datTmp_ss, myTokenField.c_str());
								if ((res == JVX_NO_ERROR) && datTmp_ss)
								{
									res = config_local->put_configuration(callConf, processor, datTmp_ss, filename, lineno);
									if (res != JVX_NO_ERROR)
									{
										if (res == JVX_ERROR_ELEMENT_NOT_FOUND)
										{
											REPORT_ERROR(((std::string)"--> One or more configuration entries not found in object type " + cType2 + " <" + entry + "> - state selected - in config file.").c_str(), JVX_REPORT_PRIORITY_WARNING);
										}
										{
											REPORT_ERROR(((std::string)"Failed to read configuration in object type " + cType2 + " <" + entry + "> - state selected - in config file.").c_str(), JVX_REPORT_PRIORITY_WARNING);
										}
									}
								}
								else
								{
									REPORT_ERROR(((std::string)"Failed to read entry " + myTokenField + ", object type " + cType2 + " <" + entry + ">, from config file.").c_str(), JVX_REPORT_PRIORITY_WARNING);
									datTmp_ss = NULL;
								}
							}

							// Check wether activation is desired
							jvxSize idxC = jvx_stateToIndex(JVX_STATE_ACTIVE);
							assert(JVX_CHECK_SIZE_SELECTED(idxC));
							if (targetStateIdx >= idxC)
							{
								res = this_pointer->activate_selected_component(idTp);
								if (res != JVX_NO_ERROR)
								{
									jvxBitField myFeatureClass;
									REPORT_ERROR(((std::string)"Unexpected error when activating component " + cType2 + ".").c_str(), JVX_REPORT_PRIORITY_ERROR);
									wantToContinue = false;

									this_pointer->feature_class_selected_component(idTp, &myFeatureClass);
									if (jvx_bitTest(myFeatureClass, JVX_FEATURE_CLASS_NON_ESSENTIAL_SHIFT))
									{
										unselectDevice = true;
									}
								}
								else
								{
									if (reportOnConfig)
									{
										reportOnConfig->report_component_active_on_config(idTp);
									}
								}
							}
							else
							{
								wantToContinue = false;
							}

							if (config_local && datTmp_ss)
							{
								res = config_local->put_configuration(callConf, processor, datTmp_ss, filename, lineno);
								if (res != JVX_NO_ERROR)
								{
									if (res == JVX_ERROR_ELEMENT_NOT_FOUND)
									{
										REPORT_ERROR(((std::string)"--> One or more configuration entries not found in object type " + cType2 + " <" + entry + "> - state active - in config file.").c_str(), JVX_REPORT_PRIORITY_WARNING);
									}
									else
									{
										REPORT_ERROR(((std::string)"Failed to read configuration in object type " + cType2 + " <" + entry + "> - state active - in config file.").c_str(), JVX_REPORT_PRIORITY_WARNING);
									}
								}
							}
						}

						// Return configuration handle the 
						if (config_local)
						{
							theStruct.selTech[idTp.slotid].theHandle_shortcut_dev[idTp.slotsubid].dev->return_hidden_interface(JVX_INTERFACE_CONFIGURATION, config_local);
						}
						config_local = NULL;
						datTmp_ss = NULL;

						// If the component is not essential, we can completely remove it. This happens e.g. if the select was successful but the 
						// activate was not!
						if (!wantToContinue && unselectDevice)
						{
							if (reportOnConfig)
							{
								reportOnConfig->report_component_unselect_on_config(idTp);
							}
							REPORT_ERROR(((std::string)"--> Removing non-essential object type " + cType2 + " <" + entry + "> due to failed activation.").c_str(), JVX_REPORT_PRIORITY_WARNING);
							res = this_pointer->unselect_selected_component(idTp);
							assert(res == JVX_NO_ERROR);
						}
					} // for (h = 0; h < numSlots: h++)
				} // if (*slotid < theStruct.selTech.size())
			} // if (wantToContinue)
		}
	}
}

template<typename T> static void get_configuration_technology(
	jvxCallManagerConfiguration* callConf,
	T& theStruct,
	IjvxConfigProcessor* processor,
	jvxHandle* datTmp, 
	std::string tokena,
	std::string tokenb,
	std::string token1,
	std::string token2,
	std::string tokenss1,
	std::string tokenss2,
	jvxSize slotid)
{
	jvxConfigData* datTmp_add = NULL;
	jvxConfigData* datTmp_intro = nullptr;
	jvxConfigData* datTmp_introd = nullptr;

	IjvxConfiguration* config_local = NULL;
	jvxBool errorDetected = false;
	std::string entry;
	jvxErrorType res = JVX_NO_ERROR;
	jvxData valD = 0;
	jvxSize valS = 0;
	jvxBool storeNameAndState = false;
	std::string myToken;
	std::string myTokenField;
	std::string myTokenState;
	jvxSize h;
	jvxSize numSlots = 0;
	jvxConfigData* theSlotNum = NULL;

	if (slotid < theStruct.selTech.size())
	{

		if (theStruct.selTech[slotid].theHandle_shortcut_tech)
		{
			// Write the name of the selected scanner technology
			errorDetected = false;

			processor->createComment(&datTmp_intro, "==========================================================");

			config_local = NULL;
			datTmp_add = NULL;
			IjvxObject* theOwner = NULL;
			theStruct.selTech[slotid].theHandle_shortcut_tech->owner(&theOwner);
			if (theOwner == nullptr)
			{
				theStruct.selTech[slotid].theHandle_shortcut_tech->request_hidden_interface(JVX_INTERFACE_CONFIGURATION, (jvxHandle**)&config_local);
				if (config_local)
				{
					processor->createEmptySection(&datTmp_add, tokenss1.c_str());
					config_local->get_configuration(callConf, processor, datTmp_add);
				}
				theStruct.selTech[slotid].theHandle_shortcut_tech->return_hidden_interface(JVX_INTERFACE_CONFIGURATION, config_local);


				// Check if anything should be stored at all
				storeNameAndState = true;
				if (callConf->configModeFlags & JVX_CONFIG_MODE_OVERLAY)
				{
					if (!datTmp_add)
					{
						storeNameAndState = false;
					}
					else
					{
						jvxBool isEmpty = false;
						processor->isSectionEmpty(datTmp_add, &isEmpty);
						if (isEmpty)
						{
							processor->removeHandle(datTmp_add);
							processor->removeHandle(datTmp_intro);
							datTmp_add = NULL;
							datTmp_intro = nullptr;
							storeNameAndState = false;
						}
					}
				}

				if (storeNameAndState)
				{
					jvxApiString strL;
					theStruct.selTech[slotid].theHandle_shortcut_tech->name(&strL);

					processor->addSubsectionToSection(datTmp, datTmp_intro);
					datTmp_intro = nullptr;

					entry = strL.std_str();
					HjvxConfigProcessor_writeEntry_assignmentString(processor, datTmp, token1, &entry);

					// Write the state of the selected technology
					jvxState stat = JVX_STATE_NONE;
					res = theStruct.selTech[slotid].theHandle_shortcut_tech->state(&stat);
					if (res == JVX_NO_ERROR)
					{
						valS = jvx_stateToIndex(stat);
						HjvxConfigProcessor_writeEntry_assignment<jvxSize>(processor, datTmp, tokena, &valS);
					}
					else
					{
						errorDetected = true;
					}
				}
			}
			else
			{
				processor->addSubsectionToSection(datTmp, datTmp_intro);
				datTmp_intro = nullptr;

				jvxApiString strL;
				jvxComponentIdentification cpId;
				std::string txt = " Dependent technology <";
				theStruct.selTech[slotid].theHandle_shortcut_tech->name(&strL);
				txt += strL.std_str();
				txt += "> is not handled here, the owner is component <";
				theOwner->name(&strL);
				txt += strL.std_str();
				txt += ">, component identification <";
				theOwner->request_specialization(NULL, &cpId, NULL);
				txt += jvxComponentIdentification_txt(cpId);
				txt += ">.";
				processor->createComment(&datTmp_add, txt.c_str());
			}

			// Add the content of the section
			if (datTmp_add)
			{
				processor->addSubsectionToSection(datTmp, datTmp_add);
			}
		}
		else
		{
			if (callConf->configModeFlags & JVX_CONFIG_MODE_FULL)
			{
				jvxState stat = JVX_STATE_NONE;
				valS = jvx_stateToIndex(stat);
				HjvxConfigProcessor_writeEntry_assignment<jvxSize>(processor, datTmp, tokena, &valS);
			}
		}
		assert(!errorDetected);

		// + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + +
		// Device(s)
		// + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + +

		numSlots = theStruct.selTech[slotid].theHandle_shortcut_dev.size();
		if (numSlots > 1)
		{
			//Write te config section specifically for the slots
			processor->createAssignmentValue(&theSlotNum,
				(SECTIONNAME_NUMSLOTS_DEVICE_PREFIX + token2).c_str(),
				numSlots);
			processor->addSubsectionToSection(datTmp, theSlotNum);
		}

		for (h = 0; h < numSlots; h++)
		{
			myToken = token2;
			myTokenField = tokenss2;
			myTokenState = tokenb;
			if (numSlots > 1)
			{
				myTokenState += "_slot_" + jvx_size2String(h);
				myToken += "_slot_" + jvx_size2String(h);
				myTokenField += "_slot_" + jvx_size2String(h);
			}

			if (theStruct.selTech[slotid].theHandle_shortcut_dev[h].dev)
			{
				IjvxObject* theOwner = NULL;
				processor->createComment(&datTmp_introd, "==========================================================");
				theStruct.selTech[slotid].theHandle_shortcut_dev[h].dev->owner(&theOwner);
				if (theOwner == NULL)
				{
					// Write the name of the device
					errorDetected = false;
					datTmp_add = NULL;

					processor->addSubsectionToSection(datTmp, datTmp_intro);
					datTmp_intro = nullptr;

					config_local = NULL;
					theStruct.selTech[slotid].theHandle_shortcut_dev[h].dev->request_hidden_interface(JVX_INTERFACE_CONFIGURATION, (jvxHandle**)&config_local);
					if (config_local)
					{
						processor->createEmptySection(&datTmp_add, myTokenField.c_str());
						config_local->get_configuration(callConf, processor, datTmp_add);
					}
					theStruct.selTech[slotid].theHandle_shortcut_dev[h].dev->return_hidden_interface(JVX_INTERFACE_CONFIGURATION, config_local);

					// Check if anything should be stored at all
					storeNameAndState = true;
					if (callConf->configModeFlags & JVX_CONFIG_MODE_OVERLAY)
					{
						if (!datTmp_add)
						{
							storeNameAndState = false;
						}
						else
						{
							jvxBool isEmpty = false;
							processor->isSectionEmpty(datTmp_add, &isEmpty);
							if (isEmpty)
							{
								processor->removeHandle(datTmp_add);
								processor->removeHandle(datTmp_introd);
								datTmp_add = NULL;
								datTmp_introd = nullptr;
								storeNameAndState = false;
							}
						}
					}

					if (storeNameAndState)
					{
						jvxApiString strL;
						theStruct.selTech[slotid].theHandle_shortcut_dev[h].dev->name(&strL);

						processor->addSubsectionToSection(datTmp, datTmp_introd);
						datTmp_introd = nullptr;

						entry = strL.std_str();
						HjvxConfigProcessor_writeEntry_assignmentString(processor, datTmp, myToken, &entry);

						// Write the state of the selected scanner technology
						jvxState stat = JVX_STATE_NONE;
						res = theStruct.selTech[slotid].theHandle_shortcut_dev[h].dev->state(&stat);
						if (res == JVX_NO_ERROR)
						{
							valS = jvx_stateToIndex(stat);
							HjvxConfigProcessor_writeEntry_assignment<jvxSize>(processor, datTmp, myTokenState, &valS);
						}
						else
						{
							errorDetected = true;
						}
					}
				}
				else
				{
					processor->addSubsectionToSection(datTmp, datTmp_introd);
					datTmp_introd = nullptr;

					jvxApiString strL;
					jvxComponentIdentification cpId;
					std::string txt = " Dependent component <";
					theStruct.selTech[slotid].theHandle_shortcut_dev[h].dev->name(&strL);
					txt += strL.std_str();
					txt += "> is not handled here, the owner is component <";
					theOwner->name(&strL);
					txt += strL.std_str();
					txt += ">, component identification <";
					theOwner->request_specialization(NULL, &cpId, NULL);
					txt += jvxComponentIdentification_txt(cpId);
					txt += ">.";
					processor->createComment(&datTmp_add, txt.c_str());
				}

				if (datTmp_add)
				{
					processor->addSubsectionToSection(datTmp, datTmp_add);
				}
			}
			else
			{
				if (callConf->configModeFlags & JVX_CONFIG_MODE_FULL)
				{
					jvxState stat = JVX_STATE_NONE;
					valS = jvx_stateToIndex(stat);
					HjvxConfigProcessor_writeEntry_assignment<jvxSize>(processor, datTmp, myTokenState, &valS);
				}
			}
			assert(!errorDetected);
		}// for (h = 0; h < numSlots; h++)
	}// if (slotid < elmIt_tech->technologyInstances.selTech.size())
}

// =============================================================================================
// =============================================================================================
// =============================================================================================

/*
Wrapping typedefs for components must be like:

typedef struct
{
	oneComponentWrap common;
	IjvxAscanEndpoint* theHandle;
} oneAscanEndpoint;

typedef struct
{
	std::vector<oneAscanEndpoint> availableAscanEndpoints;
	IjvxAscanEndpoint* theHandle_shortcut;
} ascanEndpointsT;
*/
#define JVX_PUT_CONFIGURATION_COMPONENT_SELECT_PRI 0x1
#define JVX_PUT_CONFIGURATION_COMPONENT_SELECT_CONFIG_PRI 0x2
#define JVX_PUT_CONFIGURATION_COMPONENT_ACTIVATE_PRI 0x4
#define JVX_PUT_CONFIGURATION_COMPONENT_ACTIVATE_CONFIG_PRI 0x8
#define JVX_PUT_CONFIGURATION_COMPONENT_SELECT_SEC 0x10
#define JVX_PUT_CONFIGURATION_COMPONENT_SELECT_CONFIG_SEC 0x20
#define JVX_PUT_CONFIGURATION_COMPONENT_ACTIVATE_SEC 0x40
#define JVX_PUT_CONFIGURATION_COMPONENT_ACTIVATE_CONFIG_SEC 0x80
#define JVX_PUT_CONFIGURATION_COMPONENT_ALL 0xFF
#define JVX_PUT_CONFIGURATION_COMPONENT_NUMBER_ITERATIONS_NODES 4
#define JVX_PUT_CONFIGURATION_COMPONENT_NUMBER_ITERATIONS_TECHS 8

template<typename T1, typename T2> static void put_configuration_component_node(
	jvxCallManagerConfiguration* callConf, 
	T1& theStruct, T2& theList,
	IjvxConfigProcessor* processor,
	jvxHandle* datTmp, 
	const char* filename, jvxInt32 lineno,
	std::string tokena,
	std::string token1,
	std::string tokenss1,
	jvxComponentIdentification tp1,
	std::string cType1,
	IjvxReport* report,
	IjvxReportOnConfig* reportOnConfig,
	IjvxHost* this_pointer,
	jvxCBitField whatToDo = JVX_PUT_CONFIGURATION_COMPONENT_ALL)
{
	jvxSize i;
	IjvxConfiguration* config_local = NULL;
	jvxSize valS;
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize targetStateIdx = 0;
	jvxApiString strL;
	std::string entry;
	std::string desc;
	jvxBool wantToContinue = true;
	jvxInt16 selectme = -1;
	jvxSize num = 0;
	// Step I: Get the desired state from configuration
	targetStateIdx = 0;
	jvxHandle* datTmp_ss = NULL;
	jvxState theStat = JVX_STATE_NONE;

	if (callConf->configModeFlags & JVX_CONFIG_MODE_OVERLAY)
	{
		// If a technology has been selected before
		res = HjvxConfigProcessor_readEntry_assignmentString(processor, datTmp, token1, &entry);
		if (res == JVX_NO_ERROR)
		{
			if (tp1.slotid < theStruct.theHandle_shortcut.size())
			{
				if (theStruct.theHandle_shortcut[tp1.slotid].obj)
				{
					jvxApiString strL;
					theStruct.theHandle_shortcut[tp1.slotid].obj->name(&strL);
					
					desc = strL.std_str();
					if (entry == desc)
					{
						// Correct entry, pass configuration
						// Get configuration interface and pass configuration
						theStruct.theHandle_shortcut[tp1.slotid].obj->state(&theStat);
						datTmp_ss = NULL;
						config_local = NULL;
						theStruct.theHandle_shortcut[tp1.slotid].obj->request_hidden_interface(JVX_INTERFACE_CONFIGURATION, (jvxHandle**)&config_local);

						if (config_local)
						{
							res = processor->getReferenceSubsectionCurrentSection_name(datTmp, &datTmp_ss, tokenss1.c_str());
							if ((res == JVX_NO_ERROR) && datTmp_ss)
							{
								// Run config only if flag indicates to do so
								jvxBool runConfig = false;
								if (theStat == JVX_STATE_SELECTED)
								{
									if (whatToDo & JVX_PUT_CONFIGURATION_COMPONENT_SELECT_CONFIG_PRI)
									{
										runConfig = true;
									}
								}
								if (theStat == JVX_STATE_ACTIVE)
								{
									if (whatToDo & JVX_PUT_CONFIGURATION_COMPONENT_ACTIVATE_CONFIG_PRI)
									{
										runConfig = true;
									}
								}
								if (runConfig)
								{
									res = config_local->put_configuration(callConf, processor, datTmp_ss, filename, lineno);
									if (res != JVX_NO_ERROR)
									{
										if (res == JVX_ERROR_ELEMENT_NOT_FOUND)
										{
											REPORT_ERROR(((std::string)"--> One or more configuration entries not found in object type " + cType1 + " <" + entry + "> - state selected - in config file.").c_str(), JVX_REPORT_PRIORITY_WARNING);
										}
										else
										{
											REPORT_ERROR(((std::string)"Failed to read configuration in object type " + cType1 + " <" + entry + "> - state selected - in config file.").c_str(), JVX_REPORT_PRIORITY_WARNING);
										}
									}
								}
							}
							else
							{
								REPORT_ERROR(((std::string)"Failed to read entry " + tokenss1 + ", object type " + cType1 + " <" + entry + ">, from config file.").c_str(), JVX_REPORT_PRIORITY_WARNING);
							}
							datTmp_ss = NULL; // It was only a reference, no deallocate required
							theStruct.theHandle_shortcut[tp1.slotid].obj->return_hidden_interface(JVX_INTERFACE_CONFIGURATION, (jvxHandle**)&config_local);
						}
					}
					else
					{
						// error
						REPORT_ERROR(((std::string)"--> Component description mismatch in object type " + cType1 + " <" + entry + "> - " + entry + " vs. " + desc + "-.").c_str(), JVX_REPORT_PRIORITY_WARNING);
					}
				}
				else
				{
					REPORT_ERROR(((std::string)"--> Component configuration mismatch in object type " + cType1 + " <" + entry + "> - No component has been selected in slot " + jvx_size2String(tp1.slotid) + ".").c_str(), JVX_REPORT_PRIORITY_WARNING);
				}
			}
			else
			{
				REPORT_ERROR(((std::string)"--> Component configuration mismatch in object type " + cType1 + " <" + entry + "> - No slot with id " + jvx_size2String(tp1.slotid) + " exists.").c_str(), JVX_REPORT_PRIORITY_WARNING);
			}
		}
	} // if (callConf->configModeFlags & JVX_CONFIG_MODE_OVERLAY)
	else
	{
		res = HjvxConfigProcessor_readEntry_assignment<jvxSize>(processor, datTmp, tokena, &valS);
		if (res == JVX_NO_ERROR)
		{
			targetStateIdx = valS;
		}
		else
		{
			//REPORT_ERROR(((std::string)"Failed to obtain entry " + token1 + " from config file " + filename + ".").c_str(), JVX_REPORT_PRIORITY_WARNING);
			wantToContinue = false;
		}

		if (wantToContinue)
		{
			if (whatToDo & JVX_PUT_CONFIGURATION_COMPONENT_SELECT_PRI)
			{
				// Check state to see what needs to be done.
				jvxSize idxC = jvx_stateToIndex(JVX_STATE_SELECTED);
				assert(idxC >= 0);
				if (targetStateIdx >= (jvxSize)idxC)
				{
					// if selection is desired, go and get the name of the selected component
					res = HjvxConfigProcessor_readEntry_assignmentString(processor, datTmp, token1, &entry);
					if (res == JVX_NO_ERROR)
					{
						// Find the selection id for the desired component
						selectme = -1;
						for (i = 0; i < theList.size(); i++)
						{
							desc = "";
							theList[i].common.hObject->name(&strL);

							desc = strL.std_str();

							if (entry == desc)
							{
								selectme = (jvxInt16)i;
								break;
							}
						}

						// Step II: If selection is valid, select technology
						if (selectme >= 0)
						{
							res = this_pointer->select_component(tp1, selectme);
							if (res != JVX_NO_ERROR)
							{
								REPORT_ERROR(((std::string)"Unexpected error when selecting component " + cType1 + ".").c_str(), JVX_REPORT_PRIORITY_WARNING);
								wantToContinue = false;
							}
							else
							{
								if (reportOnConfig)
								{
									reportOnConfig->report_component_select_on_config(tp1);
								}
							}
						}
						else
						{
							REPORT_ERROR(((std::string)"Failed to find component with description " + entry + " on target machine.").c_str(), JVX_REPORT_PRIORITY_WARNING);
							wantToContinue = false;
						}
					}
					else
					{
						REPORT_ERROR(((std::string)"Failed to find component description entry for symbol " + token1 + " in config file.").c_str(), JVX_REPORT_PRIORITY_WARNING);
						wantToContinue = false;
					}
				}
				else
				{
					// No selection is desired
					wantToContinue = false;
				}
			}
			else
			{
				jvxState curState = JVX_STATE_NONE;
				this_pointer->state_selected_component(tp1, &curState);
				if (curState < JVX_STATE_SELECTED)
				{
					wantToContinue = false;
				}
			}

			// If selection was successful, next, pass config entries to component to be read in state selected. Afterwards, activate
			if (wantToContinue)
			{
				// Get configuration interface and pass configuration
				datTmp_ss = NULL;
				config_local = NULL;
				if (tp1.slotid < theStruct.theHandle_shortcut.size())
				{
					theStruct.theHandle_shortcut[tp1.slotid].obj->request_hidden_interface(JVX_INTERFACE_CONFIGURATION, (jvxHandle**)&config_local);
					if (config_local)
					{
						res = processor->getReferenceSubsectionCurrentSection_name(datTmp, &datTmp_ss, tokenss1.c_str());
						if ((res == JVX_NO_ERROR) && datTmp_ss)
						{
							if (whatToDo & JVX_PUT_CONFIGURATION_COMPONENT_SELECT_CONFIG_PRI)
							{
								res = config_local->put_configuration(callConf, processor, datTmp_ss, filename, lineno);
								if (res != JVX_NO_ERROR)
								{
									if (res == JVX_ERROR_ELEMENT_NOT_FOUND)
									{
										REPORT_ERROR(((std::string)"--> One or more configuration entries not found in object type " + cType1 + " <" + entry + "> - state selected - in config file.").c_str(), JVX_REPORT_PRIORITY_WARNING);
									}
									else
									{
										REPORT_ERROR(((std::string)"Failed to read configuration in object type " + cType1 + " <" + entry + "> - state selected - in config file.").c_str(), JVX_REPORT_PRIORITY_WARNING);
									}
								}
							}
						}
						else
						{
							REPORT_ERROR(((std::string)"Failed to read entry " + tokenss1 + ", object type " + cType1 + " <" + entry + ">, from config file.").c_str(), JVX_REPORT_PRIORITY_WARNING);
							datTmp_ss = NULL;
						}
					}
				}
				/* Do not return handle yet, we may need it lateron */

				if (whatToDo & JVX_PUT_CONFIGURATION_COMPONENT_ACTIVATE_PRI)
				{
					// Step III: Check state to see what needs to be done.
					jvxSize idxC = jvx_stateToIndex(JVX_STATE_ACTIVE);
					assert(JVX_CHECK_SIZE_SELECTED(idxC));
					if (targetStateIdx >= idxC)
					{
						// Seems that activation is desired
						res = this_pointer->activate_selected_component(tp1);
						if (res != JVX_NO_ERROR)
						{
							REPORT_ERROR(((std::string)"Unexpected error when activating component " + cType1 + ".").c_str(), JVX_REPORT_PRIORITY_WARNING);
							wantToContinue = false;
						}
						else
						{
							if (reportOnConfig)
							{
								reportOnConfig->report_component_active_on_config(tp1);
							}
						}
					}
					else
					{
						wantToContinue = false;
					}
				}
				else
				{
					jvxState curState = JVX_STATE_NONE;
					this_pointer->state_selected_component(tp1, &curState);
					if (curState < JVX_STATE_ACTIVE)
					{
						wantToContinue = false;
					}
				}
				if (wantToContinue)
				{
					// Activation successful, next, configure component specific. Handle should still be "alive"
					if (config_local && datTmp_ss)
					{
						if (whatToDo & JVX_PUT_CONFIGURATION_COMPONENT_ACTIVATE_CONFIG_PRI)
						{
							res = config_local->put_configuration(callConf, processor, datTmp_ss, filename, lineno);
							if (res != JVX_NO_ERROR)
							{
								if (res == JVX_ERROR_ELEMENT_NOT_FOUND)
								{
									REPORT_ERROR(((std::string)"--> One or more configuration entries not found in object type " + cType1 + " <" + entry + "> - state active - in config file.").c_str(), JVX_REPORT_PRIORITY_WARNING);
								}
								else
								{
									REPORT_ERROR(((std::string)"Failed to read configuration in object type " + cType1 + " <" + entry + "> - state active - in config file.").c_str(), JVX_REPORT_PRIORITY_WARNING);
								}
							}
						}
					}
				}

				// Return handle for ocmponent configuration
				if (config_local)
				{
					theStruct.theHandle_shortcut[tp1.slotid].obj->return_hidden_interface(JVX_INTERFACE_CONFIGURATION, config_local);
				}
				config_local = NULL;
				datTmp_ss = NULL;
			}
		}
	}
}

template<typename T> static void get_configuration_component_node(
	jvxCallManagerConfiguration* callConf,
	T& theStruct,
	IjvxConfigProcessor* processor,
	jvxHandle* datTmp, 
	std::string tokena,
	std::string token1,
	std::string token11,
	jvxSize slotid)
{
	jvxConfigData* datTmp_add = NULL;
	jvxConfigData* datTmp_intro = nullptr;
	IjvxConfiguration* config_local = NULL;
	jvxBool errorDetected = false;
	std::string entry;
	jvxErrorType res = JVX_NO_ERROR;
	jvxData valD = 0;
	jvxSize valS = 0;
	jvxBool storeNameAndState = false;

	if (slotid < theStruct.theHandle_shortcut.size())
	{
		if (theStruct.theHandle_shortcut[slotid].obj)
		{
			// Write the name of the selected scanner technology
			errorDetected = false;
			datTmp_add = NULL;

			config_local = NULL;
			IjvxObject* theOwner = NULL;
			
			processor->createComment(&datTmp_intro, "==========================================================");
			
			theStruct.theHandle_shortcut[slotid].obj->owner(&theOwner);
			if (theOwner == nullptr)
			{
				if (theStruct.theHandle_shortcut[slotid].noCfgSave)
				{
					processor->addSubsectionToSection(datTmp, datTmp_intro);
					jvxApiString strL;
					jvxComponentIdentification cpId;
					std::string txt = " Component <";
					theStruct.theHandle_shortcut[slotid].obj->name(&strL);
					txt += strL.std_str();
					txt += "> is not handled here since the <no save> flag is set.";
					processor->createComment(&datTmp_add, txt.c_str());
				}
				else
				{
					theStruct.theHandle_shortcut[slotid].obj->request_hidden_interface(JVX_INTERFACE_CONFIGURATION, (jvxHandle**)&config_local);
					if (config_local)
					{
						processor->createEmptySection(&datTmp_add, token11.c_str());
						config_local->get_configuration(callConf, processor, datTmp_add);
					}
					theStruct.theHandle_shortcut[slotid].obj->return_hidden_interface(JVX_INTERFACE_CONFIGURATION, config_local);


					// Check if anything should be stored at all
					storeNameAndState = true;
					if (callConf->configModeFlags & JVX_CONFIG_MODE_OVERLAY)
					{
						if (!datTmp_add)
						{
							storeNameAndState = false;
						}
						else
						{
							jvxBool isEmpty = false;
							processor->isSectionEmpty(datTmp_add, &isEmpty);
							if (isEmpty)
							{
								processor->removeHandle(datTmp_add);
								processor->removeHandle(datTmp_intro);
								datTmp_add = NULL;
								datTmp_intro = nullptr;
								storeNameAndState = false;
							}
						}
					}

					if (storeNameAndState)
					{
						jvxApiString strL;
						theStruct.theHandle_shortcut[slotid].obj->name(&strL);

						processor->addSubsectionToSection(datTmp, datTmp_intro);

						entry = strL.std_str();
						HjvxConfigProcessor_writeEntry_assignmentString(processor, datTmp, token1, &entry);

						// Write the state of the selected scanner technology
						jvxState stat = JVX_STATE_NONE;
						res = theStruct.theHandle_shortcut[slotid].obj->state(&stat);
						if (res == JVX_NO_ERROR)
						{
							valS = jvx_stateToIndex(stat);
							HjvxConfigProcessor_writeEntry_assignment<jvxSize>(processor, datTmp, tokena, &valS);
						}
						else
						{
							errorDetected = true;
						}
					}
				}
			}
			else
			{
				processor->addSubsectionToSection(datTmp, datTmp_intro);
				jvxApiString strL;
				jvxComponentIdentification cpId;
				std::string txt = " Dependent component <";
				theStruct.theHandle_shortcut[slotid].obj->name(&strL);
				txt += strL.std_str();
				txt += "> is not handled here, the owner is component <";
				theOwner->name(&strL);
				txt += strL.std_str();
				txt += ">, component identification <";
				theOwner->request_specialization(NULL, &cpId, NULL);
				txt += jvxComponentIdentification_txt(cpId);
				txt += ">.";
				processor->createComment(&datTmp_add, txt.c_str());
			}

			if (datTmp_add)
			{
				processor->addSubsectionToSection(datTmp, datTmp_add);
			}
			return;
		}
		if (callConf->configModeFlags & JVX_CONFIG_MODE_FULL)
		{
			jvxState stat = JVX_STATE_NONE;
			valS = jvx_stateToIndex(stat);
			HjvxConfigProcessor_writeEntry_assignment<jvxSize>(processor, datTmp, tokena, &valS);
		}
	}
	assert(!errorDetected);
}

#endif
