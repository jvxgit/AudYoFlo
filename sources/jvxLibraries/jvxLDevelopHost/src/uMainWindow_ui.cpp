#include "uMainWindow.h"
#include "uMainWindow_widgets.h"

void 
uMainWindow::updateWindow(jvxCBitField prio)
{
	if(subWidgets.messages.added)
	{
		this->action_viewMessages->setChecked(true);
	}
	else
	{
		this->action_viewMessages->setChecked(false);
	}

	if(subWidgets.sequencer.added)
	{
		this->action_viewSequencer->setChecked(true);
	}
	else
	{
		this->action_viewSequencer->setChecked(false);
	}

	if(subWidgets.realtimeViewer.props.added)
	{
		this->action_viewRealtime_props->setChecked(true);
	}
	else
	{
		this->action_viewRealtime_props->setChecked(false);
	}
	if(subWidgets.realtimeViewer.plots.added)
	{
		this->action_viewRealtime_plots->setChecked(true);
	}
	else
	{
		this->action_viewRealtime_plots->setChecked(false);
	}

	if (subWidgets.connections.added)
	{
		this->action_connections->setChecked(true);
	}
	else
	{
		this->action_connections->setChecked(false);
	}

	if (subWidgets.connections.theWidget)
	{
		subWidgets.connections.theWidget->update_window();
	}

	if (subWidgets.sequencer.theWidget)
	{
		subWidgets.sequencer.theWidget->update_window();
	}

	if(subWidgets.main.theWidget)
	{
		subWidgets.main.theWidget->inform_update_window(prio);
	}

	if (subWidgets.bridgeMenuComponents)
	{
		subWidgets.bridgeMenuComponents->updateWindow();
		subWidgets.bridgeMenuComponents->updateWindow_tools();
	}

	theControl.update_window();

	if (theHostFeatures.allowOverlayOnly_config)
	{
		this->actionOpen->setText("Open Configuration Overlay");
		this->actionSave->setText("Save Configuration Overlay");
		this->actionSave_as->setText("Save Configuration Overlay As");
		if (_command_line_parameters.configFilename_ovlay.empty())
		{
			this->actionSave->setEnabled(false);
			this->actionSave->setToolTip("No configuration overlay file selected, use <Open as..>");
		}
		else
		{
			this->actionSave->setEnabled(true);
			this->actionSave->setToolTip(("Save configuration overlay in file " + _command_line_parameters.configFilename_ovlay).c_str());
		}
	}
	else
	{
		this->actionOpen->setText("Open Configuration");
		this->actionSave->setText("Save Configuration");
		this->actionSave_as->setText("Save Configuration As");
		if (_command_line_parameters.configFilename.empty())
		{
			this->actionSave->setEnabled(false);
			this->actionSave->setToolTip("No configuration file selected, use <Open as..>");
		}
		else
		{
			this->actionSave->setEnabled(true);
			this->actionSave->setToolTip(("Save configuration in file " + _command_line_parameters.configFilename_ovlay).c_str());
		}
	}

}

void 
uMainWindow::updateWindowPeriodic()
{
	if(subWidgets.main.theWidget)
	{
		subWidgets.main.theWidget->inform_update_window_periodic();
	}

	theControl.update_window_periodic();
}

void 
uMainWindow::triggered_viewMessages()
{
	if (!lockKeyShortcuts)
	{
		if (this->subWidgets.messages.added)
		{
			this->removeDockWidget(this->subWidgets.messages.theWidgetD);
			this->subWidgets.messages.theWidgetD->hide();
			this->subWidgets.messages.added = false;
		}
		else
		{
			this->addDockWidget(JVX_QT_POSITION_MESSAGE_DOCK_WIDGET, this->subWidgets.messages.theWidgetD);
			this->subWidgets.messages.theWidgetD->show();
			this->subWidgets.messages.theWidgetD->raise();
			this->subWidgets.messages.added = true;
		}
	}
}
	
void 
uMainWindow::triggered_viewParameters()
{
}
	
void 
uMainWindow::triggered_skipStateSelected()
{
	if(this->systemParams.skipStateSelected)
	{
		this->systemParams.skipStateSelected = false;
	}
	else
	{
		this->systemParams.skipStateSelected = true;
	}
	updateWindow();
}

void 
uMainWindow::triggered_toggleCryptConfig()
{
	if(systemParams.encryptionConfigFiles)
	{
		systemParams.encryptionConfigFiles = false;
	}
	else
	{
		systemParams.encryptionConfigFiles = true;
	}
	updateWindow();
}

void 
uMainWindow::triggered_viewRealtime_props()
{
	if (!lockKeyShortcuts)
	{
		if (this->subWidgets.realtimeViewer.props.added)
		{
			this->removeDockWidget(this->subWidgets.realtimeViewer.props.theWidgetD);
			this->subWidgets.realtimeViewer.props.theWidgetD->hide();
			this->subWidgets.realtimeViewer.props.added = false;
		}
		else
		{
			this->addDockWidget(JVX_QT_POSITION_REALTIMEVIEWER_DOCK_WIDGET, this->subWidgets.realtimeViewer.props.theWidgetD);
			this->subWidgets.realtimeViewer.props.theWidgetD->show();
			this->subWidgets.realtimeViewer.props.theWidgetD->raise();
			this->subWidgets.realtimeViewer.props.added = true;
		}

		this->arrangeTabsRightDock();
	}
}
	
void 
uMainWindow::triggered_viewRealtime_plots()
{
	if (!lockKeyShortcuts)
	{
		if (this->subWidgets.realtimeViewer.plots.added)
		{
			this->removeDockWidget(this->subWidgets.realtimeViewer.plots.theWidgetD);
			this->subWidgets.realtimeViewer.plots.theWidgetD->hide();
			this->subWidgets.realtimeViewer.plots.added = false;
		}
		else
		{
			this->addDockWidget(JVX_QT_POSITION_REALTIMEVIEWER_DOCK_WIDGET, this->subWidgets.realtimeViewer.plots.theWidgetD);
			this->subWidgets.realtimeViewer.plots.theWidgetD->show();
			this->subWidgets.realtimeViewer.plots.theWidgetD->raise();
			this->subWidgets.realtimeViewer.plots.added = true;
		}

		this->arrangeTabsRightDock();
	}
}

void
uMainWindow::triggered_viewConnections()
{
	if (!lockKeyShortcuts)
	{
		if (this->subWidgets.connections.added)
		{
			this->removeDockWidget(this->subWidgets.connections.theWidgetD);
			this->subWidgets.connections.theWidgetD->hide();
			this->subWidgets.connections.added = false;
		}
		else
		{
			this->addDockWidget(JVX_QT_POSITION_REALTIMEVIEWER_DOCK_WIDGET, this->subWidgets.connections.theWidgetD);
			this->subWidgets.connections.theWidgetD->show();
			this->subWidgets.connections.theWidgetD->raise();
			this->subWidgets.connections.added = true;
		}

		this->arrangeTabsRightDock();
	}
}

void 
uMainWindow::triggered_viewSequencer()
{
	if (!lockKeyShortcuts)
	{
		if (this->subWidgets.sequencer.added)
		{
			this->removeDockWidget(this->subWidgets.sequencer.theWidgetD);
			this->subWidgets.sequencer.theWidgetD->hide();
			this->subWidgets.sequencer.added = false;
		}
		else
		{
			this->addDockWidget(JVX_QT_POSITION_SEQUENCE_DOCK_WIDGET, this->subWidgets.sequencer.theWidgetD);
			this->subWidgets.sequencer.theWidgetD->show();
			this->subWidgets.sequencer.theWidgetD->raise();
			this->subWidgets.sequencer.added = true;
		}

		this->arrangeTabsRightDock();
	}
}

void 
uMainWindow::arrangeTabsRightDock()
{
	jvxCBitField caseC = 0;
	if (this->subWidgets.sequencer.added)
	{
		jvx_bitSet(caseC, 0);
	}
	if (this->subWidgets.realtimeViewer.props.added)
	{
		jvx_bitSet(caseC, 1);
	}
	if (this->subWidgets.realtimeViewer.plots.added)
	{
		jvx_bitSet(caseC, 2);
	}
	if (this->subWidgets.connections.added)
	{
		jvx_bitSet(caseC, 3);
	}

	switch (caseC)
	{
	case 0:
		// No link required since only one
		break;
	case 1:
		// No link required since only one
		break;
	case 2:
		// No link required since only one
		break;
	case 3:
		this->tabifyDockWidget(this->subWidgets.sequencer.theWidgetD, this->subWidgets.realtimeViewer.props.theWidgetD);
		break;
	case 4:
		// No link required since only one
		break;
	case 5:
		this->tabifyDockWidget(this->subWidgets.sequencer.theWidgetD, this->subWidgets.realtimeViewer.plots.theWidgetD);
		break;
	case 6:
		this->tabifyDockWidget(this->subWidgets.realtimeViewer.props.theWidgetD, this->subWidgets.realtimeViewer.plots.theWidgetD);
		break;
	case 7:
		this->tabifyDockWidget(this->subWidgets.sequencer.theWidgetD, this->subWidgets.realtimeViewer.props.theWidgetD);
		this->tabifyDockWidget(this->subWidgets.realtimeViewer.props.theWidgetD, this->subWidgets.realtimeViewer.plots.theWidgetD);
		break;
	case 8:
		// No link required since only one
		break;
	case 9:
		this->tabifyDockWidget(this->subWidgets.sequencer.theWidgetD, this->subWidgets.connections.theWidgetD);
		break;
	case 10:
		this->tabifyDockWidget(this->subWidgets.realtimeViewer.props.theWidgetD, this->subWidgets.connections.theWidgetD);
		break;
	case 11:
		this->tabifyDockWidget(this->subWidgets.sequencer.theWidgetD, this->subWidgets.realtimeViewer.props.theWidgetD);
		this->tabifyDockWidget(this->subWidgets.realtimeViewer.props.theWidgetD, this->subWidgets.connections.theWidgetD);
		break;
	case 12:
		this->tabifyDockWidget(this->subWidgets.realtimeViewer.plots.theWidgetD, this->subWidgets.connections.theWidgetD);
		break;
	case 13:
		this->tabifyDockWidget(this->subWidgets.sequencer.theWidgetD, this->subWidgets.realtimeViewer.plots.theWidgetD);
		this->tabifyDockWidget(this->subWidgets.realtimeViewer.plots.theWidgetD, this->subWidgets.connections.theWidgetD);
		break;
	case 14:
		this->tabifyDockWidget(this->subWidgets.realtimeViewer.props.theWidgetD, this->subWidgets.realtimeViewer.plots.theWidgetD);
		this->tabifyDockWidget(this->subWidgets.realtimeViewer.plots.theWidgetD, this->subWidgets.connections.theWidgetD);
		break;
	case 15:
		this->tabifyDockWidget(this->subWidgets.sequencer.theWidgetD, this->subWidgets.realtimeViewer.props.theWidgetD);
		this->tabifyDockWidget(this->subWidgets.realtimeViewer.props.theWidgetD, this->subWidgets.realtimeViewer.plots.theWidgetD);
		this->tabifyDockWidget(this->subWidgets.realtimeViewer.plots.theWidgetD, this->subWidgets.connections.theWidgetD);
		break;

	default:
		break;
	}

	setTabPosition(Qt::BottomDockWidgetArea, QTabWidget::North);
	setTabPosition(Qt::RightDockWidgetArea, QTabWidget::North);

	/*
	if(this->subWidgets.sequences.added)
	{
		if(this->subWidgets.realtimeViewer.props.added )
		{
			this->tabifyDockWidget(this->subWidgets.sequences.theWidgetD, this->subWidgets.realtimeViewer.props.theWidgetD);
			if(this->subWidgets.realtimeViewer.plots.added )
			{
				this->tabifyDockWidget(this->subWidgets.realtimeViewer.props.theWidgetD, this->subWidgets.realtimeViewer.plots.theWidgetD);
				
			}
			//setTabPositionRight(1);
		}
		else
		{
			if(this->subWidgets.realtimeViewer.plots.added )
			{
				this->tabifyDockWidget(this->subWidgets.sequences.theWidgetD, this->subWidgets.realtimeViewer.plots.theWidgetD);
				
			}
			//setTabPositionRight(0);
		}
	}
	else
	{
		if(this->subWidgets.realtimeViewer.props.added )
		{
			if(this->subWidgets.realtimeViewer.plots.added )
			{
				this->tabifyDockWidget(this->subWidgets.realtimeViewer.props.theWidgetD, this->subWidgets.realtimeViewer.plots.theWidgetD);
				
			}
			//setTabPositionRight(0);
		}
	}*/
}

void 
uMainWindow::dockWidgetReturned(Qt::DockWidgetArea area)
{
	if(area == JVX_QT_POSITION_SEQUENCE_DOCK_WIDGET)
	{
		if(this->subWidgets.bootupComplete)
		{
			arrangeTabsRightDock();
			/*
			if(this->subWidgets.sequences.added && this->subWidgets.realtimeViewer.added )
			{
				this->tabifyDockWidget(this->subWidgets.sequences.theWidgetD, this->subWidgets.realtimeViewer.theWidgetD);
				setTabPositionRight(0);
			}
			*/
		}
	}
}

void 
uMainWindow::report_widget_closed(QWidget *)
{
	this->updateWindow();
}

void
uMainWindow::report_widget_specific(jvxSize, jvxHandle *)
{
}
