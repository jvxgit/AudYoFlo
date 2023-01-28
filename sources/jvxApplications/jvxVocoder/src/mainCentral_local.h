#ifndef __MAINCENTRAL_H__
#define __MAINCENTRAL_H__

#include "ui_mainCentral.h"
#include "jvx.h"
#include "interfaces/qt/mainWindow_UiExtension.h"

class mainCentral: public QWidget, public mainWindow_UiExtension_host, public Ui::Form_mainCentral
{
private:
	struct
	{
		struct
		{
			jvxSize id_MixingCrit;
			jvxSize id_pitchFrequency;
			jvxSize id_pitchFrequencyMin;
			jvxSize id_pitchFrequencyMax;
			jvxSize id_outputSelect;
			jvxSize id_inputSelect;
			jvxSize id_hardVuVDecision;
			jvxSize id_autoModeVuv;
			jvxSize id_autoModePitch;
			jvxSize id_lpcOrder;
			jvxSize id_volume;
			jvxSize id_pitchScale;
			jvxSize id_formantWarp;
		} node;
	} properties;

	IjvxHost* theHostRef;
	jvxBool processing;

	Q_OBJECT

public:

	mainCentral(QWidget* parent): QWidget(parent){};

	// ====================================================================
	void init_submodule(IjvxHost* theHost) override;
	
	virtual void get_reference_feature(jvxHandle** priv) override;

	void terminate_submodule() override;

	virtual void init_extend_specific_widgets(IjvxMainWindowControl* ctrl, IjvxQtSpecificWidget_report* rep_spec_widgets) override;

	virtual void terminate_extend_specific_widgets() override;

	void return_widget(QWidget**) override;

	void inform_bootup_complete(jvxBool* wantsToAdjustSize) override;

	virtual void inform_about_to_shutdown()override{};

	void inform_request_shutdown(jvxBool *requestHandled)override;

	void inform_select(const jvxComponentIdentification& tp, IjvxAccessProperties* theProps)override;

	void inform_unselect(const jvxComponentIdentification& tp, IjvxAccessProperties* propRef)override;

	void inform_active(const jvxComponentIdentification& tp, IjvxAccessProperties* theProps)override;

	void inform_inactive(const jvxComponentIdentification& tp, IjvxAccessProperties* propRef)override;

	void inform_sequencer_about_to_start()override;

	virtual void inform_sequencer_started() override;

	void inform_sequencer_about_to_stop()override;

	void inform_sequencer_stopped()override;

	void inform_viewer_about_to_start(jvxSize* tout_msec)override;

	virtual void inform_viewer_started() override;

	void inform_viewer_about_to_stop()override;

	void inform_viewer_stopped()override;

	void inform_update_window(jvxCBitField prio = (1 << JVX_REPORT_REQUEST_UPDATE_WINDOW_SHIFT))override;

	void inform_update_window_periodic()override;

	virtual void inform_update_properties(jvxComponentIdentification& tp) override {};

	void inform_internals_have_changed(const jvxComponentIdentification& tp, IjvxObject* theObj, jvxPropertyCategoryType cat, jvxSize propId, bool onlyContent, 
		const jvxComponentIdentification& tpTo, jvxPropertyCallPurpose purpose)override;

	virtual jvxErrorType inform_sequencer_callback(jvxSize functionId)override;

	void inform_sequencer_error(const char *, const char *) override
	{
	};

	virtual jvxErrorType inform_close_triggered() override
	{
		return(JVX_NO_ERROR);
	};

	virtual void inform_config_read_complete(const char* fName)override
	{ 
	};

	virtual jvxErrorType inform_specific(jvxSize idSpec, jvxHandle* fldSpecific, jvxSize fldTypeId) override
	{
		return JVX_ERROR_UNSUPPORTED;
	};

	virtual bool is_ready() override { return true; };
	// ====================================================================

	void refresh_property_ids();
	void reset_property_ids();

	void update_window_user(jvxCBitField prio = (jvxCBitField)1 << JVX_REPORT_REQUEST_UPDATE_WINDOW_SHIFT);
	void update_window_periodic();

public slots:

	void button_out1();
    void button_out2();
    void button_out3();
    void button_out4();
	void button_out5();
	void toggle_pitch(bool);
	void toggle_autovuv(bool);
	void select_in(int);
	void changed_mix(int);
	void changed_pitch_scale(int);
	void newValue_pitch_max();
	void newValue_pitch_min();
	void newValue_lpc_order();
	void newValue_form_warp(int);
	void newValue_pitch();

};

#endif
