#include "CjvxAppHostBase.h"
#include "jvxHost_config.h"

jvxErrorType
JVX_APP_FACTORY_HOST_CLASSNAME::configureFromFile(jvxCallManagerConfiguration* callConf,
	const std::string& fName, std::list<std::string>& filepaths, jvxHandle* context)
{
	jvxConfigSectionTypes myCfgTp = JVX_CONFIG_SECTION_TYPE_UNKNOWN;
	IjvxConfigurationExtender* cfgExt = NULL;
	jvxValue readVal;
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;
	//std::string content;
	jvxConfigData* datTmp1 = NULL, *datTmp2 = NULL;
	jvxApiString  fldStr;
	IjvxConfiguration* cfg = NULL;
	
	
	jvxComponentIdentification tpCfg(JVX_COMPONENT_CONFIG_PROCESSOR, JVX_SIZE_UNSELECTED, JVX_SIZE_UNSELECTED);
	jvxComponentIdentification tpCrypt(JVX_COMPONENT_CRYPT, JVX_SIZE_UNSELECTED, JVX_SIZE_UNSELECTED);

	std::cout << __FUNCTION__ << " -- Trying to open configuration file  <" << fName << ">." << std::endl;
	for (auto dirs : filepaths)
	{
		std::cout << __FUNCTION__ << " ---- Search in folder <" << dirs << ">" << std::endl;
	}

	std::string secName;
	std::vector<std::string> warnings;
	jvxInt16 valI;
	
	// Obtain encryption for text file
	IjvxObject* theCryptObj = NULL;
	IjvxCrypt* theCryptInst = NULL;
	jvxErrorType resL = JVX_NO_ERROR;

	if (involvedComponents.theTools.hTools)
	{
		resL = involvedComponents.theTools.hTools->reference_tool(tpCrypt, &theCryptObj, 0, NULL);
		if ((resL == JVX_NO_ERROR) && (theCryptObj))
		{
			resL = theCryptObj->request_specialization(reinterpret_cast<jvxHandle**>(&theCryptInst), NULL, NULL);
			assert(resL == JVX_NO_ERROR);
		}
	}

	// When reading, always pass cryppto instance
	//res = jvx_readContentFromFile(fName, content, theCryptInst);

	if (theCryptInst)
	{
		resL = involvedComponents.theTools.hTools->return_reference_tool(tpCrypt, theCryptObj);
		assert(resL == JVX_NO_ERROR);
	}

	theCryptInst = NULL;
	theCryptObj = NULL;

	// ================================================================
	// Note: due to include files in config file, the crypt is currently not supported
	// ================================================================

	if (res == JVX_NO_ERROR)
	{
		IjvxObject* obj = NULL;
		IjvxConfigProcessor* cfgProc = NULL;

		this->involvedComponents.theTools.hTools->reference_tool(tpCfg, &obj, 0, NULL);
		if (obj)
		{
			obj->request_specialization((jvxHandle**)&cfgProc, NULL, NULL);
			if (cfgProc)
			{
				//res = cfgProc->parseTextField(content.c_str(), fName.c_str(), 0);
				for (auto elm: filepaths)
				{
					cfgProc->addIncludePath(elm.c_str());
				}
				res = cfgProc->parseFile(fName.c_str());
				if (res == JVX_NO_ERROR)
				{
					inform_config_read_start(fName.c_str(), context);

					if (callConf->configModeFlags & JVX_CONFIG_MODE_OVERLAY)
					{
						datTmp1 = NULL;
						res = cfgProc->getConfigurationHandle(&datTmp1);

						if ((res == JVX_NO_ERROR) && (datTmp1))
						{
							// Get the section to file mapper
							cfgProc->generate_section_origin_list(&lstSectionsFile, datTmp1);

							cfgProc->getNameCurrentEntry(datTmp1, &fldStr);
							secName = fldStr.std_str();
							if (JVX_QT_HOST_1_SECTION_COMPONENTS == secName)
							{
								res = this->involvedComponents.theHost.hFHost->request_hidden_interface(JVX_INTERFACE_CONFIGURATION, (jvxHandle**)&cfg);
								if ((res == JVX_NO_ERROR) && cfg)
								{
									cfg->put_configuration(callConf, cfgProc, datTmp1, fName.c_str(), 0);
									this->involvedComponents.theHost.hFHost->return_hidden_interface(JVX_INTERFACE_CONFIGURATION, (jvxHandle*)cfg);
								}
								else
								{
									this->report_text_output("Failed to obtain configuration interface from host.", JVX_REPORT_PRIORITY_WARNING, NULL);
									res = JVX_ERROR_INTERNAL;
								}
							}
							
							cfgProc->removeHandle(datTmp1);
							datTmp1 = NULL;

						}
						else
						{
							res = JVX_ERROR_ELEMENT_NOT_FOUND;
						}
					}
					else
					{

						datTmp1 = NULL;
						res = cfgProc->getConfigurationHandle(&datTmp1);
						if ((res == JVX_NO_ERROR) && (datTmp1))
						{
							// Get the section to file mapper
							cfgProc->generate_section_origin_list(&lstSectionsFile, datTmp1);

							cfgProc->getNameCurrentEntry(datTmp1, &fldStr);

							secName = fldStr.std_str();
							if (JVX_QT_HOST_1_SECTION_MAIN == secName)
							{
								// Yes, I found the right section..

								// Obtain all host specific contents here.
								/*
								 * External trigger is always on - sequencer in dedicated thread is not allowed!
								 *
								if (HjvxConfigProcessor_readEntry_assignment<jvxInt16>(cfgProc, datTmp1, JVX_QT_HOST_1_EXTERNAL_TRIGGER, &valI) == JVX_NO_ERROR)
								{
									report_text_output("Parameter for external trigger ignored!", JVX_REPORT_PRIORITY_WARNING, NULL);
									//subWidgets.sequences.theWidget->setExternalTrigger((valI != 0)); TODO
								}
								*/

								// Next, pass the reserved subsection to used host
								datTmp2 = NULL;
								cfgProc->getReferenceSubsectionCurrentSection_name(datTmp1, &datTmp2, JVX_QT_HOST_1_SECTION_COMPONENTS);
								if (datTmp2)
								{
									res = this->involvedComponents.theHost.hFHost->request_hidden_interface(JVX_INTERFACE_CONFIGURATION, (jvxHandle**)&cfg);
									if ((res == JVX_NO_ERROR) && cfg)
									{
										cfg->put_configuration(callConf, cfgProc, datTmp2, fName.c_str(), 0);
										this->involvedComponents.theHost.hFHost->return_hidden_interface(JVX_INTERFACE_CONFIGURATION, (jvxHandle*)cfg);
									}
									else
									{
										report_text_output("Failed to obtain configuration interface from host.", JVX_REPORT_PRIORITY_WARNING, NULL);
										res = JVX_ERROR_INTERNAL;
									}
								}
								else
								{
									report_text_output("Failed to obtain subsection for involved host from config file.", JVX_REPORT_PRIORITY_WARNING, NULL);
									res = JVX_ERROR_ELEMENT_NOT_FOUND;
								}

								put_configuration_specific(callConf, cfgProc, datTmp1, fName.c_str());

								res = involvedComponents.theHost.hFHost->request_hidden_interface(JVX_INTERFACE_CONFIGURATION_EXTENDER, (jvxHandle**)&cfgExt);
								if ((res == JVX_NO_ERROR) && cfgExt)
								{
									put_configuration_specific_ext(callConf, cfgExt);
									involvedComponents.theHost.hFHost->return_hidden_interface(JVX_INTERFACE_CONFIGURATION_EXTENDER, (jvxHandle*)cfgExt);
									cfgExt = NULL;
								}
							}
							else
							{
								report_text_output("Failed to obtain section for jvx-qt-host-1 from config file.", JVX_REPORT_PRIORITY_ERROR, NULL);
								res = JVX_ERROR_ELEMENT_NOT_FOUND;
							}


							cfgProc->removeHandle(datTmp1);
							datTmp1 = NULL;
						}
						else
						{
							res = JVX_ERROR_ELEMENT_NOT_FOUND;
						}
					} // else for if (flagtag & JVX_PROPERTY_FLAG_ADD_TO_OVERLAY_MASK)
					
					inform_config_read_stop(res, fName.c_str(), context);
				}
				else
				{
					jvxApiError err;
					switch (res)
					{
					case JVX_ERROR_OPEN_FILE_FAILED:
						std::cout << __FUNCTION__ << " -- Opening config file <" + fName + "> failed." << std::endl;
						report_text_output(("Opening config file <" + fName + "> failed.").c_str(), JVX_REPORT_PRIORITY_ERROR, NULL);
						break;
					case JVX_ERROR_PARSE_ERROR:
						cfgProc->getParseError(&err);
						std::cout << __FUNCTION__ << " -- Parse error in config file <" + fName + ">, description: " << err.errorDescription.std_str() << "." << std::endl;
						report_text_output(("Parse error in config file <" + fName + ">, error: " + err.errorDescription.std_str()).c_str(), JVX_REPORT_PRIORITY_ERROR, NULL);
						cfgProc->getParseError(&err);
						report_text_output(err.errorDescription.c_str(), JVX_REPORT_PRIORITY_ERROR, NULL);
						break;
					default:
						report_text_output(("Error in reading configuration from file <" + fName + ">.").c_str(), JVX_REPORT_PRIORITY_ERROR, NULL);
					}
					res = JVX_ERROR_INTERNAL;
				}
				//cfgProc->clearIncludePaths();
			}
			else
			{
				report_text_output("Failed to config file processor tool specialization.", JVX_REPORT_PRIORITY_ERROR, NULL);
				res = JVX_ERROR_INTERNAL;
			}
			this->involvedComponents.theTools.hTools->return_reference_tool(tpCfg, obj);
			cfgProc = NULL;
		}
		else
		{
		report_text_output("Failed to access config file processor tool.", JVX_REPORT_PRIORITY_ERROR, NULL);
			res = JVX_ERROR_UNSUPPORTED;
		}
		obj = NULL;
	}
	else
	{
		res = JVX_ERROR_OPEN_FILE_FAILED;
	}


	if (res == JVX_NO_ERROR)
	{
		report_config_file_read_successful(callConf, fName.c_str());
		if (callConf->configModeFlags & JVX_CONFIG_MODE_OVERLAY)
		{
			this->report_text_output(("Successfully used configuration overlay file <" + fName + ">").c_str(), JVX_REPORT_PRIORITY_SUCCESS, NULL);
		}
		else
		{
			this->report_text_output(("Successfully used configuration file <" + fName + ">").c_str(), JVX_REPORT_PRIORITY_SUCCESS, NULL);
		}
	}
	else
	{
		if (callConf->configModeFlags & JVX_CONFIG_MODE_OVERLAY)
		{
			this->report_text_output(("Error when using configuration overlay file <" + fName + ">: " + jvxErrorType_txt(res)).c_str(), JVX_REPORT_PRIORITY_WARNING, NULL);
		}
		else
		{
			this->report_text_output(("Error when using configuration file <" + fName + ">: " + jvxErrorType_txt(res)).c_str(), JVX_REPORT_PRIORITY_WARNING, NULL);
		}
	}
	
	return(res);
}

jvxErrorType
JVX_APP_FACTORY_HOST_CLASSNAME::configureComplete()
{
	IjvxConfigurationDone* cfg = nullptr;
	jvxErrorType res = this->involvedComponents.theHost.hFHost->request_hidden_interface(JVX_INTERFACE_CONFIGURATION_DONE, (jvxHandle**)&cfg);
	if ((res == JVX_NO_ERROR) && cfg)
	{
		cfg->done_configuration();
		this->involvedComponents.theHost.hFHost->return_hidden_interface(JVX_INTERFACE_CONFIGURATION_DONE, (jvxHandle*)cfg);
	}	
	return JVX_NO_ERROR;
}

jvxErrorType
JVX_APP_FACTORY_HOST_CLASSNAME::configureToFile(jvxCallManagerConfiguration* callConf, const std::string& fName, jvxBool forceFlat, const std::string& fNameOld)
{
	jvxErrorType res = JVX_NO_ERROR;
	std::string content;
	jvxComponentIdentification tpCfg(JVX_COMPONENT_CONFIG_PROCESSOR, JVX_SIZE_UNSELECTED, JVX_SIZE_UNSELECTED);
	jvxComponentIdentification tpCrypt(JVX_COMPONENT_CRYPT, JVX_SIZE_UNSELECTED, JVX_SIZE_UNSELECTED);

	std::cout << __FUNCTION__ << " Trying to save to configuration file  <" << fName << ">." << std::endl;

	IjvxObject* obj = NULL;
	IjvxConfigProcessor* cfgProc = NULL;
	IjvxConfiguration* cfg = NULL;
	jvxConfigData* datTmp1 = NULL, * datTmp2 = NULL;
	jvxApiString  fldStr;
	jvxValue val((jvxInt16)0);
	IjvxConfigurationExtender* cfgExt = NULL;

	res = involvedComponents.theHost.hFHost->request_hidden_interface(JVX_INTERFACE_CONFIGURATION_EXTENDER, (jvxHandle**)&cfgExt);
	if ((res == JVX_NO_ERROR) && cfgExt)
	{
		cfgExt->clear_configuration_entries();
		get_configuration_specific_ext(callConf, cfgExt);
		involvedComponents.theHost.hFHost->return_hidden_interface(JVX_INTERFACE_CONFIGURATION_EXTENDER, (jvxHandle*)cfgExt);
		cfgExt = NULL;
	}

	if (lstSectionsFile)
	{
		lstSectionsFile->start_collect_file_contents();
	}

	auto lstSectionsFile_use = lstSectionsFile;

	this->involvedComponents.theTools.hTools->reference_tool(tpCfg, &obj, 0, NULL);
	if (obj)
	{
		obj->request_specialization((jvxHandle**)&cfgProc, NULL, NULL);
		if (cfgProc)
		{
			if (callConf->configModeFlags & JVX_CONFIG_MODE_OVERLAY)
			{
				datTmp1 = NULL;
				res = this->involvedComponents.theHost.hFHost->request_hidden_interface(JVX_INTERFACE_CONFIGURATION, (jvxHandle**)&cfg);
				if ((res == JVX_NO_ERROR) && cfg)
				{
					cfgProc->createEmptySection(&datTmp1, JVX_QT_HOST_1_SECTION_COMPONENTS);
					if (datTmp1)
					{
						cfg->get_configuration(callConf, cfgProc, datTmp1);
					}

					this->involvedComponents.theHost.hFHost->return_hidden_interface(JVX_INTERFACE_CONFIGURATION, (jvxHandle*)cfg);

					res = cfgProc->printConfiguration(datTmp1, &fldStr, JVX_QT_HOST_1_PRINT_COMPACT_FORM);
					if (res == JVX_NO_ERROR)
					{
						content = fldStr.std_str();
					}

					cfgProc->removeHandle(datTmp1);
				}
				else
				{
					report_text_output("Failed to obtain configuration interface from host.", JVX_REPORT_PRIORITY_WARNING, NULL);
					res = JVX_ERROR_INTERNAL;
				}
			}
			else
			{

				cfg = NULL;
				this->involvedComponents.theHost.hFHost->request_hidden_interface(JVX_INTERFACE_CONFIGURATION, (jvxHandle**)&cfg);
				if (cfg)
				{
					if (fNameOld != fName)
					{
						cfgProc->replaceFileOriginSections(fName, fNameOld);
					}
					datTmp1 = NULL;
					cfgProc->createEmptySection(&datTmp1, JVX_QT_HOST_1_SECTION_MAIN);

					// Add all specific parameters here
					/*
					jvxInt16 valI = 0;
					jvxBool extTrigger = false;
					//subWidgets.sequences.theWidget->getExternalTrigger(extTrigger);
					if (extTrigger)
					{
						valI = 1;
					}
					HjvxConfigProcessor_writeEntry_assignment<jvxInt16>(cfgProc, datTmp1, JVX_QT_HOST_1_EXTERNAL_TRIGGER, &valI);
					*/

					cfgProc->createEmptySection(&datTmp2, JVX_QT_HOST_1_SECTION_COMPONENTS);
					if (datTmp2)
					{
						cfg->get_configuration(callConf, cfgProc, datTmp2);
					}
					cfgProc->addSubsectionToSection(datTmp1, datTmp2);
					this->involvedComponents.theHost.hFHost->return_hidden_interface(JVX_INTERFACE_CONFIGURATION, (jvxHandle*)cfg);

					get_configuration_specific(callConf, cfgProc, datTmp1);

					if (forceFlat)
					{
						lstSectionsFile_use = nullptr;
					}
					res = cfgProc->printConfiguration(datTmp1, &fldStr, JVX_QT_HOST_1_PRINT_COMPACT_FORM, fName.c_str(), lstSectionsFile_use);					
					if (res == JVX_NO_ERROR)
					{
						content = fldStr.std_str();
					}
					cfgProc->removeHandle(datTmp1);
				}
			} // if (callConf->configModeFlags & JVX_CONFIG_MODE_OVERLAY) -- else

			if (forceFlat)
			{
				std::cout << __FUNCTION__ << " Successfully generated config content in <flat> mode." << std::endl;
			}			
		}
		else
		{
			report_text_output("Failed to config file processor tool specialization.", JVX_REPORT_PRIORITY_ERROR, NULL);
			res = JVX_ERROR_INTERNAL;
		}
		this->involvedComponents.theTools.hTools->return_reference_tool(tpCfg, obj);
		cfgProc = NULL;
	}
	else
	{
		report_text_output("Failed to access config file processor tool.", JVX_REPORT_PRIORITY_ERROR, NULL);
		res = JVX_ERROR_ELEMENT_NOT_FOUND;
	}
	obj = NULL;

	if (res == JVX_NO_ERROR)
	{
		if (lstSectionsFile_use)
		{
			jvxSize num = 0;
			lstSectionsFile_use->number_collected_file_contents(&num);
			for (int i = 0; i < num; i++)
			{
				jvxApiString fNameOne;
				jvxApiString contentOne;
				std::string allContents;
				jvxSize numSections = 0;
				lstSectionsFile_use->number_collected_file_idx_number_content(i, &numSections);
				for (int j = 0; j < numSections; j++)
				{
					lstSectionsFile_use->get_collected_file_idx_content_idx(i, &fNameOne, &contentOne, j);
					allContents += contentOne.std_str();
				}

				std::cout << __FUNCTION__ << " Output config content to file <" << fNameOne.std_str() << ">." << std::endl;
				res = jvx_writeContentToFile(fNameOne.std_str(), allContents);
				if (res != JVX_NO_ERROR)
				{
					break;
				}
			}
		}
		else
		{
			std::cout << __FUNCTION__ << " Output config content to file <" << fName << ">.." << std::flush;
			res = jvx_writeContentToFile(fName, content);
			if (res != JVX_NO_ERROR)
			{
				std::cout << "-> failed, error: <" << jvxErrorType_descr(res) << ">." << std::endl;
			}
			else
			{
				std::cout << "-> successful." << std::endl;
			}
		}
	}

	if (lstSectionsFile)
	{
		lstSectionsFile->stop_collect_file_contents();
	}

	return(res);
}
