#include <QtWidgets/QMessageBox>
#include <QtWidgets/QListWidgetItem>
#include "uMainWindow.h"
#include "uMainWindow_config.h"

#include "helpers/tools/HjvxReadConfigEntries.h"
#include "helpers/tools/HjvxWriteConfigEntries.h"

/*
void 
uMainWindow::saveConfigFile()
{
	jvxErrorType res = JVX_NO_ERROR;
	if(!this->_command_line_parameters.configFilename.empty())
	{
		res = configurationToFile(this->_command_line_parameters.configFilename);
	}
}*/

jvxErrorType 
uMainWindow::configureFromFile(std::string fName)
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

		this->involvedComponents.theTools.hTools->reference_tool(JVX_COMPONENT_CONFIG_PROCESSOR, 0, &obj);
		if(obj)
		{
			obj->request_specialization((jvxHandle**)&cfgProc, NULL, NULL);
			if(cfgProc)
			{
				res = cfgProc->parseTextField(content.c_str(), fName.c_str(), 0);
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
							if(JVX_QT_HOST_1_SECTION_MAIN == secName)
							{
								// Yes, I found the right section..

								// Obtain all host specific contents here.
								if(HjvxConfigProcessor_readEntry_assignmentInt16(cfgProc, datTmp1,  JVX_QT_HOST_1_EXTERNAL_TRIGGER, &valI) == JVX_NO_ERROR)
								{
									sequencerStruct.extTrigger = (valI != 0);
								}


								// Next, pass the reserved subsection to used host
								datTmp2 = NULL;
								cfgProc->getReferenceSubsectionCurrentSection_name(datTmp1, &datTmp2, JVX_QT_HOST_1_SECTION_COMPONENTS);
								if(datTmp2)
								{
									res = this->involvedComponents.theHost.hHost->request_hidden_interface(JVX_INTERFACE_CONFIGURATION, (jvxHandle**)&cfg);
									if((res == JVX_NO_ERROR) && cfg)
									{
										cfg->put_configuration(cfgProc, datTmp2, fName.c_str(), 0);
										this->involvedComponents.theHost.hHost->return_hidden_interface(JVX_INTERFACE_CONFIGURATION, (jvxHandle*)cfg);
									}
									else
									{
										this->postMessage_outThread("Failed to obtain configuration interface from host.", JVX_QT_COLOR_WARNING);
										res = JVX_ERROR_INTERNAL;
									}
								}
								else
								{
									this->postMessage_outThread("Failed to obtain subsection for involved host from config file.", JVX_QT_COLOR_WARNING);
									res = JVX_ERROR_ELEMENT_NOT_FOUND;
								}	
							}
							else
							{
								this->postMessage_outThread("Failed to obtain section for jvx-qt-host-1 from config file.", Qt::red);
								res = JVX_ERROR_ELEMENT_NOT_FOUND;
							}
						}
						else
						{
							this->postMessage_outThread("Failed to get name of main section from config file processor.", Qt::red);
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
					this->postMessage_outThread("Parse error in config file:", Qt::red);
					jvxError* err = NULL;
					cfgProc->getParseError(&err);
					if(err)
					{
						this->postMessage_outThread(err->errorDescription->bString, Qt::red);
						cfgProc->deallocate(err);
					}

					res = JVX_ERROR_INTERNAL;
				}
			}
			else
			{
				this->postMessage_outThread("Failed to config file processor tool specialization.", JVX_QT_COLOR_ERROR);
				res = JVX_ERROR_INTERNAL;
			}
			this->involvedComponents.theTools.hTools->return_reference_tool(JVX_COMPONENT_CONFIG_PROCESSOR, obj);
			cfgProc = NULL;
		}
		else
		{
			this->postMessage_outThread("Failed to access config file processor tool.", JVX_QT_COLOR_ERROR);
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
		this->postMessage_outThread(("Successfully used configuration file " + fName).c_str(), JVX_QT_COLOR_SUCCESS);
	}
	else
	{
		this->postMessage_outThread(("Error when using configuration file " + fName + ": " + jvxErrorType_txt(res)).c_str(), JVX_QT_COLOR_WARNING);
	}
	return(res);
}

jvxErrorType 
uMainWindow::configurationToFile(std::string& fName)
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

		this->involvedComponents.theTools.hTools->reference_tool(JVX_COMPONENT_CONFIG_PROCESSOR, 0, &obj);
		if(obj)
		{
			obj->request_specialization((jvxHandle**)&cfgProc, NULL, NULL);
			if(cfgProc)
			{
				cfg = NULL;
				this->involvedComponents.theHost.hHost->request_hidden_interface(JVX_INTERFACE_CONFIGURATION, (jvxHandle**)&cfg);
				if(cfg)
				{
					datTmp1 = NULL;
					cfgProc->createEmptySection(&datTmp1, JVX_QT_HOST_1_SECTION_MAIN);
	
					// Add all specific parameters here
					jvxInt16 valI = 0;
					/*
					jvxBool extTrigger = false;
					subWidgets.sequences.theWidget->getExternalTrigger(extTrigger);
					if(extTrigger)
					{
						valI = 1;
					}
					HjvxConfigProcessor_writeEntry_assignmentInt16(cfgProc, datTmp1,  JVX_QT_HOST_1_EXTERNAL_TRIGGER, &valI);
					*/
					cfgProc->createEmptySection(&datTmp2, JVX_QT_HOST_1_SECTION_COMPONENTS);
					if(datTmp2)
					{
						cfg->get_configuration(cfgProc, datTmp2);
					}
					cfgProc->addSubsectionToSection(datTmp1, datTmp2);
					this->involvedComponents.theHost.hHost->return_hidden_interface(JVX_INTERFACE_CONFIGURATION, (jvxHandle*)cfg);

					//this->subWidgets.realtimeViewer.theWidget->get_configuration(cfgProc, datTmp1, this->involvedComponents.theHost.hHost);

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
				this->postMessage_outThread("Failed to config file processor tool specialization.", JVX_QT_COLOR_ERROR);
				res = JVX_ERROR_INTERNAL;
			}
			this->involvedComponents.theTools.hTools->return_reference_tool(JVX_COMPONENT_CONFIG_PROCESSOR, obj);
			cfgProc = NULL;
		}
		else
		{
			this->postMessage_outThread("Failed to access config file processor tool.", JVX_QT_COLOR_ERROR);
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