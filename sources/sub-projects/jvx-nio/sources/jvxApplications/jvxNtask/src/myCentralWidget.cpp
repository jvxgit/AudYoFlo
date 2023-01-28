#include "myCentralWidget.h"
#include <QTableWidget>
#include "jvx-helpers-product.h"
#include "typedefs/jvxTools/TjvxGenericWrapperApi.h"

#ifdef JVX_PROJECT_NAMESPACE
namespace JVX_PROJECT_NAMESPACE {
#endif

myCentralWidget::myCentralWidget(QWidget* parent) : mainCentral_host_ww(parent)
{

};

// ============================================================================================


void
myCentralWidget::init_submodule(IjvxHost* theHost)
{
	this->setupUi(this);
	mainCentral_host_ww::init_submodule(theHost);

	// Attach the generic tree widget
	JVX_MAIN_CENTRAL_ATTACH_TREE_WIDGET(propertyTreeWidget, tabWidget);
}

void
myCentralWidget::terminate_submodule()
{
	mainCentral_host_ww::terminate_submodule();

	JVX_MAIN_CENTRAL_DETACH_TREE_WIDGET(propertyTreeWidget, tabWidget);
}

void
myCentralWidget::inform_active(const jvxComponentIdentification& tp, IjvxAccessProperties* propRef)
{
	mainCentral_host_ww::inform_active(tp, propRef);
	JVX_MAIN_CENTRAL_TREE_WIDGET_ACTIVE(propRef, propertyTreeWidget);
}


void 
myCentralWidget::inform_inactive(const jvxComponentIdentification& tp, IjvxAccessProperties* propRef)
{
	JVX_MAIN_CENTRAL_TREE_WIDGET_INACTIVE(propRef, propertyTreeWidget);
	// Must follow, not precede!
	mainCentral_host_ww::inform_inactive(tp, propRef);

}

void 
myCentralWidget::inform_update_window(jvxCBitField prio)
{
	std::string propDescr;
	jvxErrorType res = JVX_NO_ERROR;
	mainCentral_host_ww::inform_update_window(prio);

	JVX_MAIN_CENTRAL_TREE_WIDGET_UPDATE_WINDOW(propertyTreeWidget, prio);
}

void 
myCentralWidget::inform_update_window_periodic()
{
	JVX_MAIN_CENTRAL_TREE_WIDGET_UPDATE_WINDOW_PERIODIC(propertyTreeWidget);
	mainCentral_host_ww::inform_update_window_periodic();
}

// ===========================================================================================

jvxErrorType 
myCentralWidget::get_configuration_ext(jvxCallManagerConfiguration* callConf, IjvxConfigProcessor* processor,
	jvxHandle* sectionWhereToAddAllSubsections)
{
	jvxSize i,j;
	jvxSize cnt = 0;
	jvxConfigData* theSec = NULL, *theSubSec = NULL, *theSubSubSec = NULL, *theSubSubSubSec = NULL;
	if (sectionWhereToAddAllSubsections)
	{
		if (processor)
		{
			
		}
	}
	return JVX_NO_ERROR;
}

jvxErrorType
myCentralWidget::put_configuration_ext(jvxCallManagerConfiguration* callConf, IjvxConfigProcessor* processor,
	jvxHandle* sectionToContainAllSubsectionsForMe, 
	const char* filename, jvxInt32 lineno)
{
	jvxSize i;// , j;
	jvxSize cnt = 0;
	jvxConfigData* theSec = NULL, *theSubSec = NULL, *theSubSubSec = NULL, *theSubSubSubSec = NULL;
	jvxApiString fldStr;
	jvxValue val = 0;
	jvxErrorType resL = JVX_NO_ERROR;
	if (sectionToContainAllSubsectionsForMe)
	{
		if (processor)
		{
			// ===========================================================================================
			// ===========================================================================================

		}
	}
	return JVX_NO_ERROR;
}


void
myCentralWidget::inform_config_read_complete(const char* fName)
{
	
}

void
myCentralWidget::inform_sequencer_about_to_stop()
{
	// This is solved in ASIO module	
}

void
myCentralWidget::inform_about_to_shutdown()
{
	mainCentral_host_ww::inform_about_to_shutdown();
	myWidgetWrapper.deassociateAutoWidgets("audio_node");
}

void
myCentralWidget::report_widget_closed(QWidget* theClosedWidget)
{
}

void
myCentralWidget::report_widget_specific(jvxSize id, jvxHandle* spec)
{
}

#ifdef JVX_PROJECT_NAMESPACE
}
#endif
