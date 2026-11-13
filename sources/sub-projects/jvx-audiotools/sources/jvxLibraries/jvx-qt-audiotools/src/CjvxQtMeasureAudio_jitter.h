#ifndef __CJVXQTMEASUREAUDIO_JITTER_H__
#define __CJVXQTMEASUREAUDIO_JITTER_H__

#include "jvx.h"

#ifdef JVX_PLOTS_QWT
#else
#include "ui_CjvxQtMeasureAudio_jitter_qcp.h"
#endif

#include <qwt_plot_curve.h>
#include <qwt_plot_grid.h>
#include "qwt_text.h"
#include "qwt_legend.h"

#include "jvxQtMeasureAudioWidget.h"

#ifdef JVX_PLOTS_QWT
#else
#include <QVector>
#endif

#define JVX_PIXEL_SIZE_FONTS_PLOT_AXIS 8
#define NUMBERPIXELS_PLOT_SIGNAL 1024

class CjvxQtMeasureAudio_jitter: public QWidget, public Ui::widget_measureJitter, public IjvxQtSpecificHWidget
{

private:

	Q_OBJECT

	IjvxHost* theHost;
	//jvx_propertyReferenceTriple theTripleAuN;
	IjvxAccessProperties* propRef;

	jvxBool seqRunning;

	struct
	{
		jvxBool isValidPlot;
		jvxExternalBuffer* fldPlot;
		jvxSize szPlot;

#ifdef JVX_PLOTS_QWT
		QwtPlotGrid* theGrid;
		QwtPlotCurve* theCurve;
		jvxData secBufferY[NUMBERPIXELS_PLOT_SIGNAL];
		jvxData secBufferX[NUMBERPIXELS_PLOT_SIGNAL];
#else
		QVector<double> secBufferY;
		QVector<double> secBufferX;
#endif
	} rtView_channel;

	struct
	{
		jvxBool isValidPlot;
		jvxExternalBuffer* fldPlot;
		jvxSize szPlot;
#ifdef JVX_PLOTS_QWT
		QwtPlotGrid* theGrid;
		QwtPlotCurve* theCurve;
		jvxData secBufferY[NUMBERPIXELS_PLOT_SIGNAL];
		jvxData secBufferX[NUMBERPIXELS_PLOT_SIGNAL];
#else
		QVector<double> secBufferY;
		QVector<double> secBufferX;
#endif
	} rtView_timing;

	jvxTimeStampData tStamp;

	jvx::propertyAddress::CjvxPropertyAddressDescriptor ident;
	jvx::propertyDetail::CjvxTranferDetail trans;
public:

	CjvxQtMeasureAudio_jitter(QWidget* parent);

	~CjvxQtMeasureAudio_jitter();
	virtual void init(IjvxHost* theHost, jvxCBitField mode = 0, jvxHandle* specPtr = NULL, IjvxQtSpecificWidget_report* bwd = NULL)override;

	virtual void getMyQWidget(QWidget** retWidget, jvxSize id)override;

	virtual void terminate()override;

	virtual jvxErrorType addPropertyReference(IjvxAccessProperties* propRef, const std::string& prefixArg = "", const std::string& identArg = "")override;

	virtual void activate()override;

	virtual void deactivate()override;

	virtual jvxErrorType removePropertyReference(IjvxAccessProperties* propRef)override;

	virtual void processing_started()override;

	virtual void processing_stopped()override;

	virtual void update_window(jvxCBitField prio = JVX_REPORT_REQUEST_UPDATE_DEFAULT, const char* propLst = nullptr)override;

	virtual void update_window_periodic()override;

	virtual jvxErrorType request_sub_interface(jvxQtInterfaceType, jvxHandle**) override;
	virtual jvxErrorType return_sub_interface(jvxQtInterfaceType, jvxHandle*) override;

	virtual void resizeEvent(QResizeEvent *event)override;

private:

	void trigger_prepareMeasureTimingMode();

signals:

public slots:
	void selected_waveformType(int);
	void changed_amplitude();
	void modify_amplitude();
	void selected_output(int);
	void clicked_outputallon();
	void clicked_outputalloff();
	void changed_frequency();
	void modify_frequency();
	void select_observationChannel(int id);

};

#endif
