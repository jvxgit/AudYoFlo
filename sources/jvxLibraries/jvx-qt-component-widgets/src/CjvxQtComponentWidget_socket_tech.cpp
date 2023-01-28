#include "CjvxQtComponentWidget_socket_tech.h"
#include "jvx-helpers.h"
#include "jvx-qt-helpers.h"

#include <QGridLayout>

// =============================================================
// =============================================================
// =============================================================

CjvxQtComponentWidget_socket_tech::CjvxQtComponentWidget_socket_tech(const char* additionalUiConfigTokens)
{
	theHostRef = NULL;
	theNetworkW = NULL;
	addConfigToken = additionalUiConfigTokens;
}

CjvxQtComponentWidget_socket_tech::~CjvxQtComponentWidget_socket_tech()
{
}

void
CjvxQtComponentWidget_socket_tech::init_submodule(IjvxHost* hostRef)
{
	theHostRef = hostRef;
	theNetworkW = NULL;
}

void
CjvxQtComponentWidget_socket_tech::terminate_submodule()
{
}


void
CjvxQtComponentWidget_socket_tech::updateWindow()
{
	widgetWrapper.trigger_updateWindow("socket_tech");
}

void
CjvxQtComponentWidget_socket_tech::updateWindow_periodic()
{
	widgetWrapper.trigger_updateWindow_periodic("socket_tech");
}

void
CjvxQtComponentWidget_socket_tech::start_show(IjvxAccessProperties* propRef, QFrame* placeHere, 
	const char* propPrefix)
{
	theNetworkW = new jvxQtSaNetworkTechnology(placeHere);

	QGridLayout* theLayout = NULL;
	theLayout = dynamic_cast<QGridLayout*>(placeHere->layout());
	if(theLayout == NULL)
	{
		theLayout = new QGridLayout; 
	}
	jvxQtSaWidgetsConfigure::jvxQtNetworkConfigure theCfgStruct;
	theCfgStruct.colorTxt = Qt::black;
	theCfgStruct.fontSize = 8;

	theNetworkW->init(&theCfgStruct);
	theNetworkW->setPropsPrefix(propPrefix);
	
	theNetworkW->setAdditonalConfigTokens(addConfigToken.c_str());

	theLayout->addWidget(theNetworkW, 0, 0, 1, 1);
	placeHere->setLayout(theLayout);
	placeHere->show();

	widgetWrapper.associateAutoWidgets(theNetworkW, propRef, 
		static_cast<IjvxQtSaWidgetWrapper_report*>(this),
		"socket_tech", true, JVX_WW_WIDGET_SEARCH_CURRENT_ONLY);
}

void
CjvxQtComponentWidget_socket_tech::stop_show(QFrame* placeHere)
{
	widgetWrapper.deassociateAutoWidgets("socket_tech");
	placeHere->hide();
	delete theNetworkW;
	theNetworkW = NULL;
}

// ===================================================================================

jvxErrorType 
CjvxQtComponentWidget_socket_tech::reportPropertySet(const char* tag, const char* propDescrptior, jvxSize groupid, jvxErrorType res_in_call)
{
	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxQtComponentWidget_socket_tech::reportPropertyGet(const char* tag, const char* propDescrptior,
	jvxHandle* ptrFld, jvxSize offset, jvxSize numElements, jvxDataFormat format,
	jvxErrorType res_in_call)
{
	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxQtComponentWidget_socket_tech::reportAllPropertiesAssociateComplete(const char* tag)
{
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxQtComponentWidget_socket_tech::reportPropertySpecific(jvxSaWidgetWrapperspecificReport, jvxHandle *)
{
	return JVX_NO_ERROR;
}