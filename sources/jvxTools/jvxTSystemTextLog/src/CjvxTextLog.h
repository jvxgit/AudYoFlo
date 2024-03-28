#ifndef __CJVXTEXTLOG_H__
#define __CJVXTEXTLOG_H__

#include "jvx.h"
#include "common/CjvxObject.h"
#include "jvx_threads.h"

#ifdef JVX_OS_WINDOWS
#include "windows/StackWalker.h"
// All stackwalker files have been downloaded from here:
// https://github.com/JochenKalmbach/StackWalker
#endif

class CjvxTextLog: public IjvxTextLog, public CjvxObject
{
	enum
	{
		RTP_LOGFILE_NOTINIT,
		RTP_LOGFILE_INIT,
		RTP_LOGFILE_STARTED
	} logFileState;

	struct
	{
		char* characterBuffer; 
		int lBuffer;
		int numCharactersInBuffer;
		int idxWrite;
	} buffer;

	struct
	{
		FILE* outFile;
		int toBeWrittenAtOnce;
	} file;

	struct
	{
		char* characterBuffer;
		int lBuffer;
		int numIterations;
		int idxWrite;
	} cbuffer;

	bool endlessLoopOn;

	JVX_MUTEX_HANDLE mutexFBuffer;
	JVX_MUTEX_HANDLE mutexCBuffer;
	JVX_MUTEX_HANDLE mutexFile;
	JVX_MUTEX_HANDLE mutexConsole;
	JVX_MUTEX_HANDLE mutexModules;

	jvxTick startTimer;

	jvxTimeStampData tStampRef;

	jvxHandle* threadHdl;
	jvx_thread_handler myThreadHandler;
	JVX_THREADS_FORWARD_C_CALLBACK_DECLARE_ALL

	jvxSize dbgLevel = 0;
	jvxBool dbgOutCout = false;

	std::vector<std::string> moduleFilterExpression;

	std::map<std::string, jvxSize> reg_modules;

private:
	void prefilterEntries(const std::string& strTime, std::string& out, std::string& txtStr);

public:

	CjvxTextLog(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);
	~CjvxTextLog();

	virtual jvxErrorType JVX_CALLINGCONVENTION initialize(IjvxHiddenInterface* hostRef, const char* strFileName, 
		jvxSize loclBuffer, jvxSize writeAtOnce, jvxSize circBufferSize, jvxSize dbgLevel, jvxBool dbgOutCout) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION addTextLogExpression(const char* oneExpr) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION terminate() override;

	virtual jvxErrorType JVX_CALLINGCONVENTION start() override;
	virtual jvxErrorType JVX_CALLINGCONVENTION stop() override;

	virtual jvxErrorType JVX_CALLINGCONVENTION debug_config(jvxSize* level, const char* moduleName, jvxBool* moduleTextLog, jvxBool* dbgOutCout) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION addEntry_direct(const char* txt, const char* moduleName = NULL, jvxCBitField outEnum = (jvxCBitField)-1) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION addEntry_buffered(const char* txt, const char* moduleName = NULL, jvxCBitField outEnum = (jvxCBitField)-1) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION addEntry_buffered_nb(const char* txt, const char* moduleName = NULL, jvxCBitField outEnum = (jvxCBitField)-1) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION produceStackframe_direct(const char* description) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION current_size_cbuffer_return(jvxSize* sz, jvxTextLogOutputPositionCircBuffer* pos, jvxSize* missed) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION update_cbuffer_return(jvxByte* buf, jvxSize* sz, jvxTextLogOutputPositionCircBuffer* pos, jvxSize* numLostBytes) override;

#include "codeFragments/simplify/jvxObjects_simplify.h"
};

#endif