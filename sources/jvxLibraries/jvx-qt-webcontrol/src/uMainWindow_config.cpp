#include <QtWidgets/QMessageBox>
#include <QtWidgets/QListWidgetItem>
#include <QtWidgets/QFileDialog>
#include <QtCore/QThread>
#include "uMainWindow.h"
#include "jvxHost_config.h"
#include <QtWidgets/QSplashScreen>
#include "jvx-qt-helpers.h"

void 
uMainWindow::saveConfigFile()
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxCallManagerConfiguration callConf;
	if (theHostFeatures.allowOverlayOnly_config)
	{
		if (!cfg_filename_olay_in_use.empty())
		{
			callConf.configModeFlags = JVX_CONFIG_MODE_OVERLAY;
			res = configureToFile(&callConf, cfg_filename_olay_in_use);
		}
	}
	else
	{
		if (!cfg_filename_in_use.empty())
		{
			callConf.configModeFlags = JVX_CONFIG_MODE_FULL;
			res = configureToFile(&callConf, cfg_filename_in_use);
		}
	}
}

void
uMainWindow::saveConfigFileAs()
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxCallManagerConfiguration callConf;
	QString str = QFileDialog::getSaveFileName(this, tr("Save to Config File"), tr(".\\"), tr("(*.jvx)"));
	if (!str.isEmpty())
	{
		std::string fname = str.toLatin1().constData();
		if (theHostFeatures.allowOverlayOnly_config)
		{
			cfg_filename_olay_in_use = fname;
			if (!this->_command_line_parameters.configFilename_ovlay.empty())
			{
				callConf.configModeFlags = JVX_CONFIG_MODE_OVERLAY;
				res = configureToFile(&callConf, cfg_filename_olay_in_use);
			}
		}
		else
		{
			cfg_filename_in_use = fname;
			if (!this->_command_line_parameters.configFilename.empty())
			{
				callConf.configModeFlags = JVX_CONFIG_MODE_FULL;
				res = configureToFile(&callConf, cfg_filename_in_use);
			}
		}
	}
}

void
uMainWindow::openConfigFile()
{
	jvxErrorType res = JVX_NO_ERROR;
	IjvxSequencer* sequencer = NULL;
	jvxState theStat = JVX_STATE_NONE;
	QString str = QFileDialog::getOpenFileName(this, tr("Open Config File"), tr(".\\"), tr("(*.jvx)"));
	jvxBool requestHandled = false;
	if (!str.isEmpty())
	{
		openConfigFile_core(str.toLatin1().constData(), theHostFeatures.allowOverlayOnly_config);
	}
}

void
uMainWindow::openConfigFile_core(const char* fName, jvxBool allowOverlayOnly_config)
{
	jvxErrorType res = JVX_NO_ERROR;
	IjvxSequencer* sequencer = NULL;
	jvxState theStat = JVX_STATE_NONE;
	jvxBool requestHandled = false;
	QSplashScreen* spl = NULL;
	jvxCallManagerConfiguration callConf;
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
			//
#ifndef JVX_SPLASH_SUPPRESSED
			spl = jvx_start_splash_qt(theHostFeatures.config_ui.scalefac_splash,
				theHostFeatures.config_ui.pixmap_splash,
				theHostFeatures.config_ui.ftsize_splash,
				theHostFeatures.config_ui.italic_splash);
			if(spl)
			{
				spl->showMessage("Stopping all running tasks...", Qt::AlignHCenter, QColor(0, 176, 240));
				qApp->processEvents();
			}
#endif


#ifndef JVX_SPLASH_SUPPRESSED
			if(spl)
			{
				spl->showMessage("Shutting down host...", Qt::AlignHCenter, QColor(0, 176, 240));
				qApp->processEvents();
			}
#endif

#ifndef JVX_SPLASH_SUPPRESSED
			if(spl)
			{
				spl->showMessage(("Reading configuration <" + _command_line_parameters.configFilename_ovlay + ">...").c_str(), Qt::AlignHCenter, QColor(0, 176, 240));
				qApp->processEvents();
			}
#endif
						
			cfg_filename_olay_in_use = jvx_makeFilePath(_command_line_parameters.userPath, _command_line_parameters.configFilename_ovlay);
			callConf.configModeFlags = JVX_CONFIG_MODE_OVERLAY_READ;

#ifndef JVX_SPLASH_SUPPRESSED
			configureFromFile(&callConf, cfg_filename_olay_in_use,
				_command_line_parameters.configDirectories, (jvxHandle*)spl);
#else
			configureFromFile(&callConf, cfg_filename_olay_in_use,
				_command_line_parameters.configDirectories, (jvxHandle*)nullptr);
#endif

#ifndef JVX_SPLASH_SUPPRESSED
			jvx_stop_splash_qt(spl, this);
#endif
		}
		updateWindow((jvxCBitField)1 << JVX_UPDATE_WINDOW_CONFIG_READ_SHIFT);
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

			// Do an application specific shutdown
			//
#ifndef JVX_SPLASH_SUPPRESSED
			spl = jvx_start_splash_qt(theHostFeatures.config_ui.scalefac_splash,
				theHostFeatures.config_ui.pixmap_splash,
				theHostFeatures.config_ui.ftsize_splash,
				theHostFeatures.config_ui.italic_splash);
			if (spl)
			{
				spl->showMessage("Stopping all running tasks...", Qt::AlignHCenter, QColor(0, 176, 240));
				qApp->processEvents();
			}
#endif

#ifndef JVX_SPLASH_SUPPRESSED
			if (spl)
			{
				spl->showMessage("Shutting down host...", Qt::AlignHCenter, QColor(0, 176, 240));
				qApp->processEvents();
			}
#endif

			// Shutdown realtime viewer

#ifndef JVX_SPLASH_SUPPRESSED
			if (spl)
			{
				spl->showMessage("Terminating host...", Qt::AlignHCenter, QColor(0, 176, 240));
				qApp->processEvents();
			}
#endif

			//terminateHost();

#ifndef JVX_SPLASH_SUPPRESSED
			if (spl)
			{
				spl->showMessage("Reinitializing host...", Qt::AlignHCenter, QColor(0, 176, 240));
				qApp->processEvents();
			}
#endif

			// =====================================================
			// Bootup cleanly
			// =====================================================
			// TODO: initializeHost();

#ifndef JVX_SPLASH_SUPPRESSED
			if (spl)
			{
				spl->showMessage(("Reading configuration <" + _command_line_parameters.configFilename + ">...").c_str(), Qt::AlignHCenter, QColor(0, 176, 240));
				qApp->processEvents();
			}
#endif
			cfg_filename_in_use = jvx_makeFilePath(_command_line_parameters.userPath, _command_line_parameters.configFilename);
			callConf.configModeFlags = JVX_CONFIG_MODE_FULL;
#ifndef JVX_SPLASH_SUPPRESSED
			configureFromFile(&callConf, cfg_filename_in_use,
				_command_line_parameters.configDirectories, (jvxHandle*)spl);
#else
			configureFromFile(&callConf, cfg_filename_in_use,
				_command_line_parameters.configDirectories, nullptr);

#endif

#ifndef JVX_SPLASH_SUPPRESSED
			if (spl)
			{
				spl->showMessage("Post boot processing...", Qt::AlignHCenter, QColor(0, 176, 240));
				qApp->processEvents();
			}
#endif
			
			// TODO: postbootup_specific();

#ifndef JVX_SPLASH_SUPPRESSED
			if (spl)
			{
				spl->showMessage("Creating windows...", Qt::AlignHCenter, QColor(0, 176, 240));
				qApp->processEvents();
			}
#endif

			// Update ui
			updateWindow((jvxCBitField)1 << JVX_UPDATE_WINDOW_CONFIG_READ_SHIFT);

#ifndef JVX_SPLASH_SUPPRESSED
			jvx_stop_splash_qt(spl, this);
#endif

		}
	}
}

#if 0
jvxErrorType 
uMainWindow::configureFromFile(std::string fName, jvxFlagTag flagtag)
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
	if(theHost.hThost)
	{
		resL = theHost.hThost->reference_tool(tpCrypt, &theCryptObj, 0, NULL);
		if((resL == JVX_NO_ERROR) && (theCryptObj))
		{
			resL = theCryptObj->request_specialization(reinterpret_cast<jvxHandle**>(&theCryptInst), NULL, NULL);
			assert(resL == JVX_NO_ERROR);
		}
	}

	// When reading, always pass cryppto instance
	//res = jvx_readContentFromFile(fName, content, theCryptInst);

	if(theCryptInst)
	{
		resL = theHost.hThost->return_reference_tool(tpCrypt, theCryptObj);
		assert(resL == JVX_NO_ERROR);
	}

	theCryptInst = NULL;
	theCryptObj = NULL;

	// ================================================================
	// Note: due to include files in config file, the crypt is currently not supported
	// ================================================================

	if(res == JVX_NO_ERROR)
	{
		IjvxObject* obj = NULL;
		IjvxConfigProcessor* cfgProc = NULL;

		this->theHost.hThost->reference_tool(tpConf, &obj, 0, NULL);
		if(obj)
		{
			obj->request_specialization((jvxHandle**)&cfgProc, NULL, NULL);
			if(cfgProc)
			{
				res = cfgProc->parseFile(fName.c_str());
				//res = cfgProc->parseTextField(content.c_str(), fName.c_str(), 0);
				if(res == JVX_NO_ERROR)
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
								res = theHost.hHost->request_hidden_interface(JVX_INTERFACE_CONFIGURATION, (jvxHandle**)&cfg);
								if ((res == JVX_NO_ERROR) && cfg)
								{
									cfg->put_configuration(cfgProc, datTmp1, flagtag, fName.c_str(), flagtag);
									this->theHost.hHost->return_hidden_interface(JVX_INTERFACE_CONFIGURATION, (jvxHandle*)cfg);
								}
								else
								{
									this->postMessage_outThread("Failed to obtain configuration interface from host.", JVX_QT_COLOR_WARNING);
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

								// Next, pass the reserved subsection to used host
								datTmp2 = NULL;
								cfgProc->getReferenceSubsectionCurrentSection_name(datTmp1, &datTmp2, JVX_QT_HOST_1_SECTION_COMPONENTS);
								if (datTmp2)
								{
									res = this->theHost.hHost->request_hidden_interface(JVX_INTERFACE_CONFIGURATION, (jvxHandle**)&cfg);
									if ((res == JVX_NO_ERROR) && cfg)
									{
										cfg->put_configuration(cfgProc, datTmp2, flagtag, fName.c_str(), 0);
										this->theHost.hHost->return_hidden_interface(JVX_INTERFACE_CONFIGURATION, (jvxHandle*)cfg);
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
						this->postMessage_outThread(("Opening config file <" + fName + "> failed.").c_str(), JVX_REPORT_PRIORITY_ERROR);
						break;
					case JVX_ERROR_PARSE_ERROR:
						this->postMessage_outThread(("Parse error in config file <" + fName + ">.").c_str(), JVX_REPORT_PRIORITY_ERROR);
						cfgProc->getParseError(&err);
						this->postMessage_outThread(err.errorDescription.c_str(), JVX_REPORT_PRIORITY_ERROR);
						break;
					default:
						this->postMessage_outThread(("Error in reading configuration from file <" + fName + ">.").c_str(), JVX_REPORT_PRIORITY_ERROR);
					}

					res = JVX_ERROR_INTERNAL;
				}
			}
			else
			{
				this->postMessage_outThread("Failed to config file processor tool specialization.", JVX_QT_COLOR_ERROR);
				res = JVX_ERROR_INTERNAL;
			}
			this->theHost.hThost->return_reference_tool(tpConf, obj);
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
		subWidgets.main.theWidget->inform_config_read_complete(fName.c_str());
		if (flagtag & JVX_PROPERTY_FLAG_ADD_TO_OVERLAY_MASK)
		{
			this->postMessage_outThread(("Successfully used configuration overlay file " + fName).c_str(), JVX_QT_COLOR_SUCCESS);
		}
		else
		{
			this->postMessage_outThread(("Successfully used configuration file " + fName).c_str(), JVX_QT_COLOR_SUCCESS);
		}
	}
	else
	{
		if (flagtag & JVX_PROPERTY_FLAG_ADD_TO_OVERLAY_MASK)
		{
			this->postMessage_outThread(("Error when using configuration overlay file " + fName + ": " + jvxErrorType_txt(res)).c_str(), JVX_QT_COLOR_WARNING);
		}
		else
		{
			this->postMessage_outThread(("Error when using configuration file " + fName + ": " + jvxErrorType_txt(res)).c_str(), JVX_QT_COLOR_WARNING);
		}
	}
	return(res);
}

jvxErrorType 
uMainWindow::configurationToFile(std::string& fName, jvxFlagTag flagtag)
{
	jvxErrorType res = JVX_NO_ERROR;
	std::string content;
	jvxComponentIdentification tpCrypt(JVX_COMPONENT_CRYPT, JVX_SIZE_UNSELECTED, JVX_SIZE_UNSELECTED);
	jvxComponentIdentification tpConf(JVX_COMPONENT_CONFIG_PROCESSOR, JVX_SIZE_UNSELECTED, JVX_SIZE_UNSELECTED);
	if(res == JVX_NO_ERROR)
	{
		IjvxObject* obj = NULL;
		IjvxConfigProcessor* cfgProc = NULL;
		IjvxConfiguration* cfg = NULL;
		IjvxConfigurationExtender* cfgExt = NULL;
		jvxConfigData* datTmp1 = NULL, * datTmp2 = NULL;
		jvxApiString fldStr;
		jvxValue val;
		this->theHost.hThost->reference_tool(tpConf,  &obj, 0, NULL);
		if(obj)
		{
			obj->request_specialization((jvxHandle**)&cfgProc, NULL, NULL);
			if(cfgProc)
			{
				if (flagtag & JVX_PROPERTY_FLAG_ADD_TO_OVERLAY_MASK)
				{
					datTmp1 = NULL;
					res = this->theHost.hHost->request_hidden_interface(JVX_INTERFACE_CONFIGURATION, (jvxHandle**)&cfg);
					if ((res == JVX_NO_ERROR) && cfg)
					{
						cfgProc->createEmptySection(&datTmp1, JVX_QT_HOST_1_SECTION_COMPONENTS);
						if (datTmp1)
						{
							cfg->get_configuration(cfgProc, datTmp1, flagtag);
						}
						
						this->theHost.hHost->return_hidden_interface(JVX_INTERFACE_CONFIGURATION, (jvxHandle*)cfg);

						res = cfgProc->printConfiguration(datTmp1, &fldStr, JVX_QT_HOST_1_PRINT_COMPACT_FORM);
						if ((res == JVX_NO_ERROR))
						{
							content = fldStr.std_str();
						}

						cfgProc->removeHandle(datTmp1);
					}
					else
					{
						this->postMessage_outThread("Failed to obtain configuration interface from host.", JVX_QT_COLOR_WARNING);
						res = JVX_ERROR_INTERNAL;
					}
				}
				else
				{
					cfg = NULL;
					this->theHost.hHost->request_hidden_interface(JVX_INTERFACE_CONFIGURATION, (jvxHandle**)&cfg);
					if (cfg)
					{
						datTmp1 = NULL;
						cfgProc->createEmptySection(&datTmp1, JVX_QT_HOST_1_SECTION_MAIN);

						cfgProc->createEmptySection(&datTmp2, JVX_QT_HOST_1_SECTION_COMPONENTS);
						if (datTmp2)
						{
							cfg->get_configuration(cfgProc, datTmp2, flagtag);
						}
						cfgProc->addSubsectionToSection(datTmp1, datTmp2);
						theHost.hHost->return_hidden_interface(JVX_INTERFACE_CONFIGURATION, (jvxHandle*)cfg);

						res = cfgProc->printConfiguration(datTmp1, &fldStr, JVX_QT_HOST_1_PRINT_COMPACT_FORM);
						if ((res == JVX_NO_ERROR))
						{
							content = fldStr.std_str();
						}

						cfgProc->removeHandle(datTmp1);
					}
				}
			}
			else
			{
				this->postMessage_outThread("Failed to config file processor tool specialization.", JVX_QT_COLOR_ERROR);
				res = JVX_ERROR_INTERNAL;
			}
			this->theHost.hThost->return_reference_tool(tpConf, obj);
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
		// Obtain encryption for text file
		IjvxObject* theCryptObj = NULL;
		IjvxCrypt* theCryptInst = NULL;
		if(theHost.hThost)
		{
			res = theHost.hThost->reference_tool(tpCrypt, &theCryptObj, 0, NULL);
			if((res == JVX_NO_ERROR) && (theCryptObj))
			{
				res = theCryptObj->request_specialization(reinterpret_cast<jvxHandle**>(&theCryptInst), NULL, NULL);
				assert(res == JVX_NO_ERROR);
			}
		}

		/*
		TODO 
		if(systemParams.encryptionConfigFiles)
		{
			res = jvx_writeContentToFile(fName, content, theCryptInst);
		}
		else
		{*/
			res = jvx_writeContentToFile(fName, content);
		//}
		
		if(theCryptInst)
		{
			jvxErrorType resL = theHost.hThost->return_reference_tool(tpCrypt, theCryptObj);
			assert(resL == JVX_NO_ERROR);
		}

		theCryptInst = NULL;
		theCryptObj = NULL;
	}
	return(res);
}
#endif