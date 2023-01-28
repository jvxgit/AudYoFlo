#include "CjvxQtComponentWidget_socket_master_dev.h"
#include "jvx-helpers.h"
#include "jvx-qt-helpers.h"

#include <QGridLayout>

#define JVX_LOCAL_SHOW_HOSTNAME 0
#define JVX_LOCAL_SHOW_HOSTPORT 1
#define JVX_LOCAL_SHOW_CONNECT_DISCONNECT 2
#define JVX_LOCAL_SHOW_LABEL_LOST_FRAMES 3
#define JVX_LOCAL_SHOW_TEXTEDIT_LOST_FRAMES 4
#define JVX_LOCAL_SHOW_LABEL_UNSENT_FRAMES 5
#define JVX_LOCAL_SHOW_TEXTEDIT_UNSENT_FRAMES 6

#define JVX_LOCAL_SHOW_CONNECTSTATE_OFF 16

#define JVX_NUMBER_STATES_CONNECTION 6

#define JVX_TAG_CONNECT "socket_master_dev"

// =============================================================
// =============================================================
// =============================================================

CjvxQtComponentWidget_socket_master_dev::CjvxQtComponentWidget_socket_master_dev(const char* additionalUiConfigTokens)
{
	theHostRef = NULL;
	theNetworkW = NULL;
	addConfigToken = additionalUiConfigTokens;
}

CjvxQtComponentWidget_socket_master_dev::~CjvxQtComponentWidget_socket_master_dev()
{
}

void
CjvxQtComponentWidget_socket_master_dev::init_submodule(IjvxHost* hostRef)
{
	theHostRef = hostRef;
	theNetworkW = NULL;
}

void
CjvxQtComponentWidget_socket_master_dev::terminate_submodule()
{
}


void
CjvxQtComponentWidget_socket_master_dev::updateWindow()
{
	widgetWrapper.trigger_updateWindow(JVX_TAG_CONNECT);
}

void
CjvxQtComponentWidget_socket_master_dev::updateWindow_periodic()
{
	widgetWrapper.trigger_updateWindow_periodic(JVX_TAG_CONNECT);
}

void
CjvxQtComponentWidget_socket_master_dev::start_show(IjvxAccessProperties* propRef, QFrame* placeHere, const char* propPrefix)
{
	theNetworkW = new jvxQtSaNetworkMasterDevice(placeHere);

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
		JVX_TAG_CONNECT, true, JVX_WW_WIDGET_SEARCH_CURRENT_ONLY);
}

void
CjvxQtComponentWidget_socket_master_dev::stop_show(QFrame* placeHere)
{
	widgetWrapper.deassociateAutoWidgets(JVX_TAG_CONNECT);
	placeHere->hide();
	delete theNetworkW;
	theNetworkW = NULL;
}

// ===================================================================================

jvxErrorType
CjvxQtComponentWidget_socket_master_dev::reportPropertySet(const char* tag, const char* propDescrptior, jvxSize groupid, jvxErrorType res_in_call)
{
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxQtComponentWidget_socket_master_dev::reportPropertyGet(const char* tag, const char* propDescrptior,
	jvxHandle* ptrFld, jvxSize offset, jvxSize numElements, jvxDataFormat format,
	jvxErrorType res_in_call)
{
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxQtComponentWidget_socket_master_dev::reportAllPropertiesAssociateComplete(const char* tag)
{
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxQtComponentWidget_socket_master_dev::reportPropertySpecific(jvxSaWidgetWrapperspecificReport, jvxHandle *)
{
	return JVX_NO_ERROR;
}
