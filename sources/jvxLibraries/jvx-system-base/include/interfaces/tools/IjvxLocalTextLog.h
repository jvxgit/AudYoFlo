#ifndef _IJVXLOCALTEXTLOG_H__
#define _IJVXLOCALTEXTLOG_H__

typedef enum
{
	JVX_LOCAL_TEXT_LOG_CONFIG_SEPARATOR_CHAR = 1
} jvxLocalTextLogConfigType;

JVX_INTERFACE IjvxLocalTextLog : public IjvxObject
{
public:
	virtual JVX_CALLINGCONVENTION ~IjvxLocalTextLog() {};
	virtual jvxErrorType JVX_CALLINGCONVENTION initialize(IjvxHiddenInterface* hostRef) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION terminate() = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION start(jvxSize bsize) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION stop() = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION attach_entry(const char* txt) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION read_update(jvxApiString* strReturn, 
		jvxContext* uId, 
		jvxSize* numCharLost,
		jvxBool* newReadStarted) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION status_update(jvxSize* roundsOnReturn, jvxSize* idxWriteOnReturn) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION clear() = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION configure(jvxLocalTextLogConfigType tp, jvxHandle* load) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION get_text_lost_characters(jvxApiString* strReturn, jvxSize numCharsLost) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION get_text_new_read(jvxApiString* strReturn) = 0;

};

#endif
