#ifndef __CJVXCOMMANDLINE_H__
#define __CJVXCOMMANDLINE_H__

#include <vector>
#include <string>
#include "jvx.h"

class CjvxCommandLine: public IjvxCommandLine
{
protected:

	jvxState state_parse;

	struct oneEntryContentCommandLine
	{
		jvxInt32 cInt;
		jvxSize cSize;
		jvxData cData;
		std::string cStr;
	} ;

	struct oneEntryCommandLine
	{
		std::string scan_token;
		std::string scan_tokenshortcut;
		std::string scan_description;
		std::string default_setting;
		jvxDataFormat format_scan;
		jvxSize num_args_max;
		jvxBool expect_second_arg;

		jvxSize num_specified;
		std::vector<oneEntryContentCommandLine> theContent;
	};

	std::map<std::string, oneEntryCommandLine> registered_options;

	std::vector<std::string> copiedargv;
	std::string copiedappname;

	std::vector<std::string> unmatched_args;

public:

	CjvxCommandLine();
	~CjvxCommandLine();

	jvxErrorType assign_args(const char* clStr[], int clNum, const char* appname) override;
	jvxErrorType unassign_args() override;

	jvxErrorType register_option(
		const char* scan_token,
		const char* scan_tokenshortcut,
		const char* scan_description,
		const char* default_assign = "",
		jvxBool expect_second_arg = false,
		jvxDataFormat format_scan = JVX_DATAFORMAT_NONE,
		jvxSize num_args_max = 1) override;

	virtual jvxErrorType reset_command_line_content()override;

	virtual jvxErrorType reset_command_line() override;

	virtual jvxErrorType parse_command_line(jvxSize* idErrToken, jvxApiString* descr = NULL, jvxBool allow_unmatched = false)override;

	virtual jvxErrorType number_entries_option(const char* scan_token, jvxSize* numEntries)override;

	virtual jvxErrorType content_entry_option(const char* scan_token, jvxSize id, jvxHandle* fld, jvxDataFormat form)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION arg_app_name(jvxApiString* retStr)override;
	virtual jvxErrorType JVX_CALLINGCONVENTION number_args(jvxSize* num)override;
	virtual jvxErrorType JVX_CALLINGCONVENTION single_arg_index(jvxSize id, jvxApiString* retStr)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION number_unmatched(jvxSize* sz)override;
	virtual jvxErrorType JVX_CALLINGCONVENTION single_unmatched_index(jvxSize id, jvxApiString* retStr)override;
	
	virtual void JVX_CALLINGCONVENTION printUsage(jvxApiString* usage_string = NULL)override;
};

#endif
