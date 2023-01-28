#import "jvxIosCppHost.h"

jvxErrorType
jvxIosCppHost::configureFromFile(const char* fName)
{
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;
	std::string content;
	jvxConfigData* datTmp1 = NULL, *datTmp2 = NULL;
	jvxString* fldStr = NULL;
	IjvxConfiguration* cfg = NULL;
	std::string secName;
	std::vector<std::string> warnings;
	jvxInt16 valI;
	res = jvx_readContentFromFile(fName, content);
	if(res == JVX_NO_ERROR)
	{
		IjvxObject* obj = NULL;
		IjvxConfigProcessor* cfgProc = NULL;

		theSystemRefs.hTools->reference_tool(JVX_COMPONENT_CONFIG_PROCESSOR, 0, &obj, JVX_STATE_DONTCARE);
		if(obj)
		{
			obj->request_specialization((jvxHandle**)&cfgProc, NULL, NULL);
			if(cfgProc)
			{
				res = cfgProc->parseTextField(content.c_str(), fName, 0);
				if(res == JVX_NO_ERROR)
				{
					datTmp1 = NULL;
					res = cfgProc->getConfigurationHandle(&datTmp1);
					if((res == JVX_NO_ERROR) && (datTmp1))
					{
						cfgProc->getNameCurrentEntry(datTmp1, &fldStr);
						if(fldStr)
						{
							secName = fldStr->bString;
							cfgProc->deallocate(fldStr);
							fldStr = NULL;
							if(JVX_IOS_HOST_1_SECTION_MAIN == secName)
							{
								// Yes, I found the right section..

								// Obtain all host specific contents here.
								if(HjvxConfigProcessor_readEntry_assignmentInt16(cfgProc, datTmp1,  JVX_IOS_HOST_1_EXTERNAL_TRIGGER, &valI) == JVX_NO_ERROR)
								{
									this->postMessage_outThread("Parameter for external trigger ignored!", JVX_REPORT_PRIORITY_WARNING);
									//subWidgets.sequences.theWidget->setExternalTrigger((valI != 0)); TODO
								}


								// Next, pass the reserved subsection to used host
								datTmp2 = NULL;
								cfgProc->getReferenceSubsectionCurrentSection_name(datTmp1, &datTmp2, JVX_IOS_HOST_1_SECTION_COMPONENTS);
								if(datTmp2)
								{
									res = theSystemRefs.hHost->request_hidden_interface(JVX_INTERFACE_CONFIGURATION, (jvxHandle**)&cfg);
									if((res == JVX_NO_ERROR) && cfg)
									{
										cfg->put_configuration(cfgProc, datTmp2, fName, 0);
										theSystemRefs.hHost->return_hidden_interface(JVX_INTERFACE_CONFIGURATION, (jvxHandle*)cfg);
									}
									else
									{
										this->postMessage_outThread("Failed to obtain configuration interface from host.", JVX_REPORT_PRIORITY_WARNING);
										res = JVX_ERROR_INTERNAL;
									}
								}
								else
								{
									this->postMessage_outThread("Failed to obtain subsection for involved host from config file.", JVX_REPORT_PRIORITY_WARNING);
									res = JVX_ERROR_ELEMENT_NOT_FOUND;
								}

								this->rtv_put_configuration(cfgProc, datTmp1, theSystemRefs.hHost, fName, 0, warnings, viewer_props, JVX_REALTIME_VIEWER_VIEW_PROPERTIES);
								for(i = 0; i < warnings.size(); i++)
								{
									this->postMessage_outThread(("Warning when reading configuration for real-time viewer: " + warnings[i]).c_str(), JVX_REPORT_PRIORITY_WARNING);
								}

								//this->subWidgets.realtimeViewer.props.theWidget->createAllWidgetsFromConfiguration();

								this->rtv_put_configuration(cfgProc, datTmp1, theSystemRefs.hHost, fName, 0, warnings, viewer_plots, JVX_REALTIME_VIEWER_VIEW_PLOTS);
								for(i = 0; i < warnings.size(); i++)
								{
									this->postMessage_outThread(("Warning when reading configuration for real-time viewer: " + warnings[i]).c_str(), JVX_REPORT_PRIORITY_WARNING);
								}

								//this->subWidgets.realtimeViewer.plots.theWidget->createAllWidgetsFromConfiguration();
							}
							else
							{
								this->postMessage_outThread("Failed to obtain section for jvx-qt-host-1 from config file.", JVX_REPORT_PRIORITY_ERROR);
								res = JVX_ERROR_ELEMENT_NOT_FOUND;
							}
						}
						else
						{
							this->postMessage_outThread("Failed to get name of main section from config file processor.", JVX_REPORT_PRIORITY_ERROR);
							res = JVX_ERROR_INTERNAL;
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
					this->postMessage_outThread("Parse error in config file:",JVX_REPORT_PRIORITY_ERROR);
					jvxError* err = NULL;
					cfgProc->getParseError(&err);
					if(err)
					{
						this->postMessage_outThread(err->errorDescription->bString, JVX_REPORT_PRIORITY_ERROR);
						cfgProc->deallocate(err);
					}

					res = JVX_ERROR_INTERNAL;
				}
			}
			else
			{
				this->postMessage_outThread("Failed to config file processor tool specialization.", JVX_REPORT_PRIORITY_ERROR);
				res = JVX_ERROR_INTERNAL;
			}
			theSystemRefs.hTools->return_reference_tool(JVX_COMPONENT_CONFIG_PROCESSOR, obj);
			cfgProc = NULL;
		}
		else
		{
			this->postMessage_outThread("Failed to access config file processor tool.", JVX_REPORT_PRIORITY_ERROR);
			res = JVX_ERROR_UNSUPPORTED;
		}
		obj = NULL;
	}
	else
	{
		res = JVX_ERROR_OPEN_FILE_FAILED;
	}

	if(res == JVX_NO_ERROR)
	{
        this->postMessage_outThread(((std::string)"Successfully used configuration file " + fName).c_str(), JVX_REPORT_PRIORITY_SUCCESS);
	}
	else
	{
        this->postMessage_outThread(((std::string)"Error when using configuration file " + fName + ": " + jvxErrorType_txt(res)).c_str(), JVX_REPORT_PRIORITY_WARNING);
	}
	return(res);
}

jvxErrorType
jvxIosCppHost::configurationToFile(const char* fName)
{
	jvxErrorType res = JVX_NO_ERROR;
	std::string content;
    
	if(res == JVX_NO_ERROR)
	{
		IjvxObject* obj = NULL;
		IjvxConfigProcessor* cfgProc = NULL;
		IjvxConfiguration* cfg = NULL;
		jvxConfigData* datTmp1 = NULL, * datTmp2 = NULL;
		jvxString* fldStr = NULL;

		theSystemRefs.hTools->reference_tool(JVX_COMPONENT_CONFIG_PROCESSOR, 0, &obj, JVX_STATE_DONTCARE);
		if(obj)
		{
			obj->request_specialization((jvxHandle**)&cfgProc, NULL, NULL);
			if(cfgProc)
			{
				cfg = NULL;
				theSystemRefs.hHost->request_hidden_interface(JVX_INTERFACE_CONFIGURATION, (jvxHandle**)&cfg);
				if(cfg)
				{
					datTmp1 = NULL;
					cfgProc->createEmptySection(&datTmp1, JVX_IOS_HOST_1_SECTION_MAIN);

					// Add all specific parameters here
					jvxInt16 valI = 0;
					jvxBool extTrigger = false;
					//subWidgets.sequences.theWidget->getExternalTrigger(extTrigger);
					if(extTrigger)
					{
						valI = 1;
					}
					HjvxConfigProcessor_writeEntry_assignmentInt16(cfgProc, datTmp1,  JVX_IOS_HOST_1_EXTERNAL_TRIGGER, &valI);

					cfgProc->createEmptySection(&datTmp2, JVX_IOS_HOST_1_SECTION_COMPONENTS);
					if(datTmp2)
					{
						cfg->get_configuration(cfgProc, datTmp2);
					}
					cfgProc->addSubsectionToSection(datTmp1, datTmp2);
					theSystemRefs.hHost->return_hidden_interface(JVX_INTERFACE_CONFIGURATION, (jvxHandle*)cfg);

					this->rtv_get_configuration(cfgProc, datTmp1, theSystemRefs.hHost, viewer_props, JVX_REALTIME_VIEWER_VIEW_PROPERTIES);

					fldStr = NULL;
					res = cfgProc->printConfiguration(datTmp1, &fldStr, JVX_QT_HOST_1_PRINT_COMPACT_FORM);
					if((res == JVX_NO_ERROR) && (fldStr))
					{
						content = fldStr->bString;
						cfgProc->deallocate(fldStr);
					}

					this->rtv_get_configuration(cfgProc, datTmp1, theSystemRefs.hHost, viewer_plots, JVX_REALTIME_VIEWER_VIEW_PLOTS);

					fldStr = NULL;
					res = cfgProc->printConfiguration(datTmp1, &fldStr, JVX_QT_HOST_1_PRINT_COMPACT_FORM);
					if((res == JVX_NO_ERROR) && (fldStr))
					{
						content = fldStr->bString;
						cfgProc->deallocate(fldStr);
					}

					cfgProc->removeHandle(datTmp1);
				}
			}
			else
			{
				this->postMessage_outThread("Failed to config file processor tool specialization.", JVX_REPORT_PRIORITY_ERROR);
				res = JVX_ERROR_INTERNAL;
			}
			theSystemRefs.hTools->return_reference_tool(JVX_COMPONENT_CONFIG_PROCESSOR, obj);
			cfgProc = NULL;
		}
		else
		{
			this->postMessage_outThread("Failed to access config file processor tool.", JVX_REPORT_PRIORITY_ERROR);
			res = JVX_ERROR_ELEMENT_NOT_FOUND;
		}
		obj = NULL;
	}

	if(res == JVX_NO_ERROR)
	{
		res = jvx_writeContentToFile(fName, content);
	}
	return(res);
}

jvxErrorType
jvxIosCppHost::rtv_get_configuration(IjvxConfigProcessor* theReader, jvxHandle* ir, IjvxHost* theHost, CjvxRealtimeViewer& theViewer, jvxRealtimeViewerType configurationType)
{
    std::string postfix = "";
    switch(configurationType)
    {
        case JVX_REALTIME_VIEWER_VIEW_PROPERTIES:
            postfix = "props";
            break;
        case JVX_REALTIME_VIEWER_VIEW_PLOTS:
            postfix = "plots";
            break;
        default:
            break;
    }
    return(theViewer._get_configuration( theReader, ir, theHost, static_getConfiguration, postfix));
};

jvxErrorType
jvxIosCppHost::rtv_put_configuration(IjvxConfigProcessor* theReader, jvxHandle* ir, IjvxHost* theHost, const char* fName, int lineno, std::vector<std::string>& warnings, CjvxRealtimeViewer& theViewer, jvxRealtimeViewerType configurationType)
{
    std::string postfix = "";
    switch(configurationType)
    {
        case JVX_REALTIME_VIEWER_VIEW_PROPERTIES:
            postfix = "props";
            break;
        case JVX_REALTIME_VIEWER_VIEW_PLOTS:
            postfix = "plots";
            break;
        default:
            break;
    }
    return(theViewer._put_configuration( theReader, ir, theHost, fName, lineno, warnings, static_putConfiguration, postfix));
};
