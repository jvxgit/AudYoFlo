#ifndef __JVXACOUSTICMEASURE__H__
#define __JVXACOUSTICMEASURE__H__

#include "jvx.h"
#include "ui_jvxAcousticMeasurement.h"
#include "jvxQtAcousticMeasurement.h"
#include "jvxSpNMeasureIr_props.h"

#include "jvxAcousticEqualizer.h"
#include "jvxExtractHrtfs.h"

#include "jvx-qcp-qt-helpers.h"

#define JVX_LOG10_EPS 1e-10
#define JVX_EPS_XY 0.01

class jvxAcousticMeasure: 
	public QWidget, 
	public Ui::acousticMeasure,
	public IjvxQtAcousticMeasurement,
	public IjvxQtSpecificHWidget_config_si,
	public IjvxConfigurationExtender_report
{
public:
	class oneRegisteredProcessor
	{
	public:
		jvxMeasurementDataProcessorTask task;
		IjvxQtAcousticMeasurement_process* proc;
		std::string descr;
		oneRegisteredProcessor()
		{
			proc = NULL;
			task = JVX_ACOUSTIC_MEASURE_TASK_NONE;
		};
	};

	typedef enum
	{
		JVX_PLOT_MODE_TD_IR,
		JVX_PLOT_MODE_TD_IR_LOG,
		JVX_PLOT_MODE_TD_MEAS,
		JVX_PLOT_MODE_TD_TEST,
		JVX_PLOT_MODE_TD_LIMIT
	} jvxPlotModeEnumTimeDomain;

	typedef enum
	{
		JVX_PLOT_MODE_SEC_MAG,
		JVX_PLOT_MODE_SEC_PHASE,
		JVX_PLOT_MODE_SEC_GROUP_DELAY,
		JVX_PLOT_MODE_SEC_HIST_DELAY,
		JVX_PLOT_MODE_SEC_LIMIT
	} jvxPlotModeEnumSecondary;

private:

	typedef enum
	{
		JVX_EDIT_POINT_TD_MARKER1,
		JVX_EDIT_POINT_TD_MARKER2,
		JVX_EDIT_POINT_SEC_MARKER1,
		JVX_EDIT_POINT_SEC_MARKER2,
		JVX_EDIT_POINT_SEC_INVALID
	} jvxEditPointMarker;

	typedef enum
	{
		JVX_MOUSE_MODE_NONE,
		JVX_MOUSE_MODE_EDIT_DIAGRAM, // when pushed 1
		JVX_MOUSE_MODE_MEASURE, // when pushed 2
		JVX_MOUSE_MODE_EDIT_MARKER1, // when pushed 3
		JVX_MOUSE_MODE_EDIT_MARKER2 // when pushed 4
	}jvxPlotMouseMode;


	class onePlotSelection
	{
	public:
		jvxSize optionColor;
		jvxSize optionWidth;
		jvxSize optionStyle;
		jvxSize optionMarker;
		jvxBool plotActive;
		onePlotSelection()
		{
			optionColor = 0;
			optionWidth = 0;
			optionStyle = 0;
			optionMarker = 0;
			plotActive = true;
		};
	};

	class onePlotParams
	{
	public:
		jvxData minx;
		jvxData maxx;
		jvxData miny;
		jvxData maxy;
		jvxBool autox;
		jvxBool autoy;
		jvxBool plotLinearx;

		jvxBool start_select;
		jvxData start_x;
		jvxData start_y;
		jvxTick start_tick;

		jvxBool showLegend;
		struct
		{
			jvxData minx;
			jvxData maxx;
			jvxData miny;
			jvxData maxy;
		} latest;

		onePlotParams()
		{
			minx = 0;
			maxx = 0;
			miny = 0;
			maxy = 0;
			autox = true;
			autoy = true;
			plotLinearx = true;
			start_select = false;
			start_x = 0;
			start_y = 0;
			latest.minx = 0;
			latest.maxx = 0;
			latest.miny = 0;
			latest.maxy = 0;
			start_tick = 0;
			showLegend = true;
		};
	};

	class oneSetDataPlot
	{
	public:
		oneMeasurementChannel oneChan;
		std::string measurement_name;
		std::string channel_name;
		std::string file_folder_read;
		struct
		{
			QVector<jvxData> ind_timedomain;
			QVector<jvxData> ir;
			QVector<jvxData> meas;
			QVector<jvxData> test;

			QVector<jvxData> show_freqdomain_y;
			QVector<jvxData> show_freqdomain_x;
			jvxBool show_freqdomain_valid;
		} data;

		void relink()
		{
			oneChan.bufIr = data.ir.data();
			oneChan.bufMeas = data.meas.data();
			oneChan.bufTest = data.test.data();
			oneChan.lBuf = data.ir.size();
		};
	};

	class oneSetMarkers
	{
	public:
		QVector<jvxData> markers_y;
		QVector<jvxData> markers_x;
		jvxBool show;
		oneSetMarkers()
		{
			show = false;
		};
	};
		
	class set_markers
	{
	public:
		oneSetMarkers markers_1;
		oneSetMarkers markers_2;
		void clear()
		{
			markers_1.markers_x.clear();
			markers_1.markers_y.clear();
			markers_1.show = false;

			markers_2.markers_x.clear();
			markers_2.markers_y.clear();
			markers_2.show = false;
		};
	};

	onePlotParams td;
	onePlotParams sec;

	onePlotSelection plot1;
	onePlotSelection plot2;

	oneSetDataPlot dataPlot1;
	oneSetDataPlot dataPlot2;
	oneSetDataPlot dataPlot12;

	jvxPlotModeEnumTimeDomain modeTd;
	jvxPlotModeEnumSecondary modeSec;

	set_markers timedomain_markers;
	set_markers secondary_markers;

	Q_OBJECT

	IjvxAccessProperties* propRef;
	jvxPlotMouseMode mouseMode;
	jvxTimeStampData tStamp;

	std::map<jvxPlotModeEnumTimeDomain, set_markers> lstTdMarkers;
	std::map<jvxPlotModeEnumSecondary, set_markers> lstSecMarkers;
	std::string dataTag;

	jvxAcousticEqualizer* equalizerWidget;
	jvxExtractHrtfs* hrtfWidget;

	std::map< jvxMeasurementDataProcessorTask, 
		std::list<oneRegisteredProcessor > > registeredProcessors;

	IjvxHost* theHostRef = nullptr;

	std::string nmRegConfig;
public:

	jvxAcousticMeasure(QWidget* parent = NULL);
	virtual ~jvxAcousticMeasure();

	// ========================================================================	

	virtual void getMyQWidget(QWidget** retWidget, jvxSize id = 0) override;

	virtual void terminate() override;

	virtual void activate() override;

	virtual void deactivate() override;

	virtual void processing_started() override;

	virtual void processing_stopped() override;

	virtual void update_window(jvxCBitField prio = JVX_REPORT_REQUEST_UPDATE_DEFAULT) override;

	virtual void update_window_periodic() override;

	virtual jvxErrorType request_sub_interface(jvxQtInterfaceType, jvxHandle**) override;

	virtual jvxErrorType return_sub_interface(jvxQtInterfaceType, jvxHandle*) override;

	// =====================================================================
	
	virtual jvxErrorType addPropertyReference(IjvxAccessProperties* propRef, const std::string&  prefixArg = "", const std::string& identArg = "") override;

	virtual jvxErrorType removePropertyReference(IjvxAccessProperties* propRef) override;

	// =====================================================================

	virtual void init(IjvxHost* theHost, jvxCBitField mode = 0, jvxHandle* specPtr = NULL, IjvxQtSpecificWidget_report* bwd = NULL) override;

	// =====================================================================

	virtual jvxErrorType register_data_processor(const char* descr, jvxMeasurementDataProcessorTask task, IjvxQtAcousticMeasurement_process* fld) override;
	virtual jvxErrorType unregister_data_processor(jvxMeasurementDataProcessorTask task, IjvxQtAcousticMeasurement_process* fld) override;

	// =====================================================================

	virtual jvxErrorType invite_processor(IjvxQtAcousticMeasurement* fld) override;
	virtual jvxErrorType goodbye_processor(IjvxQtAcousticMeasurement* fld) override;
	
	// =====================================================================

	virtual jvxErrorType registerConfigExtensions(IjvxConfigurationExtender* cfgExt) override;
	virtual jvxErrorType unregisterConfigExtensions(IjvxConfigurationExtender* cfgExt) override;
	/*
	virtual jvxErrorType request_configuration_ext_report(IjvxConfigurationExtender_report** theConfigExtenderReport) override;
	virtual jvxErrorType return_configuration_ext_report(IjvxConfigurationExtender_report* theConfigExtenderReport)override;
	*/

	// =====================================================================

	virtual jvxErrorType JVX_CALLINGCONVENTION get_configuration_ext(jvxCallManagerConfiguration* callConf,
		IjvxConfigProcessor* processor, jvxHandle* sectionWhereToAddAllSubsections) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION put_configuration_ext(jvxCallManagerConfiguration* callConf,
		IjvxConfigProcessor* processor,
		jvxHandle* sectionToContainAllSubsectionsForMe,
		const char* filename, jvxInt32 lineno) override;

	// =====================================================================

	void set_params_plots();

	// =====================================================================

	void replot_diagrams(jvxBool updateDataPrimary, jvxBool updateDataSecondary);
	void replot_freqdomain(jvxBool updateDataSecondary,
		QVector<jvxData>* ploty1,
		QVector<jvxData>* ploty2, jvxSize xmin, jvxSize xmax);

	void verify_min_gap(jvxData& minv, jvxData& maxv);

	virtual void keyPressEvent(QKeyEvent* event)override;
	// virtual void keyReleaseEvent(QKeyEvent* event)override;
	void switchMouseMode(jvxPlotMouseMode newMode);

	void replot_markers(
		const QVector<jvxData>& markers_x,
		const QVector<jvxData>& markers_y,
			QCustomPlot* qcp,
			jvxSize gId);

	void updateMarkerPlots(
		QVector<jvxData>& markers_x,
		QVector<jvxData>& markers_y,
		QCustomPlot* qcp, jvxSize gId);

	void removeSingleMarker(jvxData pos_x,
			QVector<jvxData>& markers_x,
			QVector<jvxData>& markers_y,
			QCustomPlot* qcp, jvxSize gId);

	void moveSingleMarker(
		jvxData start_x, 
		jvxData start_y,
		jvxData pos_x, 
		jvxData pos_y,
		QVector<jvxData>& markers_x,
		QVector<jvxData>& markers_y,
		QCustomPlot* qcp, 
		jvxSize gId);

	void activate_marker_edit(
		jvxEditPointMarker markerSelect,
		jvxData pos_x,
		QVector<jvxData>& markers_x,
		QVector<jvxData>& markers_y);

	void reset_marker_td_edit();
	void reset_marker_sec_edit();
	
	/**
	 * This function evaluates the currently shown data plots and then shows only those processors which
	 * fit to the shown data plots.
	 */
	jvxBool showThisProcessor(jvxMeasurementDataProcessorTask task);

	jvxErrorType import_data_plot(
		oneSetDataPlot& dtPlot,
		const std::string& nmMeas,
		const std::string& nmChan);

	void write_single_marker(
		const char* sec_name,
		jvxConfigData** secDat,
		set_markers& oneSet,
		IjvxConfigProcessor* processor);

	void read_single_marker(
		jvxConfigData* secDatArg,
		set_markers& oneSet,
		IjvxConfigProcessor* processor);

	void trigger_proc_equalizer(IjvxQtAcousticMeasurement_process* proc);
	void trigger_proc_hrtfs(IjvxQtAcousticMeasurement_process* proc);

signals:

public slots:

	void changed_td_ymax();
	void changed_td_ymin();
	void toggled_td_yauto(bool);
	void changed_td_xmin();
	void changed_td_xmax();
	void toggled_td_xauto(bool);

	void changed_fd_ymax();
	void changed_fd_ymin();
	void toggled_fd_yauto(bool);
	void changed_fd_xmin();
	void changed_fd_xmax();
	void toggled_fd_xauto(bool);

	void changed_color_p1(int);
	void changed_pixels_p1(int);
	void changed_style_p1(int);
	void changed_markers_p1(int);

	void changed_color_p2(int);
	void changed_pixels_p2(int);
	void changed_style_p2(int);
	void changed_markers_p2(int);

	void import_data_plot1();
	void import_data_plot2();
	void import_data_plot12();

	void show_plot1(bool);
	void show_plot2(bool);

	void changed_td_signal(int);
	void changed_sec_mode(int);

	void mouse_press_td(QMouseEvent *event);
	void mouse_release_td(QMouseEvent *event);
	void mouse_move_td(QMouseEvent *event);
	void mouse_dblclick_td(QMouseEvent *event);
	void mouse_wheel_td(QWheelEvent *event);

	void mouse_press_sec(QMouseEvent *event);
	void mouse_release_sec(QMouseEvent *event);
	void mouse_move_sec(QMouseEvent *event);
	void mouse_dblclick_sec(QMouseEvent *event);
	void mouse_wheel_sec(QWheelEvent *event);

	void changed_axis_linlog(int);

	void toggled_show_td_marker1(bool);
	void toggled_show_td_marker2(bool);
	void clicked_clear_td_marker1();
	void clicked_clear_td_marker2();
	void activated_process_td(int);
	void clicked_process_td(bool);
	void toggled_show_sec_marker1(bool);
	void toggled_show_sec_marker2(bool);
	void clicked_clear_sec_marker1();
	void clicked_clear_sec_marker2();
	void activated_process_sec(int);
	void clicked_process_sec(bool);

	void apply_td_direct_xy();
	void apply_td_move_xy();

	void apply_sec_direct_xy();
	void apply_sec_move_xy();

	void toggled_td_legend(bool);
	void toggled_sec_legend(bool);

	void trigger_processor();

	void copy_td();
	void copy_fd();

	void apply_copy_td_point();
	void changed_data_tag();
	
	void marker_export_fd();
	void marker_export_td();
	void marker_import_fd();
	void marker_import_td();
};

#endif
