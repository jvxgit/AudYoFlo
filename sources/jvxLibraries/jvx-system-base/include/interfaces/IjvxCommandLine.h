#ifndef __IJVXCOMMANDLINE_H__
#define __IJVXCOMMANDLINE_H__

JVX_INTERFACE IjvxCommandLine
{
public:
	virtual JVX_CALLINGCONVENTION ~IjvxCommandLine(){};

	virtual  jvxErrorType JVX_CALLINGCONVENTION assign_args(const char* clStr[], int clNum, const char* appname) = 0;
	
	virtual  jvxErrorType JVX_CALLINGCONVENTION unassign_args() = 0;
	
	virtual  jvxErrorType JVX_CALLINGCONVENTION register_option(
		const char* scan_token,
		const char* scan_tokenshortcut,
		const char* scan_description,
		const char* default_assign = "",
		jvxBool expect_second_arg = false,
		jvxDataFormat format_scan = JVX_DATAFORMAT_NONE,
		jvxSize num_args_max = 1) = 0;

	virtual  jvxErrorType JVX_CALLINGCONVENTION reset_command_line_content() = 0;

	virtual  jvxErrorType JVX_CALLINGCONVENTION reset_command_line() = 0;

	virtual  jvxErrorType JVX_CALLINGCONVENTION parse_command_line(jvxSize* idErrToken, jvxApiString* descr = NULL, jvxBool allow_unmatched = false) = 0;

	virtual  jvxErrorType JVX_CALLINGCONVENTION  number_entries_option(const char* scan_token, jvxSize* numEntries) = 0;

	virtual  jvxErrorType JVX_CALLINGCONVENTION  content_entry_option(const char* scan_token, jvxSize id, jvxHandle* fld, jvxDataFormat form) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION arg_app_name(jvxApiString* retStr) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION number_args(jvxSize* num) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION single_arg_index(jvxSize id, jvxApiString* retStr) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION number_unmatched(jvxSize* sz) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION single_unmatched_index(jvxSize id, jvxApiString* retStr) = 0;

	virtual  void  JVX_CALLINGCONVENTION printUsage(jvxApiString* usage_string = NULL) = 0;
};

#endif
