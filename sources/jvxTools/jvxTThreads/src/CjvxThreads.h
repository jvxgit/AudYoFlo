#ifndef __CJVXTHREADCONTROLLER_H__
#define __CJVXTHREADCONTROLLER_H__

#include <string>
#include <vector>

#include "jvx.h"
#include "common/CjvxObject.h"

#include "jvx_threads.h"

class CjvxThreads : public IjvxThreads, public CjvxObject
{
public:

private:
	struct
	{
		jvxHandle* hdl = nullptr;
		jvx_thread_handler cbs;
	} thread;

	JVX_THREADS_FORWARD_C_CALLBACK_DECLARE_ALL;
	IjvxThreads_report* rep;

public:

	//! Constructor: Do nothing
	CjvxThreads(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);

	//! Destructor
	~CjvxThreads();

	// =====================================================================================
	// Interface API
	// =====================================================================================
	virtual jvxErrorType JVX_CALLINGCONVENTION initialize(IjvxThreads_report* bwdReport) override;
	
	virtual jvxErrorType JVX_CALLINGCONVENTION start(jvxSize timeout, jvxBool push_prio, jvxBool rep_tstamp, jvxBool nonblock, jvxBool async) override;
	
	virtual jvxErrorType JVX_CALLINGCONVENTION set_thread_prio(jvxBool highPrio) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION thread_id(JVX_THREAD_ID* t_id) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION stop(jvxSize timeout_stop) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION terminate()override;

	virtual jvxErrorType JVX_CALLINGCONVENTION trigger_wakeup(jvxBool triggere_even_if_busy )override;

#include "codeFragments/simplify/jvxObjects_simplify.h"

};

#endif
