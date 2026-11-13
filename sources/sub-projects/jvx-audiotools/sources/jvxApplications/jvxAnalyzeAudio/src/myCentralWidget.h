#ifndef __MYCENTRALWIDGET_H__
#define __MYCENTRALWIDGET_H__

#include "jvx.h"
#include "interfaces/qt/mainWindow_UiExtension.h"
#include "ui_myMainWidget.h"

#include "mainCentral_host_ww_log.h"
#include "jvxQtMeasureAudioWidget.h"

#include "MjvxCentralWidgetMacros.h"
#include "jvxQtCentralWidgets.h" 

#include <qwt_plot_curve.h>
#include <qwt_plot_grid.h>
#include "qwt_text.h"
#include "qwt_legend.h"

#define JVX_PIXEL_SIZE_FONTS_PLOT_AXIS 8

#ifdef JVX_PROJECT_NAMESPACE
namespace JVX_PROJECT_NAMESPACE {
#endif


class myCentralWidget: public mainCentral_host_ww_log, public IjvxQtSpecificWidget_report, public Ui::Form_myMainWidget
{

	IjvxQtSpecificHWidget* myWidgetAudioSpectrum;

	jvxBool seqRunning;

	IjvxAccessProperties* thePropRef_algo;
	IjvxAccessProperties* thePropRef_dev;

	struct
	{
		jvxSize width;
		jvxSize height;
	} szPass;

	Q_OBJECT

	JVX_MAIN_CENTRAL_DECLARE_TREE_WIDGET(propertyTreeWidget);

public:

	myCentralWidget(QWidget* parent): mainCentral_host_ww_log(parent){};

	~myCentralWidget();

	virtual void init_submodule(IjvxHost* theHost) override;
	void terminate_submodule() override;

	virtual void inform_active(const jvxComponentIdentification& tp, IjvxAccessProperties* theProps)override;

	virtual void inform_inactive(const jvxComponentIdentification& tp, IjvxAccessProperties* theProps)override;

	virtual void inform_sequencer_about_to_start()override;

	virtual void inform_sequencer_stopped()override;
	
	virtual void inform_update_window(jvxCBitField prio = (jvxCBitField)1 << JVX_REPORT_REQUEST_UPDATE_WINDOW_SHIFT)override;

	virtual void inform_update_window_periodic()override;

	virtual void preferredSize(jvxInt32& height, jvxInt32& width);

	virtual void report_widget_closed(QWidget* theClosedWidget) override;
	virtual void report_widget_specific(jvxSize id, jvxHandle* spec) override;

public slots:

	void clicked_tabBarWidget(int);
};

#ifdef JVX_PROJECT_NAMESPACE
}
#endif

#endif
