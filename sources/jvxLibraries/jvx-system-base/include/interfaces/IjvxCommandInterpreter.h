#ifndef __IJVXCOMMANDINTERPRETER_H__
#define __IJVXCOMMANDINTERPRETER_H__

JVX_INTERFACE IjvxCommandInterpreter
{
 public:
  virtual JVX_CALLINGCONVENTION ~IjvxCommandInterpreter(){};

	// this function must be called from within the main thread!! You need to reschedule main if in doubt!
	virtual jvxErrorType JVX_CALLINGCONVENTION interpret_command(const char* command, jvxApiString* result, jvxBool jsonOut) = 0;
};

#endif

