#ifndef __CJVXLOGREMOTEHANDLER_H__
#define __CJVXLOGREMOTEHANDLER_H__

#include <string>
#include <vector>

#include "jvx.h"
#include "common/CjvxObject.h"

namespace _myJvxTools {

class CjvxLogRemoteHandler : public IjvxLogRemoteHandler, public CjvxObject
{
public:

	/** Enum for the datatype for stored datachunk fields, can be enhanced in the future
	 *///===================================================
private:

	std::string logDescriptor;
	jvxLogLevel lev;
	jvxBool isLocked = false;
public:

	//! Constructor: Do nothing
	CjvxLogRemoteHandler(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);

	// =====================================================================================
	// Interface API
	// =====================================================================================
	virtual jvxErrorType initialize(IjvxHiddenInterface* hostRef) override;
	virtual jvxErrorType terminate() override;
	virtual void configure(const char* logTagModule, jvxLogLevel lev) override;
	virtual std::ostream* log_str() override;
	virtual jvxErrorType start_lock() override;
	virtual void stop_lock() override;

	// =====================================================================================

	//! Destructor
	~CjvxLogRemoteHandler(void);

#include "codeFragments/simplify/jvxObjects_simplify.h"

};

} // namespace

#endif