#include <QtWidgets/QMessageBox>
#include <QtWidgets/QListWidgetItem>
#include <QtGui/QCloseEvent>
#include "uMainWindow.h"

#include "jvxQtHostWidgets.h"
// Includes for main host application
#ifdef JVX_HOST_USE_ONLY_STATIC_OBJECTS
#include "jvxHAppHost_nd.h"
#else
#include "jvxHAppHost.h"
#endif

#include "jvxQtAudioHostHookupEntries.h"

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
	jvxSequencerStatus  seqStat = JVX_SEQUENCER_STATUS_NONE;
	subWidgets.theSequencerWidget->status_process(&seqStat, NULL);
	switch(seqStat)
	{
	case JVX_SEQUENCER_STATUS_IN_OPERATION:
		this->on_request_startstop(JVX_START_STOP_INTENDED_STOP);
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

    if(this->subWidgets.main.theWidget)
    {
        // I do not why but on Mac OS X closeEvent may be called more than once
        res = this->subWidgets.main.theWidget->inform_close_triggered();
        if(res == JVX_ERROR_NOT_READY)
        {
            // If the mainCentral widget is not yet ready, we need to come back later. We have no chance
            // to wait within the QT loop as callbacks which must be stopped need to wait for QT thread
            emit emit_close_app();
            event->ignore();
            return;
        }

		theControl.unregister_functional_callback(id_period_seq_ms);
		theControl.unregister_functional_callback(id_rtview_period_maincentral_ms);
		theControl.unregister_functional_callback(id_autostart_id);

		jvxApiString errMess;
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
    }
    event->accept();
}

void
uMainWindow::close_app()
{
	this->close();
}

void
uMainWindow::preshutdown_widgets()
{
	if (theHostFeatures.viewerUpdateAlways)
	{
		subWidgets.theAudioDialog->post_allow_timer();
	}
	theControl.deactivate();
	// subWidgets.theAudioDialog->terminate(); <- does not exist
}

void
uMainWindow::shutdown_widgets()
{
	if (subWidgets.main.theWidget)
	{
		subWidgets.main.theWidget->terminate_extend_specific_widgets();
	}

	// ===============================================================================================
	
	subWidgets.bridgeMenuComponents->removeMenues();
	terminate_jvxQtMenuComponentsBridge(subWidgets.bridgeMenuComponents);
	subWidgets.bridgeMenuComponents = nullptr;

	// ===========================================================================

	theControl.unregister_action_widget(JVX_DESCRIPTION_CONNECTION_WIDGET, subWidgets.theConnectionsWidget, subWidgets.theConnectionsFrame);
	theControl.unregister_action_widget(JVX_DESCRIPTION_SEQUENCER_WIDGET, subWidgets.theSequencerWidget, subWidgets.theSequencerFrame);


	subWidgets.theConnectionsWidget->terminate();
	subWidgets.theConnectionsFrame->detachWidget(subWidgets.theConnectionsQWidget);

	terminate_jvxQtConnectionWidget(subWidgets.theConnectionsWidget);
	subWidgets.theConnectionsWidget = NULL;
	subWidgets.theConnectionsQWidget = NULL;

	terminate_jvxQtWidgetFrameDialog(subWidgets.theConnectionsFrame);
	subWidgets.theConnectionsFrame = NULL;
	subWidgets.theConnectionsFrameDialog = NULL;


	subWidgets.theSequencerWidget->terminate();
	subWidgets.theSequencerFrame->detachWidget(subWidgets.theSequencerQWidget);

	terminate_jvxQtSequencerWidget(subWidgets.theSequencerWidget);
	subWidgets.theSequencerWidget = NULL;
	subWidgets.theSequencerQWidget = NULL;

	terminate_jvxQtWidgetFrameDialog(subWidgets.theSequencerFrame);
	subWidgets.theSequencerFrame = NULL;
	subWidgets.theSequencerFrameDialog = NULL;
}
