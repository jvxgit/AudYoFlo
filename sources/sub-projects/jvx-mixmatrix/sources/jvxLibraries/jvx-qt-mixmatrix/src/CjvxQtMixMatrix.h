#ifndef __CJVXQTMIXMATRIX_H__
#define __CJVXQTMIXMATRIX_H__

#include "jvx.h"
#include "ui_CjvxQtMixMatrix.h"

#include "jvxQtMixMatrixWidget.h"

#define JVX_COLOR_MATRIX_BGRD_SELECTED Qt::darkGray
#define JVX_COLOR_MATRIX_TEXT_SELECTED Qt::white
#define JVX_COLOR_MATRIX_BGRD_UNSELECTED Qt::lightGray
#define JVX_COLOR_MATRIX_TEXT_UNSELECTED Qt::black
#define JVX_OFFSET_DB_GAIN 80
#define JVX_OFFSET_DB_LEVEL 100
#define JVX_QTMIXMATRIX_FIELDWIDTH 50
#define JVX_QTMIXMATRIX_FIELDWIDTH_ROTATED_TEXT 250
#define JVX_QTMIXMATRIX_WIDGET_HEIGHT 1000
#define JVX_QTMIXMATRIX_WIDGET_WIDTH 1000
#define CLIP_THRESHOLD 0.98

#include "jvxQTableWidgetHeader_types.h"

class CjvxQtMixMatrix : public QWidget, public Ui::widget_mixMatrix, public IjvxQtMixMatrixWidget
{
/*
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
	} oneMeasurement;
*/
private:

	typedef struct
	{
		jvxData gain;
		jvxData level;
		jvxBool mute;
	} valueTuple;

	Q_OBJECT

	IjvxHost* theHost;
	IjvxAccessProperties* propRef;
	std::string thePrefix;

	jvxBool seqRunning;

	myHeaderViewH* theHeaderH;
	myHeaderViewV* theHeaderV;

	struct
	{
		QStringList lst_input;
		QStringList lst_output;
		QList<QColor> colors_input;
		QList<QColor> colors_output;

		jvxSize selection_x;
		jvxSize selection_y;
	} config;

	jvxMatrix<std::string, valueTuple> theContent;
	jvxBool buttonPressed;
	jvxBool stereoLockModeOn;
	jvxData main_in;
	jvxData main_out;


public:

	CjvxQtMixMatrix(QWidget* parent);

	~CjvxQtMixMatrix();
	virtual void init(IjvxHost* theHost, jvxCBitField mode = 0, jvxHandle* specPtr = NULL, IjvxQtSpecificWidget_report* bwd = NULL)override;

	virtual void getMyQWidget(QWidget** retWidget, jvxSize id)override;

	virtual void terminate()override;

	virtual jvxErrorType addPropertyReference(IjvxAccessProperties* propRefIn, const std::string& prefixArg, const std::string& identArg) override;

	virtual void setNamesColorsChannels(QStringList& lstIn, QStringList& lstOut, QList<QColor>& colorsIn, QList<QColor>& colorsOut)override;

	virtual void activate()override;

	virtual void deactivate()override;

	virtual jvxErrorType removePropertyReference(IjvxAccessProperties* propRefIn)override;

	virtual void processing_started()override;

	virtual void processing_stopped()override;

	virtual void update_window(jvxCBitField prio = JVX_REPORT_REQUEST_UPDATE_DEFAULT, const char* propLst = nullptr)override;

	virtual void update_window_periodic()override;

	virtual jvxErrorType request_sub_interface(jvxQtInterfaceType, jvxHandle**) override;
	virtual jvxErrorType return_sub_interface(jvxQtInterfaceType, jvxHandle*) override;
private:

	bool eventFilter(QObject *object, QEvent *event)override;

	void drawContentMatrix();

	void transferMatrixNode(jvxBool toNode, jvxSize idx = JVX_SIZE_UNSELECTED, jvxSize idy = JVX_SIZE_UNSELECTED);
	void modifyLevelIncDec(jvxData fac, jvxSize selx, jvxSize sely);

	/*
	void trigger_prepareMeasureIrMode();
	void trigger_prepareMeasureTimingMode();
	void updatePlots(jvxBool updatePlotIr = true, jvxBool updateLimitsIr = true, jvxBool replotother = true, jvxBool setAxisLimitsother = true, jvxBool updateMeasurementList = true, jvxBool rescaleAxesir = true, jvxBool rescaleAxesother = true, jvxBool updateIrYOnly = false);
	void updateDataLimits_oneMeasurement(oneMeasurement* theMeasure, jvxInt32 start = -1, jvxInt32 stop = INT_MAX);
	void updateSecondaryPlot(jvxBool updateContent = true, jvxBool updateScale = true);
	*/
signals:

public slots :

	void clicked_Cell(int, int);
	void doubleClicked_Cell(int, int);
	void released_Slider();

	void released_Slider_lockStereo();
	void release_Slider_mainout();
	void editing_Finished_gain();
	void button_Pushed_alloff();

	void clicked_expandOut();
	void clicked_expandIn();

	/*
	void selected_waveformType(int);
	void changed_lengthSignal();
	void changed_silenceSignal();
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
	void toggled_noPause(bool);
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
	*/
};

#endif
