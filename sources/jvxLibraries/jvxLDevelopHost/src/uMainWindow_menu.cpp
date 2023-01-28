#include <QtWidgets/QMessageBox>
#include <QtWidgets/QListWidgetItem>
#include "uMainWindow.h"


jvxErrorType 
uMainWindow::get_variable_edit(jvxHandle* privData, jvxValue& val, jvxSize id)
{
	if (id == id_rtview_period_props_ms)
	{
		val.assign(theHostFeatures.rtv_period_props_ms);
	}
	if (id == id_rtview_period_plots_ms)
	{
		val.assign(theHostFeatures.rtv_period_plots_ms);
	}
	if (id == id_period_seq_ms)
	{
		val.assign(theHostFeatures.timeout_period_seq_ms);
	}
	
	if (id == id_rtview_period_maincentral_ms)
	{
		val.assign(theHostFeatures.timeout_viewer_maincentral_ms);
	}
	if (id == id_autoc_id)
	{
		val.assign(systemParams.auto_start);
	}
	if (id == id_skip_ssel_id)
	{
		val.assign(systemParams.skipStateSelected);
	}
	if (id == id_enc_config_id)
	{
		val.assign(systemParams.encryptionConfigFiles);
	}
	return JVX_NO_ERROR;
}

jvxErrorType 
uMainWindow::set_variable_edit(jvxHandle* privData, jvxValue& val, jvxSize id)
{
	if (id == id_rtview_period_props_ms)
	{
		val.toContent(&theHostFeatures.rtv_period_props_ms);
		subWidgets.realtimeViewer.props.theWidget->setPeriod_ms(theHostFeatures.rtv_period_props_ms);
	}
	if (id == id_rtview_period_plots_ms)
	{
		val.toContent(&theHostFeatures.rtv_period_plots_ms);
		subWidgets.realtimeViewer.plots.theWidget->setPeriod_ms(theHostFeatures.rtv_period_plots_ms);
	}
	if (id == id_period_seq_ms)
	{
		val.toContent(&theHostFeatures.timeout_period_seq_ms);
	}
	if (id == id_rtview_period_maincentral_ms)
	{
		val.toContent(&theHostFeatures.timeout_viewer_maincentral_ms);
	}
	if (id == id_autoc_id)
	{
		systemParams.auto_start = !systemParams.auto_start;
	}
	if (id == id_skip_ssel_id)
	{
		systemParams.skipStateSelected  = !systemParams.skipStateSelected;
	}
	if (id == id_enc_config_id)
	{
		systemParams.encryptionConfigFiles = !systemParams.skipStateSelected;
	}
	return JVX_NO_ERROR;
}

void
uMainWindow::free_ext_menu_system()
{
	theControl.unregister_functional_callback(	
		id_rtview_period_props_ms);

	theControl.unregister_functional_callback(
		id_rtview_period_plots_ms);

	theControl.unregister_functional_callback(
		id_period_seq_ms);
	
	theControl.unregister_functional_callback(
		id_rtview_period_maincentral_ms);

	theControl.unregister_functional_callback(
		id_autoc_id);

	theControl.unregister_functional_callback(
		id_skip_ssel_id);

	theControl.unregister_functional_callback(
		id_enc_config_id);

}

void
uMainWindow::setup_ext_menu_system()
{
	theControl.register_functional_callback(JVX_MAINWINDOWCONTROLLER_REGISTER_MENU_ITEM_INT_VALUE,
		static_cast<IjvxMainWindowController_report*>(this),
		reinterpret_cast<jvxHandle*>(this),
		"Timeout Period Realtime Properties - properties [ms]",
		&id_rtview_period_props_ms,
		false, JVX_REGISTER_FUNCTION_MENU_SEPARATOR_IF_NOT_FIRST);

	theControl.register_functional_callback(JVX_MAINWINDOWCONTROLLER_REGISTER_MENU_ITEM_INT_VALUE,
		static_cast<IjvxMainWindowController_report*>(this),
		reinterpret_cast<jvxHandle*>(this),
		"Timeout Period Realtime Properties - plots [ms]",
		&id_rtview_period_plots_ms,
		false, JVX_REGISTER_FUNCTION_MENU_NO_SEPARATOR);

	theControl.register_functional_callback(JVX_MAINWINDOWCONTROLLER_REGISTER_MENU_ITEM_INT_VALUE,
		static_cast<IjvxMainWindowController_report*>(this),
		reinterpret_cast<jvxHandle*>(this),
		"Timeout Period Sequencer [ms]",
		&id_period_seq_ms,
		false, JVX_REGISTER_FUNCTION_MENU_NO_SEPARATOR);
	
	theControl.register_functional_callback(JVX_MAINWINDOWCONTROLLER_REGISTER_MENU_ITEM_INT_VALUE,
		static_cast<IjvxMainWindowController_report*>(this),
		reinterpret_cast<jvxHandle*>(this),
		"Timeout Period Main Central [ms]",
		&id_rtview_period_maincentral_ms,
		false, JVX_REGISTER_FUNCTION_MENU_NO_SEPARATOR);

	theControl.register_functional_callback(JVX_MAINWINDOWCONTROLLER_REGISTER_MENU_ITEM_ONOFF,
		static_cast<IjvxMainWindowController_report*>(this),
		reinterpret_cast<jvxHandle*>(this),
		"Autostart",
		&id_autoc_id,
		false, JVX_REGISTER_FUNCTION_MENU_SEPARATOR);

	theControl.register_functional_callback(JVX_MAINWINDOWCONTROLLER_REGISTER_MENU_ITEM_ONOFF,
		static_cast<IjvxMainWindowController_report*>(this),
		reinterpret_cast<jvxHandle*>(this),
		"Skip State Selected",
		&id_skip_ssel_id,
		false, JVX_REGISTER_FUNCTION_MENU_SEPARATOR);

	theControl.register_functional_callback(JVX_MAINWINDOWCONTROLLER_REGISTER_MENU_ITEM_ONOFF,
		static_cast<IjvxMainWindowController_report*>(this),
		reinterpret_cast<jvxHandle*>(this),
		"Encrypt Configuration",
		&id_enc_config_id,
		false, JVX_REGISTER_FUNCTION_MENU_NO_SEPARATOR);
}



void
uMainWindow::updateAllWidgetsOnStateChange()
{
	this->subWidgets.realtimeViewer.props.theWidget->updateAllWidgetsOnStateChange();
	this->subWidgets.realtimeViewer.plots.theWidget->updateAllWidgetsOnStateChange();
	this->subWidgets.connections.theWidget->update_window();
}

void
uMainWindow::postMessageError(const char* mess)
{
	this->postMessage_inThread(mess, Qt::red);
}
