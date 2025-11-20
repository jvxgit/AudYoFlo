#ifndef __JVX_MAINWINDOW_CONTROL_H__
#define __JVX_MAINWINDOW_CONTROL_H__

#include <QtWidgets/QDialog>
#include <QtWidgets/QMenu>
#include <QtWidgets/QAction>
#include <QtWidgets/QWidget>

#include "jvx.h"
#include "jvxQtHostWidgets.h"

JVX_INTERFACE IjvxMainWindowControl_connect
{
public:

	virtual ~IjvxMainWindowControl_connect() {};
	
	virtual jvxErrorType JVX_CALLINGCONVENTION forward_trigger_operation(
		jvxMainWindowController_trigger_operation_ids id_operation,
		jvxHandle* theData = 0) = 0;
};

class CjvxMainWindowControl : public QObject, public IjvxMainWindowControl
{
	Q_OBJECT

	typedef struct
	{
		IjvxQtWidgetFrameDialog* fWidget;
		std::list<QDialog*> fQWidgets;
		IjvxQtSpecificWidget_base* widget;
		QAction* action;
	} subWidgetAndAction;

	std::map<std::string, subWidgetAndAction> registeredSubwidgets;

	jvxSize uniqueIdRegisterFcts;
	std::map<jvxSize, QAction*> myRegisteredActions;

	QMenu* menuSystem;
	QMenu* menuApplication;
	QWidget* parentObj;
	IjvxMainWindowControl_connect* report;

	jvxState theState;
public:

	CjvxMainWindowControl();

	virtual jvxErrorType init(IjvxMainWindowControl_connect* rep, QMenu* menAttachControl, QMenu* menAttachParameters, QWidget* parO);

	virtual jvxErrorType terminate();

	virtual jvxErrorType JVX_CALLINGCONVENTION trigger_operation(
		jvxMainWindowController_trigger_operation_ids id_operation,
		jvxHandle* theData = 0)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION register_functional_callback(
		jvxMainWindowController_register_functional_ids id_operation,
		IjvxMainWindowController_report* bwd,
		jvxHandle* privData,
		const char* contentData,
		jvxSize* id, jvxBool is_parameter,
		jvxMainWindowController_register_separator_ids sepEnum = JVX_REGISTER_FUNCTION_MENU_SEPARATOR_IF_NOT_FIRST,
		jvxValue* ini_val = nullptr) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION unregister_functional_callback(jvxSize id) override;

	jvxErrorType activate();
	jvxErrorType deactivate();
	jvxErrorType update_window(jvxCBitField prio = JVX_REPORT_REQUEST_UPDATE_DEFAULT);
	jvxErrorType update_window_periodic();

	jvxErrorType register_action_widget(const std::string& descr, IjvxQtSpecificWidget_base* widget, IjvxQtWidgetFrameDialog* fwidget,
		jvxMainWindowController_register_separator_ids sepEnum = JVX_REGISTER_FUNCTION_MENU_SEPARATOR_IF_NOT_FIRST);
	jvxErrorType unregister_action_widget(const std::string& descr, IjvxQtSpecificWidget_base* widget, IjvxQtWidgetFrameDialog* fwidget);

	void updateSingleAction(QAction* act);

public slots:
	void action_value_selection_slot();
	void action_triggered();
};

#endif