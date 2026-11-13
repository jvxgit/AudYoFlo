#ifndef __MYCENTRALWIDGET_H__
#define __MYCENTRALWIDGET_H__

#include "jvx.h"
#include "interfaces/qt/mainWindow_UiExtension.h"
#include "ui_myMainWidget.h"

#include "mainCentral_host_ww_log.h"
#include "jvxQtMeasureAudioWidget.h"

#include <qwt_plot_curve.h>
#include <qwt_plot_grid.h>
#include "qwt_text.h"
#include "qwt_legend.h"

#include "templates/jvxProperties/CjvxAccessProperties_direct.h"

#define JVX_PIXEL_SIZE_FONTS_PLOT_AXIS 8

#ifdef JVX_PROJECT_NAMESPACE
namespace JVX_PROJECT_NAMESPACE {
#endif


class myCentralWidget: public mainCentral_host_ww_log, public Ui::Form_myMainWidget
{

	typedef enum
	{
		JVX_AUNMEASURE_NO_OPERATION,
		JVX_AUNMEASURE_MEASUREIR,
		JVX_AUNMEASURE_MEASURETIMING
	} jvxAunMeasureOperation;

	IjvxQtMeasureAudio_ir* myWidgetIrMeasure;
	IjvxQtSpecificHWidget* myWidgetJitterMeasure;
	jvxAunMeasureOperation currentOperation;

	jvxBool seqRunning;

	IjvxAccessProperties* thePropRef_algo;
	IjvxAccessProperties* thePropRef_dev;

	Q_OBJECT

public:

	myCentralWidget(QWidget* parent): mainCentral_host_ww_log(parent)
	{
		myWidgetIrMeasure = NULL;
		myWidgetJitterMeasure = NULL;;
		currentOperation = JVX_AUNMEASURE_NO_OPERATION;
		seqRunning = false;
		thePropRef_algo = NULL;
		thePropRef_dev = NULL;
	};

	~myCentralWidget();

	virtual void init_submodule(IjvxHost* theHost) override;

	virtual void inform_active(const jvxComponentIdentification& tp, IjvxAccessProperties* theProps)override;

	virtual void inform_inactive(const jvxComponentIdentification& tp, IjvxAccessProperties* theProps)override;

	virtual void inform_sequencer_about_to_start()override;

	virtual void inform_sequencer_stopped()override;
	
	virtual void inform_update_window(jvxCBitField prio = (jvxCBitField)1 << JVX_REPORT_REQUEST_UPDATE_WINDOW_SHIFT)override;

	virtual void inform_update_window_periodic()override;

	virtual void preferredSize(jvxInt32& height, jvxInt32& width);

public slots:

	void clicked_tabBarWidget(int);
};

#ifdef JVX_PROJECT_NAMESPACE
}
#endif

#endif
