#ifndef __UMAINWINDOW_H__
#define __UMAINWINDOW_H__

#include "ui_mainWindow.h"
#include "jvx.h"
#include <QTimer>
#include "CjvxRtAudioBuffers.h"

#include <qwt_plot_curve.h>
#include <qwt_plot_grid.h>
#include "qwt_text.h"
#include "qwt_legend.h"

// =====================================================================
// Local configuration lookup token for this host
// =====================================================================
#define JVX_QT_HOST_1_SECTION_MAIN "jvxQtHost1_main"
#define JVX_QT_HOST_1_SECTION_COMPONENTS "jvxQtHost1_components"
#define JVX_QT_HOST_1_EXTERNAL_TRIGGER  "jvxQtHost1_exttrigger"

// Print all configuration text in compact form
#define JVX_QT_HOST_1_PRINT_COMPACT_FORM false


#define JVX_QT_COLOR_WARNING Qt::blue
#define JVX_QT_COLOR_ERROR Qt::red
#define JVX_QT_COLOR_SUCCESS Qt::green
#define JVX_QT_COLOR_INFORM Qt::black
#define JVX_QT_WIDTH_BITFIELD_ONE_UNIT 5

#include "jvxStandaloneHost_config.h"

// ======================================================================================
// ======================================================================================
// ======================================================================================
// ======================================================================================
// ======================================================================================

//#include "uMainWindow_defines.h"

#include "codeFragments/commandline/CjvxCommandline.h"
class QDockWidget_ext;

class uMainWindow: public QMainWindow, public Ui::MainWindow, public IjvxReport, public IjvxExternalAudioChannels_data, public CjvxCommandline, public IjvxSequencer_report
{
	friend QDockWidget_ext;

	Q_OBJECT
private:

	typedef enum
	{
		JVX_STANDALONE_HOST_STATE_NONE,
		JVX_STANDALONE_HOST_STATE_WAITING,
		JVX_STANDALONE_HOST_STATE_PLAYING,
		JVX_STANDALONE_HOST_STATE_RECORDING,
		JVX_STANDALONE_HOST_STATE_PROCESSING,
		JVX_STANDALONE_HOST_STATE_LISTENING,
		JVX_STANDALONE_HOST_STATE_SAVING
	} jvxStandaloneHostState;

	struct
	{
		struct
		{
			IjvxObject* hobject;
			IjvxHost* hHost;
		} theHost;

		struct
		{
			IjvxToolsHost* hTools;
		} theTools;

		struct
		{
			IjvxObject* theWrapper;
			IjvxObject* theAsioDriver;
			IjvxObject* theAlgorithm;
		} components;
	} involvedComponents;

	struct
	{
		jvxBool bootupComplete;

		struct
		{
			jvxErrorType returnVal;
			JVX_THREAD_ID theQtThreadId;
		} qthost;

	} subWidgets;

	struct
	{
		jvxInt32 rtv_period_ms;
		struct
		{
			jvxData levelGain_user;
			jvxData levelGain_inuse;
			jvxBool mute;
		} in;
		struct
		{
			jvxData levelGain_user;
			jvxData levelGain_inuse;
			jvxBool mute;
		} out;

		jvxBool listen;
		jvxSize inSlider;
	} systemParams;

	struct
	{
		jvxBool extTrigger;
		JVX_THREAD_ID idQtThread;
		jvxTimeStampData tStamp;
		bool sequenceComplete;
		struct
		{
			QTimer* theTimer;
			jvxBool externalTrigger;
		} runtime;
	} sequencerStruct;

	jvxStandaloneHostState myState;

	struct
	{
		IjvxObject* theObject;
		IjvxTechnology* theTech;
		IjvxExternalAudioChannels* theChannels;
		jvxSize inId;
		jvxSize outId;
	} externalChannels;

	struct
	{
		CjvxRtAudioBuffers theBuffers;
		jvxInt32 framesize_read;
		jvxInt32 framesize_write;
	} readwrite;

	struct
	{
		QTimer* theTimer;
	} rtViewer;

	struct
	{
		QTimer* theTimer;
	} rtPlot;

	struct
	{
		QwtPlotGrid* theGrid_left;
		QwtLegend * theLegend_left;
		QwtPlotGrid* theGrid_right;
		QwtLegend * theLegend_right;
		QwtPlotCurve* theCurve_left;
		QwtPlotCurve* theCurve_right;

		jvxData x_left[JVX_STANDALONE_HOST_NUMBER_PIXELS_PLOT];
		jvxData x_right[JVX_STANDALONE_HOST_NUMBER_PIXELS_PLOT];
		jvxData y_left[JVX_STANDALONE_HOST_NUMBER_PIXELS_PLOT];
		jvxData y_right[JVX_STANDALONE_HOST_NUMBER_PIXELS_PLOT];
	} plot;
public:
	uMainWindow();

	~uMainWindow(void);

	jvxErrorType connect_specific();
	jvxErrorType bootup_specific();
	jvxErrorType shutdown_specific();

	/* = = = = = = = = = = = = = = = = = */
	/* Functions in uMainWindow_bootup.cpp */
	/* = = = = = = = = = = = = = = = = = */

	jvxErrorType initSystem(QApplication* hdlApp, char*[], int);
	jvxErrorType initializeHost();
	void resetJvxReferences();

	jvxErrorType startExternalChannels();
	jvxErrorType checkStatusAlgorithm_engaged(jvxBool& isEngaged);
	jvxErrorType toggleStatusAlgorithm_engaged();

	jvxErrorType prepareChannels(jvxStandaloneHostState targetState);

	jvxErrorType wait_for_process_stop();

	void readAudioFile(std::string& fileName);
	void writeAudioFile(std::string& fileName);
	void updatePlots();
	void updatePlotData();
	void updatePlotData_online();

	/* = = = = = = = = = = = = = = = = = */
	/* Functions in uMainWindow.cpp */
	/* = = = = = = = = = = = = = = = = = */
	void postMessage_outThread(const char* txt, QColor col);
	virtual jvxErrorType JVX_CALLINGCONVENTION report_simple_text_message(const char* txt, jvxReportPriority prio);
	virtual jvxErrorType JVX_CALLINGCONVENTION report_internals_have_changed(const jvxComponentIdentification& thisismytype, IjvxObject* thisisme,
		jvxPropertyCategoryType cat, jvxSize propId, bool onlyContent, const jvxComponentIdentification& tpTo,
		jvxPropertyCallPurpose callpurpose = JVX_PROPERTY_CALL_PURPOSE_NONE_SPECIFIC);
	virtual jvxErrorType JVX_CALLINGCONVENTION report_command_request(jvxCommandRequestType request,jvxHandle *,jvxSize);
	virtual jvxErrorType JVX_CALLINGCONVENTION report_take_over_property(const jvxComponentIdentification& thisismytype, IjvxObject* thisisme,
		 jvxHandle* fld, jvxSize numElements, jvxDataFormat format, jvxSize offsetStart, jvxBool onlyContent,
		 jvxPropertyCategoryType cat, jvxSize propId, const jvxComponentIdentification& tpTo, jvxPropertyCallPurpose callpurp);
	virtual jvxErrorType JVX_CALLINGCONVENTION report_os_specific(jvxSize commandId, void* context);

	virtual jvxErrorType JVX_CALLINGCONVENTION report_event(jvxCBitField event_mask, const char* description, jvxSize sequenceId, jvxSize stepId, jvxSequencerQueueType tp, jvxSequencerElementType stp);
	virtual jvxErrorType JVX_CALLINGCONVENTION sequencer_callback(jvxSize sequenceId, jvxSize stepId, jvxSequencerQueueType tp, jvxSize functionId);


	virtual jvxErrorType JVX_CALLINGCONVENTION get_one_frame(jvxSize register_id, jvxHandle** fld);

	virtual jvxErrorType JVX_CALLINGCONVENTION put_one_frame(jvxSize register_id, jvxHandle** fld);

	virtual jvxErrorType JVX_CALLINGCONVENTION prepare(jvxBool is_input, jvxSize register_id, jvxSize framesize, jvxDataFormat format);

	virtual jvxErrorType JVX_CALLINGCONVENTION start(jvxBool is_input, jvxSize register_id);

	virtual jvxErrorType JVX_CALLINGCONVENTION stop(jvxBool is_input, jvxSize register_id);

	virtual jvxErrorType JVX_CALLINGCONVENTION postprocess(jvxBool is_input, jvxSize register_id);



	void updateStateSequencer_outThread(jvxUInt64 event_mask, int sequenceId, int stepId, int queuetp);

	/* = = = = = = = = = = = = = = = = = */
	/* Functions in uMainWindow_shutdown.cpp */
	/* = = = = = = = = = = = = = = = = = */
	jvxErrorType terminateHost();
	jvxErrorType terminateSystem();
	void fatalStop(const char* str1, const char* str2);

	/* = = = = = = = = = = = = = = = = = */
	/* Functions in uMainWindow_config.cpp */
	/* = = = = = = = = = = = = = = = = = */
	jvxErrorType configureFromFile(std::string fName);
	jvxErrorType configurationToFile(std::string& fName);

	void start_process();
	void stop_process();

	void closeEvent(QCloseEvent* event);
private:

	/* = = = = = = = = = = = = = = = = = */
	/* Functions in uMainWindow_ui.cpp */
	/* = = = = = = = = = = = = = = = = = */

	void updateWindow();

	/* = = = = = = = = = = = = = = = = = */
	/* Functions in uMainWindow_menu.cpp */
	/* = = = = = = = = = = = = = = = = = */
	void report_internals_have_changed_outThread(jvxComponentIdentification tp, IjvxObject* theObj, jvxPropertyCategoryType cat, jvxSize propId, bool onlyContent, jvxComponentIdentification tpTo, jvxPropertyCallPurpose callpurp);
public slots:

	void postMessage_inThread(QString txt, QColor col);

	/* = = = = = = = = = = = = = = = = = */
	/* Functions in uMainWindow_config.cpp */
	/* = = = = = = = = = = = = = = = = = */
	void bootDelayed();

	void report_internals_have_changed_inThread(jvxComponentIdentification, IjvxObject*, jvxPropertyCategoryType, jvxSize, bool, jvxComponentIdentification, jvxPropertyCallPurpose);
	void report_command_request_inThread(short);

	void newSelection_inoutslider();

	void close_app();

	void pushed_Play();
	void pushed_Record();
    void pushed_Process();
    void pushed_Save();
    void pushed_Enhance();
    void pushed_Listen();
    void pushed_Edit();
    void pushed_Load();

	void toggled_loop();
	void toggled_listen();
	void toggled_mute();


	void timerExpired();
	void updateStateSequencer_inThread(jvxUInt64 event_mask, int sequenceId, int stepId, int queuetp);
	void newSelection_currentItem(QTreeWidgetItem* it,int column);

	void timeout_rtViewer();
	void timeout_rtPlot();
	void newValue_level();


signals:
	void emit_postMessage(QString txt, QColor col);
	void emit_bootDelayed();
	void emit_report_internals_have_changed_inThread(jvxComponentType, IjvxObject*, jvxPropertyCategoryType, jvxInt16, bool, jvxComponentType, jvxPropertyCallPurpose);
	void emit_close_app();
	void emit_report_command_request(short);

	void emit_updateStateSequencer(jvxUInt64,int, int, int);

};

#endif
