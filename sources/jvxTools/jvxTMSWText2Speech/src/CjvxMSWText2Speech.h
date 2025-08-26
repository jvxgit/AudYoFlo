#ifndef __CJVXTEXT2SPEECH_H__
#define __CJVXTEXT2SPEECH_H__

#include "jvx.h"
#include "common/CjvxObject.h"

#include "jvx_threads.h"

#include <Windows.h>
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // some CString constructors will be explicit

#include <atlbase.h>
#include <atlstr.h>

// TODO: reference additional headers your program requires here
#include <sapi.h>
#include <iostream>


class CjvxMSWText2Speech : public IjvxText2Speech, public CjvxObject
{
private:

	jvxSize refCount;
	JVX_MUTEX_HANDLE safeAccess;
	jvxSize seedUniqueIds;
	typedef struct
	{
		std::string text;
		IjvxText2Speech_report* reportHandle;
		jvxSize selector;
		jvxText2SpeechFormat format;
	} oneEntryQueue;

	std::vector<oneEntryQueue> theTasks;
	jvxBool thread_run;
	IjvxText2Speech_report* rep;
	jvxSize uId;
	std::string mess;
	jvxText2SpeechFormat format;

	jvxHandle* thread_handle;
	jvx_thread_handler thread_handler;

	CComPtr <ISpVoice>	cpVoice;
	CComPtr <ISpStream> cpStream;

public:
	CjvxMSWText2Speech(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);

	~CjvxMSWText2Speech();

	virtual jvxErrorType JVX_CALLINGCONVENTION initialize(IjvxHiddenInterface* theHostRef);

	virtual jvxErrorType JVX_CALLINGCONVENTION prepare();

	virtual jvxErrorType JVX_CALLINGCONVENTION add_task(const char* txtFld, jvxText2SpeechFormat* out_format, IjvxText2Speech_report* backward_ref, jvxSize* uniqueId);

	virtual jvxErrorType JVX_CALLINGCONVENTION remove_task(jvxSize uniqueId);

	virtual jvxErrorType JVX_CALLINGCONVENTION postprocess();

	virtual jvxErrorType JVX_CALLINGCONVENTION terminate() ;

#include "codeFragments/simplify/jvxObjects_simplify.h"

	
	static jvxErrorType cb_thread_startup(jvxHandle* privateData_thread, jvxInt64 timestamp_us);
	static jvxErrorType cb_thread_timer_expired(jvxHandle* privateData_thread, jvxInt64 timestamp_us, jvxSize* delta_ms);
	static jvxErrorType cb_thread_wokeup(jvxHandle* privateData_thread, jvxInt64 timestamp_us, jvxSize* delta_ms);
	static jvxErrorType cb_thread_stopped(jvxHandle* privateData_thread, jvxInt64 timestamp_us);

	jvxErrorType ic_thread_startup(jvxInt64 timestamp_us);
	jvxErrorType ic_thread_timer_expired(jvxInt64 timestamp_us);
	jvxErrorType ic_thread_wokeup(jvxInt64 timestamp_us);
	jvxErrorType ic_thread_stopped(jvxInt64 timestamp_us);
};

#endif
