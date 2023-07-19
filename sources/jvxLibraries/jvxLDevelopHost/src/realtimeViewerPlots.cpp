#include "realtimeViewerPlots.h"
#include "realtimeViewerOneLinearPlot.h"
#include "realtimeViewerTDSPlot.h"
//#include "realtimeViewerOnePolarPlot.h"
#include "realtimeViewer_helpers.h"

realtimeViewerPlots::realtimeViewerPlots(QWidget* parent, CjvxRealtimeViewer* theRef, IjvxHost* hostRef, std::string& descr, jvxSize sectionId, 
	IjvxReport* report, jvxComponentIdentification*tpAllI): QWidget(parent),
	realtimeViewer_base("realtimeViewerOneProperty", hostRef, theRef, report), tpAll(tpAllI)
{
	references.sectionId = sectionId;

	connect(this, SIGNAL(emit_removeMe_inThread(jvxSize, jvxSize, jvxSize)), this, SLOT(removeMe_inThread(jvxSize, jvxSize, jvxSize)), Qt::QueuedConnection);
	connect(this, SIGNAL(emit_updateWindow_redraw_all()), this, SLOT(updateWindow_redraw_all()), Qt::QueuedConnection);

	// Set reference for low level function calls
	this->setProperty("BASE_REALTIMEVIEWER", QVariant::fromValue(reinterpret_cast<void*>(static_cast<realtimeViewer_base*>(this))));
//	this->selectionUser.description = "No description";
//	selectionUser.idInsert = -1;
}

// ======================================================================================
// ======================================================================================

void 
realtimeViewerPlots::init()
{
	this->setupUi(this);
	updateWindow_redraw_all();
	updateWindow();
}

/**
 * Function to be called right before destructor is called.
 *///=========================================================================
void 
realtimeViewerPlots::cleanBeforeDelete()
{
	jvxSize i;
	for(i = 0; i < tabWidget_plots->count(); i++)
	{
		QWidget* theW = tabWidget_plots->widget((int)i);
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
realtimeViewerPlots::updateWindow()
{
	jvxRealtimeViewerGroupType tp;
	std::string descr;
	int id = tabWidget_plots->currentIndex();
	references.theRef->_description_group_in_section(references.sectionId, id, tp, descr);
	lineEdit_name->setText(descr.c_str());
}


/**
 * Function call from within the property to indicate that someone has pushed a remove button
 *///====================================================================
void 
realtimeViewerPlots::removeMe_delayed(jvxSize secId, jvxSize grpId, jvxSize itId)
{
	// Actual remove process comes in delayed not to pull out an object the function call lives in
	emit emit_removeMe_inThread(secId, grpId, itId);
}

/**
 * Actually remove the item. If successful, update the windows
 *///====================================================================
void 
realtimeViewerPlots::removeMe_inThread(jvxSize secId, jvxSize grpId, jvxSize itId)
{
	jvxErrorType res = JVX_NO_ERROR;
	res = references.theRef->_remove_item_in_group_in_section(secId, grpId, itId);
	if(res == JVX_NO_ERROR)
	{
		this->updateWindow_redraw_all();
	}
	else
	{
		references.report->report_simple_text_message("Failed to call <_remove_item_in_group_in_section> in <realtimeViewerProperties::removeGroup>", JVX_REPORT_PRIORITY_ERROR);
	}
}

void 
realtimeViewerPlots::updateWindow_redraw_all()
{
	jvxSize i;
	jvxSize numGs = 0, numIs = 0;
	jvxRealtimeViewerGroupType tp = JVX_REALTIME_VIEWER_GROUP_NONE;
	std::string descr;
	realtimeViewerOneLinearPlot* theLinearPlot = NULL;
	realtimeViewerTDSPlot* theLinearCircPlot = NULL;

	while(this->tabWidget_plots->count())
	{
		QWidget* theW = NULL;
		theW = tabWidget_plots->widget(0);
		tabWidget_plots->removeTab(0);
		if(theW)
		{
			delete(theW);
		}
	}

	// Next, add new items
	references.theRef->_number_groups_in_section(references.sectionId, &numGs);

	for(i = 0 ; i < numGs; i++)
	{
		tp = JVX_REALTIME_VIEWER_GROUP_NONE;
		references.theRef->_description_group_in_section(references.sectionId, i, tp, descr);

		switch(tp)
		{
		case JVX_REALTIME_VIEWER_GROUP_LINEAR_PLOT:
			theLinearPlot = new realtimeViewerOneLinearPlot(this, references.theHostRef, references.theRef, references.report, references.sectionId, i, tpAll);
			theLinearPlot->init();
			tabWidget_plots->addTab(theLinearPlot, descr.c_str());
			break;
		case JVX_REALTIME_VIEWER_GROUP_CIRC_PLOT:
			theLinearCircPlot = new realtimeViewerTDSPlot(this, references.theHostRef, references.theRef, references.report, references.sectionId, i, tpAll);
			theLinearCircPlot->init();
			tabWidget_plots->addTab(theLinearCircPlot, descr.c_str());
			break;
/*
		case JVX_REALTIME_VIEWER_GROUP_POLAR_PLOT:
			theLinearPlot = new realtimeViewerPolarPlot(this);
			tabWidget_plots->addItem(theLinearPlot);
			break;*/
		default:
			assert(0);
		}
	}
}


void 
realtimeViewerPlots::buttonPushed_add_lin_plot()
{
	jvxRealtimeViewerPropertyPlotGroup groupProps;
	groupProps.autox = true;
	groupProps.autoy = true;
	groupProps.xmax = 1.0;
	groupProps.xmin = 0.0;
	groupProps.ymax = 1.0;
	groupProps.ymin = -1.0;
	references.theRef->_insert_group_in_section(references.sectionId, JVX_SIZE_UNSELECTED, "", JVX_REALTIME_VIEWER_GROUP_LINEAR_PLOT, &groupProps, sizeof(jvxRealtimeViewerPropertyPlotGroup), 
		JVX_REALTIME_PRIVATE_MEMORY_LINEAR_PLOT_GROUP);
	updateWindow_redraw_all();
}

void 
realtimeViewerPlots::buttonPushed_add_circ_plot()
{
	jvxRealtimeViewerPropertyPlotGroup groupProps;
	groupProps.autox = true;
	groupProps.autoy = true;
	groupProps.xmax = 1.0;
	groupProps.xmin = 0.0;
	groupProps.ymax = 1.0;
	groupProps.ymin = -1.0;
	references.theRef->_insert_group_in_section(references.sectionId, JVX_SIZE_UNSELECTED, "", JVX_REALTIME_VIEWER_GROUP_CIRC_PLOT, &groupProps, sizeof(jvxRealtimeViewerPropertyPlotGroup), 
		JVX_REALTIME_PRIVATE_MEMORY_LINEAR_PLOT_GROUP);
	updateWindow_redraw_all();
}

void 
realtimeViewerPlots::buttonPushed_add_pol_plot()
{
	references.theRef->_insert_group_in_section(references.sectionId, JVX_SIZE_UNSELECTED, "", JVX_REALTIME_VIEWER_GROUP_POLAR_PLOT, NULL, 0, JVX_REALTIME_PRIVATE_MEMORY_DECODER_NONE);
	updateWindow_redraw_all();
}

void 
realtimeViewerPlots::buttonPushed_rem_current()
{
	int id = tabWidget_plots->currentIndex();
	QWidget* theW = tabWidget_plots->widget(id);
	QVariant var = theW->property("BASE_REALTIMEVIEWER");
	if(var.isValid())
	{
		realtimeViewer_base* thePointer = reinterpret_cast<realtimeViewer_base*>(var.value<void *>());
		if(thePointer)
		{
			thePointer->cleanBeforeDelete();
		}
	}
	references.theRef->_remove_group_in_section(references.sectionId, id);
	updateWindow_redraw_all();
}

void 
realtimeViewerPlots::newText_description()
{
	int id = tabWidget_plots->currentIndex();
	references.theRef->_set_description_group_in_section(references.sectionId, id, lineEdit_name->text().toLatin1().constData());
	updateWindow_redraw_all();
}

void
realtimeViewerPlots::newSelection_tab(int id)
{
	updateWindow();
}