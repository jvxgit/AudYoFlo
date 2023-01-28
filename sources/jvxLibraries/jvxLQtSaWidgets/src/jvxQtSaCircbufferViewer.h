#ifndef _H_jvxQtSaCircbufferViewer_H__
#define _H_jvxQtSaCircbufferViewer_H__

#include "jvx.h"

#include <QtWidgets/QWidget>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>

#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_grid.h>
#include <qwt_text.h>
#include <qwt_legend.h>

#include "jvx-helpers.h"
#include "typedefs/misc/TjvxCircBufferDataCollector.h"
#include "jvxQtSaWidgetsConfigure.h"
#include "jvxQtSaCustomBase.h"


class jvxQtSaCircbufferViewer: public QWidget, public jvxQtSaCustomBase
{
public:

	typedef struct
	{
		std::string descr_circplot;
		std::string descr_showFactor;
		std::string descr_num_in_channels;
		std::string descr_plot_mode;
		std::string descr_line_trigger;
		std::string descr_trigger_amplitude;
		std::string descr_command;
		std::string descr_xaxis_data;
		std::string descr_xaxis_title;
		IjvxAccessProperties* theProps;
	} configurePropertyInterface;

	jvx::propertyAddress::CjvxPropertyAddressGlobalId ident;
	jvx::propertyDetail::CjvxTranferDetail trans;

	Q_OBJECT

private:

	struct onePlotLane
	{
		struct
		{
			QFrame* oneFrame;

			QwtPlotGrid* theGrid;
			QwtLegend * theLegend;
			QwtPlot* thePlot;
			QwtPlotCurve* theCurve;

			QCheckBox* update;
			QCheckBox* auto_scale;

			QLineEdit* le_ymin;
			QLineEdit* le_ymax;
			QLineEdit* le_xmin;
			QLineEdit* le_xmax;

			QLabel *label_title;
		} ui_elements;

		jvxData* plotY;

		jvxSize bufIdx_y;
		jvxSize bufIdx_x;

		jvxData ymin;
		jvxData ymax;
		jvxData xmin;
		jvxData xmax;

		jvxBool auto_scale;
		jvxBool update;
	};



protected:

	jvxBool uiSetupComplete;
	jvxCircBufferDataCollectorMode myDataCollectionMode;

	struct
	{
		// All ids default to -1 to detect undefined properties, therefore signed 
		jvxInt32 id_circplot; 
		jvxInt32 id_showFactor;
		jvxInt32 id_num_in_channels;
		jvxInt32 id_plot_mode;
		jvxInt32 id_line_trigger;
		jvxInt32 id_trigger_amplitude;
		jvxInt32 id_command;
		jvxInt32 id_plot_behavior_type;

		jvxInt32 id_xaxis_data;
		jvxInt32 id_xaxis_title;
	} linkedProperties;

	onePlotLane* allPlots;

	jvxData* plotX;
	std::string title_xdata;

	QScrollArea *scrollArea;

	QWidget *scrollAreaWidgetContents;

	QHBoxLayout *allContentsLayout;

	QVBoxLayout *vertLayoutPlots;
	QGridLayout *gridLayout;

	QVBoxLayout *vertLayoutControl;

	QFrame* framePlots;

	QFrame* frameControl;

	QLabel *label_plotMode;
	QComboBox *comboBox_plotMode;

	QSpacerItem *horizontalSpacer_pm;

	QLabel *label_plotSpeed;
	QComboBox *comboBox_plotSpeed;

	QSpacerItem *horizontalSpacer_ps;

	QPushButton *pushButton_clearPlots;

	QLabel *label_triggerAmplt;
	QLineEdit *lineEdit_triggerAmplt;


	QLabel *label_triggerSelect;
	QComboBox *comboBox_lineTriggerSelect;

private:

	jvxQtSaWidgetsConfigure::jvxQtWidgetsLinePlotConfigure myConfig;
	jvxQtSaWidgetsConfigure::jvxQtWidgetsLinePlotConfigure myConfig_supersede;
	configurePropertyInterface myProperties;
	jvxState myState;

	struct
	{
		jvxExternalBuffer* fld;
		jvxSize sz;
		jvxBool isValid;
	} theCircHeader;
	jvxInt32 fillHeightLastPlot;

public:
	jvxQtSaCircbufferViewer(QWidget* parent);
	~jvxQtSaCircbufferViewer();

	jvxErrorType init(jvxQtSaWidgetsConfigure::jvxQtWidgetsLinePlotConfigure* theViewerConfig);
	jvxErrorType associateProperties(configurePropertyInterface* thePropsConfig);
	jvxErrorType refresh_property_ids();
	jvxErrorType deassociateProperties();
	jvxErrorType terminate();

	void setupUi();
	void update_window();
	void update_window_periodic();

	jvxErrorType read_properties_shortcut();


	jvxErrorType startPlotting();
	jvxErrorType stopPlotting();

	void updatePlotParameters(jvxBool updatexminmax);

public slots:

	void auto_scale_clicked(bool);
	void update_clicked(bool);

	void ymin_edit();
	void ymax_edit();
	void xmin_edit();
	void xmax_edit();

	void newSelectionPlotSpeed(int sel);
	void newSelectionPlotMode(int selId);

	void newSelectionLineTrigger(int selId);
	void newValueTriggerAmpl();
	void clearAllPlots();
};

#endif
