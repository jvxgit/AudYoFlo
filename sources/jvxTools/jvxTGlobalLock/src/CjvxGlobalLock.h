#ifndef __CJVXGLOBALLOCK_H__
#define __CJVXNEWSOCKET_H__

#include "jvx.h"
#include "common/CjvxObject.h"

namespace jvxGlobalLock
{
	class oneLock: public IjvxLock
	{
		friend class CjvxGlobalLock;
		JVX_MUTEX_HANDLE safeAccess;
		jvxSize refId = 0;
	public:
		oneLock();
		~oneLock();

		virtual jvxErrorType JVX_CALLINGCONVENTION  lock() override;
		virtual jvxErrorType JVX_CALLINGCONVENTION  unlock() override;
		virtual jvxErrorType JVX_CALLINGCONVENTION  try_lock() override;
	};

	class CjvxGlobalLock : public IjvxGlobalLock, public CjvxObject
	{
	private:
		std::map<std::string, oneLock*> allLocks;

	public:
		CjvxGlobalLock(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);

		~CjvxGlobalLock();

#include "codeFragments/simplify/jvxObjects_simplify.h"

		virtual jvxErrorType JVX_CALLINGCONVENTION reference_global_lock(const char* idToken, IjvxLock** lock, jvxBool* installedOnRequest) override;
		virtual jvxErrorType JVX_CALLINGCONVENTION return_reference_global_lock(IjvxLock* lock) override;
		virtual jvxErrorType JVX_CALLINGCONVENTION number_global_locks(jvxSize* num) override;
		virtual jvxErrorType JVX_CALLINGCONVENTION token_global_locks(jvxSize id, jvxApiString& tokenOnReturn) override;
	};
}

#endif
