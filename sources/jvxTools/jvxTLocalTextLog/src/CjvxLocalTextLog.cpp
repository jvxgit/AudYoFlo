#include "CjvxLocalTextLog.h"

/**
 * Constructor: Set all variables to zero
 *///==============================================
CjvxLocalTextLog::CjvxLocalTextLog(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE): CjvxObject(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
{
	_common_set.theComponentType.unselected(JVX_COMPONENT_LOCAL_TEXT_LOG);
	_common_set.theObjectSpecialization = reinterpret_cast<jvxHandle*>(static_cast<IjvxLocalTextLog*>(this));
	_common_set.thisisme = static_cast<IjvxObject*>(this);

	// Start some mutexes
	JVX_INITIALIZE_MUTEX(safeAccessList);
	JVX_INITIALIZE_MUTEX(safeAccessBuffer);

	uIdEntries = 1;
	separator_string = "\n";
}

/**
 * Destructor: Set object to initial state
 *///===============================================
CjvxLocalTextLog::~CjvxLocalTextLog()
{
	JVX_TERMINATE_MUTEX(safeAccessList);
	JVX_TERMINATE_MUTEX(safeAccessBuffer);
}

jvxErrorType 
CjvxLocalTextLog::initialize(IjvxHiddenInterface* hostRef)
{
	jvxErrorType res = CjvxObject::_initialize(hostRef);
	return res;
}

jvxErrorType CjvxLocalTextLog::terminate()
{
	this->stop();
	return CjvxObject::_terminate();
}

jvxErrorType 
CjvxLocalTextLog::start(jvxSize bsize)
{
	jvxErrorType res = CjvxObject::_select();
	if (res == JVX_NO_ERROR)
	{
		res = CjvxObject::_activate();
		assert(res == JVX_NO_ERROR);

		res = CjvxObject::_prepare();
		assert(res == JVX_NO_ERROR);

		res = CjvxObject::_start_lock();
		assert(res == JVX_NO_ERROR);

		circular_buffer.lBuffer = bsize;
		JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(circular_buffer.characterBuffer, char, circular_buffer.lBuffer);
		circular_buffer.fRounds = 0;
		circular_buffer.idxWrite = 0;
		res = CjvxObject::_unlock_state();
	}
	return res;
}

jvxErrorType 
CjvxLocalTextLog::stop()
{
	jvxErrorType res = JVX_NO_ERROR;
	if (_common_set_min.theState == JVX_STATE_PROCESSING)
	{
		res = CjvxObject::_stop_lock();
		assert(res == JVX_NO_ERROR);

		JVX_DSP_SAFE_DELETE_FIELD(circular_buffer.characterBuffer);
		circular_buffer.lBuffer = 0;
		circular_buffer.fRounds = 0;
		circular_buffer.idxWrite = 0;

		res = CjvxObject::_unlock_state();
		assert(res == JVX_NO_ERROR);

		res = CjvxObject::_postprocess();
		assert(res == JVX_NO_ERROR);

		res = CjvxObject::_deactivate();
		assert(res == JVX_NO_ERROR);

		res = CjvxObject::_unselect();
		assert(res == JVX_NO_ERROR);
	}
	return res;
}

jvxErrorType
CjvxLocalTextLog::attach_entry(const char* txt)
{
	jvxErrorType res = JVX_NO_ERROR;
	std::string str_attach = txt;
	this->lock_state();
	if (_common_set_min.theState == JVX_STATE_PROCESSING)
	{
		jvxSize togo = str_attach.size();
		const char* copyFrom = str_attach.c_str();
		char* copyTo = circular_buffer.characterBuffer;
		copyTo += circular_buffer.idxWrite;

		while (togo)
		{
			jvxSize ll2, ll1, ll0 = togo;
			ll0 = JVX_MIN(ll0, circular_buffer.lBuffer);
			ll1 = JVX_MIN(circular_buffer.lBuffer - circular_buffer.idxWrite, ll0);
			ll2 = ll0 - ll1;

			if (ll1)
			{
				memcpy(copyTo, copyFrom, ll1);
				circular_buffer.idxWrite += ll1;
				if (circular_buffer.idxWrite == circular_buffer.lBuffer)
				{
					JVX_LOCK_MUTEX(safeAccessBuffer);
					circular_buffer.idxWrite = 0;
					circular_buffer.fRounds++;
					JVX_UNLOCK_MUTEX(safeAccessBuffer);
				}
				copyFrom += ll1;
				togo -= ll1;
			}

			if (ll2)
			{
				copyTo = circular_buffer.characterBuffer;
				memcpy(copyTo, copyFrom, ll2);
				circular_buffer.idxWrite += ll2;
				copyFrom += ll2;
				togo -= ll2;
			}
		}
	}
	else
	{
		res = JVX_DSP_ERROR_WRONG_STATE;
	}
	this->unlock_state();
	return res;
}

jvxErrorType
CjvxLocalTextLog::read_update(jvxApiString* strReturn, jvxContext* ctxt, jvxSize* numCharLost, jvxBool* newReadStarted)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize numChar = 0;
	jvxSize ll1, ll2;
	jvxSize numLost = 0;
	jvxSize locId = JVX_SIZE_UNSELECTED;
	oneReadPosition* workme = NULL;
	jvxBool pushAtEnd = false;
	oneReadPosition newPosi;
	std::string retToken;

	this->lock_state();
	if (_common_set_min.theState == JVX_STATE_PROCESSING)
	{
		if (ctxt)
		{
			locId = ctxt->id;
		}

		JVX_LOCK_MUTEX(safeAccessList);
		if (JVX_CHECK_SIZE_UNSELECTED(locId))
		{
			locId = uIdEntries++;
		}

		auto elml = logs_read.find(locId);
		JVX_LOCK_MUTEX(safeAccessBuffer);

		if (elml == logs_read.end())
		{
			jvxSize numCharsIn = circular_buffer.fRounds * circular_buffer.lBuffer;
			numCharsIn += circular_buffer.idxWrite;
			numCharsIn = JVX_MIN(numCharsIn, circular_buffer.lBuffer);
			
			newPosi.sshot_fRounds = circular_buffer.fRounds;
			newPosi.sshot_idxWrite = circular_buffer.idxWrite;

			int loc_int_idxWrite = newPosi.sshot_idxWrite - numCharsIn;
			if (loc_int_idxWrite < 0)
			{
				if (newPosi.sshot_fRounds > 0)
				{
					newPosi.sshot_fRounds--;
				}
				else
				{
					newPosi.sshot_idxWrite = 0;
				}
			}
			else
			{
				newPosi.sshot_idxWrite = loc_int_idxWrite;
			}

			newPosi.uId = locId;
			workme = &newPosi;

			// Avoid that entries are added if no context is specified
			if (ctxt)
			{
				pushAtEnd = true;
			}
		}
		else
		{
			workme = &elml->second;
		}

		numChar = (circular_buffer.idxWrite - workme->sshot_idxWrite) +
			(circular_buffer.fRounds - workme->sshot_fRounds) * circular_buffer.lBuffer;
		if (numChar > circular_buffer.lBuffer)
		{
			numLost = numChar - circular_buffer.lBuffer;
			numChar = circular_buffer.lBuffer;
			workme->sshot_fRounds = circular_buffer.fRounds - 1;
			workme->sshot_idxWrite = circular_buffer.idxWrite;
		}

		ll1 = JVX_MIN(circular_buffer.lBuffer - workme->sshot_idxWrite, numChar);
		ll2 = numChar - ll1;
		char* ptrFrom = circular_buffer.characterBuffer;
		if (ll1)
		{

			ptrFrom += workme->sshot_idxWrite;
			retToken += std::string(ptrFrom, ll1);
		}
		if (ll2)
		{
			ptrFrom = circular_buffer.characterBuffer;
			retToken += std::string(ptrFrom, ll2);
		}
		workme->sshot_idxWrite += numChar;
		if (workme->sshot_idxWrite >= circular_buffer.lBuffer)
		{
			workme->sshot_fRounds++;
			workme->sshot_idxWrite -= circular_buffer.lBuffer;
		}
		if (numLost > 0)
		{
			retToken = "<..lost " + jvx_size2String(numLost) + "..>" + retToken;
		}

		strReturn->assign(retToken);
		JVX_UNLOCK_MUTEX(safeAccessBuffer);

		if (pushAtEnd)
		{
			logs_read[newPosi.uId] = newPosi;
		}
		JVX_UNLOCK_MUTEX(safeAccessList);

		if (ctxt)
		{
			ctxt->id = locId;
		}
		if (numCharLost)
		{
			*numCharLost = numLost;
		}
		if (newReadStarted)
		{
			*newReadStarted = pushAtEnd;
		}
	}
	else
	{
		res = JVX_ERROR_WRONG_STATE;
	}
	this->unlock_state();
	return res;
}

jvxErrorType
CjvxLocalTextLog::status_update(jvxSize* roundsOnReturn, jvxSize* idxWriteOnReturn)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize numChar = 0;
	jvxSize ll1, ll2;
	jvxSize numLost = 0;
	jvxSize locId = JVX_SIZE_UNSELECTED;
	oneReadPosition* workme = NULL;
	jvxBool pushAtEnd = false;
	oneReadPosition newPosi;

	this->lock_state();
	if (_common_set_min.theState == JVX_STATE_PROCESSING)
	{
		JVX_LOCK_MUTEX(safeAccessBuffer);
		if (roundsOnReturn)
		{
			*roundsOnReturn = circular_buffer.fRounds;
		}
		if (idxWriteOnReturn)
		{
			*idxWriteOnReturn = circular_buffer.idxWrite;
		}
		JVX_UNLOCK_MUTEX(safeAccessBuffer);
	}
	else
	{
		res = JVX_ERROR_WRONG_STATE;
	}
	this->unlock_state();
	return res;
}

jvxErrorType
CjvxLocalTextLog::clear()
{
	JVX_LOCK_MUTEX(safeAccessBuffer);
	JVX_LOCK_MUTEX(safeAccessList);

	circular_buffer.idxWrite = 0;
	circular_buffer.fRounds = 0;
	logs_read.clear();

	JVX_UNLOCK_MUTEX(safeAccessList);
	JVX_UNLOCK_MUTEX(safeAccessBuffer);
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxLocalTextLog::configure(jvxLocalTextLogConfigType tp, jvxHandle* load)
{
	jvxErrorType res = JVX_NO_ERROR;
	switch (tp)
	{
	case JVX_LOCAL_TEXT_LOG_CONFIG_SEPARATOR_CHAR:
		separator_string = (const char*)load;
		break;
	default:
		res = JVX_ERROR_UNSUPPORTED;
	}
	return res;
}

jvxErrorType
CjvxLocalTextLog::get_text_lost_characters(jvxApiString* strReturn, jvxSize numCharsLost)
{
	std::string txt = separator_string + "-- " + jvx_size2String(numCharsLost) + " characters lost --" + separator_string;
	strReturn->assign(txt);
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxLocalTextLog::get_text_new_read(jvxApiString* strReturn)
{
	std::string txt = "Attaching to local text log --" + separator_string;
	if (strReturn)
	{
		strReturn->assign(txt);
	}
	return JVX_NO_ERROR;
}
