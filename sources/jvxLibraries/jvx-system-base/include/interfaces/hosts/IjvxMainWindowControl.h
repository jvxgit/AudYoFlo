#ifndef __IJVXMAINWINDOWCONTROL_H__
#define __IJVXMAINWINDOWCONTROL_H__

JVX_INTERFACE IjvxMainWindowController_report
{
public:
	virtual ~IjvxMainWindowController_report() {};
	virtual jvxErrorType JVX_CALLINGCONVENTION get_variable_edit(jvxHandle* privData, jvxValue& var, jvxSize id) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION set_variable_edit(jvxHandle* privData, jvxValue& var, jvxSize id) = 0;
};

JVX_INTERFACE IjvxMainWindowControl
{
public:

	IjvxMainWindowControl(){};

	virtual jvxErrorType JVX_CALLINGCONVENTION trigger_operation(
		jvxMainWindowController_trigger_operation_ids id_operation,
		jvxHandle* theData = 0) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION register_functional_callback(
		jvxMainWindowController_register_functional_ids id_operation,
		IjvxMainWindowController_report* bwd,
		jvxHandle* privData,
		const char* showData,
		jvxSize* id, jvxBool is_parameter, 
		jvxMainWindowController_register_separator_ids separator = JVX_REGISTER_FUNCTION_MENU_NO_SEPARATOR,
		jvxValue* ini_val = nullptr) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION unregister_functional_callback(jvxSize id) = 0;

};

#endif
