#ifndef __MYCENTRALWIDGET_H__
#define __MYCENTRALWIDGET_H__

#include "jvx.h"
#include "interfaces/qt/mainWindow_UiExtension.h"
#include "ui_myMainWidget.h"
#include <qpixmap.h>

#ifdef JVX_COMPILE_REMOTE_CONTROL
#include "mainCentral_hostfactory_ww_http.h"
#define JVX_MAINCENTRAL_CLASSNAME mainCentral_hostfactory_ww_http
#else
#include "mainCentral_host_ww.h"
#define JVX_MAINCENTRAL_CLASSNAME mainCentral_host_ww
#endif

#ifdef JVX_PROJECT_NAMESPACE
namespace JVX_PROJECT_NAMESPACE {
#endif

class myCentralWidget: public JVX_MAINCENTRAL_CLASSNAME, public Ui::Form_myMainWidget
{
private:

	IjvxAccessProperties* thePropRef;
	Q_OBJECT

public:

	myCentralWidget(QWidget* parent): JVX_MAINCENTRAL_CLASSNAME(parent){};
	~myCentralWidget(){};

#ifdef JVX_COMPILE_REMOTE_CONTROL
	virtual void init_submodule(IjvxFactoryHost* theHost) override;
#else
	virtual void init_submodule(IjvxHost* theHost) override;
#endif

	virtual void inform_active(const jvxComponentIdentification& tp, IjvxAccessProperties* theProps)override;

	virtual void inform_inactive(const jvxComponentIdentification& tp, IjvxAccessProperties* theProps)override;

	virtual void inform_sequencer_about_to_start()override;

	virtual void inform_sequencer_stopped()override;

	virtual void inform_update_window(jvxCBitField prio = (jvxCBitField)1 << JVX_REPORT_REQUEST_UPDATE_WINDOW_SHIFT)override;

	virtual void reportCloseEventTriggered();
	virtual void reportUpdateWindow();

	void preferredSize(jvxInt32& height, jvxInt32& width);

#ifdef JVX_COMPILE_REMOTE_CONTROL

	virtual jvxErrorType report_nonblocking_delayed_update_complete(jvxSize uniqueId, jvxHandle* privatePtr) override;
	virtual jvxErrorType report_nonblocking_delayed_update_terminated(jvxSize uniqueId, jvxHandle* privatePtr) override;

	virtual void process_websocket_binary(jvxSize userData, jvxByte* payload, jvxPropertyTransferPriority prio) override;
#endif
signals:

public slots:


};

#ifdef JVX_PROJECT_NAMESPACE
}
#endif

#endif
