#include "CjvxLogRemoteHandler.h"

namespace _myJvxTools {

	CjvxLogRemoteHandler::CjvxLogRemoteHandler(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE) :
		CjvxObject(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
	{
		_common_set.theComponentType.unselected(JVX_COMPONENT_LOGREMOTEHANDLER);
		_common_set.theObjectSpecialization = reinterpret_cast<jvxHandle*>(static_cast<IjvxLogRemoteHandler*>(this));
		_common_set.thisisme = static_cast<IjvxObject*>(this);
	}
	
	CjvxLogRemoteHandler::~CjvxLogRemoteHandler(void)
	{
	}

	jvxErrorType 
		CjvxLogRemoteHandler::initialize(IjvxHiddenInterface* hostRef)
	{
		jvxErrorType res = CjvxObject::_initialize(hostRef);
		if (res == JVX_NO_ERROR)
		{
			res = CjvxObject::_select();
		}
		if (res == JVX_NO_ERROR)
		{
			res = CjvxObject::_activate();
		}

		if (res != JVX_NO_ERROR)
		{
			terminate();
		}
		return res;
	}

	jvxErrorType
		CjvxLogRemoteHandler::terminate()
	{
		if (_common_set_min.theState == JVX_STATE_ACTIVE)
		{
			CjvxObject::_deactivate();
		}

		if (_common_set_min.theState == JVX_STATE_SELECTED)
		{
			CjvxObject::_unselect();
		}
		return CjvxObject::_terminate();
	}

	void 
	CjvxLogRemoteHandler::configure(const char* logTagModule, jvxLogLevel levArg)
	{
		logDescriptor = logTagModule;
		jvxrtst_bkp.set_module_name(logDescriptor);
		lev = levArg;
	}

	std::ostream* CjvxLogRemoteHandler::log_str()
	{
		return &jvxrtst;
	}


#ifdef JVX_PROFILE_TEXT_LOG_LOCK 
	jvxErrorType
	CjvxLogRemoteHandler::start_lock(const std::string& tag)
#else
	jvxErrorType 
	CjvxLogRemoteHandler::start_lock()
#endif
	{
		jvxSize logLev = jvxLogLevel2Id(this->lev);
		if(jvxrtst_bkp.theTextLogger_hdl && jvxrtst_bkp.theTextLogger_hdl->check_log_output(nullptr, logLev, nullptr)) 
		{
#ifdef JVX_PROFILE_TEXT_LOG_LOCK 
			jvx_lock_text_log(jvxrtst_bkp, logLev, tag );
#else
			jvx_lock_text_log(jvxrtst_bkp, logLev);
#endif
			isLocked = true;
			return JVX_NO_ERROR;
		}
		return JVX_ERROR_INVALID_SETTING;
	}

#ifdef JVX_PROFILE_TEXT_LOG_LOCK 
	void
		CjvxLogRemoteHandler::stop_lock(const std::string& tag)
#else
	void 
	CjvxLogRemoteHandler::stop_lock()
#endif
	{
		if (isLocked)
		{			
#ifdef JVX_PROFILE_TEXT_LOG_LOCK 
			jvx_unlock_text_log(jvxrtst_bkp, tag);
#else
			jvx_unlock_text_log(jvxrtst_bkp);
#endif
			isLocked = false;
		}
	}
} // namespace
