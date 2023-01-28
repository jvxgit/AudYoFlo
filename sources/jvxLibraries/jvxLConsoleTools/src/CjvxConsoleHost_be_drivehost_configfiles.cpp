#include "CjvxConsoleHost_be_drivehost.h"
#include "jvxHost_config.h"

jvxErrorType
CjvxConsoleHost_be_drivehost::try_reopen_config()
{
	jvxApiString errMess;
	std::string message;
	jvxErrorType res = JVX_NO_ERROR;
	IjvxSequencer* sequencer = NULL;

	// Do an application specific shutdown
	//
	postMessage_inThread(jvx_textMessagePrioToString("Stopping all running tasks...", JVX_REPORT_PRIORITY_INFO));

	res = involvedComponents.theHost.hFHost->request_hidden_interface(JVX_INTERFACE_SEQUENCER, reinterpret_cast<jvxHandle**>(&sequencer));
	if ((res == JVX_NO_ERROR) && sequencer)
	{
		jvxSequencerStatus stat = JVX_SEQUENCER_STATUS_NONE;
		res = sequencer->status_process(&stat, NULL, NULL, NULL, NULL);
		/*
		while (stat != JVX_SEQUENCER_STATUS_NONE)
		{
			if (stat == JVX_SEQUENCER_STATUS_IN_OPERATION)
			{
				// TODO: Stop the sequencer operation
			}
			res = sequencer->status_process(&stat, NULL, NULL, NULL, NULL);
		}*/
		if (stat != JVX_SEQUENCER_STATUS_NONE)
		{
			return JVX_ERROR_NOT_READY;
		}
		res = involvedComponents.theHost.hFHost->return_hidden_interface(JVX_INTERFACE_SEQUENCER, reinterpret_cast<jvxHandle*>(sequencer));
	}
	return JVX_NO_ERROR;

#if 0
	// terminateHost();
	res = shutdown_postprocess(&errMess, NULL);
	if (res != JVX_NO_ERROR)
	{
		assert(0);
	}

	res = shutdown_deactivate(&errMess, NULL);
	if (res != JVX_NO_ERROR)
	{
		assert(0);
	}

	res = shutdown_terminate(&errMess, NULL);
	if (res != JVX_NO_ERROR)
	{
		assert(0);
	}

	// =====================================================
	// Bootup cleanly 
	// =====================================================
	/*
	initializeHost();
	configureFromFile(_command_line_parameters.configFilename, jvxFlagTag(0));
	postbootup_specific();
	*/

	message = textMessagePrioToString("Initializing web host ...", JVX_REPORT_PRIORITY_INFO);
	postMessage_inThread(message);

	res = boot_initialize(&errMess, NULL);
	if (res != JVX_NO_ERROR)
	{
		this->fatalStop("Fatal Error", errMess.c_str());
	}

	message = textMessagePrioToString("Activating audio host ...", JVX_REPORT_PRIORITY_INFO);
	postMessage_inThread(message);

	res = boot_activate(&errMess, NULL);
	if (res != JVX_NO_ERROR)
	{
		this->fatalStop("Fatal Error", errMess.c_str());
	}

	message = textMessagePrioToString("Preparing audio host ...", JVX_REPORT_PRIORITY_INFO);
	postMessage_inThread(message);

	res = boot_prepare(&errMess, NULL);
	if (res != JVX_NO_ERROR)
	{
		this->fatalStop("Fatal Error", errMess.c_str());
	}
#endif
}
#if 0
// =============================================================================================
// Config Files
// =============================================================================================

void
CjvxConsoleHost_be_drivehost::saveConfigFile(jvxBool allowOverlayOnly_config)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxFlagTag flagtag = 0;


	if (allowOverlayOnly_config)
	{
		if (!this->_command_line_parameters.configFilename_ovlay.empty())
		{
			flagtag = jvxFlagTag(1 << JVX_PROPERTY_FLAG_ADD_TO_OVERLAY_SHIFT);
			res = configurationToFile(this->_command_line_parameters.configFilename_ovlay, flagtag);
		}
	}
	else
	{
		if (!this->_command_line_parameters.configFilename.empty())
		{
			res = configurationToFile(this->_command_line_parameters.configFilename, flagtag);
		}
	}
}

void
CjvxConsoleHost_be_drivehost::openConfigFile_core(const char* fName, jvxBool allowOverlayOnly_config)
{
	jvxErrorType res = JVX_NO_ERROR;
	IjvxSequencer* sequencer = NULL;
	jvxState theStat = JVX_STATE_NONE;
	jvxBool requestHandled = false;
	if (allowOverlayOnly_config)
	{
		// Shutdown cleanly
		if (fName)
		{
			_command_line_parameters.configFilename_ovlay = fName;
		}

		if (!_command_line_parameters.configFilename_ovlay.empty())
		{

			// Do an application specific shutdown
			postMessage_inThread(textMessagePrioToString("Stopping all running tasks...", JVX_REPORT_PRIORITY_INFO));

			// Stop the sequencer
			res = involvedComponents.theHost.hHost->request_hidden_interface(JVX_INTERFACE_SEQUENCER, reinterpret_cast<jvxHandle**>(&sequencer));
			if ((res == JVX_NO_ERROR) && sequencer)
			{
				jvxSequencerStatus stat = JVX_SEQUENCER_STATUS_NONE;
				res = sequencer->status_process(&stat, NULL, NULL, NULL, NULL);
				while (stat != JVX_SEQUENCER_STATUS_NONE)
				{
					if (stat == JVX_SEQUENCER_STATUS_IN_OPERATION)
					{
						// TODO: Shutdown the Sequencer
					}
					res = sequencer->status_process(&stat, NULL, NULL, NULL, NULL);
				}
			}


			// postMessage_inThread(textMessagePrioToString(("Reading configuration <" + _command_line_parameters.configFilename_ovlay + ">..."), JVX_REPORT_PRIORITY_INFO));
			configureFromFile(_command_line_parameters.configFilename_ovlay, jvxFlagTag(1 << JVX_PROPERTY_FLAG_ADD_TO_OVERLAY_SHIFT));
		}
	}
	else
	{
		// Shutdown cleanly
		if (fName)
		{
			_command_line_parameters.configFilename = fName;
		}

		if (!_command_line_parameters.configFilename.empty())
		{
			jvxApiString errMess;
			std::string message;

			// Do an application specific shutdown
			//
			postMessage_inThread(textMessagePrioToString("Stopping all running tasks...", JVX_REPORT_PRIORITY_INFO));

			res = involvedComponents.theHost.hHost->request_hidden_interface(JVX_INTERFACE_SEQUENCER, reinterpret_cast<jvxHandle**>(&sequencer));
			if ((res == JVX_NO_ERROR) && sequencer)
			{
				jvxSequencerStatus stat = JVX_SEQUENCER_STATUS_NONE;
				res = sequencer->status_process(&stat, NULL, NULL, NULL, NULL);
				while (stat != JVX_SEQUENCER_STATUS_NONE)
				{
					if (stat == JVX_SEQUENCER_STATUS_IN_OPERATION)
					{
						// TODO: Stop the sequencer operation
					}
					res = sequencer->status_process(&stat, NULL, NULL, NULL, NULL);
				}
			}


			// terminateHost();
			res = shutdown_postprocess(&errMess, NULL);
			if (res != JVX_NO_ERROR)
			{
				assert(0);
			}

			res = shutdown_deactivate(&errMess, NULL);
			if (res != JVX_NO_ERROR)
			{
				assert(0);
			}

			res = shutdown_terminate(&errMess, NULL);
			if (res != JVX_NO_ERROR)
			{
				assert(0);
			}

			// =====================================================
			// Bootup cleanly
			// =====================================================
			/*
			initializeHost();
			configureFromFile(_command_line_parameters.configFilename, jvxFlagTag(0));
			postbootup_specific();
			*/

			message = textMessagePrioToString("Initializing web host ...", JVX_REPORT_PRIORITY_INFO);
			postMessage_inThread(message);

			res = boot_initialize(&errMess, NULL);
			if (res != JVX_NO_ERROR)
			{
				this->fatalStop("Fatal Error", errMess.c_str());
			}

			message = textMessagePrioToString("Activating audio host ...", JVX_REPORT_PRIORITY_INFO);
			postMessage_inThread(message);

			res = boot_activate(&errMess, NULL);
			if (res != JVX_NO_ERROR)
			{
				this->fatalStop("Fatal Error", errMess.c_str());
			}

			message = textMessagePrioToString("Preparing audio host ...", JVX_REPORT_PRIORITY_INFO);
			postMessage_inThread(message);

			res = boot_prepare(&errMess, NULL);
			if (res != JVX_NO_ERROR)
			{
				this->fatalStop("Fatal Error", errMess.c_str());
			}
		}
	}
}

jvxErrorType
CjvxConsoleHost_be_drivehost::configureFromFile(std::string fName, jvxFlagTag flagtag)
{
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR, resL = JVX_NO_ERROR;
	std::string content;
	jvxConfigData* datTmp1 = NULL, *datTmp2 = NULL;
	jvxApiString fldStr;
	IjvxConfiguration* cfg = NULL;
	std::string secName;
	std::vector<std::string> warnings;
	jvxInt16 valI;
	jvxValue val;
	jvxConfigSectionTypes secTp;
	jvxComponentIdentification tpCrypt(JVX_COMPONENT_CRYPT, JVX_SIZE_UNSELECTED, JVX_SIZE_UNSELECTED);
	jvxComponentIdentification tpConf(JVX_COMPONENT_CONFIG_PROCESSOR, JVX_SIZE_UNSELECTED, JVX_SIZE_UNSELECTED);

	// Obtain encryption for text file
	IjvxObject* theCryptObj = NULL;
	IjvxCrypt* theCryptInst = NULL;

	IjvxConfigurationExtender* cfgExt = NULL;
	if (involvedComponents.theTools.hTools)
	{
		res = involvedComponents.theTools.hTools->reference_tool(tpCrypt, &theCryptObj, 0, NULL);
		if ((res == JVX_NO_ERROR) && (theCryptObj))
		{
			res = theCryptObj->request_specialization(reinterpret_cast<jvxHandle**>(&theCryptInst), NULL, NULL);
			assert(res == JVX_NO_ERROR);
		}
	}

	// When reading, always pass cryppto instance
	//res = jvx_readContentFromFile(fName, content, theCryptInst);

	if (theCryptInst)
	{
		jvxErrorType resL = involvedComponents.theTools.hTools->return_reference_tool(tpCrypt, theCryptObj);
		assert(resL == JVX_NO_ERROR);
	}

	theCryptInst = NULL;
	theCryptObj = NULL;

	if (res == JVX_NO_ERROR)
	{
		IjvxObject* obj = NULL;
		IjvxConfigProcessor* cfgProc = NULL;

		this->involvedComponents.theTools.hTools->reference_tool(tpConf, &obj, 0, NULL);
		if (obj)
		{
			obj->request_specialization((jvxHandle**)&cfgProc, NULL, NULL);
			if (cfgProc)
			{
				res = cfgProc->parseFile(fName.c_str());
				//res = cfgProc->parseTextField(content.c_str(), fName.c_str(), 0);
				if (res == JVX_NO_ERROR)
				{
					if (flagtag & JVX_PROPERTY_FLAG_ADD_TO_OVERLAY_MASK)
					{
						datTmp1 = NULL;
						res = cfgProc->getConfigurationHandle(&datTmp1);

						if ((res == JVX_NO_ERROR) && (datTmp1))
						{

							cfgProc->getNameCurrentEntry(datTmp1, &fldStr);
							secName = fldStr.std_str();
							if (JVX_QT_HOST_1_SECTION_COMPONENTS == secName)
							{
								res = this->involvedComponents.theHost.hHost->request_hidden_interface(JVX_INTERFACE_CONFIGURATION, (jvxHandle**)&cfg);
								if ((res == JVX_NO_ERROR) && cfg)
								{
									cfg->put_configuration(cfgProc, datTmp1, flagtag, fName.c_str(), 0);
									this->involvedComponents.theHost.hHost->return_hidden_interface(JVX_INTERFACE_CONFIGURATION, (jvxHandle*)cfg);
								}
								else
								{
									postMessage_outThread(textMessagePrioToString("Failed to obtain configuration interface from host.", JVX_REPORT_PRIORITY_WARNING));
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
							cfgProc->getNameCurrentEntry(datTmp1, &fldStr);

							secName = fldStr.std_str();
							if (JVX_QT_HOST_1_SECTION_MAIN == secName)
							{
								// Yes, I found the right section..

								// Obtain all host specific contents here.
								if (HjvxConfigProcessor_readEntry_assignment<jvxInt16>(cfgProc, datTmp1, JVX_QT_HOST_1_EXTERNAL_TRIGGER, &valI) == JVX_NO_ERROR)
								{
									// TODO: subWidgets.sequences.theWidget->setExternalTrigger((valI != 0));
								}


								// Next, pass the reserved subsection to used host
								datTmp2 = NULL;
								cfgProc->getReferenceSubsectionCurrentSection_name(datTmp1, &datTmp2, JVX_QT_HOST_1_SECTION_COMPONENTS);
								if (datTmp2)
								{
									res = this->involvedComponents.theHost.hHost->request_hidden_interface(JVX_INTERFACE_CONFIGURATION, (jvxHandle**)&cfg);
									if ((res == JVX_NO_ERROR) && cfg)
									{
										cfg->put_configuration(cfgProc, datTmp2, flagtag, fName.c_str(), 0);
										this->involvedComponents.theHost.hHost->return_hidden_interface(JVX_INTERFACE_CONFIGURATION, (jvxHandle*)cfg);
									}
									else
									{
										postMessage_outThread(textMessagePrioToString("Failed to obtain configuration interface from host.", JVX_REPORT_PRIORITY_WARNING));
										res = JVX_ERROR_INTERNAL;
									}
								}
								else
								{
									postMessage_outThread(textMessagePrioToString("Failed to obtain subsection for involved host from config file.", JVX_REPORT_PRIORITY_WARNING));
									res = JVX_ERROR_ELEMENT_NOT_FOUND;
								}

								//this->subWidgets.realtimeViewer.props.theWidget->put_configuration(cfgProc, datTmp1, flagtag, this->involvedComponents.theHost.hHost, fName.c_str(), 0, warnings);
								//for (i = 0; i < warnings.size(); i++)
								//{
								//	this->postMessage_outThread(("Warning when reading configuration for real-time viewer: " + warnings[i]).c_str(), JVX_QT_COLOR_WARNING);
								//}

								//this->subWidgets.realtimeViewer.props.theWidget->createAllWidgetsFromConfiguration();

								//this->subWidgets.realtimeViewer.plots.theWidget->put_configuration(cfgProc, datTmp1, flagtag, this->involvedComponents.theHost.hHost, fName.c_str(), 0, warnings);
								//for (i = 0; i < warnings.size(); i++)
								//{
								//	this->postMessage_outThread(("Warning when reading configuration for real-time viewer: " + warnings[i]).c_str(), JVX_QT_COLOR_WARNING);
								//}

								//this->subWidgets.realtimeViewer.plots.theWidget->createAllWidgetsFromConfiguration();
							}
							else
							{
								postMessage_outThread(textMessagePrioToString("Failed to obtain section for jvx-qt-host-1 from config file.", JVX_REPORT_PRIORITY_ERROR));
								res = JVX_ERROR_ELEMENT_NOT_FOUND;
							}


							cfgProc->removeHandle(datTmp1);
							datTmp1 = NULL;
						}
						else
						{
							res = JVX_ERROR_ELEMENT_NOT_FOUND;
						}
					}
				}
				else
				{
					jvxApiError err;
					switch (res)
					{
					case JVX_ERROR_OPEN_FILE_FAILED:
						postMessage_outThread(textMessagePrioToString(("Opening config file <" + fName + "> failed."), JVX_REPORT_PRIORITY_ERROR));
						break;
					case JVX_ERROR_PARSE_ERROR:
						postMessage_outThread(textMessagePrioToString(("Parse error in config file <" + fName + ">."), JVX_REPORT_PRIORITY_ERROR));
						cfgProc->getParseError(&err);
						postMessage_outThread(textMessagePrioToString(err.errorDescription.std_str(), JVX_REPORT_PRIORITY_ERROR));
						break;
					default:
						postMessage_outThread(textMessagePrioToString(("Error in reading configuration from file <" + fName + ">."), JVX_REPORT_PRIORITY_ERROR));
					}

					res = JVX_ERROR_INTERNAL;
				}
			}
			else
			{
				postMessage_outThread(textMessagePrioToString("Failed to config file processor tool specialization.", JVX_REPORT_PRIORITY_ERROR));
				res = JVX_ERROR_INTERNAL;
			}
			this->involvedComponents.theTools.hTools->return_reference_tool(tpConf, obj);
			cfgProc = NULL;
		}
		else
		{
			postMessage_outThread(textMessagePrioToString("Failed to access config file processor tool.", JVX_REPORT_PRIORITY_ERROR));
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
		if (!(flagtag & JVX_PROPERTY_FLAG_ADD_TO_OVERLAY_MASK))
		{
			secTp = JVX_CONFIG_SECTION_TYPE_UNKNOWN;

			res = involvedComponents.theHost.hHost->request_hidden_interface(JVX_INTERFACE_CONFIGURATION_EXTENDER, (jvxHandle**)&cfgExt);
			if ((res == JVX_NO_ERROR) && cfgExt)
			{
				resL = cfgExt->get_configuration_entry(
					JVX_QT_HOST_1_ENTRY_UPDATERATE_RT_PLOTS,
					NULL,
					&secTp,
					0);
				if (resL == JVX_NO_ERROR)
				{
					if (secTp == JVX_CONFIG_SECTION_TYPE_ASSIGNMENTVALUE)
					{
						resL = cfgExt->get_configuration_entry(
							JVX_QT_HOST_1_ENTRY_UPDATERATE_RT_PLOTS,
							(jvxHandle**)&val,
							&secTp,
							0);
						if (resL == JVX_NO_ERROR)
						{
							//val.toContent(&systemParams.rtv_period_plots_ms);
							//subWidgets.realtimeViewer.plots.theWidget->setPeriod_ms(systemParams.rtv_period_plots_ms);
						}
					}
				}

				secTp = JVX_CONFIG_SECTION_TYPE_UNKNOWN;
				resL = cfgExt->get_configuration_entry(
					JVX_QT_HOST_1_ENTRY_UPDATERATE_RT_PROPS,
					NULL,
					&secTp,
					0);
				if (resL == JVX_NO_ERROR)
				{
					if (secTp == JVX_CONFIG_SECTION_TYPE_ASSIGNMENTVALUE)
					{
						resL = cfgExt->get_configuration_entry(
							JVX_QT_HOST_1_ENTRY_UPDATERATE_RT_PROPS,
							(jvxHandle**)&val,
							&secTp,
							0);
						if (resL == JVX_NO_ERROR)
						{
							//val.toContent(&systemParams.rtv_period_props_ms);
							//subWidgets.realtimeViewer.props.theWidget->setPeriod_ms(systemParams.rtv_period_props_ms);
						}
					}
				}

				secTp = JVX_CONFIG_SECTION_TYPE_UNKNOWN;
				resL = cfgExt->get_configuration_entry(
					JVX_QT_HOST_1_ENTRY_UPDATERATE_CENTRAL,
					NULL,
					&secTp,
					0);
				if (resL == JVX_NO_ERROR)
				{
					if (secTp == JVX_CONFIG_SECTION_TYPE_ASSIGNMENTVALUE)
					{
						resL = cfgExt->get_configuration_entry(
							JVX_QT_HOST_1_ENTRY_UPDATERATE_CENTRAL,
							(jvxHandle**)&val,
							&secTp,
							0);
						if (resL == JVX_NO_ERROR)
						{
							//val.toContent(&systemParams.rtv_period_central_ms);
						}
					}
				}
				involvedComponents.theHost.hHost->return_hidden_interface(JVX_INTERFACE_CONFIGURATION_EXTENDER, (jvxHandle*)cfgExt);
				cfgExt = NULL;
			}
		}
	}
	if (res == JVX_NO_ERROR)
	{
		//subWidgets.main.theWidget->inform_config_read_complete(fName.c_str());
		//if (flagtag & JVX_PROPERTY_FLAG_ADD_TO_OVERLAY_MASK)
		//{
		//	this->postMessage_outThread(("Successfully used configuration overlay file " + fName).c_str(), JVX_QT_COLOR_SUCCESS);
		//}
		//else
		//{
		//	this->postMessage_outThread(("Successfully used configuration file " + fName).c_str(), JVX_QT_COLOR_SUCCESS);
		//}
	}
	else
	{
		if (flagtag & JVX_PROPERTY_FLAG_ADD_TO_OVERLAY_MASK)
		{
			postMessage_outThread(textMessagePrioToString(("Error when using configuration overlay file " + fName + ": " + jvxErrorType_txt(res)), JVX_REPORT_PRIORITY_ERROR));
		}
		else
		{
			postMessage_outThread(textMessagePrioToString(("Error when using configuration file " + fName + ": " + jvxErrorType_txt(res)), JVX_REPORT_PRIORITY_ERROR));
		}
	}
	return(res);
}

jvxErrorType
CjvxConsoleHost_be_drivehost::configurationToFile(std::string& fName, jvxFlagTag flagtag)
{
	jvxErrorType res = JVX_NO_ERROR;
	std::string content;
	jvxComponentIdentification tpCrypt(JVX_COMPONENT_CRYPT, JVX_SIZE_UNSELECTED, JVX_SIZE_UNSELECTED);
	jvxComponentIdentification tpConf(JVX_COMPONENT_CONFIG_PROCESSOR, JVX_SIZE_UNSELECTED, JVX_SIZE_UNSELECTED);
	if (res == JVX_NO_ERROR)
	{
		IjvxObject* obj = NULL;
		IjvxConfigProcessor* cfgProc = NULL;
		IjvxConfiguration* cfg = NULL;
		IjvxConfigurationExtender* cfgExt = NULL;
		jvxConfigData* datTmp1 = NULL, *datTmp2 = NULL;
		jvxApiString fldStr;
		jvxValue val;
		this->involvedComponents.theTools.hTools->reference_tool(tpConf, &obj, 0, NULL);
		if (obj)
		{
			obj->request_specialization((jvxHandle**)&cfgProc, NULL, NULL);
			if (cfgProc)
			{
				if (flagtag & JVX_PROPERTY_FLAG_ADD_TO_OVERLAY_MASK)
				{
					datTmp1 = NULL;
					res = this->involvedComponents.theHost.hHost->request_hidden_interface(JVX_INTERFACE_CONFIGURATION, (jvxHandle**)&cfg);
					if ((res == JVX_NO_ERROR) && cfg)
					{
						cfgProc->createEmptySection(&datTmp1, JVX_QT_HOST_1_SECTION_COMPONENTS);
						if (datTmp1)
						{
							cfg->get_configuration(cfgProc, datTmp1, flagtag);
						}

						this->involvedComponents.theHost.hHost->return_hidden_interface(JVX_INTERFACE_CONFIGURATION, (jvxHandle*)cfg);

						res = cfgProc->printConfiguration(datTmp1, &fldStr, JVX_QT_HOST_1_PRINT_COMPACT_FORM);
						if (res == JVX_NO_ERROR)
						{
							content = fldStr.std_str();
						}

						cfgProc->removeHandle(datTmp1);
					}
					else
					{
						postMessage_outThread(textMessagePrioToString("Failed to obtain configuration interface from host.", JVX_REPORT_PRIORITY_ERROR));
						res = JVX_ERROR_INTERNAL;
					}
				}
				else
				{
					cfg = NULL;
					this->involvedComponents.theHost.hHost->request_hidden_interface(JVX_INTERFACE_CONFIGURATION, (jvxHandle**)&cfg);
					if (cfg)
					{
						this->involvedComponents.theHost.hHost->request_hidden_interface(JVX_INTERFACE_CONFIGURATION_EXTENDER, (jvxHandle**)&cfgExt);
						if (cfgExt)
						{
							/*
							val = systemParams.rtv_period_plots_ms;
							cfgExt->set_configuration_entry(
								JVX_QT_HOST_1_ENTRY_UPDATERATE_RT_PLOTS,
								&val,
								JVX_CONFIG_SECTION_TYPE_ASSIGNMENTVALUE,
								0);

							val = systemParams.rtv_period_props_ms;
							cfgExt->set_configuration_entry(
								JVX_QT_HOST_1_ENTRY_UPDATERATE_RT_PROPS,
								&val,
								JVX_CONFIG_SECTION_TYPE_ASSIGNMENTVALUE,
								0);

							val = systemParams.rtv_period_central_ms;
							cfgExt->set_configuration_entry(
								JVX_QT_HOST_1_ENTRY_UPDATERATE_CENTRAL,
								&val,
								JVX_CONFIG_SECTION_TYPE_ASSIGNMENTVALUE,
								0);
								*/
							this->involvedComponents.theHost.hHost->return_hidden_interface(JVX_INTERFACE_CONFIGURATION_EXTENDER, (jvxHandle*)cfgExt);
							cfgExt = NULL;
						}

						datTmp1 = NULL;
						cfgProc->createEmptySection(&datTmp1, JVX_QT_HOST_1_SECTION_MAIN);

						// Add all specific parameters here
						jvxInt16 valI = 0;
						jvxBool extTrigger = false;
						//subWidgets.sequences.theWidget->getExternalTrigger(extTrigger);
						//if (extTrigger)
						//{
						//	valI = 1;
						//}
						//HjvxConfigProcessor_writeEntry_assignment<jvxInt16>(cfgProc, datTmp1, JVX_QT_HOST_1_EXTERNAL_TRIGGER, &valI);

						cfgProc->createEmptySection(&datTmp2, JVX_QT_HOST_1_SECTION_COMPONENTS);
						if (datTmp2)
						{
							cfg->get_configuration(cfgProc, datTmp2, flagtag);
						}
						cfgProc->addSubsectionToSection(datTmp1, datTmp2);
						this->involvedComponents.theHost.hHost->return_hidden_interface(JVX_INTERFACE_CONFIGURATION, (jvxHandle*)cfg);

						//this->subWidgets.realtimeViewer.props.theWidget->get_configuration(cfgProc, datTmp1, flagtag, this->involvedComponents.theHost.hHost);
						//this->subWidgets.realtimeViewer.plots.theWidget->get_configuration(cfgProc, datTmp1, flagtag, this->involvedComponents.theHost.hHost);

						res = cfgProc->printConfiguration(datTmp1, &fldStr, JVX_QT_HOST_1_PRINT_COMPACT_FORM);
						if (res == JVX_NO_ERROR)
						{
							content = fldStr.std_str();
						}

						cfgProc->removeHandle(datTmp1);
					}
				}
			}
			else
			{
				postMessage_outThread(textMessagePrioToString("Failed to config file processor tool specialization.", JVX_REPORT_PRIORITY_ERROR));
				res = JVX_ERROR_INTERNAL;
			}
			this->involvedComponents.theTools.hTools->return_reference_tool(tpConf, obj);
			cfgProc = NULL;
		}
		else
		{
			postMessage_outThread(textMessagePrioToString("Failed to access config file processor tool.", JVX_REPORT_PRIORITY_ERROR));
			res = JVX_ERROR_ELEMENT_NOT_FOUND;
		}
		obj = NULL;
	}

	if (res == JVX_NO_ERROR)
	{
		// Obtain encryption for text file
		IjvxObject* theCryptObj = NULL;
		IjvxCrypt* theCryptInst = NULL;
		if (involvedComponents.theTools.hTools)
		{
			res = involvedComponents.theTools.hTools->reference_tool(tpCrypt, &theCryptObj, 0, NULL);
			if ((res == JVX_NO_ERROR) && (theCryptObj))
			{
				res = theCryptObj->request_specialization(reinterpret_cast<jvxHandle**>(&theCryptInst), NULL, NULL);
				assert(res == JVX_NO_ERROR);
			}
		}

		/*if (systemParams.encryptionConfigFiles)
		{
			res = jvx_writeContentToFile(fName, content, theCryptInst);
		}
		else
		{*/
			res = jvx_writeContentToFile(fName, content);
		//}

		/*
		if (theCryptInst)
		{
			jvxErrorType resL = involvedComponents.theTools.hTools->return_reference_tool(tpCrypt, theCryptObj);
			assert(resL == JVX_NO_ERROR);
		}*/

		theCryptInst = NULL;
		theCryptObj = NULL;
	}
	return(res);
}

#endif

void
CjvxConsoleHost_be_drivehost::get_configuration_specific_ext(jvxCallManagerConfiguration* callConf, IjvxConfigurationExtender* ext)
{
}

void
CjvxConsoleHost_be_drivehost::get_configuration_specific(jvxCallManagerConfiguration* callConf, IjvxConfigProcessor* cfg, jvxConfigData* data)
{
}

void
CjvxConsoleHost_be_drivehost::put_configuration_specific(jvxCallManagerConfiguration* callConf, IjvxConfigProcessor* cfg, jvxConfigData* data, const char* fName)
{

}

void
CjvxConsoleHost_be_drivehost::put_configuration_specific_ext(jvxCallManagerConfiguration* callConf, IjvxConfigurationExtender* ext)
{
}