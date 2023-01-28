#ifndef __MYCENTRALWIDGET_H__
#define __MYCENTRALWIDGET_H__

#include "jvx.h"
#include "interfaces/qt/mainWindow_UiExtension.h"
#include "ui_myMainWidget.h"

#include "mainCentral_host_ww.h"

#ifdef JVX_PROJECT_NAMESPACE
namespace JVX_PROJECT_NAMESPACE {
#endif

class myCentralWidget: public mainCentral_host_ww, public Ui::Form_myMainWidget
{
private:
	jvxBool audioNodeActive; 
	IjvxAccessProperties* thePropRef_algo;
	Q_OBJECT

public:

	myCentralWidget(QWidget* parent) : mainCentral_host_ww(parent)
	{
		audioNodeActive = false; 
		thePropRef_algo = NULL;
	};
	~myCentralWidget(){};

	virtual void init_submodule(IjvxHost* theHost) override;

	virtual void inform_update_window(jvxCBitField prio) override;

	virtual void inform_update_window_periodic() override;

	virtual void inform_active(const jvxComponentIdentification& tp, IjvxAccessProperties* theProps) override;

	virtual void inform_inactive(const jvxComponentIdentification& tp, IjvxAccessProperties* propRef) override;

	virtual void inform_about_to_shutdown() override;

signals:

public slots:
};

#ifdef JVX_PROJECT_NAMESPACE
}
#endif

#endif
