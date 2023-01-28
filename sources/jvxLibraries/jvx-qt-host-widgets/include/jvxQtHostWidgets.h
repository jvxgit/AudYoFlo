#ifndef __JVXQTHOSTWIDGETS_EXPORTS_H__
#define __JVXQTHOSTWIDGETS_EXPORTS_H__

#include "IjvxQtSpecificWidget.h"
#include "IjvxQtWidgetFrameDialog.h"

JVX_INTERFACE IjvxQtConnectionWidget : public IjvxQtSpecificWidget_h_base
{
public:
	~IjvxQtConnectionWidget() {};
	virtual jvxErrorType check_connections_ready(jvxApiString* astr_ready) = 0;
};

typedef struct
{
	IjvxQtWidgetFrameDialog* theConnectionsFrame;
	IjvxQtSpecificWidget_base* theConnectionsWidget;
	const char* description;
} jvxQtFrameAndWidget;

JVX_QT_WIDGET_INIT_DECLARE(IjvxQtConnectionWidget, jvxQtConnectionWidget);
JVX_QT_WIDGET_TERMINATE_DECLARE(IjvxQtConnectionWidget, jvxQtConnectionWidget);

JVX_INTERFACE IjvxQtSequencerWidget_report
{
public:
	virtual ~IjvxQtSequencerWidget_report() {};

	virtual void report_sequencer_callback(jvxSize fId) = 0;
	virtual void report_sequencer_stopped() = 0;
	virtual void report_sequencer_error(const char* str1, const char* str2) = 0;
	virtual void report_sequencer_update_timeout() = 0;
};

JVX_INTERFACE IjvxQtSequencerWidget: public IjvxQtSpecificWidget_h_base
{
public:
	virtual ~IjvxQtSequencerWidget() {};

	virtual void update_window_rebuild() = 0;
	virtual void set_control_ref(IjvxMainWindowControl* cntrl, IjvxQtSequencerWidget_report* seqrep) = 0;
	virtual void unset_control_ref() = 0;

	virtual void status_process(jvxSequencerStatus* seqStat, jvxSize* stackDepth) = 0;

	virtual jvxErrorType control_start_process(jvxSize period_msec, jvxSize granularity_state_report) = 0;
	virtual jvxErrorType control_stop_process() = 0;
	virtual jvxErrorType control_continue_process() = 0;
	virtual jvxErrorType immediate_sequencer_step() = 0;

	virtual void add_default_sequence(bool	onlyIfNoSequence, jvxOneSequencerStepDefinition* defaultSteps_run, 
		jvxSize numSteps_run, jvxOneSequencerStepDefinition* defaultSteps_leave, jvxSize numSteps_leave,
		const std::string& nm_def_seq) = 0;
	virtual void remove_all_sequence() = 0;
};

JVX_QT_WIDGET_INIT_DECLARE(IjvxQtSequencerWidget, jvxQtSequencerWidget);
JVX_QT_WIDGET_TERMINATE_DECLARE(IjvxQtSequencerWidget, jvxQtSequencerWidget);

JVX_QT_WIDGET_INIT_DECLARE(IjvxQtWidgetFrameDialog, jvxQtWidgetFrameDialog);
JVX_QT_WIDGET_TERMINATE_DECLARE(IjvxQtWidgetFrameDialog, jvxQtWidgetFrameDialog);

// ===============================================================================

#include "interfaces/qt-develop-host/configureQtDevelopHost_features.h"
JVX_INTERFACE jvx_menu_host_bridge_components_report
{
public:
    virtual ~jvx_menu_host_bridge_components_report() {};

	virtual void updateAllWidgetsOnStateChange() = 0;
	virtual void postMessageError(const char* mess) = 0;
};

JVX_INTERFACE IjvxQtMenuComponentsBridge
{
public:
	virtual ~IjvxQtMenuComponentsBridge() {};

	virtual void createMenues(configureHost_features* host_features_arg,
		IjvxHost* hHost_arg,
		IjvxToolsHost* hTools_arg,
		QMenu* menuConfiguration_arg,
		jvxComponentIdentification* tpAll_arg,
		jvxBool* skipStateSelected_arg,
		jvx_menu_host_bridge_components_report* report_arg) = 0;

	virtual void removeMenues() = 0;

	virtual void updateWindow() = 0;
	virtual void updateWindow_tools() = 0;
	virtual void updateWindowSingle(jvxComponentIdentification tp) = 0;
};

JVX_QT_WIDGET_INIT_DECLARE(IjvxQtMenuComponentsBridge, jvxQtMenuComponentsBridge);
JVX_QT_WIDGET_TERMINATE_DECLARE(IjvxQtMenuComponentsBridge, jvxQtMenuComponentsBridge);

#include "IjvxQtWidgetFrameDialog.h"
JVX_QT_WIDGET_INIT_DECLARE(IjvxQtWidgetFrameDialog, jvxQtWidgetFrameDialog);
JVX_QT_WIDGET_TERMINATE_DECLARE(IjvxQtWidgetFrameDialog, jvxQtWidgetFrameDialog);

#endif
