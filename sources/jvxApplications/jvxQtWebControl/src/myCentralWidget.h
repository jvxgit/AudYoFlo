#ifndef __MYCENTRALWIDGET_H__
#define __MYCENTRALWIDGET_H__

#include "jvx.h"
#include "interfaces/qt/mainWindow_UiExtension.h"
#include "ui_myCentralWidget.h"

#include "mainCentral_hostfactory_ww_http.h"

#ifdef JVX_PROJECT_NAMESPACE
namespace JVX_PROJECT_NAMESPACE {
#endif

class myCentralWidget: public mainCentral_hostfactory_ww_http, public Ui::Form_myMainWidget
{
private:

	Q_OBJECT

public:

	myCentralWidget(QWidget* parent): mainCentral_hostfactory_ww_http(parent){};
	~myCentralWidget(){};
	virtual void init_submodule(IjvxFactoryHost* theHost) override;
	virtual void process_websocket_binary(jvxSize userData, jvxByte* payload, jvxPropertyTransferPriority prio) override;
	virtual jvxErrorType report_nonblocking_delayed_update_complete(jvxSize uniqueId, jvxHandle* privatePtr)override;
	virtual jvxErrorType report_nonblocking_delayed_update_terminated(jvxSize uniqueId, jvxHandle* privatePtr)override;
};

#ifdef JVX_PROJECT_NAMESPACE
}
#endif

#endif
