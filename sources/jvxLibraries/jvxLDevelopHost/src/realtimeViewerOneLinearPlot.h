#ifndef __REALTIMEVIEWERONELINEARPLOT_H__
#define __REALTIMEVIEWERONELINEARPLOT_H__

#include "jvx.h"

#include "ui_realtimeViewerOneLinearPlot.h"
#include "realtimeViewer_base.h"
#include "common/CjvxRealtimeViewer.h"

#include <qwt_plot_curve.h>
#include <qwt_plot_grid.h>
#include "qwt_text.h"
#include "qwt_legend.h"

class realtimeViewerPlots;

class realtimeViewerOneLinearPlot: public QWidget, public Ui::Form_oneLinearPlot, public realtimeViewer_base
{
	typedef struct
	{
		int itemId;
		struct
		{
			jvxPropertyCategoryType category;
			jvxUInt64 allowStateMask;
			jvxUInt64 allowThreadingMask;
			jvxDataFormat format;
			jvxSize num;
			jvxBool readonly;
			jvxPropertyDecoderHintType decHtTp;
			jvxSize hdlIdx;
			std::string description;
		} propParams;
		QwtPlotCurve* theCurve;
	} oneCurve;

	struct
	{
		jvxComponentIdentification tpV;
		jvxPropertyContext ctxt;
		jvxBool showDescriptors;
		struct
		{
			jvxSize inPropIdComponent;
			jvxSize inPropIdShow;
		} y;
		struct
		{
			jvxSize inPropIdComponent;
			jvxSize inPropIdShow;
		} x;

		jvxSize plotColorId;
		jvxSize plotStyleId;

		std::string description;

		jvxSize plotSelection;

		jvxSize plotWidthId;
	} selection;

	callbackStruct_group myCallback;

	struct
	{
		QwtPlotGrid* theGrid;
		QwtLegend * theLegend;
	} plot;

	std::vector<oneCurve> theCurves;

	jvxComponentIdentification* tpAll;

/*
	struct
	{

		jvxComponentType tp;
		jvxSize inPropId;
	} description;

	struct
	{
		jvxBool update;
	} local;
	*/

	/*
	struct
	{
		bool setup_complete;
		QLineEdit_ext* line_edit;
		std::vector<QLabel*> label_flags;
		QComboBox_ext* combo_box;
		QLabel* label_min;
		QSlider_ext* slider;
		QLabel* label_max;

		QLabel* label_valid;
	} uielements;
	*/
	Q_OBJECT

public:

	realtimeViewerOneLinearPlot(realtimeViewerPlots* parent, IjvxHost* hostRef, CjvxRealtimeViewer* rtvRef, IjvxReport* report, jvxSize sectionId, 
		jvxSize groupId, jvxComponentIdentification* tpAllI);
	~realtimeViewerOneLinearPlot();

	void init();
	void cleanBeforeDelete();


	void updateWindow();

	static jvxErrorType callbackUpdateItem(jvxSize SectionId, jvxSize groupId, jvxSize itemId,  jvxHandle* privateData);
	static jvxErrorType callbackUpdateGroup(jvxSize SectionId, jvxSize groupId, jvxBool, jvxHandle* privateData);

	jvxErrorType cbUpdateItem(jvxSize SectionId, jvxSize groupId, jvxSize itemId);
	jvxErrorType cbUpdateGroup(jvxSize SectionId, jvxSize groupId, jvxBool onStart);

	void updateWindow_redraw_plots();

	bool addPlot_core();

	void setXYAxisTitle();

	void updateAllPropertyDescriptors();

	/*
	void updateWindow_construct();

	void updateWindow_contents();

	void updateWindow_update();

	void updatePropertyIfValid(jvxSize sectionId, jvxSize groupId, jvxSize itemId);

	void newSelection_combo(jvxUInt64 bitset);
	void newPosition_slider(int val);
	void newText_lineEdit(QString txt);
	*/
public slots:
	void newSelection_reg_plots(int);
    void checkedBox_show_plot(bool);
    void buttonPushed_add_plot();
	void buttonPushed_edit_plot();
	void buttonPushed_remove_plot();
	void checkedBox_autoscale_x_plot(bool);
	void newText_description_plot();
	void newSelection_component(int);
	void newSelection_y_property(int);
	void newSelection_x_property(int);
	void newSelection_plot_color(int);
	void newSelection_plot_linestyle(int);
	void newText_y_min();
	void newText_x_min();
	void newText_x_max();
	void newText_y_max();
	void checkedBox_autoscale_y_plot(bool);
	void newText_yaxistitle();
    void newText_xaxistitle();
	void newSelection_plot_width(int);
	void newSelection_context(int);
	void toggle_showDescriptors(bool);
};



#endif
