#ifndef __JVX_SEQUENCER_WIDGET_H__
#define __JVX_SEQUENCER_WIDGET_H__

#include "jvx.h"
#include "ui_jvx_sequencer.h"
#include "jvxQtHostWidgets.h"
#include "QTimer"

#define JVX_UPDATE_UI_ELEMENTS 0
#define JVX_UPDATE_STATUS_SEQUENCER 1

class jvx_sequencer_widget: public QWidget, public IjvxQtSequencerWidget, public Ui::Form_sequence_editor, public IjvxSequencer_report
{
protected:

	IjvxHost* theHostRef;
	IjvxQtSpecificWidget_report* theBwdRef;
	IjvxSequencer* theSequencer;
	IjvxReport* theReport;

	IjvxMainWindowControl* theControl;
	IjvxQtSequencerWidget_report* theSeqRep;

	struct
	{
		int cntLabels;
		QTimer* theTimerSeq = nullptr;		
		jvxBool externalTrigger;
		jvxTimeStampData myTimerRef;
		std::string firstError;	
	} runtime;

	QTimer* theTimerWait = nullptr;
	struct
	{
		struct
		{
			jvxSequencerElementType elementTp;
			jvxComponentIdentification targetTp;
			std::string label;
			std::string labelTrue;
			std::string labelFalse;
			jvxSize fId;
			jvxInt64 timeout_ms;
			std::string description;
			jvxSize uId_process;
			jvxBool break_active;
			jvxCBitField assoc_mode;
		} newStep;

		struct
		{
			std::string description;
			std::string label;
		} newSequence;

		struct
		{
			jvxSize sequenceId;
			jvxSize stepId;
			jvxSequencerQueueType queueSelection;
		} selection;

	} currentSetupShown;

	struct
	{
		jvxBool extTrigger;
		JVX_THREAD_ID idQtThread;
		jvxTimeStampData tStamp;
		bool sequenceComplete;
	} others;

	struct
	{
		jvxSize min_width;
		jvxSize min_height;
	} config;

	jvxBool activateApply_step;

	Q_OBJECT

public:

	jvx_sequencer_widget(QWidget* parent);
    ~jvx_sequencer_widget() {};

	QSize sizeHint() const override;
	QSize minimumSizeHint() const override;

	virtual void init(IjvxHost* theHost, jvxCBitField mode = 0, jvxHandle* specPtr = NULL, IjvxQtSpecificWidget_report* bwd = NULL) override;
	virtual void getMyQWidget(QWidget** retWidget, jvxSize id = 0) override;
	virtual void terminate()override;
	virtual void activate() override;
	virtual void deactivate()override;
	virtual void processing_started() override;
	virtual void processing_stopped()override;
	virtual void update_window(jvxCBitField prio, const char* propLst = nullptr) override;
	virtual void update_window_periodic() override;

	virtual void update_window_rebuild() override;
	virtual void set_control_ref(IjvxMainWindowControl* cntrl, IjvxQtSequencerWidget_report* seqRep)override;
	virtual void unset_control_ref() override;
	virtual jvxErrorType control_start_process(jvxSize period_msec, jvxSize granularity_state_report) override;
	virtual jvxErrorType control_stop_process() override;
	virtual jvxErrorType control_continue_process() override;
	virtual jvxErrorType immediate_sequencer_step() override;

	virtual void add_default_sequence(bool	onlyIfNoSequence, jvxOneSequencerStepDefinition* defaultSteps_run, 
		jvxSize numSteps_run, jvxOneSequencerStepDefinition* defaultSteps_leave, jvxSize numSteps_leave,
		const std::string& nm_default) override;
	virtual void remove_all_sequence() override;
	virtual void status_process(jvxSequencerStatus* seqStat, jvxSize* stackDepth) override;

	void resetCurrentSetup(bool resetSeq = true, bool resetStep = true, bool resetSelect = true);

	void updateWindow_rebuild();
	void updateWindow_highlight_set(jvxSize& sequenceId, jvxSize& stepId, jvxSequencerQueueType& tp, QColor col);
	void updateWindow_highlight_reset(QColor col);
	jvxErrorType wait_for_process_stop();

	void updateWindow(jvxBitField whattoupdate);

	bool checkRightStateForEdit();
	void reportError(std::string err);

	virtual jvxErrorType JVX_CALLINGCONVENTION report_event(jvxSequencerStatus stat,
		jvxCBitField event_mask, const char* description, jvxSize sequenceId, jvxSize stepId, 
		jvxSequencerQueueType tp, jvxSequencerElementType stp, jvxSize fId, jvxSize operation_state, jvxBool inMainLoop) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION sequencer_callback(jvxSize sequenceId, jvxSize stepId, jvxSequencerQueueType tp, jvxSize functionId)override;

	void updateStateSequencer_outThread(jvxCBitField event_mask, jvxSize sequenceId, jvxSize stepId, jvxSequencerQueueType queuetp);

	void getReferenceComponentType(jvxComponentIdentification tp, IjvxObject*& theObj, bool isStandardCommand);
	void returnReferenceComponentType(jvxComponentIdentification tp, IjvxObject* theObj, bool isStandardCommand);
	//void listComponentsInComboBox(bool isStandardCommand);
	void listConditionsInComboBox(jvxComponentIdentification tp, jvxBool& allowApply);
	void listCommandsInComboBox(jvxComponentIdentification tp, jvxBool& allowApply);
	void listRelativeJumpsInComboBox(jvxComponentIdentification tp);
	void listAllLabelsInSequence(QComboBox* theBox, QLineEdit* ledit, std::string& txt, QLabel* lab, const std::string& labTxt);
	void listAllLabelsSequence(QComboBox* theBox, QLineEdit* ledit, std::string& txt, QLabel* lab, const std::string& labTxt);
	void viewTimeout(QLineEdit* line);

	void setTimeout();
	
	void start_timer(jvxSize period_msec);
	void stop_timer();

	void start_timer_wait(jvxSize period_msec);
	void stop_timer_wait();

signals:

	void emit_updateStateSequencer(jvxCBitField, jvxSize, jvxSize, jvxSequencerQueueType);
	void emit_timerExpired_local();

public slots:
	// Common
	void newSelection_currentItem(QTreeWidgetItem*,int);

	void newButtonPush_edit();
	void newButtonPush_insert();
	void newButtonPush_remove();
	void newButtonPush_up();
	void newButtonPush_down();

	void newText_timeout_step();
	void newText_label_step();
	void newSelection_labeltrue_step(int);
	void newSelection_labelfalse_step(int);
	void newSelection_commandid_step(int);
	void newSelection_elementtype_step(int);
	void newSelection_component_step(int);
	void newText_description_step();

	void newButtonPush_seqadd();
	void newButtonPush_seqreset();
	void newButtonPush_seqremove();
	void newText_description_sequence();
	void newText_label_sequence();

    void newButtonPush_add_process();
    void newButtonPush_start_process();
	void newButtonPush_stop_process();
	void newButtonPush_stop_sequence();


	void newButtonPush_remove_process();

	void newButtonPush_up_sequence();
	void newButtonPush_down_sequence();

	void newSelectionCheckbox_loop(bool);

	void updateStateSequencer_inThread(jvxCBitField event_mask, jvxSize sequenceId, jvxSize stepId, jvxSequencerQueueType);

	void timerExpired();
	void timerExpired_local();
	void timerExpiredWait();

	void newText_process_match();
	void newSelection_process(int sel);

	void pushed_print_code();
	void pushed_duplicate_seq();
	void pushed_duplicate_step();
	void pushed_apply_seq();

	void newText_labeltrue_p1();
	void newText_labelfalse_p2();

	void newText_callbackid();
	void local_close();

	void toggle_step_active(bool);
	void toggle_break_active(bool);
	void trigger_continue();
	void toggle_break_seq(bool);

	void newSelectionAssociatedMode(int idx);
	void newSelectionRunMode(int idx);

	void buttonPushedModeAllOff();
	void buttonPushedModeAllOn();
};

#endif
