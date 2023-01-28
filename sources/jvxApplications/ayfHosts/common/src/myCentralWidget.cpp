#include "myCentralWidget.h"
#include "jvx-helpers-product.h"
#include "typedefs/jvxTools/TjvxGenericWrapperApi.h"

#ifndef JVX_COMPILE_REMOTE_CONTROL
#endif

#ifdef JVX_PROJECT_NAMESPACE
namespace JVX_PROJECT_NAMESPACE {
#endif

myCentralWidget::myCentralWidget(QWidget* parent) : JVX_MAINCENTRAL_CLASSNAME(parent)
{
	thePropRef_host = NULL;
};

// ============================================================================================


void
#ifdef JVX_COMPILE_REMOTE_CONTROL
myCentralWidget::init_submodule(IjvxFactoryHost* theHost)
#else

myCentralWidget::init_submodule(IjvxHost* theHost)
#endif
{
	this->setupUi(this);
	JVX_MAINCENTRAL_CLASSNAME::init_submodule(theHost);

#ifndef JVX_COMPILE_REMOTE_CONTROL
	JVX_MAIN_CENTRAL_ATTACH_TREE_WIDGET(propertyTreeWidget, tabWidget_app);
#endif

}

void
myCentralWidget::terminate_submodule()
{
#ifndef JVX_COMPILE_REMOTE_CONTROL
	JVX_MAIN_CENTRAL_DETACH_TREE_WIDGET(propertyTreeWidget, tabWidget_app);
#endif 
}

void
myCentralWidget::inform_internals_have_changed(const jvxComponentIdentification& tp, IjvxObject* theObj, jvxPropertyCategoryType cat, jvxSize propId,
	bool onlyContent, const jvxComponentIdentification& tpTo, jvxPropertyCallPurpose purpose)
{
	jvxCBitField req;
	jvx_bitZSet(req, JVX_REPORT_REQUEST_UPDATE_WINDOW_SHIFT);
	this->inform_update_window(req);
}

void
myCentralWidget::inform_select(const jvxComponentIdentification& tp, IjvxAccessProperties* propRef)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxApiString astr;
	JVX_MAINCENTRAL_CLASSNAME::inform_select(tp, propRef);
	switch (tp.tp)
	{
	default:
		break;
	}
}

void
myCentralWidget::inform_active(const jvxComponentIdentification& tp, IjvxAccessProperties* propRef)
{
	jvxApiString astr;
	JVX_MAINCENTRAL_CLASSNAME::inform_active(tp, propRef);

#ifndef JVX_COMPILE_REMOTE_CONTROL
	JVX_MAIN_CENTRAL_TREE_WIDGET_ACTIVE(propRef, propertyTreeWidget);
#endif

	switch (tp.tp)
	{
	case JVX_COMPONENT_HOST:
		thePropRef_host = propRef;
		/*
		myWidgetWrapper.associateAutoWidgets(tab_main0,
			thePropRef_host, static_cast<IjvxQtSaWidgetWrapper_report*>(this), "host");
			*/
		break;

	case JVX_COMPONENT_AUDIO_NODE:
		switch (tp.slotid)
		{
		
		}
		break;
	case JVX_COMPONENT_SIGNAL_PROCESSING_NODE:
		switch (tp.slotid)
		{

		}
		break;
	default:
		break;
	}
}


void 
myCentralWidget::inform_inactive(const jvxComponentIdentification& tp, IjvxAccessProperties* propRef)
{
#ifndef JVX_COMPILE_REMOTE_CONTROL
	JVX_MAIN_CENTRAL_TREE_WIDGET_INACTIVE(propRef, propertyTreeWidget);
#endif

	switch (tp.tp)
	{
	case JVX_COMPONENT_HOST:
		myWidgetWrapper.deassociateAutoWidgets("host");
		thePropRef_host = NULL;
		break;
	case JVX_COMPONENT_AUDIO_NODE:
		switch (tp.slotid)
		{
		
		}
		break;
	case JVX_COMPONENT_SIGNAL_PROCESSING_NODE:
		switch (tp.slotid)
		{
		
		}
		break;
	default:
		break;
	}

	// Must follow, not precede!
	JVX_MAINCENTRAL_CLASSNAME::inform_inactive(tp, propRef);

}

void 
myCentralWidget::inform_update_window(jvxCBitField prio)
{
	std::string propDescr;
	jvxErrorType res = JVX_NO_ERROR;
	JVX_MAINCENTRAL_CLASSNAME::inform_update_window(prio);

#ifndef JVX_COMPILE_REMOTE_CONTROL
	JVX_MAIN_CENTRAL_TREE_WIDGET_UPDATE_WINDOW(propertyTreeWidget, prio);
#endif

	// =====================================================================
	// Do not update all properties on STATE report - this happens all 5 secs
	// =====================================================================
	jvxCBitField prio_skip = 0;
	jvx_bitZSet(prio_skip, JVX_UPDATE_WINDOW_SEQ_STATE_REPORT_SHIFT);
	if (prio == prio_skip)
	{
		// Skip update since too much traffic
		return;
	}
	// =====================================================================
	// =====================================================================

	if (jvx_bitTest(prio, JVX_REPORT_REQUEST_UPDATE_PROPERTY_VIEWER_FULL_SHIFT))
	{
		/*
		if (hoaNodeActive)
		{
			myWidgetWrapper.deassociateAutoWidgets("hoa2Dev_node");
			myWidgetWrapper.associateAutoWidgets(tab_main0, thePropRef_hoaNode,
				static_cast<IjvxQtSaWidgetWrapper_report*>(this),
				"hoa2Dev_node");
		}
		*/
	}

	// myWidgetWrapper.trigger_updateWindow("hoa2Dev_node");
}

void 
myCentralWidget::inform_update_window_periodic()
{
#ifndef JVX_COMPILE_REMOTE_CONTROL
	JVX_MAIN_CENTRAL_TREE_WIDGET_UPDATE_WINDOW_PERIODIC(propertyTreeWidget);
	
#endif
	// myWidgetWrapper.trigger_updateWindow_periodic("chanAnn_node");
	JVX_MAINCENTRAL_CLASSNAME::inform_update_window_periodic();
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
myCentralWidget::inform_sequencer_about_to_start()
{
	JVX_MAINCENTRAL_CLASSNAME::inform_sequencer_about_to_start();

}

void
myCentralWidget::inform_sequencer_stopped()
{
	JVX_MAINCENTRAL_CLASSNAME::inform_sequencer_stopped();
}

void
myCentralWidget::inform_about_to_shutdown()
{
	JVX_MAINCENTRAL_CLASSNAME::inform_about_to_shutdown();
	myWidgetWrapper.deassociateAutoWidgets("hoa_node");
}

jvxErrorType
myCentralWidget::reportPropertyGet(const char* tag, const char* propDescrptior, jvxHandle* ptrFld, jvxSize offset, jvxSize numElements, jvxDataFormat format, jvxErrorType res_in_call)
{
	return JVX_NO_ERROR;
}

jvxErrorType
myCentralWidget::reportPropertySet(const char* tag, const char* descror, jvxSize groupid, jvxErrorType res)
{
	return JVX_NO_ERROR;
}

#ifdef JVX_COMPILE_REMOTE_CONTROL

/*
jvxErrorType
myCentralWidget::report_property_binary_data(jvxComponentIdentification tp, jvxSize uId, jvxByte* payload,
	jvxPropertyTransferPriority prio)
{
	return JVX_NO_ERROR;
}

jvxErrorType
myCentralWidget::report_property_registered(jvxComponentIdentification tp, jvxSize uId)
{
	return JVX_NO_ERROR;
}

jvxErrorType
myCentralWidget::report_property_unregistered(jvxComponentIdentification tp, jvxSize uId)
{
	return JVX_NO_ERROR;
}
*/

void 
myCentralWidget::process_websocket_binary(jvxSize userData, jvxByte* payload, jvxPropertyTransferPriority prio)
{
}

jvxErrorType
myCentralWidget::report_nonblocking_delayed_update_complete(jvxSize uniqueId, jvxHandle* privatePtr)
{
	return JVX_NO_ERROR;
}

jvxErrorType 
myCentralWidget::report_nonblocking_delayed_update_terminated(jvxSize uniqueId, jvxHandle* privatePtr)
{
	return JVX_NO_ERROR;
}

#else

void
myCentralWidget::report_widget_closed(QWidget* theClosedWidget)
{}

void
myCentralWidget::report_widget_specific(jvxSize id, jvxHandle* spec)
{
}

#endif


#ifdef JVX_PROJECT_NAMESPACE
}
#endif
