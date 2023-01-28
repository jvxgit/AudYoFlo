#include "jvx.h"
#include "CjvxSharedMemory.h"


/**
 * Constructor: Set all into uninitialized.
 *///===================================================
CjvxSharedMemory::CjvxSharedMemory(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE):
	CjvxObject(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
{
	_common_set.theComponentType.unselected(JVX_COMPONENT_SHARED_MEMORY);
	_common_set.theObjectSpecialization = reinterpret_cast<jvxHandle*>(static_cast<IjvxSharedMemory*>(this));
	_common_set.thisisme = static_cast<IjvxObject*>(this);
	myUniqueSeed = 1;

	JVX_INITIALIZE_MUTEX(safeAccess);
}

CjvxSharedMemory::~CjvxSharedMemory()
{
	JVX_TERMINATE_MUTEX(safeAccess);
}

// ==========================================================================================

jvxErrorType
CjvxSharedMemory::initialize()
{
	if (_common_set_min.theState != JVX_STATE_NONE)
	{
		return JVX_ERROR_WRONG_STATE;
	}

	_common_set_min.theState = JVX_STATE_ACTIVE;
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxSharedMemory::terminate()
{
	jvxErrorType res = JVX_NO_ERROR;
	std::list<IjvxSharedMemoryListener*> listeners;
	jvxSize uId = JVX_SIZE_UNSELECTED;

	if (_common_set_min.theState == JVX_STATE_ACTIVE)
	{
		while (1)
		{
			jvxBool oneRemoved = false;
			JVX_LOCK_MUTEX(safeAccess);
			auto elm = allocated_areas.begin();
			while ((elm != allocated_areas.end()) && (elm->second.uId == 0))
			{
				elm++;
			}
			if (elm != allocated_areas.end())
			{
				if (elm->second.theListener)
				{
					res = JVX_ERROR_COMPONENT_BUSY;
					break;
				}
				else
				{
					listeners = elm->second.listeners;
					uId = elm->second.uId;
					JVX_DSP_SAFE_DELETE_FIELD(elm->second.fld);
					allocated_areas.erase(elm);
					oneRemoved = true;
				}
			}
			JVX_UNLOCK_MUTEX(safeAccess);

			if (JVX_CHECK_SIZE_SELECTED(uId))
			{
				auto elmL = listeners.begin();
				for (; elmL != elm->second.listeners.end(); elmL++)
				{
					(*elmL)->content_has_been_removed(uId);
				}
			}

			// End loop if all elements were erased
			if (!oneRemoved)
				break;
		} // while(1)

		uId = JVX_SIZE_UNSELECTED;
		JVX_LOCK_MUTEX(safeAccess);

		auto elm = allocated_areas.find(0);
		if (elm != allocated_areas.end())
		{
			uId = 0;
			listeners = elm->second.listeners;
		}
		// Clear the listener only entry
		allocated_areas.clear();
		JVX_UNLOCK_MUTEX(safeAccess);

		if (JVX_CHECK_SIZE_SELECTED(uId))
		{
			auto elmL = listeners.begin();
			for (; elmL != elm->second.listeners.end(); elmL++)
			{
				(*elmL)->content_has_been_removed(uId);
			}
		}
		_common_set_min.theState = JVX_STATE_NONE;
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_WRONG_STATE;
}

// ==========================================================================================

//! Initialize the module, set size of buffer and strategy
jvxErrorType 
CjvxSharedMemory::create_area(const char* name, const char* description, jvxSize* uId,
	jvxDataFormat format, jvxSize numElements, IjvxSharedMemoryOwner* theOwner,
	jvxComponentIdentification origin, jvxSharedMemoryAccessMode mode)
{
	jvxErrorType res = JVX_NO_ERROR;
	oneAreaShared elmS;

	if (uId)
		*uId = JVX_SIZE_UNSELECTED;

	JVX_LOCK_MUTEX(safeAccess);
	auto elm = allocated_areas.begin();
	while (elm != allocated_areas.end())
	{
		if (elm->second.name == name)
		{
			break;
		}
		elm++;
	}

	if (elm == allocated_areas.end())
	{
		elmS.name = name;
		elmS.description = description;
		elmS.format = format;
		elmS.num = numElements;
		elmS.szOneElm = jvxDataFormat_getsize(format);
		elmS.szAllElms = elmS.szOneElm * elmS.num;
		elmS.owner = theOwner;
		elmS.origin = origin;
		elmS.mode = mode;
		elmS.version_id = 1;

		if (elmS.szAllElms)
		{
			elmS.uId = myUniqueSeed++;                       
			JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(elmS.fld, jvxByte, elmS.szAllElms);
			allocated_areas[elmS.uId] = elmS;
			if (uId)
			{
				*uId = elmS.uId;
			}
		}
		else
		{
			res = JVX_ERROR_INVALID_FORMAT;
		}
	}

	JVX_UNLOCK_MUTEX(safeAccess);

	if (res == JVX_NO_ERROR)
	{
		auto elmO = global_listener.begin();
		for (; elmO != global_listener.end(); elmO++)
		{
			elmO->second.theListener->content_has_been_added(elmS.uId,
				elmS.name.c_str(), elmS.description.c_str(), 
				elmS.origin);
		}
	}
	return res;
}

jvxErrorType 
CjvxSharedMemory::destroy_area(jvxSize uId, IjvxSharedMemoryOwner* theOwner)
{
	jvxErrorType res = JVX_NO_ERROR;

	std::list<IjvxSharedMemoryListener*> listeners;
	jvxSize uIdLocal = JVX_SIZE_UNSELECTED;
	std::string descr;
	jvxComponentIdentification cpId;
	JVX_LOCK_MUTEX(safeAccess);
	auto elm = allocated_areas.find(uId);
	if (elm != allocated_areas.end())
	{
		if (elm->second.theListener)
		{
			res = JVX_ERROR_COMPONENT_BUSY;
		}
		else
		{
			listeners = elm->second.listeners;
			uIdLocal = elm->second.uId;

			descr = elm->second.description;
			cpId = elm->second.origin;
			JVX_DSP_SAFE_DELETE_FIELD(elm->second.fld);
			allocated_areas.erase(elm);
		}
	}
	JVX_UNLOCK_MUTEX(safeAccess);

	if (JVX_CHECK_SIZE_SELECTED(uIdLocal))
	{
		auto elmL = listeners.begin();
		for (; elmL != listeners.end(); elmL++)
		{
			(*elmL)->observed_content_has_changed(uIdLocal, JVX_SHARED_MEMORY_DESTROYED);
		}
	}

	if (res == JVX_NO_ERROR)
	{
		auto elmO = global_listener.begin();
		for (; elmO != global_listener.end(); elmO++)
		{
			elmO->second.theListener->content_has_been_removed(uIdLocal);
		}
	}
	return JVX_NO_ERROR;
}

// ====================================================================================

jvxErrorType
CjvxSharedMemory::number_areas(jvxSize* num)
{
	if (_common_set_min.theState != JVX_STATE_ACTIVE)
		return JVX_ERROR_WRONG_STATE;

	if (num)
		*num = allocated_areas.size();

	return JVX_NO_ERROR;
}

jvxErrorType
CjvxSharedMemory::ident_area(jvxSize id, jvxSize* uId)
{
	jvxErrorType res = JVX_NO_ERROR;
	if (_common_set_min.theState != JVX_STATE_ACTIVE)
		return JVX_ERROR_WRONG_STATE;

	JVX_LOCK_MUTEX(safeAccess);

	if (id >= allocated_areas.size())
	{
		res = JVX_ERROR_ID_OUT_OF_BOUNDS;
	}
	else
	{
		auto elm = allocated_areas.begin();

		std::advance(elm, id);

		if (uId)
		{
			*uId = elm->second.uId;
		}
	}

	JVX_UNLOCK_MUTEX(safeAccess);
	return res;
}

jvxErrorType
CjvxSharedMemory::description_area(jvxSize uId, jvxApiString* nm, jvxApiString* description,
	jvxDataFormat* format, jvxSize* numElements)
{
	jvxErrorType res = JVX_NO_ERROR;
	if (_common_set_min.theState != JVX_STATE_ACTIVE)
		return JVX_ERROR_WRONG_STATE;

	JVX_LOCK_MUTEX(safeAccess);
	auto elm = allocated_areas.find(uId);
	if (elm != allocated_areas.end())
	{
		if (nm)
		{
			nm->assign(elm->second.name);
		}

		if (description)
		{
			description->assign(elm->second.description);
		}

		if (format)
		{
			*format = elm->second.format;
		}

		if (numElements)
		{
			*numElements = elm->second.num;
		}

		res = JVX_NO_ERROR;
	}
	else
	{
		res = JVX_ERROR_ELEMENT_NOT_FOUND;
	}
	JVX_UNLOCK_MUTEX(safeAccess);
	return res;
}

jvxErrorType
CjvxSharedMemory::start_observe_shared_memory(IjvxSharedMemoryListener* theListener, const char* wc_report, jvxComponentIdentification cp_id)
{
	auto elm = global_listener.find(theListener);
	if (elm == global_listener.end())
	{
		oneMemoryListener oneList;
		oneList.wc_memory = wc_report;
		oneList.id = cp_id;
		oneList.theListener = theListener;
		global_listener[theListener] = oneList;

		report_all_areas_start(&oneList);
	}


	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxSharedMemory::stop_observe_shared_memory(IjvxSharedMemoryListener* theListener)
{
	auto elm = global_listener.find(theListener);
	if (elm != global_listener.end())
	{
		report_all_areas_stop(&elm->second);
		global_listener.erase(elm);
	}
	return JVX_NO_ERROR;
}

// ====================================================================================

jvxErrorType
CjvxSharedMemory::start_observe_area(jvxSize uId, IjvxSharedMemoryListener* theListener)
{
	jvxErrorType res = JVX_NO_ERROR;

	if (_common_set_min.theState != JVX_STATE_ACTIVE)
		return JVX_ERROR_WRONG_STATE;

	JVX_LOCK_MUTEX(safeAccess);
	auto elm = allocated_areas.find(uId);
	if (elm != allocated_areas.end())
	{
		auto elmL = elm->second.listeners.begin();
		for (; elmL != elm->second.listeners.end(); elmL++)
		{
			if (*elmL == theListener)
			{
				break;
			}
		}
		if (elmL == elm->second.listeners.end())
		{
			elm->second.listeners.push_back(theListener);
		}
		else
		{
			res = JVX_ERROR_DUPLICATE_ENTRY;
		}
	}
	JVX_UNLOCK_MUTEX(safeAccess);

	if (res == JVX_NO_ERROR)
	{
		theListener->observed_content_has_changed(elm->second.uId, JVX_SHARED_MEMORY_CONTENT_FIRST_ACCESS);
	}
	return res;
}

jvxErrorType
CjvxSharedMemory::stop_observe_area(jvxSize uId, IjvxSharedMemoryListener* theListener)
{
	jvxErrorType res = JVX_NO_ERROR;
	
	if (_common_set_min.theState != JVX_STATE_ACTIVE)
		return JVX_ERROR_WRONG_STATE;

	JVX_LOCK_MUTEX(safeAccess);
	auto elm = allocated_areas.find(uId);
	if (elm != allocated_areas.end())
	{
		auto elmL = elm->second.listeners.begin();
		for (; elmL != elm->second.listeners.end(); elmL++)
		{
			if (*elmL == theListener)
			{
				break;
			}
		}
		if (elmL != elm->second.listeners.end())
		{
			elm->second.listeners.erase(elmL);
		}
		else
		{
			res = JVX_ERROR_ELEMENT_NOT_FOUND;
		}
	}
	JVX_UNLOCK_MUTEX(safeAccess);
	return res;
}

// ====================================================================================

jvxErrorType
CjvxSharedMemory::try_lock_area(jvxSize uId, 
	IjvxSharedMemoryListener* listener, 
	jvxSharedMemoryAccessMode acc,
	IjvxSharedMemoryOwner** owner,
	jvxHandle** mem,
	jvxDataFormat* form,
	jvxSize* num ,
	jvxSize* version_id ,
	jvxComponentIdentification* origin,
	jvxSharedMemoryAccessMode* mode)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxBool accessAllowed = false;

	if (_common_set_min.theState != JVX_STATE_ACTIVE)
		return JVX_ERROR_WRONG_STATE;

	JVX_LOCK_MUTEX(safeAccess);
	auto elm = allocated_areas.find(uId);
	if (elm != allocated_areas.end())
	{
		if (elm->second.theListener == NULL)
		{
			accessAllowed = false;
			if (owner)
			{
				if (elm->second.owner == *owner)
				{
					accessAllowed = true;
				}
			}
			if (!accessAllowed)
			{
				if (elm->second.mode & acc)
				{
					accessAllowed = true;
				}
			}

			if (accessAllowed)
			{
				elm->second.theListener = listener;
				if (mem)
				{
					*mem = elm->second.fld;
				}
			}
			else
			{
				res = JVX_ERROR_NO_ACCESS;
			}

			if (form)
			{
				*form = elm->second.format;
			}
			if (num)
			{
				*num = elm->second.num;
			}
			if (version_id)
			{
				*version_id = elm->second.version_id;
			}
			if (origin)
			{
				*origin = elm->second.origin;
			}
			if (mode)
			{
				*mode = elm->second.mode;
			}
			if (owner)
			{
				if (*owner == NULL)
				{
					*owner = elm->second.owner;
				}
			}
		}
		else
		{
			res = JVX_ERROR_COMPONENT_BUSY;
		}
	}
	else
	{
		res = JVX_ERROR_ELEMENT_NOT_FOUND;
	}
	JVX_UNLOCK_MUTEX(safeAccess);

	return res;
}

jvxErrorType
CjvxSharedMemory::unlock_area(jvxSize uId, IjvxSharedMemoryListener* theListener, jvxBool content_written)
{
	jvxErrorType res = JVX_NO_ERROR;
	std::list<IjvxSharedMemoryListener*> listeners;
	jvxSize uIdLocal = JVX_SIZE_UNSELECTED;

	if (_common_set_min.theState != JVX_STATE_ACTIVE)
		return JVX_ERROR_WRONG_STATE;

	JVX_LOCK_MUTEX(safeAccess);
	auto elm = allocated_areas.find(uId);
	if (elm != allocated_areas.end())
	{
		if (elm->second.theListener == theListener)
		{
			elm->second.theListener = NULL;
			if (content_written)
			{
				elm->second.version_id++;
				uIdLocal = elm->second.uId;
				listeners = elm->second.listeners;
			}
		}
		else
		{
			res = JVX_ERROR_INVALID_ARGUMENT;
		}
	}
	else
	{
		res = JVX_ERROR_ELEMENT_NOT_FOUND;
	}
	JVX_UNLOCK_MUTEX(safeAccess);

	if (JVX_CHECK_SIZE_SELECTED(uIdLocal))
	{
		auto elmL = listeners.begin();
		for (; elmL != listeners.end(); elmL++)
		{
			if ((*elmL) != theListener)
			{
				(*elmL)->observed_content_has_changed(uIdLocal, JVX_SHARED_MEMORY_CONTENT_CHANGED);
			}
		}
	}

	return res;
}

// ==========================================================================================================

void
CjvxSharedMemory::report_all_areas_start(oneMemoryListener* oneList)
{
	JVX_LOCK_MUTEX(safeAccess);
	auto elm = allocated_areas.begin();
	for (; elm != allocated_areas.end(); elm++)
	{
		std::string descr = elm->second.description;
		std::string name = elm->second.name;
		jvxComponentIdentification cp = elm->second.origin;
		jvxBool report = check_match_area(oneList, descr, cp);

		if (report)
		{
			oneList->theListener->content_has_been_added(elm->second.uId, name.c_str(), descr.c_str(), cp);
		}
	}
	JVX_UNLOCK_MUTEX(safeAccess);
}

void
CjvxSharedMemory::report_all_areas_stop(oneMemoryListener* oneList)
{
	JVX_LOCK_MUTEX(safeAccess);
	auto elm = allocated_areas.begin();
	for (; elm != allocated_areas.end(); elm++)
	{
		std::string descr = elm->second.description;
		std::string name = elm->second.name;
		jvxComponentIdentification cp = elm->second.origin;
		jvxBool report = check_match_area(oneList, descr, cp);
		if (report)
		{
			oneList->theListener->content_has_been_removed(elm->second.uId);
		}
	}
	JVX_UNLOCK_MUTEX(safeAccess);
}

jvxBool
CjvxSharedMemory::check_match_area(oneMemoryListener* oneList, const std::string& descr, const jvxComponentIdentification& cpId)
{
	jvxBool report = true;
	if (!jvx_compareStringsWildcard(oneList->wc_memory, descr))
	{
		report = false;
	}
	if (!jvx_compareComponentIdentWildcard(oneList->id, cpId))
	{
		report = false;
	}
	return report;
}

// =====================================================================================

jvxErrorType 
CjvxSharedMemory::single_instance_pre_shutdown()
{
	terminate();
	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxSharedMemory::single_instance_post_bootup(IjvxHiddenInterface* hostRef)
{
	initialize();
	return JVX_NO_ERROR;
}
