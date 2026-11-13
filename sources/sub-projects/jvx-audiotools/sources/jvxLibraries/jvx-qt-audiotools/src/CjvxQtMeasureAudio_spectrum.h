#ifndef __CJVXQTMEASUREAUDIO_SPECTRUM_H__
#define __CJVXQTMEASUREAUDIO_SPECTRUM_H__

#include "ui_CjvxQtMeasureAudio_spectrum.h"
#include "jvx.h"

#include "jvxQtMeasureAudioWidget.h"
#include "jvxQtSaGlWidget.h"

class CjvxQtMeasureAudio_spectrum: public QWidget, public Ui::widget_measureSpectrum, public IjvxQtSpecificHWidget
{
public:

private:

	Q_OBJECT

	IjvxHost* theHost = nullptr;
	IjvxAccessProperties* propRef = nullptr;
	//jvx_propertyReferenceTriple theTripleAuN;
	jvxBool seqRunning = false;;
	std::string prefix;
	CjvxMyGlWidget *openGLWidget_pri = nullptr;
	CjvxMyGlWidget_config glCfg_pri;

	CjvxMyGlWidget* openGLWidget_sec = nullptr;
	CjvxMyGlWidget_config glCfg_sec;

public:

	CjvxQtMeasureAudio_spectrum(QWidget* parent);

	~CjvxQtMeasureAudio_spectrum();
	
	virtual void init(IjvxHost* theHost, jvxCBitField mode = 0, jvxHandle* specPtr = NULL, IjvxQtSpecificWidget_report* bwd = NULL)override;

	virtual void getMyQWidget(QWidget** retWidget, jvxSize id)override;

	virtual void terminate()override;

	virtual jvxErrorType addPropertyReference(IjvxAccessProperties* propRefIn , const std::string& prefixArg = "", const std::string& identArg = "")override;

	virtual void activate()override;

	virtual void deactivate()override;

	virtual jvxErrorType removePropertyReference(IjvxAccessProperties* propRefIn)override;

	virtual void processing_started()override;

	virtual void processing_stopped()override;
	
	virtual void update_window(jvxCBitField prio = JVX_REPORT_REQUEST_UPDATE_DEFAULT, const char* propLst = nullptr)override;

	virtual void update_window_periodic()override;

	virtual jvxErrorType request_sub_interface(jvxQtInterfaceType, jvxHandle**) override;
	virtual jvxErrorType return_sub_interface(jvxQtInterfaceType, jvxHandle*) override;

	void update_overlay_config(jvxVideoOverlayConfig& ovlConfig);
	//virtual void resizeEvent(QResizeEvent *event);

private:

signals:

public slots:
	void newSelectionLegendPos(int idx);
	void toggledGrid(bool show);

};

#endif