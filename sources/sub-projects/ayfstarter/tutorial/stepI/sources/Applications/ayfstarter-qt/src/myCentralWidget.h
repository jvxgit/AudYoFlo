#ifndef __MYCENTRALWIDGET_H__
#define __MYCENTRALWIDGET_H__

#include "jvx.h"
#include "interfaces/qt/mainWindow_UiExtension.h"
#include "ui_myMainWidget.h"
#include <qpixmap.h>

#include "mainCentral_host_ww.h"

#ifdef JVX_PROJECT_NAMESPACE
namespace JVX_PROJECT_NAMESPACE {
#endif

class myCentralWidget: public mainCentral_host_ww, public Ui::Form_myMainWidget
{
private:

	IjvxAccessProperties* thePropRef_algo = nullptr;

	Q_OBJECT

public:

	myCentralWidget(QWidget* parent): mainCentral_host_ww(parent){};
	~myCentralWidget(){};

	virtual void init_submodule(IjvxHost* theHost)override; 

	virtual void inform_active(const jvxComponentIdentification& tp, IjvxAccessProperties* theProps)override;

	virtual void inform_inactive(const jvxComponentIdentification& tp, IjvxAccessProperties* theProps)override;

	virtual void inform_sequencer_about_to_start()override;

	virtual void inform_sequencer_stopped()override;


	virtual void inform_update_window(jvxCBitField prio = (jvxCBitField)1 << JVX_REPORT_REQUEST_UPDATE_WINDOW_SHIFT)override;

	// =====================================================

	virtual void reportCloseEventTriggered();
	virtual void reportUpdateWindow();

	void preferredSize(jvxInt32& height, jvxInt32& width);
signals:

public slots:


};

#ifdef JVX_PROJECT_NAMESPACE
}
#endif

#endif
