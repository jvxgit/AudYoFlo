#ifndef __MYCENTRALWIDGET_H__
#define __MYCENTRALWIDGET_H__

#include "jvx.h"
#include "interfaces/qt/mainWindow_UiExtension.h"
#include "ui_myMainWidget.h"
#include "jvxQtMixMatrixWidget.h"

#include "mainCentral_host.h"

#ifdef JVX_PROJECT_NAMESPACE
namespace JVX_PROJECT_NAMESPACE {
#endif

#define JVX_CHECK_CONNECTIONS

class myCentralWidget: public mainCentral_host, public Ui::Form_myMainWidget 
{
private:

	Q_OBJECT

	struct
	{
		jvxInt32 inChannels;
		jvxInt32 outChannels;
		jvxSize selection_in;
		jvxSize selection_out;
	} currentlyShown;

	IjvxQtMixMatrixWidget* myWidgetMixMatrix;
	QWidget* theQWidget;

	IjvxAccessProperties* thePropRef_algo;
	IjvxAccessProperties* thePropRef_dev;

public:

	myCentralWidget(QWidget* parent): mainCentral_host(parent)
	{
		myWidgetMixMatrix = NULL;
		theQWidget = NULL;
		thePropRef_algo = NULL;
		thePropRef_dev = NULL;
	};
	~myCentralWidget(){};
	virtual void init_submodule(IjvxHost* theHost) override;

	virtual void inform_update_window(jvxCBitField prio = (jvxCBitField)1 << JVX_REPORT_REQUEST_UPDATE_WINDOW_SHIFT)override;

	virtual void inform_update_window_periodic()override;

	virtual void inform_active(const jvxComponentIdentification& tp, IjvxAccessProperties* theProps)override;

	virtual void inform_inactive(const jvxComponentIdentification& tp, IjvxAccessProperties* propRef)override;

	// ====================================================================

	void rebuildMixMatrix(jvxInt32 numInChannels, jvxInt32 numOutChannels);

signals:

public slots:
};

#ifdef JVX_PROJECT_NAMESPACE
}
#endif

#endif
