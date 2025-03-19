#include <QtWidgets/QMessageBox>
#include <QtWidgets/QListWidgetItem>
#include "uMainWindow.h"
#include "jvxHost_config.h"
#include "realtimeViewer_helpers.h"
#include <QtWidgets/QFileDialog>
#include <QtCore/QThread>
#include <QtWidgets/QSplashScreen>
#include "jvx-qt-helpers.h"

void
uMainWindow::saveConfigFile()
{
	jvxCallManagerConfiguration callConf;
	jvxErrorType res = JVX_NO_ERROR;
	if(!cfg_filename_in_use.empty())
	{
		callConf.configModeFlags = JVX_CONFIG_MODE_FULL;
		callConf.accessFlags = JVX_ACCESS_ROLE_DEFAULT;
		res = configureToFile(&callConf, cfg_filename_in_use);
	}
}

void
uMainWindow::saveConfigFileAs()
{
	jvxCallManagerConfiguration callConf;
	jvxErrorType res = JVX_NO_ERROR;
	QString str = QFileDialog::getSaveFileName(this, tr("Save to Config File"), tr(".\\"), tr("(*.jvx)"));
	if (!str.isEmpty())
	{
		callConf.configModeFlags = JVX_CONFIG_MODE_FULL;
		callConf.accessFlags = JVX_ACCESS_ROLE_DEFAULT;
		std::string cfg_filename_in_use_old = cfg_filename_in_use;
		cfg_filename_in_use = str.toLatin1().constData();
		res = configureToFile(&callConf, cfg_filename_in_use, cfg_filename_in_use_old);
	}
}

void
uMainWindow::openConfigFile()
{

	jvxErrorType res = JVX_NO_ERROR;
	jvxState theStat = JVX_STATE_NONE;
	QString str = QFileDialog::getOpenFileName(this, tr("Open Config File"), tr(".\\"), tr("(*.jvx)"));
	jvxBool requestHandled = false;
	jvxSequencerStatus seqstat = JVX_SEQUENCER_STATUS_NONE;

	if (!str.isEmpty())
	{
		jvxBool alreadyThere = false;
		std::string newFile = str.toLatin1().constData();
		this->_command_line_parameters.configFilename = jvx_extractFileFromFilePath(newFile);
		//jvx_removePathFromFile(newFile);		
		newFile = jvx_extractDirectoryFromFilePath(newFile);
		// If we have a direct specification, 
		for (auto elm : this->_command_line_parameters.configDirectories)
		{
			std::string str1 = jvx_replaceDirectorySeparators_toWindows(newFile, JVX_SEPARATOR_DIR_CHAR_THE_OTHER, JVX_SEPARATOR_DIR_CHAR);
			std::string str2 = jvx_replaceDirectorySeparators_toWindows(elm, JVX_SEPARATOR_DIR_CHAR_THE_OTHER, JVX_SEPARATOR_DIR_CHAR);
			if (str1 == str2)
			{
				alreadyThere = true;
				break;
			}
		}
		if (!alreadyThere)
		{
			this->_command_line_parameters.configDirectories.push_front(newFile);
		}
		// Shutdown cleanly

		reopen_config();
	}
}

void
uMainWindow::reopen_config()
{
	jvxErrorType res = JVX_NO_ERROR;
	QSplashScreen* spl = NULL;
	
	jvxSequencerStatus  seqStat = JVX_SEQUENCER_STATUS_NONE;
	subWidgets.theSequencerWidget->status_process(&seqStat, NULL);
	switch (seqStat)
	{
	case JVX_SEQUENCER_STATUS_IN_OPERATION:
		this->on_request_startstop(JVX_START_STOP_INTENDED_STOP);
		emit emit_reopen_config();
		return;
		break;
	case JVX_SEQUENCER_STATUS_NONE:
		break;
	default:
		// Come back in a moment to await sequencer callback in QT loop
		emit emit_reopen_config();
		return;
		break;
	}

#ifndef JVX_SPLASH_SUPPRESSED
	spl = jvx_start_splash_qt(theHostFeatures.config_ui.scalefac_splash, 
		theHostFeatures.config_ui.pixmap_splash, 
		theHostFeatures.config_ui.ftsize_splash, 
		theHostFeatures.config_ui.italic_splash);
#endif

	// Do an application specific shutdown
		//

#ifndef JVX_SPLASH_SUPPRESSED
	if (spl)
	{
		spl->showMessage("Stopping audio host...", Qt::AlignHCenter, QColor(0, 176, 240));
	}
#endif

	qApp->processEvents();

	jvxApiString errMess;
	res = shutdown_stop(&errMess, NULL);
	if (res != JVX_NO_ERROR)
	{
		assert(0);
	}

#ifndef JVX_SPLASH_SUPPRESSED
	if (spl)
	{
		spl->showMessage("Postprocessing audio host...", Qt::AlignHCenter, QColor(0, 176, 240));
	}
#endif

	qApp->processEvents();

	res = shutdown_postprocess(&errMess, NULL);
	if (res != JVX_NO_ERROR)
	{
		assert(0);
	}

#ifndef JVX_SPLASH_SUPPRESSED
	if (spl)
	{
		spl->showMessage("Deactivating audio host...", Qt::AlignHCenter, QColor(0, 176, 240));
	}
#endif

	qApp->processEvents();

	res = shutdown_deactivate(&errMess, NULL);
	if (res != JVX_NO_ERROR)
	{
		assert(0);
	}

#ifndef JVX_SPLASH_SUPPRESSED
	if (spl)
	{
		spl->showMessage("Terminating audio host...", Qt::AlignHCenter, QColor(0, 176, 240));
	}
#endif

	qApp->processEvents();

	res = shutdown_terminate(&errMess, NULL);
	if (res != JVX_NO_ERROR)
	{
		assert(0);
	}

	/*
	res = shutdown_unconfigure(&errMess, NULL);
	if (res != JVX_NO_ERROR)
	{
		assert(0);
	}

	res = boot_configure(&errMess, static_cast<IjvxCommandLine*>(&commLine), static_cast<IjvxReport*>(this),
		static_cast<IjvxReportOnConfig*>(this), static_cast<IjvxReportStateSwitch*>(this), NULL);
	if (res != JVX_NO_ERROR)
	{
		assert(0);
	}
	*/

#ifndef JVX_SPLASH_SUPPRESSED
	if (spl)
	{
		spl->showMessage("Reinitializing audio host...", Qt::AlignHCenter, QColor(0, 176, 240));
	}
#endif
	// =====================================================
	// Bootup cleanly
	// =====================================================
	// ===========================================================
	// Initialize the host
	// ===========================================================

	res = boot_initialize(&errMess, NULL);
	if (res != JVX_NO_ERROR)
	{
		this->fatalStop("Fatal Error", errMess.c_str());
	}

#ifndef JVX_SPLASH_SUPPRESSED
	if (spl)
	{
		spl->showMessage("Activating audio host ...", Qt::AlignHCenter, QColor(0, 176, 240));
	}
#endif

	qApp->processEvents();
	res = boot_activate(&errMess, NULL);
	if (res != JVX_NO_ERROR)
	{
		this->fatalStop("Fatal Error", errMess.c_str());
	}

#ifndef JVX_SPLASH_SUPPRESSED
	if (spl)
	{
		spl->showMessage("Preparing audio host ...", Qt::AlignHCenter, QColor(0, 176, 240));
	}

#endif

	qApp->processEvents();

	res = boot_prepare(&errMess, spl);
	if (res != JVX_NO_ERROR)
	{
		this->fatalStop("Fatal Error", errMess.c_str());
	}

	// Update ui
	updateWindow((jvxCBitField)1 << JVX_REPORT_REQUEST_UPDATE_WINDOW_SHIFT);

	// Trigger boot Delayed callback (to leave constructor)
	emit emit_bootDelayed();

#ifndef JVX_SPLASH_SUPPRESSED
	jvx_stop_splash_qt(spl, this);
#endif
}

jvxErrorType
uMainWindow::rtv_get_configuration(jvxCallManagerConfiguration* callConf,
	IjvxConfigProcessor* theReader, jvxHandle* ir, IjvxHost* theHost, CjvxRealtimeViewer& theViewer, jvxRealtimeViewerType configurationType)
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
		assert(0);
	}
	return(theViewer._get_configuration(callConf,  theReader, ir, theHost, static_getConfiguration, postfix));
};

jvxErrorType
uMainWindow::rtv_put_configuration(jvxCallManagerConfiguration* callConf, 
	IjvxConfigProcessor* theReader, jvxHandle* ir, IjvxHost* theHost, const char* fName, int lineno, std::vector<std::string>& warnings, CjvxRealtimeViewer& theViewer, jvxRealtimeViewerType configurationType)
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
			assert(0);
		}
		return(theViewer._put_configuration(callConf, theReader, ir, theHost, fName, lineno, warnings, static_putConfiguration, postfix));
	};

void
uMainWindow::get_configuration_specific_ext(jvxCallManagerConfiguration* callConf, IjvxConfigurationExtender* ext)
{
	jvxValue val;

	val.assign(theHostFeatures.rtv_period_plots_ms);
	ext->set_configuration_entry(JVX_COMMON_PROPERTIES_HOST_PERIOD_RTVIEWER_PLOTS, &val, JVX_CONFIG_SECTION_TYPE_ASSIGNMENTVALUE, 0);

	val.assign(theHostFeatures.rtv_period_props_ms); 
	ext->set_configuration_entry(JVX_COMMON_PROPERTIES_HOST_PERIOD_RTVIEWER_PROPS, &val, JVX_CONFIG_SECTION_TYPE_ASSIGNMENTVALUE, 0);

	val.assign(systemParams.auto_start);
	ext->set_configuration_entry(JVX_COMMON_PROPERTIES_HOST_AUTOSTART, &val, JVX_CONFIG_SECTION_TYPE_ASSIGNMENTVALUE, 0);

	val.assign(systemParams.auto_stop);
	ext->set_configuration_entry(JVX_COMMON_PROPERTIES_HOST_AUTOSTOP, &val, JVX_CONFIG_SECTION_TYPE_ASSIGNMENTVALUE, 0);

	val.assign(theHostFeatures.timeout_period_seq_ms);
	ext->set_configuration_entry(JVX_COMMON_PROPERTIES_HOST_PERIOD_SEQ, &val, JVX_CONFIG_SECTION_TYPE_ASSIGNMENTVALUE, 0);

	val.assign(theHostFeatures.timeout_viewer_maincentral_ms);
	ext->set_configuration_entry(JVX_COMMON_PROPERTIES_HOST_PERIOD_MAINCENTRAL, &val, JVX_CONFIG_SECTION_TYPE_ASSIGNMENTVALUE, 0);
}

void
uMainWindow::get_configuration_specific(jvxCallManagerConfiguration* callConf, IjvxConfigProcessor* cfg, jvxConfigData* data)
{
	jvxErrorType res = JVX_NO_ERROR;
	this->rtv_get_configuration(callConf, cfg, data, this->involvedHost.hHost, viewer_props, JVX_REALTIME_VIEWER_VIEW_PROPERTIES);
	this->rtv_get_configuration(callConf, cfg, data, this->involvedHost.hHost, viewer_plots, JVX_REALTIME_VIEWER_VIEW_PLOTS);
}

void
uMainWindow::put_configuration_specific(jvxCallManagerConfiguration* callConf,
	IjvxConfigProcessor* cfg, jvxConfigData* data,  const char* fName)
{
	jvxSize i;
	std::vector<std::string > warnings;
	this->rtv_put_configuration(callConf, cfg, data, this->involvedHost.hHost, fName, 0, warnings, viewer_props, JVX_REALTIME_VIEWER_VIEW_PROPERTIES);
	for (i = 0; i < warnings.size(); i++)
	{
		this->postMessage_outThread(("Warning when reading configuration for real-time viewer: " + warnings[i]).c_str(), JVX_REPORT_PRIORITY_WARNING);
	}

	//this->subWidgets.realtimeViewer.props.theWidget->createAllWidgetsFromConfiguration();

	this->rtv_put_configuration(callConf, cfg, data, this->involvedHost.hHost, fName, 0, warnings, viewer_plots, JVX_REALTIME_VIEWER_VIEW_PLOTS);
	for (i = 0; i < warnings.size(); i++)
	{
		this->postMessage_outThread(("Warning when reading configuration for real-time viewer: " + warnings[i]).c_str(), JVX_REPORT_PRIORITY_WARNING);
	}
}

void
uMainWindow::put_configuration_specific_ext(jvxCallManagerConfiguration* callConf, IjvxConfigurationExtender* ext)
{
	jvxValue val;
	jvxConfigSectionTypes myCfgTp = JVX_CONFIG_SECTION_TYPE_UNKNOWN;
	jvxErrorType res = JVX_NO_ERROR;

	myCfgTp = JVX_CONFIG_SECTION_TYPE_ASSIGNMENTVALUE;
	res = ext->get_configuration_entry(JVX_COMMON_PROPERTIES_HOST_AUTOSTART, &val, &myCfgTp, 0);
	if (res == JVX_NO_ERROR)
	{
		val.toContent(&systemParams.auto_start);
	}

	myCfgTp = JVX_CONFIG_SECTION_TYPE_ASSIGNMENTVALUE;
	res = ext->get_configuration_entry(JVX_COMMON_PROPERTIES_HOST_AUTOSTOP, &val, &myCfgTp, 0);
	if (res == JVX_NO_ERROR)
	{
		val.toContent(&systemParams.auto_stop);
	}

	myCfgTp = JVX_CONFIG_SECTION_TYPE_ASSIGNMENTVALUE;
	res = ext->get_configuration_entry(JVX_COMMON_PROPERTIES_HOST_PERIOD_SEQ, &val, &myCfgTp, 0);
	if (res == JVX_NO_ERROR)
	{
		val.toContent(&theHostFeatures.timeout_period_seq_ms);
	}

	myCfgTp = JVX_CONFIG_SECTION_TYPE_ASSIGNMENTVALUE;
	res = ext->get_configuration_entry(JVX_COMMON_PROPERTIES_HOST_PERIOD_MAINCENTRAL, &val, &myCfgTp, 0);
	if (res == JVX_NO_ERROR)
	{
		val.toContent(&theHostFeatures.timeout_viewer_maincentral_ms);
	}

	myCfgTp = JVX_CONFIG_SECTION_TYPE_ASSIGNMENTVALUE;
	res = ext->get_configuration_entry(JVX_COMMON_PROPERTIES_HOST_PERIOD_RTVIEWER_PLOTS, reinterpret_cast<jvxHandle**>(&val), &myCfgTp, 0);
	if (res == JVX_NO_ERROR)
	{
		val.toContent(&theHostFeatures.rtv_period_plots_ms);
	}

	myCfgTp = JVX_CONFIG_SECTION_TYPE_ASSIGNMENTVALUE;
	res = ext->get_configuration_entry(JVX_COMMON_PROPERTIES_HOST_PERIOD_RTVIEWER_PROPS, reinterpret_cast<jvxHandle**>(&val), &myCfgTp, 0);
	if (res == JVX_NO_ERROR)
	{
		val.toContent(&theHostFeatures.rtv_period_props_ms);
	}
}