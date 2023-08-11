#include "myCentralWidget.h"
#include <QTableWidget>
#include "jvx-helpers-product.h"
#include "typedefs/jvxTools/TjvxGenericWrapperApi.h"

#ifdef JVX_PROJECT_NAMESPACE
namespace JVX_PROJECT_NAMESPACE {
#endif

myCentralWidget::myCentralWidget(QWidget* parent) : mainCentral_host_ww(parent)
{
	// I added this line to allow to open png files from the resources in this library!
	// Found information here: https://doc.qt.io/qt-5/resources.html
	// ================================================================================
	Q_INIT_RESOURCE(jvxMeasureIr);
};

// ============================================================================================

void
myCentralWidget::init_submodule(IjvxHost* theHost)
{
	this->setupUi(this);
	mainCentral_host_ww::init_submodule(theHost);

	init_jvxQtMeasureControl(&widget_measure_ctrl, this);
	if (widget_measure_ctrl)
	{
		widget_measure_ctrl->init(theHost);
		widget_measure_ctrl->request_sub_interface(jvxQtInterfaceType::JVX_QT_SUB_INTERFACE_WWRAPPER, reinterpret_cast<jvxHandle**>(&widget_measure_ctrl_wwrapper));
		widget_measure_ctrl->request_sub_interface(jvxQtInterfaceType::JVX_QT_SUB_INTERFACE_CONFIG, reinterpret_cast<jvxHandle**>(&widget_measure_ctrl_config));
		widget_measure_ctrl->getMyQWidget(&widget_measure_ctrl_qt);	
		tabWidget_app->addTab(widget_measure_ctrl_qt, "Measure Control");
	}

	// Attach the generic tree widget
	JVX_MAIN_CENTRAL_ATTACH_TREE_WIDGET(propertyTreeWidget, tabWidget_app);

	jvxErrorType res = theHostRef->request_hidden_interface(JVX_INTERFACE_CONFIGURATION_EXTENDER, reinterpret_cast<jvxHandle**>(&cfgExtender));
	if ((res == JVX_NO_ERROR) && cfgExtender)
	{
		if (widget_measure_ctrl_config)
		{
			widget_measure_ctrl_config->registerConfigExtensions(cfgExtender);
		}
	}
}

// ===================================================================
void 
myCentralWidget::terminate_submodule()
{
	jvxSize i;

	if (widget_measure_ctrl)
	{
		if(widget_measure_ctrl_config)
		{
			widget_measure_ctrl_config->unregisterConfigExtensions(cfgExtender);
		}

		if (widget_measure_ctrl_wwrapper)
		{
			widget_measure_ctrl->return_sub_interface(jvxQtInterfaceType::JVX_QT_SUB_INTERFACE_WWRAPPER, reinterpret_cast<jvxHandle*>(widget_measure_ctrl_wwrapper));
			widget_measure_ctrl_wwrapper = nullptr;
		}
		if (widget_measure_ctrl_config)
		{
			widget_measure_ctrl->return_sub_interface(jvxQtInterfaceType::JVX_QT_SUB_INTERFACE_CONFIG, reinterpret_cast<jvxHandle*>(widget_measure_ctrl_config));
			widget_measure_ctrl_config = nullptr;
		}

		widget_measure_ctrl->terminate();
		terminate_jvxQtMeasureControl(widget_measure_ctrl);
	}

	JVX_MAIN_CENTRAL_DETACH_TREE_WIDGET(propertyTreeWidget, tabWidget_app);
}

// ====================================================================================================

void
myCentralWidget::inform_internals_have_changed(const jvxComponentIdentification& tp, IjvxObject* theObj, jvxPropertyCategoryType cat, jvxSize propId,
	bool onlyContent, const jvxComponentIdentification& tpTo, jvxPropertyCallPurpose purpose)
{
	jvxCBitField req;
	jvx_bitZSet(req, JVX_REPORT_REQUEST_UPDATE_WINDOW_SHIFT);
	this->inform_update_window(req);
}

void
myCentralWidget::inform_active(const jvxComponentIdentification& tp, IjvxAccessProperties* propRef)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxApiString tagstr;
	jvxBool relevantElementsActiveBefore = measureIrActive && channelAnnounceActive;
	mainCentral_host_ww::inform_active(tp, propRef);

	JVX_MAIN_CENTRAL_TREE_WIDGET_ACTIVE(propRef, propertyTreeWidget);

	switch (tp.tp)
	{
	case JVX_COMPONENT_AUDIO_NODE:
		propRef->descriptor_object(NULL, &tagstr);
		if(tagstr.std_str() == "audio_node/jvx-play-channel")
		{
			thePropRef_channelAnnounceNode = propRef;
			channelAnnounceActive = true;

			if (widget_measure_ctrl)
			{
				widget_measure_ctrl->addPropertyReference(thePropRef_channelAnnounceNode, "", "jvxAuNChannelAnnounce");
			}
		}
		break;
	case JVX_COMPONENT_SIGNAL_PROCESSING_NODE:
		propRef->descriptor_object(NULL, &tagstr);
		if (tagstr.std_str() == "signalprocessing_node/jvx-measure-ir")
		{		
			// Async module
			thePropRef_measureIrNode = propRef;
			measureIrActive = true;

			if (widget_measure_ctrl)
			{
				widget_measure_ctrl->addPropertyReference(thePropRef_measureIrNode, "", "jvxAuNMeasure");				
			}
		}
		break;
	default:
		break;
	}

	if (!relevantElementsActiveBefore && measureIrActive && channelAnnounceActive)
	{
		if (widget_measure_ctrl)
		{
			widget_measure_ctrl->activate();
		}
	}
}


void 
myCentralWidget::inform_inactive(const jvxComponentIdentification& tp, IjvxAccessProperties* propRef)
{
	jvxBool relevantElementsActiveBefore = measureIrActive && channelAnnounceActive;
	JVX_MAIN_CENTRAL_TREE_WIDGET_INACTIVE(propRef, propertyTreeWidget);

	switch (tp.tp)
	{
	case JVX_COMPONENT_AUDIO_NODE:
		if (propRef == thePropRef_channelAnnounceNode)
		{
			if (relevantElementsActiveBefore)
			{
				if (widget_measure_ctrl)
				{
					widget_measure_ctrl->deactivate();
				}
			}
			
			if (widget_measure_ctrl)
			{
				widget_measure_ctrl->removePropertyReference(thePropRef_channelAnnounceNode);
			}
			channelAnnounceActive = false;
			thePropRef_channelAnnounceNode = NULL;
		}
		break;
	case JVX_COMPONENT_SIGNAL_PROCESSING_NODE:
		if (propRef == thePropRef_measureIrNode)
		{
			if (relevantElementsActiveBefore)
			{
				if (widget_measure_ctrl)
				{
					widget_measure_ctrl->deactivate();
				}
			}			

			if (widget_measure_ctrl)
			{
				widget_measure_ctrl->removePropertyReference(thePropRef_measureIrNode);
			}
			measureIrActive = false;
			thePropRef_measureIrNode = NULL;
		}
		break;
	default:
		break;
	}

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

	if (widget_measure_ctrl)
	{
		widget_measure_ctrl->update_window(prio);
	}	
}

void 
myCentralWidget::inform_update_window_periodic()
{
	if (widget_measure_ctrl)
	{
		widget_measure_ctrl->update_window_periodic();
	}

	JVX_MAIN_CENTRAL_TREE_WIDGET_UPDATE_WINDOW_PERIODIC(propertyTreeWidget);
	
	mainCentral_host_ww::inform_update_window_periodic();
}

// ===========================================================================================

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
