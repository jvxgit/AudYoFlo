#ifndef __CJVXQTMEASUREAUDIO_IR_H__
#define __CJVXQTMEASUREAUDIO_IR_H__

#include "jvx.h"
#include "ui_CjvxQtMeasureAudio_ir.h"

#include <qwt_plot_curve.h>
#include <qwt_plot_grid.h>
#include "qwt_text.h"
#include "qwt_legend.h"

#include "jvxQtMeasureAudioWidget.h"
#include "interfaces/properties_hosts/IjvxAccessProperties.h"

#include "jvx-qt-helpers.h"

#define JVX_PIXEL_SIZE_FONTS_PLOT_AXIS 8

class CjvxQtMeasureAudio_ir : public QWidget, public Ui::widget_measureAudio, public IjvxQtMeasureAudio_ir, public IjvxConfigurationExtender_report
{
	typedef struct
	{
		jvxData* dataOrigY;
		jvxData* dataOrigX;
		jvxSize dataL;
		std::string descriptor;
		jvxInt32 xminir;
		jvxInt32 xmaxir;
		jvxData yminir;
		jvxData ymaxir;
		jvxBool autoYother;
		jvxBool autoXother;
		jvxData xminother;
		jvxData xmaxother;
		jvxData yminother;
		jvxData ymaxother;
		jvxBool activePlot;
		QwtPlotCurve* myCurveIr;
		QwtPlotCurve* myCurveOther;
		jvxBool store;
	} oneMeasurement;

	std::string prefix;
	jvxCBitField myMode;

	jvx::propertyAddress::CjvxPropertyAddressDescriptor ident;
	jvx::propertyDetail::CjvxTranferDetail trans;

	std::vector<oneColorOption> availableColors;

private:

	Q_OBJECT

	IjvxHost* theHost;

	IjvxAccessProperties* propRef;
	//jvx_propertyReferenceTriple theTripleAuN;

	jvxBool seqRunning;

	struct
	{
		QwtPlotGrid* theGrid_ir;
		QwtPlotGrid* theGrid_other;;
	} viewVars;

	struct
	{
		std::vector<oneMeasurement> theMeasurements;
		jvxSize selectionId;
	} myMeasurements;

	struct
	{
		jvxInt32 xminir;
		jvxInt32 xmaxir;
		jvxData yminir;
		jvxData ymaxir;

		jvxData xminother;
		jvxData xmaxother;
		jvxData yminother;
		jvxData ymaxother;

		jvxBool thereisMoreThanOne;

		jvxBool autoxir;
		jvxBool autoyir;
	} currentValues;

	public:

	CjvxQtMeasureAudio_ir(QWidget* parent);

	~CjvxQtMeasureAudio_ir();
	virtual void init(IjvxHost* theHost, jvxCBitField mode = 0, jvxHandle* specPtr = NULL, IjvxQtSpecificWidget_report* bwd = NULL)override;

	virtual void getMyQWidget(QWidget** retWidget, jvxSize id)override;

	virtual void terminate()override;

	virtual jvxErrorType addPropertyReference(IjvxAccessProperties* propRef, const std::string&  prefixArg = "", const std::string&  identifierArg = "") override;

	virtual void activate()override;

	virtual void deactivate()override;

	virtual jvxErrorType removePropertyReference(IjvxAccessProperties* propRef )override;

	virtual void processing_started()override;

	virtual void processing_stopped()override;

	virtual void update_window(jvxCBitField prio = JVX_REPORT_REQUEST_UPDATE_DEFAULT, const char* propLst = nullptr)override;

	virtual void update_window_periodic()override;

	virtual jvxErrorType request_sub_interface(jvxQtInterfaceType, jvxHandle**) override;
	virtual jvxErrorType return_sub_interface(jvxQtInterfaceType, jvxHandle*) override;

	virtual jvxErrorType number_measurements(jvxSize& num)override;
	virtual jvxErrorType name_measurement(jvxSize id, std::string& nm)override;
	virtual jvxErrorType lock_data_measurements(jvxSize id, jvxData** data, jvxSize* numEntries)override;
	virtual jvxErrorType unlock_data_measurements(jvxSize id)override;
	virtual jvxErrorType remove_measurement(jvxSize id)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION get_configuration_ext(jvxCallManagerConfiguration* callConf,
		IjvxConfigProcessor* processor,
		jvxHandle* sectionWhereToAddAllSubsections)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION put_configuration_ext(jvxCallManagerConfiguration* callConf,
		IjvxConfigProcessor* processor,
		jvxHandle* sectionToContainAllSubsectionsForMe,
		const char* filename, jvxInt32 lineno)override;


private:

	void trigger_prepareMeasureIrMode();
	void trigger_prepareMeasureTimingMode();
	void updatePlots(jvxBool updatePlotIr = true, jvxBool updateLimitsIr = true, jvxBool replotother = true, jvxBool setAxisLimitsother = true, jvxBool updateMeasurementList = true, jvxBool rescaleAxesir = true, jvxBool rescaleAxesother = true, jvxBool updateIrYOnly = false);
	void updateDataLimits_oneMeasurement(oneMeasurement* theMeasure, jvxInt32 start = -1, jvxInt32 stop = INT_MAX);
	void updateSecondaryPlot(jvxBool updateContent = true, jvxBool updateScale = true);
signals:

public slots:

	void selected_waveformType(int);
	void changed_lengthSignal();
	void changed_silenceStart();
	void changed_silenceStop();
	void changed_numberIterations();
	void modify_numberIterations();
	void selected_transferLine(int);
	void clicked_addTransferLane();
	void clicked_removeTransferLane();
	void changed_descriptionLane();
	void selected_channelTest(int);
	void selected_channelMeasure(int);
	void changed_descriptionMeasure();
	void clicked_exportMeasure();
	void changed_ymaxother();
	void changed_yminother();
	void changed_xminother();
	void changed_xmaxother();
	void changed_ymaxir();
	void changed_yminir();
	void changed_xminir();
	void changed_xmaxir();
	//void toggled_noPause(bool);
	void changed_amplitude();
	void modify_amplitude();
	void changed_countdownBeforeStart();
	void selected_output(int);
	void clicked_outputallon();
	void clicked_outputalloff();
	void selected_measurement(int);
	void selected_whatToPlot(int);
	void clicked_removeMeasurement();
	void clicked_toggleView();
	void clicked_rescaleir();
	void clicked_rescaleother();
	void clicked_rescaleYir();
	void toggle_store();
};

#endif
