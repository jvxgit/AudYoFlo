#ifndef __CJVXQTCLASS_H__
#define __CJVXQTCLASS_H__

/* #include "CjvxObject.h" */

template <class T, class Tq, class Tref> class CjvxQtClass: public T
{
protected:
	struct
	{
		Tref* theRefClass;
		Tq* theQtClassRef;
		jvxBool always_update_delayed;
	} _common_set_my_qt_props;

public:

	CjvxQtClass(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE):
		T(description, multipleObjects, module_name, descriptor, featureClass)
	{
		_common_set_my_qt_props.theRefClass = NULL;
		_common_set_my_qt_props.theQtClassRef = NULL;
		_common_set_my_qt_props.always_update_delayed = false;
	};

	~CjvxQtClass()
	{
	};

	void init_reference(Tref* theRef) { _common_set_my_qt_props.theRefClass = theRef; };


	virtual jvxErrorType JVX_CALLINGCONVENTION activate()
	{
		jvxErrorType res = T::activate();
		if(res == JVX_NO_ERROR)
		{
			if(_common_set_my_qt_props.theQtClassRef)
			{
				_common_set_my_qt_props.theQtClassRef->updateWindow_sync(JVX_STATE_ACTIVE, _common_set_my_qt_props.always_update_delayed);
			}
		}
		return(res);
	};

	virtual jvxErrorType JVX_CALLINGCONVENTION deactivate()
	{
		jvxErrorType res = T::deactivate();
		if(res == JVX_NO_ERROR)
		{
			if(_common_set_my_qt_props.theQtClassRef)
			{
				_common_set_my_qt_props.theQtClassRef->updateWindow_sync(JVX_STATE_SELECTED, _common_set_my_qt_props.always_update_delayed);
			}
		}
		return(res);
	};

	virtual jvxErrorType JVX_CALLINGCONVENTION prepare()
	{
		jvxErrorType res = T::prepare();
		if(res == JVX_NO_ERROR)
		{
			if(_common_set_my_qt_props.theQtClassRef)
			{
				_common_set_my_qt_props.theQtClassRef->updateWindow_sync(JVX_STATE_PREPARED, _common_set_my_qt_props.always_update_delayed);
			}
		}
		return(res);
	};

	virtual jvxErrorType JVX_CALLINGCONVENTION start()
	{
		jvxErrorType res = T::start();
		if(res == JVX_NO_ERROR)
		{
			if(_common_set_my_qt_props.theQtClassRef)
			{
				_common_set_my_qt_props.theQtClassRef->updateWindow_sync(JVX_STATE_PROCESSING, _common_set_my_qt_props.always_update_delayed);
			}
		}
		return(res);
	};

	virtual jvxErrorType JVX_CALLINGCONVENTION stop()
	{
		jvxErrorType res = T::stop();
		if(res == JVX_NO_ERROR)
		{
			if(_common_set_my_qt_props.theQtClassRef)
			{
				_common_set_my_qt_props.theQtClassRef->updateWindow_sync(JVX_STATE_PREPARED, _common_set_my_qt_props.always_update_delayed);
			}
		}
		return(res);
	};

	virtual jvxErrorType JVX_CALLINGCONVENTION postprocess()
	{
		jvxErrorType res = T::postprocess();
		if(res == JVX_NO_ERROR)
		{
			if(_common_set_my_qt_props.theQtClassRef)
			{
				_common_set_my_qt_props.theQtClassRef->updateWindow_sync(JVX_STATE_ACTIVE, _common_set_my_qt_props.always_update_delayed);
			}
		}
		return(res);
	};

	// ===========================================================================================

	virtual jvxErrorType JVX_CALLINGCONVENTION activate_unlock()
	{
		jvxErrorType res = T::unlock_state();
		if(res == JVX_NO_ERROR)
		{
			if(_common_set_my_qt_props.theQtClassRef)
			{
				_common_set_my_qt_props.theQtClassRef->updateWindow_sync(JVX_STATE_ACTIVE, _common_set_my_qt_props.always_update_delayed);
			}
		}
		return(res);
	};

	virtual jvxErrorType JVX_CALLINGCONVENTION deactivate_unlock()
	{
		jvxErrorType res = T::unlock_state();
		if(res == JVX_NO_ERROR)
		{
			if(_common_set_my_qt_props.theQtClassRef)
			{
				_common_set_my_qt_props.theQtClassRef->updateWindow_sync(JVX_STATE_SELECTED, _common_set_my_qt_props.always_update_delayed);
			}
		}
		return(res);
	};

	virtual jvxErrorType JVX_CALLINGCONVENTION prepare_unlock()
	{
		jvxErrorType res = T::unlock_state();
		if(res == JVX_NO_ERROR)
		{
			if(_common_set_my_qt_props.theQtClassRef)
			{
				_common_set_my_qt_props.theQtClassRef->updateWindow_sync(JVX_STATE_PREPARED, _common_set_my_qt_props.always_update_delayed);
			}
		}
		return(res);
	};

	virtual jvxErrorType JVX_CALLINGCONVENTION start_unlock()
	{
		jvxErrorType res = T::_unlock_state();
		if(res == JVX_NO_ERROR)
		{
			if(_common_set_my_qt_props.theQtClassRef)
			{
				_common_set_my_qt_props.theQtClassRef->updateWindow_sync(JVX_STATE_PROCESSING, _common_set_my_qt_props.always_update_delayed);
			}
		}
		return(res);
	};

	virtual jvxErrorType JVX_CALLINGCONVENTION stop_unlock()
	{
		jvxErrorType res = T::unlock_state();
		if(res == JVX_NO_ERROR)
		{
			if(_common_set_my_qt_props.theQtClassRef)
			{
				_common_set_my_qt_props.theQtClassRef->updateWindow_sync(JVX_STATE_PREPARED, _common_set_my_qt_props.always_update_delayed);
			}
		}
		return(res);
	};

	virtual jvxErrorType JVX_CALLINGCONVENTION postprocess_unlock()
	{
		jvxErrorType res = T::unlock_state();
		if(res == JVX_NO_ERROR)
		{
			if(_common_set_my_qt_props.theQtClassRef)
			{
				_common_set_my_qt_props.theQtClassRef->updateWindow_sync(JVX_STATE_ACTIVE, _common_set_my_qt_props.always_update_delayed);
			}
		}
		return(res);
	};

};

#endif
