#ifndef __CJVXTEXTLOG_H__
#define __CJVXTEXTLOG_H__

#include "jvx.h"
#include "common/CjvxObject.h"

class CjvxLocalTextLog: public IjvxLocalTextLog, public CjvxObject
{
	struct
	{
		char* characterBuffer; 
		jvxSize lBuffer;
		jvxSize idxWrite;
		jvxSize fRounds;
	} circular_buffer;

	struct oneReadPosition
	{
		jvxSize uId;
		jvxSize sshot_idxWrite;
		jvxSize sshot_fRounds;
	};

	std::map<jvxSize, oneReadPosition> logs_read;
	JVX_MUTEX_HANDLE safeAccessList;
	JVX_MUTEX_HANDLE safeAccessBuffer;

	jvxTimeStampData tStampRef;
	jvxSize uIdEntries;

	std::string separator_string;
private:

public:

	CjvxLocalTextLog(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);
	~CjvxLocalTextLog();

	virtual jvxErrorType JVX_CALLINGCONVENTION initialize(IjvxHiddenInterface* hostRef) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION terminate() override;
	virtual jvxErrorType JVX_CALLINGCONVENTION start(jvxSize bsize) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION stop() override;

	virtual jvxErrorType JVX_CALLINGCONVENTION attach_entry(const char* txt)override;
	virtual jvxErrorType JVX_CALLINGCONVENTION read_update(jvxApiString* strReturn, 
		jvxContext* uId, jvxSize* numCharLost,
		jvxBool* newReadStarted)override;
	virtual jvxErrorType JVX_CALLINGCONVENTION status_update(jvxSize* roundsOnReturn, jvxSize* idxWriteOnReturn)override;
	virtual jvxErrorType JVX_CALLINGCONVENTION clear()override;

	virtual jvxErrorType JVX_CALLINGCONVENTION  configure(jvxLocalTextLogConfigType tp, jvxHandle* load) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION get_text_lost_characters(jvxApiString* strReturn, jvxSize numCharsLost)override;
	virtual jvxErrorType JVX_CALLINGCONVENTION get_text_new_read(jvxApiString* strReturn) override;

#include "codeFragments/simplify/jvxObjects_simplify.h"
};

#endif