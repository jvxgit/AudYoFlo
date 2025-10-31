#include "jvx.h"
#include "CjvxGlobalLock.h"

namespace jvxGlobalLock
{
	oneLock::oneLock()
	{
		JVX_INITIALIZE_MUTEX(safeAccess);
	}

	oneLock::~oneLock()
	{
		JVX_TERMINATE_MUTEX(safeAccess);
	}

	jvxErrorType
	oneLock::lock()
	{
		JVX_LOCK_MUTEX(safeAccess);
		return JVX_NO_ERROR;
	}
	
	jvxErrorType oneLock::unlock() 
	{
		JVX_UNLOCK_MUTEX(safeAccess);
		return JVX_NO_ERROR;
	}

	jvxErrorType oneLock::try_lock()
	{
		JVX_TRY_LOCK_MUTEX_RESULT_TYPE retVal = JVX_TRY_LOCK_MUTEX_NO_SUCCESS;
		JVX_TRY_LOCK_MUTEX(retVal, safeAccess);
		if (retVal == JVX_TRY_LOCK_MUTEX_SUCCESS)
		{
			return JVX_NO_ERROR;
		}
		return JVX_ERROR_COMPONENT_BUSY;
	}

	// ======================================================================

	CjvxGlobalLock::CjvxGlobalLock(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE) :
		CjvxObject(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
	{
		_common_set.theComponentType.unselected(JVX_COMPONENT_GLOBAL_LOCK);
		_common_set.theObjectSpecialization = reinterpret_cast<jvxHandle*>(static_cast<IjvxGlobalLock*>(this));
		_common_set.thisisme = static_cast<IjvxObject*>(this);
		_common_set.theComponentSubTypeDescriptor = "/";
		_common_set.theComponentSubTypeDescriptor += "JVX_GLOBAL_LOCK_OBJECT";
		_common_set_min.theDescription = "JVX global lock";

	}

	CjvxGlobalLock::~CjvxGlobalLock()
	{
		assert(allLocks.size() == 0);
	}

	jvxErrorType
	CjvxGlobalLock::reference_global_lock(const char* idToken, IjvxLock** lock, jvxBool* installedOnRequest)
	{
		// Make sure we getthe reference in the main thread
		jvxErrorType res = JVX_ERROR_INVALID_ARGUMENT;
		if (installedOnRequest)
		{
			*installedOnRequest = false;
		}
		if (lock)
		{
			auto elm = allLocks.find(idToken);
			if (elm == allLocks.end())
			{
				oneLock* newLock = nullptr;
				JVX_SAFE_ALLOCATE_OBJECT(newLock, oneLock);
				newLock->refId++;
				allLocks[idToken] = newLock;
				*lock = static_cast<IjvxLock*>(newLock);
				if (installedOnRequest)
				{
					*installedOnRequest = true;
				}
				res = JVX_NO_ERROR;
			}
			else
			{
				elm->second->refId++;
				*lock = elm->second;
			}
		}
		return res;
	}

	jvxErrorType
		CjvxGlobalLock::return_reference_global_lock(IjvxLock* lock)
	{
		jvxErrorType res = JVX_ERROR_ELEMENT_NOT_FOUND;
		auto elm = allLocks.begin();
		for (; elm != allLocks.end(); elm++)
		{
			if (static_cast<IjvxLock*>(elm->second) == lock)
			{
				break;
			}
		}
		if (elm != allLocks.end())
		{
			assert(elm->second->refId > 0);
			elm->second->refId--;
			if (elm->second->refId == 0)
			{
				JVX_SAFE_DELETE_OBJECT(elm->second);
				allLocks.erase(elm);
				res = JVX_NO_ERROR;
			}
		}
		return res;
	}

	jvxErrorType
		CjvxGlobalLock::number_global_locks(jvxSize* num)
	{
		if (num)
		{
			*num = allLocks.size();
		}
		return JVX_NO_ERROR;
	}

	jvxErrorType
		CjvxGlobalLock::token_global_locks(jvxSize id, jvxApiString& tokenOnReturn)
	{
		jvxErrorType res = JVX_ERROR_ID_OUT_OF_BOUNDS;

		if (id < allLocks.size())
		{
			auto elm = allLocks.begin();
			std::advance(elm, id);
			tokenOnReturn = elm->first;
			res = JVX_NO_ERROR;
		}
		return res;
	}
}