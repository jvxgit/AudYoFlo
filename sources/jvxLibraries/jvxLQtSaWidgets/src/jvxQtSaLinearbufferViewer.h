#ifndef _H_jvxQtSaLinearbufferViewer_H__
#define _H_jvxQtSaLinearbufferViewer_H__

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
//#include "typedefs/misc/TjvxCircBufferDataCollector.h"
#include "jvxQtSaWidgetsConfigure.h"
#include "jvxQtSaCustomBase.h"


class jvxQtSaLinearbufferViewer: public QWidget, public jvxQtSaCustomBase
{
public:

	typedef struct
	{
		std::string descr_circplot;
		std::string descr_num_in_channels;
		std::string descr_command;
		std::string descr_legend;
		std::string descr_xaxis_data;
		std::string descr_xaxis_title;
		IjvxAccessProperties* theProps;
	} configurePropertyInterface;

	Q_OBJECT

private:

	struct allPlots
	{
		struct
		{
			QFrame* oneFrame;

			QwtPlotGrid* theGrid;
			QwtLegend * theLegend;
			QwtPlot* thePlot;

			QCheckBox* update;
			QCheckBox* auto_scale;

			QLineEdit* le_ymin;
			QLineEdit* le_ymax;
			QLineEdit* le_xmin;
			QLineEdit* le_xmax;

			QLabel *label_title;

			QwtPlotCurve** theCurves;
		} ui_elements;

		jvxData** plotY;

		jvxData ymin;
		jvxData ymax;
		jvxData xmin;
		jvxData xmax;

		jvxBool auto_scale;
		jvxBool update;
	};

	jPAGID ident;
	jPD trans;

protected:

	jvxBool uiSetupComplete;

	struct
	{
		jvxInt32 id_circplot;
		jvxInt32 id_num_in_channels;
		jvxInt32 id_command;
		jvxInt32 id_legend;
		jvxInt32 id_xaxis_data;
		jvxInt32 id_xaxis_title;

	} linkedProperties;

	allPlots thePlots;

	jvxData* plotX;
	std::string title_xdata;

	QHBoxLayout *allContentsLayout;

	QVBoxLayout *vertLayoutPlots;
	QGridLayout *gridLayout;

	QVBoxLayout *vertLayoutControl;

	QFrame* framePlots;



private:

	jvxQtSaWidgetsConfigure::jvxQtWidgetsLinePlotConfigure myConfig;
	configurePropertyInterface myProperties;
	jvxState myState;

	struct
	{
		jvxExternalBuffer* fld;
		jvxSize sz;
		jvxBool isValid;
	} theCircHeader;

public:
	jvxQtSaLinearbufferViewer(QWidget* parent);
	~jvxQtSaLinearbufferViewer();

	jvxErrorType init(jvxQtSaWidgetsConfigure::jvxQtWidgetsLinePlotConfigure* theViewerConfig);
	jvxErrorType associateProperties(configurePropertyInterface* thePropsConfig);
	jvxErrorType refresh_property_ids();
	jvxErrorType deassociateProperties();
	jvxErrorType terminate();

	void setupUi();
	void update_window();
	void update_window_periodic();

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

};

#endif
