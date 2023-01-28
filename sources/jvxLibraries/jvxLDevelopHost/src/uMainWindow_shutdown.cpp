#include <QtWidgets/QMessageBox>
#include <QtWidgets/QListWidgetItem>
#include <QtGui/QCloseEvent>
#include "uMainWindow.h"
#include "uMainWindow_widgets.h"

// Includes for main host application
#include "jvxQtDevelopHostHookupEntries.h"

void
uMainWindow::fatalStop(const char* str1, const char* str2)
{
	QMessageBox msgBox(QMessageBox::Critical, str1, str2);
	msgBox.exec();
	exit(-1);
}

void
uMainWindow::closeEvent(QCloseEvent* event)
{
	// Shutdown sequencer
	jvxErrorType res = JVX_NO_ERROR;
	jvxApiString errMess;

     // Next, try to shutdown processing
	jvxSequencerStatus seqstat = JVX_SEQUENCER_STATUS_NONE;
	this->subWidgets.sequencer.theWidget->status_process(&seqstat, NULL);
	switch (seqstat)
	{
	case JVX_SEQUENCER_STATUS_IN_OPERATION:
		/*
		if (subWidgets.main.theWidget)
		{
			subWidgets.main.theWidget->inform_sequencer_about_to_stop();
		}*/
		this->subWidgets.sequencer.theWidget->control_stop_process();
		emit emit_close_app();
		event->ignore();
		return;
		break;
	case JVX_SEQUENCER_STATUS_NONE:
		break;

	default:
		// Come back in a moment to await sequencer callback in QT loop
		emit emit_close_app();
		event->ignore();
		return;
		break;
	}

	if (this->subWidgets.main.theWidget)
	{
		// Try to shutdown the mainCentral widget. That widget may refuse to shutdown
		res = this->subWidgets.main.theWidget->inform_close_triggered();
		if (res == JVX_ERROR_NOT_READY)
		{
			// If the sequencer is not yet ready, we need to come back later. We have no chance
			// to wait within the QT loop as callbacks which must be stopped need to wait for QT thread
			emit emit_close_app();
			event->ignore();
			return;
		}
	}

	free_ext_menu_system();

	res = shutdown_stop(&errMess, NULL);
	if (res != JVX_NO_ERROR)
	{
		assert(0);
	}

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
	commLine.unassign_args();
	theControl.terminate();

	event->accept();	
}


void
uMainWindow::release_dock_widgets()
{
	if (subWidgets.main.theWidget)
	{
		subWidgets.main.theWidget->terminate_extend_specific_widgets();
	}

	// ===========================================================================

	subWidgets.bridgeMenuComponents->removeMenues();
	terminate_jvxQtMenuComponentsBridge(subWidgets.bridgeMenuComponents);
	subWidgets.bridgeMenuComponents = NULL;

	// ===========================================================================

	subWidgets.connections.theWidget->terminate();
	removeDockWidget(subWidgets.connections.theWidgetD);
	subWidgets.connections.theWidgetD->setWidget(NULL);

	subWidgets.sequencer.theWidget->terminate();
	removeDockWidget(subWidgets.sequencer.theWidgetD);
	subWidgets.sequencer.theWidgetD->setWidget(NULL);

	removeDockWidget(subWidgets.realtimeViewer.plots.theWidgetD);
	subWidgets.realtimeViewer.plots.theWidgetD->setWidget(NULL);

	removeDockWidget(subWidgets.realtimeViewer.props.theWidgetD);
	subWidgets.realtimeViewer.props.theWidgetD->setWidget(NULL);

	removeDockWidget(subWidgets.messages.theWidgetD);
	subWidgets.messages.theWidgetD->setWidget(NULL);


	terminate_jvxQtConnectionWidget(subWidgets.connections.theWidget);
	subWidgets.connections.theWidget = NULL;

	terminate_jvxQtSequencerWidget(subWidgets.sequencer.theWidget);
	subWidgets.sequencer.theWidget = NULL;

	delete(subWidgets.sequencer.theWidgetD);
	delete(subWidgets.connections.theWidgetD);

	subWidgets.sequencer.theWidgetD = NULL;
	subWidgets.connections.theWidgetD = NULL;

	delete(subWidgets.realtimeViewer.plots.theWidget);
	delete(subWidgets.realtimeViewer.props.theWidget);
	delete(subWidgets.messages.theWidget);

	delete(subWidgets.realtimeViewer.plots.theWidgetD);
	delete(subWidgets.realtimeViewer.props.theWidgetD);
	delete(subWidgets.messages.theWidgetD);

	subWidgets.realtimeViewer.plots.theWidget = NULL;
	subWidgets.realtimeViewer.props.theWidget = NULL;
	subWidgets.messages.theWidget = NULL;

	subWidgets.realtimeViewer.plots.theWidgetD = NULL;
	subWidgets.realtimeViewer.props.theWidgetD = NULL;
	subWidgets.messages.theWidgetD = NULL;
}

void
uMainWindow::pre_release_dock_widgets()
{
	theControl.deactivate();

	subWidgets.sequencer.theWidget->deactivate();
	subWidgets.connections.theWidget->deactivate();
	
}

void
uMainWindow::close_app()
{
	this->close();
}
