#include "realtimeViewer.h"
#include "realtimeViewerPlots.h"
#include "realtimeViewerProperties.h"
#include "realtimeViewer_helpers.h"
#include <QtCore/QTimer>

realtimeViewer::realtimeViewer(QWidget* parent, jvxRealtimeViewerType cfg, jvxComponentIdentification* tpAllI): 
	QWidget(parent), CjvxRealtimeViewer(static_allocateData, static_deallocateData, static_copyData), tpAll(tpAllI)
{
	selectionUser.description = "Bdx Realtime Viewer";

	references.theHostRef = NULL;
	references.report = NULL;
	period_ms = 500;
	autoStart = false;

	selectionUser.idSelectTab = JVX_SIZE_UNSELECTED;
	this->configurationType = cfg;

}

void 
realtimeViewer::setPeriod_ms(jvxInt32 period_ms_in)
{
	period_ms = period_ms_in;
	if(myTimer->isActive())
	{
		newSelectionButton_stop();
		newSelectionButton_start();
	}
}

void
realtimeViewer::toggledAutoStart(bool tog)
{
	autoStart = tog;
}

realtimeViewer::~realtimeViewer()
{
	int i;
	for(i = 0; i < tabWidget_propsandplots->count(); i++)
	{
		QWidget* theW = tabWidget_propsandplots->widget(i);
		QVariant var = theW->property("BASE_REALTIMEVIEWER");
		if(var.isValid())
		{
			realtimeViewer_base* thePointer = reinterpret_cast<realtimeViewer_base*>(var.value<void *>());
			if(thePointer)
			{
				thePointer->cleanBeforeDelete();
			}
		}
	}
}

void 
realtimeViewer::init()
{
	this->setupUi(this);
	myTimer = new QTimer(this);
	connect(myTimer, SIGNAL(timeout()), this, SLOT(timerExpired()));
	this->updateWindow();
}

// ============================================================================================
// ============================================================================================

void 
realtimeViewer::setHostRef(IjvxHost* theHostRef, IjvxReport* report)
{
	references.theHostRef = theHostRef;
	references.report = report;
}

void 
realtimeViewer::newSelectionButton_addView()
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize num = 0;
	QString name = this->lineEdit_name->text();
	// Remove all widgets
	updateWindow_destroy();

	// Add the new entry
	_number_view_sections(&num);

	jvxHandle* data = NULL;
	jvxSize sz = 0;
	jvxRealtimePrivateMemoryDecoderEnum decodeId = JVX_REALTIME_PRIVATE_MEMORY_DECODER_NONE;
	_content.theAllocater( &data, &sz, decodeId);
	res = _insert_view_section_after(this->configurationType, name.toLatin1().constData(), selectionUser.idSelectTab, data,sz, decodeId);
	if(data)
	{
		_content.theDeallocater( data, sz, decodeId);
	}

	// Rebuild the widgets
	updateWindow_rebuild(selectionUser.idSelectTab+1);
}
 
void 
realtimeViewer::newSelectionButton_remove()
{
	jvxSize i;
	jvxSize num = 0;
	jvxErrorType res = JVX_NO_ERROR;
	std::string descr;
	jvxSize selectedBeforeRemove = JVX_SIZE_UNSELECTED;
	if(JVX_CHECK_SIZE_SELECTED(selectionUser.idSelectTab))
	{
		selectedBeforeRemove = tabWidget_propsandplots->currentIndex();

		updateWindow_destroy();

		// Remove the item
		res = _remove_view_section(selectedBeforeRemove);

		_number_view_sections(&num);
		if (selectedBeforeRemove >= num)
		{
			selectionUser.idSelectTab = num - 1;
		}
		else
		{
			selectionUser.idSelectTab = selectedBeforeRemove;
		}

		// Rebuild everything
		updateWindow_rebuild(selectionUser.idSelectTab);
	}
}

void 
realtimeViewer::newSelectionButton_start()
{
	timing.deltaT_period = 0;
	timing.deltaT_run = 0;
	JVX_GET_TICKCOUNT_US_SETREF(&timing.timeBase);
	timing.timeStamp_enter = JVX_GET_TICKCOUNT_US_GET_CURRENT(&timing.timeBase);
	timing.timeStamp_leave = JVX_GET_TICKCOUNT_US_GET_CURRENT(&timing.timeBase);

	_run_viewer();
	myTimer->start(period_ms);

	this->updateWindow();
}

void 
realtimeViewer::newSelectionButton_stop()
{
	myTimer->stop();
	_stop_viewer();

	this->updateWindow();
}

void 
realtimeViewer::newSelectionTab(int id)
{
	jvxSize i;
	jvxSize num = 0;
	jvxRealtimeViewerType tp;
	std::string descr;

	selectionUser.idSelectTab = this->tabWidget_propsandplots->currentIndex();

	CjvxRealtimeViewer::_number_view_sections(&num);
	for(i = 0; i < num; i++)
	{
		CjvxRealtimeViewer::_set_section_visible(i, false);
	}

	CjvxRealtimeViewer::_description_section(selectionUser.idSelectTab, tp, descr);
	this->lineEdit_name->setText(descr.c_str());
	CjvxRealtimeViewer::_set_section_visible(selectionUser.idSelectTab, true);
}

// =====================================================================================

/*
void 
realtimeViewer::updateWindow_insertTab(jvxRealtimeViewerType tp, int idInsert)
{
	realtimeViewerPlots* theNewPlot = NULL;
	realtimeViewerProperties* theNewProperties = NULL;
	jvxSize numS = 0;

	switch(tp)
	{
	case JVX_REALTIME_VIEWER_VIEW_PROPERTIES:
		this->_number_sections(&numS);
		theNewProperties = new realtimeViewerProperties(this, static_cast<CjvxRealtimeViewer*>(this), references.theHostRef, selectionUser.description, idInsert, references.report);
		theNewProperties->init();
		theNewProperties->updateWindow();
		tabWidget_propsandplots->insertTab(idInsert, theNewProperties, selectionUser.description.c_str());
		break;
	case JVX_REALTIME_VIEWER_VIEW_PLOTS:
		theNewPlot = new realtimeViewerPlots(this, static_cast<CjvxRealtimeViewer*>(this));
		theNewPlot->init();
		theNewPlot->updateWindow();
		tabWidget_propsandplots->insertTab(idInsert, theNewPlot, selectionUser.description.c_str());
		break;
	}
	selectionUser.idSelectTab = idInsert;
	this->updateWindow();
}

void 
realtimeViewer::updateWindow_removeTab(int idRemove)
{
	QWidget* theWidget = tabWidget_propsandplots->widget(idRemove);
	
	// Remove widget from tabwidget
	tabWidget_propsandplots->removeTab(idRemove);
	
	// Delete the corresponding widget
	delete(theWidget);

	if(tabWidget_propsandplots->count() == 0)
	{
		selectionUser.idSelectTab = -1;
	}
	else
	{
		selectionUser.idSelectTab = this->tabWidget_propsandplots->currentIndex();
	}
	this->updateWindow();
}
*/
void 
realtimeViewer::updateWindow()
{
	if(JVX_CHECK_SIZE_SELECTED(selectionUser.idSelectTab))
	{
		this->tabWidget_propsandplots->setCurrentIndex(JVX_SIZE_INT(selectionUser.idSelectTab));
	}
	if(this->_content.theState == JVX_STATE_ACTIVE)
	{
		this->pushButton_start->setEnabled(true);
		this->pushButton_stop->setEnabled(false);
	}
	else
	{
		this->pushButton_start->setEnabled(false);
		this->pushButton_stop->setEnabled(true);
	}
}


void 
realtimeViewer::timerExpired()
{
	timing.timeStamp_enter = JVX_GET_TICKCOUNT_US_GET_CURRENT(&timing.timeBase);
	timing.deltaT_period = timing.timeStamp_enter - timing.timeStamp_leave;
	_trigger_viewer();
	timing.timeStamp_leave = JVX_GET_TICKCOUNT_US_GET_CURRENT(&timing.timeBase);
	timing.deltaT_run = timing.timeStamp_leave - timing.timeStamp_enter;
}

void 
realtimeViewer::createAllWidgetsFromConfiguration()
{
	realtimeViewerPlots* theNewPlot = NULL;
	realtimeViewerProperties* theNewProperties = NULL;
	jvxSize numS = 0;

	if (selectionUser.idSelectTab == JVX_SIZE_UNSELECTED)
	{
		selectionUser.idSelectTab = tabWidget_propsandplots->currentIndex();
	}
	updateWindow_destroy();

	updateWindow_rebuild(JVX_SIZE_UNSELECTED);

/*	this->_number_sections(&numS);

	for(i = 0; i < numS; i++)
	{
		this->_description_section(i, tp,selectionUser.description);

		switch(tp)
		{
		case JVX_REALTIME_VIEWER_VIEW_PROPERTIES:
			theNewProperties = new realtimeViewerProperties(this, static_cast<CjvxRealtimeViewer*>(this), references.theHostRef, selectionUser.description, i, references.report);
			theNewProperties->init();
			theNewProperties->updateWindow();
			tabWidget_propsandplots->insertTab(i, theNewProperties, selectionUser.description.c_str());
			break;
		case JVX_REALTIME_VIEWER_VIEW_PLOTS:
			theNewPlot = new realtimeViewerPlots(this, static_cast<CjvxRealtimeViewer*>(this));
			theNewPlot->init();
			theNewPlot->updateWindow();
			tabWidget_propsandplots->insertTab(i, theNewPlot, selectionUser.description.c_str());
			break;
		}
	}
	this->updateWindow();*/
}

void 
realtimeViewer::updateAllWidgetsOnStateChange()
{
	realtimeViewerPlots* theNewPlot = NULL;
	realtimeViewerProperties* theNewProperties = NULL;
	jvxSize numS = 0;

	if (selectionUser.idSelectTab == JVX_SIZE_UNSELECTED)
	{
		selectionUser.idSelectTab = tabWidget_propsandplots->currentIndex();
	}
	updateWindow_destroy();

	updateWindow_rebuild(JVX_SIZE_UNSELECTED);

/*	this->_number_sections(&numS);

	for(i = 0; i < numS; i++)
	{
		this->_description_section(i, tp,selectionUser.description);

		switch(tp)
		{
		case JVX_REALTIME_VIEWER_VIEW_PROPERTIES:
			theNewProperties = new realtimeViewerProperties(this, static_cast<CjvxRealtimeViewer*>(this), references.theHostRef, selectionUser.description, i, references.report);
			theNewProperties->init();
			theNewProperties->updateWindow();
			tabWidget_propsandplots->insertTab(i, theNewProperties, selectionUser.description.c_str());
			break;
		case JVX_REALTIME_VIEWER_VIEW_PLOTS:
			theNewPlot = new realtimeViewerPlots(this, static_cast<CjvxRealtimeViewer*>(this));
			theNewPlot->init();
			theNewPlot->updateWindow();
			tabWidget_propsandplots->insertTab(i, theNewPlot, selectionUser.description.c_str());
			break;
		}
	}
	this->updateWindow();*/
}

void
realtimeViewer::updateWindow_destroy()
{
	// First, remove all tabs
	while(tabWidget_propsandplots->count())
	{
		QWidget* theWidget = tabWidget_propsandplots->widget(0);

		// Remove widget from tabwidget
		tabWidget_propsandplots->removeTab(0);

		// Delete the corresponding widget
		delete(theWidget);
	}
}

void
realtimeViewer::updateWindow_update(jvxBool fullUpdate)
{
	jvxSize i;
	QVariant theMainClassV;

	// First, remove all tabs
	jvxSize num = tabWidget_propsandplots->count();
	for(i = 0; i < num; i++)
	{
		QWidget* theWidget = tabWidget_propsandplots->widget((int)i);

		theMainClassV = theWidget->property("BASE_REALTIMEVIEWER");
		if(theMainClassV.isValid())
		{
			realtimeViewer_base* theBaseClass = NULL;
			jvxHandle* theClass = NULL;
			realtimeViewer_base::jvxRtvMainClassType theType = realtimeViewer_base::JVX_RTV_MAIN_CLASS_NONE;

			theBaseClass = reinterpret_cast<realtimeViewer_base*>(theMainClassV.value<void *>());
			if(theBaseClass)
			{
				theBaseClass->get_pointer_main_class(&theClass, &theType);
				if(theClass)
				{
					switch(theType)
					{
					case realtimeViewer_base::JVX_RTV_MAIN_CLASS_RTV_PROPERTIES:
						(reinterpret_cast<realtimeViewerProperties*>(theClass))->updateWindow_update(fullUpdate);
						break;
					default:
						// Do nothing yet
						break;
					}
				}
			}
		}
	}
}

void 
realtimeViewer::updateWindow_rebuild(jvxSize selectAfterRefresh)
{
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize num = 0;
	jvxRealtimeViewerType tp;
	std::string descr;
	realtimeViewerPlots* theNewPlot = NULL;
	realtimeViewerProperties* theNewProperties = NULL;

	// Rebuild
	_number_view_sections(&num);
	for(i = 0; i < num; i++)
	{
		_description_section(i, tp, descr);

		switch(tp)
		{
		case JVX_REALTIME_VIEWER_VIEW_PROPERTIES:

			theNewProperties = new realtimeViewerProperties(this, static_cast<CjvxRealtimeViewer*>(this), references.theHostRef, descr, i, references.report, tpAll);
			
			{
				QVariant var = static_cast<QWidget*>(theNewProperties)->property("BASE_REALTIMEVIEWER");
				if(var.isValid())
				{
					jvxHandle* ptr = var.value<jvxHandle*>();
				}
			}

			theNewProperties->init();
			theNewProperties->updateWindow();
			tabWidget_propsandplots->insertTab((int)i, theNewProperties, descr.c_str());
			break;
		case JVX_REALTIME_VIEWER_VIEW_PLOTS:
			theNewPlot = new realtimeViewerPlots(this, static_cast<CjvxRealtimeViewer*>(this), references.theHostRef, descr, i, references.report, tpAll);
			theNewPlot->init();
			theNewPlot->updateWindow();
			tabWidget_propsandplots->insertTab((int)i, theNewPlot, descr.c_str());
			break;
		}
	}

	selectionUser.idSelectTab = selectAfterRefresh;
	if(selectionUser.idSelectTab >= num)
	{
		selectionUser.idSelectTab  = (num-1);
	}
	if(JVX_CHECK_SIZE_UNSELECTED(selectionUser.idSelectTab))
	{
		if(num > 0)
		{
			selectionUser.idSelectTab = 0;
		}
	}
	this->updateWindow();
}

void 
realtimeViewer::updateWindow_content()
{
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize num = 0;
	std::string descr;
	realtimeViewerPlots* thePlot = NULL;
	realtimeViewerProperties* theProperties = NULL;

	// First, remove all tabs
	for(i = 0; i < tabWidget_propsandplots->count(); i ++)
	{
		QWidget* theWidget = tabWidget_propsandplots->widget((int)i);

		thePlot = dynamic_cast<realtimeViewerPlots*>(theWidget);
		if(thePlot)
		{
			// We would not need to do any updates in the plot qt
			// thePlot->updateWindow_redraw_all();
		}

		theProperties = dynamic_cast<realtimeViewerProperties*>(theWidget);
		if(theProperties)
		{
			theProperties->updateWindow_detach_ui();
			theProperties->updateWindow_attach_ui();
		}
	}
}

void 
realtimeViewer::newText_name()
{
	realtimeViewerPlots* thePlot = NULL;
	realtimeViewerProperties* theProperties = NULL;
	jvxSize num = 0;
	jvxSize numTabs = 0;

	QString name = lineEdit_name->text();
	int idTab = this->tabWidget_propsandplots->currentIndex();
	numTabs = tabWidget_propsandplots->count();
	if(idTab < numTabs)
	{
		CjvxRealtimeViewer::_number_view_sections(&num);
		if(idTab < num)
		{
			CjvxRealtimeViewer::_set_description_section(idTab, name.toLatin1().constData());
		}
		
		tabWidget_propsandplots->setTabText(idTab, name);
	}
}

jvxErrorType 
realtimeViewer::get_configuration(jvxCallManagerConfiguration* callConf, IjvxConfigProcessor* theReader, jvxHandle* ir, IjvxHost* theHost)
{
	std::string postfix = "";
	switch (this->configurationType)
	{
	case JVX_REALTIME_VIEWER_VIEW_PROPERTIES:
		postfix = "props";
		break;
	case JVX_REALTIME_VIEWER_VIEW_PLOTS:
		postfix = "plots";
		break;
	}

	jvxErrorType res = JVX_NO_ERROR;
	std::string txt = "rtv_autostart_" + postfix;
	JVX_GET_CONFIGURATION_INJECT_VALUE(res, txt.c_str(), ir, autoStart, theReader);

	return(_get_configuration(callConf, theReader, ir, theHost, static_getConfiguration, postfix));
};

jvxErrorType 
realtimeViewer::put_configuration(jvxCallManagerConfiguration* callConf, IjvxConfigProcessor* theReader, jvxHandle* ir, IjvxHost* theHost, const char* fName, int lineno, std::vector<std::string>& warnings)
{
	std::string postfix = "";
	switch (this->configurationType)
	{
	case JVX_REALTIME_VIEWER_VIEW_PROPERTIES:
		postfix = "props";
		break;
	case JVX_REALTIME_VIEWER_VIEW_PLOTS:
		postfix = "plots";
		break;
	}

	jvxErrorType res = JVX_NO_ERROR;
	this->autoStart = false;
	std::string txt = "rtv_autostart_" + postfix;

	res = _put_configuration(callConf, theReader, ir, theHost, fName, lineno, warnings, static_putConfiguration, postfix);
	JVX_PUT_CONFIGURATION_EXTRACT_VALUE(res, txt.c_str(), ir, &autoStart, theReader);
	if (autoStart)
	{
		this->newSelectionButton_start();
	}
	return (res);
};