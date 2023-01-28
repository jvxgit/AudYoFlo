#include <QtWidgets/QMessageBox>
#include <QtWidgets/QListWidgetItem>
#include <QtWidgets/QFileDialog>
#include <QtCore/QThread>
#include "uMainWindow.h"
#include "jvxHost_config.h"
#include <QtWidgets/QSplashScreen>
#include <QDesktopWidget>
#include "jvx-qt-helpers.h"
#include "uMainWindow_widgets.h"

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
	jvxFlagTag flagtag = 0;
	jvxCallManagerConfiguration callConf;
	QString str = QFileDialog::getSaveFileName(this, tr("Save to Config File"), tr(".\\"), tr("(*.jvx)"));
	if (!str.isEmpty())
	{
		std::string fname = str.toLatin1().constData();

		if (theHostFeatures.allowOverlayOnly_config)
		{
			cfg_filename_olay_in_use = fname;
			if (!cfg_filename_olay_in_use.empty())
			{
				callConf.configModeFlags = JVX_CONFIG_MODE_OVERLAY;
				res = configureToFile(&callConf, cfg_filename_olay_in_use);
			}
		}
		else
		{
			cfg_filename_in_use = fname;
			if (!cfg_filename_in_use.empty())
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
	jvxState theStat = JVX_STATE_NONE;
	jvxBool requestHandled = false;
	jvxSequencerStatus seqstat = JVX_SEQUENCER_STATUS_NONE;
	QString QfName;
	if (fName)
		QfName = fName;

	reopen_config(fName, allowOverlayOnly_config);
}

void
uMainWindow::reopen_config(QString fName, bool allowOverlayOnly_config)
{
	QSplashScreen* spl = NULL;
	jvxErrorType res = JVX_NO_ERROR;
	jvxBool requestHandled = false;
	jvxCallManagerConfiguration callConf;
	jvxSequencerStatus  seqStat = JVX_SEQUENCER_STATUS_NONE;
	subWidgets.sequencer.theWidget->status_process(&seqStat, NULL);
	switch (seqStat)
	{
	case JVX_SEQUENCER_STATUS_IN_OPERATION:
		this->subWidgets.main.theWidget->inform_request_shutdown(&requestHandled);
		if (!requestHandled)
		{
			this->subWidgets.sequencer.theWidget->control_stop_process();
		}
		emit emit_reopen_config(fName, allowOverlayOnly_config);
		return;
		break;
	case JVX_SEQUENCER_STATUS_NONE:
		break;
	default:
		// Come back in a moment to await sequencer callback in QT loop
		emit emit_reopen_config(fName, allowOverlayOnly_config);
		return;
		break;
	}

	// Do an application specific shutdown
	//
#ifndef JVX_SPLASH_SUPPRESSED
	spl = jvx_start_splash_qt(theHostFeatures.config_ui.scalefac_splash,
		theHostFeatures.config_ui.pixmap_splash, 
		theHostFeatures.config_ui.ftsize_splash, 
		theHostFeatures.config_ui.italic_splash);
#endif

	if (allowOverlayOnly_config)
	{
		// Shutdown cleanly
		if (!fName.isEmpty())
		{
			cfg_filename_olay_in_use = fName.toLatin1().data();
		}

		if (!cfg_filename_olay_in_use.empty())
		{
			
#ifndef JVX_SPLASH_SUPPRESSED
			if(spl)
			{
				spl->showMessage("Shutting down host...", Qt::AlignHCenter, QColor(0, 176, 240));
				qApp->processEvents();
			}
#endif
			// Shutdown realtime viewer
			this->subWidgets.realtimeViewer.props.theWidget->newSelectionButton_stop();
			this->subWidgets.realtimeViewer.plots.theWidget->newSelectionButton_stop();

#ifndef JVX_SPLASH_SUPPRESSED
			if(spl)
			{
				spl->showMessage(("Reading configuration <" + _command_line_parameters.configFilename_ovlay + ">...").c_str(), Qt::AlignHCenter, QColor(0, 176, 240));
				qApp->processEvents();
			}
#endif
			callConf.configModeFlags = JVX_CONFIG_MODE_OVERLAY_READ;
#ifndef JVX_SPLASH_SUPPRESSED
			configureFromFile(&callConf, cfg_filename_olay_in_use,
				_command_line_parameters.configDirectories, (jvxHandle*)spl);
#else
			configureFromFile(&callConf, cfg_filename_olay_in_use,
				_command_line_parameters.configDirectories, nullptr);
#endif

#ifndef JVX_SPLASH_SUPPRESSED
			jvx_stop_splash_qt(spl, this);
#endif
		}
		updateWindow();
	}
	else
	{
#ifndef JVX_SPLASH_SUPPRESSED
		if(spl)
		{
			spl->showMessage("Stopping audio host...", Qt::AlignHCenter, QColor(0, 176, 240));
			qApp->processEvents();
		}
#endif

		jvxApiString errMess;
		res = shutdown_stop(&errMess, NULL);
		if (res != JVX_NO_ERROR)
		{
			assert(0);
		}

#ifndef JVX_SPLASH_SUPPRESSED
		if(spl)
		{
			spl->showMessage("Postprocessing audio host...", Qt::AlignHCenter, QColor(0, 176, 240));
			qApp->processEvents();
		}
#endif

		res = shutdown_postprocess(&errMess, NULL);
		if (res != JVX_NO_ERROR)
		{
			assert(0);
		}

#ifndef JVX_SPLASH_SUPPRESSED
		if(spl)
		{
			spl->showMessage("Deactivating audio host...", Qt::AlignHCenter, QColor(0, 176, 240));
			qApp->processEvents();
		}
#endif

		res = shutdown_deactivate(&errMess, NULL);
		if (res != JVX_NO_ERROR)
		{
			assert(0);
		}

#ifndef JVX_SPLASH_SUPPRESSED
		if(spl)
		{
			spl->showMessage("Terminating audio host...", Qt::AlignHCenter, QColor(0, 176, 240));
			qApp->processEvents();
		}
#endif

		res = shutdown_terminate(&errMess, NULL);
		if (res != JVX_NO_ERROR)
		{
			assert(0);
		}

		// Reached full shutdown status

		// =====================================================
		// Bootup cleanly
		// =====================================================

#ifndef JVX_SPLASH_SUPPRESSED
		if(spl)
		{
			spl->showMessage("Re-configuring audio host ...", Qt::AlignHCenter, QColor(0, 176, 240));
			qApp->processEvents();
		}
#endif

		res = this->boot_configure(&errMess, static_cast<IjvxCommandLine*>(&commLine), static_cast<IjvxReport*>(this),
			static_cast<IjvxReportOnConfig*>(this), static_cast<IjvxReportStateSwitch*>(this), NULL);
		if (res != JVX_NO_ERROR)
		{
			this->fatalStop("Fatal Error", errMess.c_str());
		}

		// ===========================================================
		// Initialize the host
		// ===========================================================

#ifndef JVX_SPLASH_SUPPRESSED
		if(spl)
		{
			spl->showMessage("Reinitializing audio host...", Qt::AlignHCenter, QColor(0, 176, 240));
			qApp->processEvents();
		}
#endif

		if (!fName.isEmpty())
		{
			_command_line_parameters.configFilename = fName.toLatin1().data();
		}

		res = boot_initialize(&errMess, NULL);
		if (res != JVX_NO_ERROR)
		{
			this->fatalStop("Fatal Error", errMess.c_str());
		}

#ifndef JVX_SPLASH_SUPPRESSED
		if(spl)
		{
			spl->showMessage("Activating audio host ...", Qt::AlignHCenter, QColor(0, 176, 240));
			qApp->processEvents();
		}
#endif

		res = boot_activate(&errMess, NULL);
		if (res != JVX_NO_ERROR)
		{
			this->fatalStop("Fatal Error", errMess.c_str());
		}

#ifndef JVX_SPLASH_SUPPRESSED
		if(spl)
		{
			spl->showMessage("Preparing audio host ...", Qt::AlignHCenter, QColor(0, 176, 240));
			qApp->processEvents();
		}
#endif

		res = boot_prepare(&errMess, spl);
		if (res != JVX_NO_ERROR)
		{
			this->fatalStop("Fatal Error", errMess.c_str());
		}

		// Update ui
		updateWindow();

		// Update sequencer/editor
		subWidgets.sequencer.theWidget->update_window_rebuild(); // <- also calls updateWindow

		emit emit_bootDelayed();
	}
#ifndef JVX_SPLASH_SUPPRESSED
	jvx_stop_splash_qt(spl, this);
#endif
}

void
uMainWindow::get_configuration_specific_ext(jvxCallManagerConfiguration* callConf, IjvxConfigurationExtender* cfgExt)
{
	jvxValue val;
	if (callConf->configModeFlags & JVX_CONFIG_MODE_FULL)
	{
		jvxCBitField selTabs = 0;
		bool vb = this->subWidgets.sequencer.theWidgetD->visibleRegion().isEmpty();
		if (!vb)
		{
			jvx_bitSet(selTabs, 0);
		}
		vb = subWidgets.realtimeViewer.props.theWidget->visibleRegion().isEmpty();
		if (!vb)
		{
			jvx_bitSet(selTabs, 1);
		}
		vb = subWidgets.realtimeViewer.plots.theWidget->visibleRegion().isEmpty();
		if (!vb)
		{
			jvx_bitSet(selTabs, 2);
		}
		vb = subWidgets.connections.theQWidget->visibleRegion().isEmpty();
		if (!vb)
		{
			jvx_bitSet(selTabs, 3);
		}

		val = selTabs;
		cfgExt->set_configuration_entry(
			JVX_COMMON_PROPERTIES_HOST_VIEW_TAB_BITFIELD,
			&val,
			JVX_CONFIG_SECTION_TYPE_ASSIGNMENTVALUE,
			0);

		val = subWidgets.realtimeViewer.props.theWidget->isVisible();
		cfgExt->set_configuration_entry(
			JVX_COMMON_PROPERTIES_HOST_VIEW_PROPS_VISIBLE,
			&val,
			JVX_CONFIG_SECTION_TYPE_ASSIGNMENTVALUE,
			0);

		val = subWidgets.realtimeViewer.plots.theWidget->isVisible();
		cfgExt->set_configuration_entry(
			JVX_COMMON_PROPERTIES_HOST_VIEW_PLOTS_VISIBLE,
			&val,
			JVX_CONFIG_SECTION_TYPE_ASSIGNMENTVALUE,
			0);

		val = subWidgets.sequencer.theQWidget->isVisible();
		cfgExt->set_configuration_entry(
			JVX_COMMON_PROPERTIES_HOST_VIEW_SEQ_VISIBLE,
			&val,
			JVX_CONFIG_SECTION_TYPE_ASSIGNMENTVALUE,
			0);

		val = subWidgets.connections.theQWidget->isVisible();
		cfgExt->set_configuration_entry(
			JVX_COMMON_PROPERTIES_HOST_VIEW_CONNECTIONS_VISIBLE,
			&val,
			JVX_CONFIG_SECTION_TYPE_ASSIGNMENTVALUE,
			0);

		// ======================================================================
		val = theHostFeatures.rtv_period_plots_ms;
		cfgExt->set_configuration_entry(
			JVX_COMMON_PROPERTIES_HOST_PERIOD_RTVIEWER_PLOTS,
			&val,
			JVX_CONFIG_SECTION_TYPE_ASSIGNMENTVALUE,
			0);

		val = theHostFeatures.rtv_period_props_ms;
		cfgExt->set_configuration_entry(
			JVX_COMMON_PROPERTIES_HOST_PERIOD_RTVIEWER_PROPS,
			&val,
			JVX_CONFIG_SECTION_TYPE_ASSIGNMENTVALUE,
			0);

		val = theHostFeatures.timeout_period_seq_ms;
		cfgExt->set_configuration_entry(
			JVX_COMMON_PROPERTIES_HOST_PERIOD_SEQ,
			&val,
			JVX_CONFIG_SECTION_TYPE_ASSIGNMENTVALUE,
			0);

		val = theHostFeatures.timeout_viewer_maincentral_ms;
		cfgExt->set_configuration_entry(
			JVX_COMMON_PROPERTIES_HOST_PERIOD_MAINCENTRAL,
			&val,
			JVX_CONFIG_SECTION_TYPE_ASSIGNMENTVALUE,
			0);

		val = theHostFeatures.timeout_period_seq_ms;
		cfgExt->set_configuration_entry(
			JVX_COMMON_PROPERTIES_HOST_PERIOD_SEQ,
			&val,
			JVX_CONFIG_SECTION_TYPE_ASSIGNMENTVALUE,
			0);

		val.assign(systemParams.auto_start);
		cfgExt->set_configuration_entry(JVX_COMMON_PROPERTIES_HOST_AUTOSTART, &val, JVX_CONFIG_SECTION_TYPE_ASSIGNMENTVALUE, 0);
	}
}

void
uMainWindow::get_configuration_specific(jvxCallManagerConfiguration* callConf,
	IjvxConfigProcessor* cfgProc, jvxConfigData* datTmp1)
{
	this->subWidgets.realtimeViewer.props.theWidget->get_configuration(callConf, cfgProc, datTmp1,  this->involvedHost.hHost);
	this->subWidgets.realtimeViewer.plots.theWidget->get_configuration(callConf, cfgProc, datTmp1, this->involvedHost.hHost);
}

void
uMainWindow::put_configuration_specific(jvxCallManagerConfiguration* callConf, IjvxConfigProcessor* cfg, jvxConfigData* data,  const char* fName)
{
	jvxSize i;
	std::vector<std::string > lst;
	this->subWidgets.realtimeViewer.props.theWidget->put_configuration(callConf, cfg, data, this->involvedHost.hHost, fName, 0, lst);
	for (i = 0; i < lst.size(); i++)
	{
		this->postMessage_outThread(("Warning when reading configuration for real-time viewer: " + lst[i]).c_str(), JVX_QT_COLOR_WARNING);
	}

	this->subWidgets.realtimeViewer.props.theWidget->createAllWidgetsFromConfiguration();

	this->subWidgets.realtimeViewer.plots.theWidget->put_configuration(callConf, cfg, data,  this->involvedHost.hHost, fName, 0, lst);
	for (i = 0; i < lst.size(); i++)
	{
		this->postMessage_outThread(("Warning when reading configuration for real-time viewer: " + lst[i]).c_str(), JVX_QT_COLOR_WARNING);
	}

	this->subWidgets.realtimeViewer.plots.theWidget->createAllWidgetsFromConfiguration();
}

void
uMainWindow::put_configuration_specific_ext(jvxCallManagerConfiguration* callConf, IjvxConfigurationExtender* cfgExt)
{
	jvxConfigSectionTypes secTp;
	jvxErrorType res = JVX_NO_ERROR;
	jvxValue val;
	jvxBool vis = false;

	if (callConf->configModeFlags & JVX_CONFIG_MODE_FULL)
	{
		secTp = JVX_CONFIG_SECTION_TYPE_ASSIGNMENTVALUE;
		res = cfgExt->get_configuration_entry(JVX_COMMON_PROPERTIES_HOST_PERIOD_RTVIEWER_PLOTS, reinterpret_cast<jvxHandle**>(&val), &secTp, 0);
		if (res == JVX_NO_ERROR)
		{
			val.toContent(&theHostFeatures.rtv_period_plots_ms);
			subWidgets.realtimeViewer.plots.theWidget->setPeriod_ms(theHostFeatures.rtv_period_plots_ms);
		}

		secTp = JVX_CONFIG_SECTION_TYPE_ASSIGNMENTVALUE;
		res = cfgExt->get_configuration_entry(JVX_COMMON_PROPERTIES_HOST_PERIOD_RTVIEWER_PROPS, reinterpret_cast<jvxHandle**>(&val), &secTp, 0);
		if (res == JVX_NO_ERROR)
		{
			val.toContent(&theHostFeatures.rtv_period_props_ms);
			subWidgets.realtimeViewer.props.theWidget->setPeriod_ms(theHostFeatures.rtv_period_props_ms);
		}

		secTp = JVX_CONFIG_SECTION_TYPE_ASSIGNMENTVALUE;
		res = cfgExt->get_configuration_entry(JVX_COMMON_PROPERTIES_HOST_PERIOD_MAINCENTRAL, reinterpret_cast<jvxHandle**>(&val), &secTp, 0);
		if (res == JVX_NO_ERROR)
		{
			val.toContent(&theHostFeatures.timeout_viewer_maincentral_ms);
		}

		secTp = JVX_CONFIG_SECTION_TYPE_ASSIGNMENTVALUE;
		res = cfgExt->get_configuration_entry(JVX_COMMON_PROPERTIES_HOST_PERIOD_SEQ, reinterpret_cast<jvxHandle**>(&val), &secTp, 0);
		if (res == JVX_NO_ERROR)
		{
			val.toContent(&theHostFeatures.timeout_period_seq_ms);
		}

		secTp = JVX_CONFIG_SECTION_TYPE_ASSIGNMENTVALUE;
		res = cfgExt->get_configuration_entry(JVX_COMMON_PROPERTIES_HOST_AUTOSTART, reinterpret_cast<jvxHandle**>(&val), &secTp, 0);
		if (res == JVX_NO_ERROR)
		{
			val.toContent(&systemParams.auto_start);
		}

		res = cfgExt->get_configuration_entry(JVX_COMMON_PROPERTIES_HOST_VIEW_PROPS_VISIBLE, reinterpret_cast<jvxHandle**>(&val), &secTp, 0);
		if (res == JVX_NO_ERROR)
		{
			val.toContent(&vis);
			if (!vis && this->subWidgets.realtimeViewer.props.added)
			{
				this->removeDockWidget(this->subWidgets.realtimeViewer.props.theWidgetD);
				this->subWidgets.realtimeViewer.props.theWidgetD->hide();
				this->subWidgets.realtimeViewer.props.added = false;
			}
			if (vis && !this->subWidgets.realtimeViewer.props.added)
			{
				this->addDockWidget(JVX_QT_POSITION_REALTIMEVIEWER_DOCK_WIDGET, this->subWidgets.realtimeViewer.props.theWidgetD);
				this->subWidgets.realtimeViewer.props.theWidgetD->show();
				this->subWidgets.realtimeViewer.props.theWidgetD->raise();
				this->subWidgets.realtimeViewer.props.added = true;
			}
		}
		res = cfgExt->get_configuration_entry(JVX_COMMON_PROPERTIES_HOST_VIEW_PLOTS_VISIBLE, reinterpret_cast<jvxHandle**>(&val), &secTp, 0);
		if (res == JVX_NO_ERROR)
		{
			val.toContent(&vis);
			if (!vis && this->subWidgets.realtimeViewer.plots.added)
			{
				this->removeDockWidget(this->subWidgets.realtimeViewer.plots.theWidgetD);
				this->subWidgets.realtimeViewer.plots.theWidgetD->hide();
				this->subWidgets.realtimeViewer.plots.added = false;
			}
			if (vis && !this->subWidgets.realtimeViewer.plots.added)
			{
				this->addDockWidget(JVX_QT_POSITION_REALTIMEVIEWER_DOCK_WIDGET, this->subWidgets.realtimeViewer.plots.theWidgetD);
				this->subWidgets.realtimeViewer.plots.theWidgetD->show();
				this->subWidgets.realtimeViewer.plots.theWidgetD->raise();
				this->subWidgets.realtimeViewer.plots.added = true;
			}
		}
		res = cfgExt->get_configuration_entry(JVX_COMMON_PROPERTIES_HOST_VIEW_CONNECTIONS_VISIBLE, reinterpret_cast<jvxHandle**>(&val), &secTp, 0);
		if (res == JVX_NO_ERROR)
		{
			val.toContent(&vis);
			if ((!vis) && this->subWidgets.connections.added)
			{
				this->removeDockWidget(this->subWidgets.connections.theWidgetD);
				this->subWidgets.connections.theWidgetD->hide();
				this->subWidgets.connections.added = false;
			}
			if ((vis) && !this->subWidgets.connections.added)
			{
				this->addDockWidget(JVX_QT_POSITION_REALTIMEVIEWER_DOCK_WIDGET, this->subWidgets.connections.theWidgetD);
				this->subWidgets.connections.theWidgetD->show();
				this->subWidgets.connections.theWidgetD->raise();
				this->subWidgets.connections.added = true;
			}
		}

		// Sequencer should be the last
		res = cfgExt->get_configuration_entry(JVX_COMMON_PROPERTIES_HOST_VIEW_SEQ_VISIBLE, reinterpret_cast<jvxHandle**>(&val), &secTp, 0);
		if (res == JVX_NO_ERROR)
		{
			val.toContent(&vis);
			if (!vis && this->subWidgets.sequencer.added)
			{
				this->removeDockWidget(this->subWidgets.sequencer.theWidgetD);
				this->subWidgets.sequencer.theWidgetD->hide();
				this->subWidgets.sequencer.added = false;
			}
			if (vis && !this->subWidgets.sequencer.added)
			{
				this->addDockWidget(JVX_QT_POSITION_SEQUENCE_DOCK_WIDGET, this->subWidgets.sequencer.theWidgetD);
				this->subWidgets.sequencer.theWidgetD->show();
				this->subWidgets.sequencer.theWidgetD->raise();
				this->subWidgets.sequencer.added = true;
			}
		}
		this->arrangeTabsRightDock();

		jvxBool widgetShown = false;
		res = cfgExt->get_configuration_entry(JVX_COMMON_PROPERTIES_HOST_VIEW_TAB_BITFIELD, reinterpret_cast<jvxHandle**>(&val), &secTp, 0);
		if (res == JVX_NO_ERROR)
		{
			jvxCBitField bf = 0;
			val.toContent(&bf);
			if (jvx_bitTest(bf, 0))
			{
				widgetShown = true;
				this->subWidgets.sequencer.theWidgetD->raise();
			}
			if (jvx_bitTest(bf, 1))
			{
				widgetShown = true;
				this->subWidgets.realtimeViewer.props.theWidgetD->raise();
			}
			if (jvx_bitTest(bf, 2))
			{
				widgetShown = true;
				this->subWidgets.realtimeViewer.plots.theWidgetD->raise();
			}
			if (jvx_bitTest(bf, 3))
			{
				widgetShown = true;
				this->subWidgets.connections.theWidgetD->raise();
			}
		}
		if (!widgetShown)
		{
			if (this->subWidgets.sequencer.added)
			{
				this->subWidgets.sequencer.theWidgetD->raise();
			}
			else if (this->subWidgets.realtimeViewer.props.added)
			{
				this->subWidgets.realtimeViewer.props.theWidgetD->raise();
			}
			else if (this->subWidgets.realtimeViewer.plots.added)
			{
				this->subWidgets.realtimeViewer.plots.theWidgetD->raise();
			}
			else if (this->subWidgets.connections.added)
			{
				this->subWidgets.connections.theWidgetD->raise();
			}
		}
	}
}
