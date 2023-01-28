#include <QtWidgets/QMessageBox>
#include <QtWidgets/QListWidgetItem>
#include <QtGui/QCloseEvent>
#include "uMainWindow.h"

// Includes for main host application
#include "jvxHJvx.h"

jvxErrorType 
uMainWindow::terminateSystem()
{
	return(JVX_NO_ERROR);
}

jvxErrorType
uMainWindow::terminateHost()
{
	jvxState theState = JVX_STATE_NONE;

	if(this->involvedComponents.theHost.hHost)
	{
		(static_cast<IjvxObject*>(this->involvedComponents.theHost.hHost))->state(&theState);
		//this->involvedComponents.theHost.hHost->state(&theState);
		
		if(theState > JVX_STATE_INIT)
		{
			this->involvedComponents.theHost.hHost->deactivate();

			//shutdown_specific();

			// Do not allow that host components are loaded via DLL
			this->involvedComponents.theHost.hHost->remove_component_load_blacklist(JVX_COMPONENT_HOST);

			this->involvedComponents.theHost.hHost->unselect();
			this->involvedComponents.theHost.hHost->terminate();
		}
		jvxHJvx_terminate(this->involvedComponents.theHost.hobject);
		this->involvedComponents.theHost.hobject = NULL;
		this->involvedComponents.theHost.hHost = NULL;
	}
	return(JVX_NO_ERROR);
}

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
	stop_process();
	wait_for_process_stop();
	jvxErrorType res = JVX_NO_ERROR;
	if(res == JVX_ERROR_NOT_READY)
	{
		emit emit_close_app();
		event->ignore();
	}
	else
	{
		//this->subWidgets.realtimeViewer.theWidget->newSelectionButton_stop();
		terminateHost();		
		event->accept();
	}
}

void 
uMainWindow::close_app()
{
	this->close();
}