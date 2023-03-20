#ifndef __MAINCENTRAL_HOSTFACTORY_H__
#define __MAINCENTRAL_HOSTFACTORY_H__

#include <QWidget>
#include <QComboBox>

#include "interfaces/qt/mainWindow_UiExtension.h"

#ifdef JVX_ADD_WIDGET_WRAPPER
#include "CjvxQtSaWidgetWrapper.h"
#endif

class mainCentral_hostfactory : public QWidget,	public mainWindow_UiExtension_hostfactory
{

protected:

	IjvxFactoryHost* theHostRef;
	IjvxMainWindowControl* mwCtrlRef;
	IjvxReport* theReport;
	IjvxToolsHost* theToolsHost;
	jvx::propertyAddress::CjvxPropertyAddressDescriptor ident;
	jvx::propertyDetail::CjvxTranferDetail trans;

private:
	Q_OBJECT

public:

	mainCentral_hostfactory(QWidget* parent);

	virtual void init_submodule(IjvxFactoryHost* theHost) override;

	virtual void init_extend_specific_widgets(IjvxMainWindowControl* ctrl, IjvxQtSpecificWidget_report* rep_spec_widgets)override;

	virtual void terminate_extend_specific_widgets() override;

	virtual void terminate_submodule() override;

	virtual void inform_select(const jvxComponentIdentification& tp, IjvxAccessProperties* propRef) override;

	virtual void inform_active(const jvxComponentIdentification& tp, IjvxAccessProperties* propRef) override;

	virtual void inform_inactive(const jvxComponentIdentification& tp, IjvxAccessProperties* theProps) override;

	virtual void inform_unselect(const jvxComponentIdentification& tp, IjvxAccessProperties* propRef) override;

	virtual void inform_sequencer_about_to_start() override;

	virtual void inform_sequencer_started() override;

	virtual void inform_sequencer_about_to_stop() override;

	virtual void inform_sequencer_stopped() override;

	virtual void inform_viewer_about_to_start(jvxSize* tout_viewer) override;

	virtual void inform_viewer_started() override;

	virtual void inform_viewer_about_to_stop() override;

	virtual void inform_viewer_stopped() override;

	virtual void inform_sequencer_error(const char* err, const char* ferr) override;

	virtual jvxErrorType inform_sequencer_callback(jvxSize functionId) override;

	virtual void return_widget(QWidget** refWidget) override;

	virtual void inform_bootup_complete(jvxBool* wantsToAdjustSize) override;

	virtual void inform_about_to_shutdown()override;

	virtual void inform_update_window(jvxCBitField prio)override;

	virtual void inform_update_window_periodic()override;

	virtual void inform_update_properties(jvxComponentIdentification& tp) override;

	virtual jvxErrorType report_command_request(const CjvxReportCommandRequest& req) override;

	virtual void inform_config_read_complete(const char* fName)override;

	virtual jvxErrorType inform_close_triggered()override;

	virtual void inform_request_shutdown(jvxBool* requestHandled)override;

	virtual void inform_internals_have_changed(const jvxComponentIdentification& tp,
		IjvxObject* theObj,
		jvxPropertyCategoryType cat,
		jvxSize propId,
		bool onlyContent,
		const jvxComponentIdentification& tpTo,
		jvxPropertyCallPurpose purpose) override;

	virtual bool is_ready() override;

	virtual jvxErrorType inform_specific(jvxSize idSpec, jvxHandle* fldSpecific, jvxSize fldTypeId) override;
};

#endif
