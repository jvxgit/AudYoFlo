#include "jvx.h"
#include "CjvxMSWText2Speech.h"

const SPSTREAMFORMAT theStreamFormats[JVX_TEXT2SPEECH_LIMIT] =
{
	SPSF_8kHz16BitMono,
	SPSF_16kHz16BitMono,
	SPSF_32kHz16BitMono,
	SPSF_44kHz16BitMono,
	SPSF_48kHz16BitMono
};

CjvxMSWText2Speech::CjvxMSWText2Speech(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE):
	CjvxObject(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
{
	_common_set.theComponentType.unselected(JVX_COMPONENT_TEXT2SPEECH);
	_common_set.theObjectSpecialization = reinterpret_cast<jvxHandle*>(static_cast<IjvxText2Speech*>(this));
	_common_set.thisisme = static_cast<IjvxObject*>(this);

	_common_set_min.theDescription = "MS Windows API Text2 Speech";
	refCount = 0;

	JVX_INITIALIZE_MUTEX(safeAccess);

	thread_handler.callback_thread_startup = cb_thread_startup;
	thread_handler.callback_thread_stopped = cb_thread_stopped;
	thread_handler.callback_thread_timer_expired = cb_thread_timer_expired;
	thread_handler.callback_thread_wokeup = cb_thread_wokeup;
}

CjvxMSWText2Speech::~CjvxMSWText2Speech()
{
	// If we reach here, all ports MUST be closed
	this->terminate();
	CjvxObject::_terminate();

	JVX_TERMINATE_MUTEX(safeAccess);
}

jvxErrorType 
CjvxMSWText2Speech::initialize(IjvxHiddenInterface* hostRef)
{	
	jvxErrorType res = CjvxObject::_initialize(hostRef);
	
	return res;
}

jvxErrorType
CjvxMSWText2Speech::prepare()
{
	jvxErrorType res = CjvxObject::_select();
	if (res == JVX_NO_ERROR)
	{
		res = CjvxObject::_activate();
		assert(res == JVX_NO_ERROR);

		jvx_thread_initialize(&thread_handle, &thread_handler, reinterpret_cast<jvxHandle*>(this), true, false);
		jvx_thread_start(thread_handle, JVX_INFINITE_MS);

		_common_set_min.theState = JVX_STATE_PROCESSING;
	}
	return res;
}

jvxErrorType
CjvxMSWText2Speech::add_task(const char* txtFld, jvxText2SpeechFormat* out_format, IjvxText2Speech_report* backward_ref, jvxSize* uniqueId)
{
	jvxErrorType res = JVX_ERROR_WRONG_STATE;
	if (_common_set_min.theState == JVX_STATE_PROCESSING)
	{
		oneEntryQueue newEntry;
		newEntry.format = *out_format;
		newEntry.reportHandle = backward_ref;
		newEntry.text = txtFld;

		JVX_LOCK_MUTEX(safeAccess);
		newEntry.selector = seedUniqueIds;
		*uniqueId = newEntry.selector;
		theTasks.push_back(newEntry);
		seedUniqueIds++;
		JVX_UNLOCK_MUTEX(safeAccess);

		jvx_thread_wakeup(thread_handle);
		return JVX_NO_ERROR;
	}
	return res;
}

jvxErrorType
CjvxMSWText2Speech::remove_task(jvxSize uniqueId)
{
	jvxErrorType res = JVX_NO_ERROR;
	if (_common_set_min.theState == JVX_STATE_PROCESSING)
	{
		JVX_LOCK_MUTEX(safeAccess);
		if (uniqueId == uId)
		{
			// This task is currently being processed..
			res = JVX_ERROR_COMPONENT_BUSY;
		}
		else
		{
			std::vector<oneEntryQueue>::iterator elm = jvx_findItemSelectorInList<oneEntryQueue, jvxSize>(theTasks, uniqueId);
			if (elm == theTasks.end())
			{
				res = JVX_ERROR_ELEMENT_NOT_FOUND;
			}
			else
			{
				if (elm->reportHandle)
				{
					elm->reportHandle->generation_failed(elm->selector);
				}
				theTasks.erase(elm);
			}
		}
		JVX_UNLOCK_MUTEX(safeAccess);
		return res;
	}
	return JVX_ERROR_WRONG_STATE;
}

jvxErrorType
CjvxMSWText2Speech::postprocess()
{
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;
	if (_common_set_min.theState == JVX_STATE_PROCESSING)
	{
		jvx_thread_trigger_stop(thread_handle);
		jvx_thread_wait_stop(thread_handle, 5000);

		jvx_thread_terminate(thread_handle);
		thread_handle = NULL;

		for (i = 0; i < theTasks.size(); i++)
		{
			if (theTasks[i].reportHandle)
			{
				theTasks[i].reportHandle->generation_failed(theTasks[i].selector);
			}
		}
		theTasks.clear();

		_common_set_min.theState = JVX_STATE_ACTIVE;
		CjvxObject::_deactivate();
		CjvxObject::_unselect();
	}
	return res;
}

jvxErrorType
CjvxMSWText2Speech::terminate()
{
	if (_common_set_min.theState == JVX_STATE_PROCESSING)
	{
		postprocess();
	}
	CjvxObject::_terminate();
	return JVX_NO_ERROR;
}

// ===========================================================================

jvxErrorType
CjvxMSWText2Speech::cb_thread_startup(jvxHandle* privateData_thread, jvxInt64 timestamp_us)
{
	if (privateData_thread)
	{
		CjvxMSWText2Speech* thisp = reinterpret_cast<CjvxMSWText2Speech*>(privateData_thread);
		return thisp->ic_thread_startup(timestamp_us);
	}
	return JVX_ERROR_INVALID_ARGUMENT;
}

 jvxErrorType 
CjvxMSWText2Speech::cb_thread_timer_expired(jvxHandle* privateData_thread, jvxInt64 timestamp_us, jvxSize* delta_ms)
 {
	 if (privateData_thread)
	 {
		 CjvxMSWText2Speech* thisp = reinterpret_cast<CjvxMSWText2Speech*>(privateData_thread);
		 return thisp->ic_thread_timer_expired(timestamp_us);
	 }
	 return JVX_ERROR_INVALID_ARGUMENT;
 }

 jvxErrorType 
	 CjvxMSWText2Speech::cb_thread_wokeup(jvxHandle* privateData_thread, jvxInt64 timestamp_us, jvxSize* delta_ms)
 {
	 if (privateData_thread)
	 {
		 CjvxMSWText2Speech* thisp = reinterpret_cast<CjvxMSWText2Speech*>(privateData_thread);
		 return thisp->ic_thread_wokeup(timestamp_us);
	 }
	 return JVX_ERROR_INVALID_ARGUMENT;
 }

 jvxErrorType 
CjvxMSWText2Speech::cb_thread_stopped(jvxHandle* privateData_thread, jvxInt64 timestamp_us)
 {
	 if (privateData_thread)
	 {
		 CjvxMSWText2Speech* thisp = reinterpret_cast<CjvxMSWText2Speech*>(privateData_thread);
		 return thisp->ic_thread_stopped(timestamp_us);
	 }
	 return JVX_ERROR_INVALID_ARGUMENT;
 }


jvxErrorType 
CjvxMSWText2Speech::ic_thread_startup(jvxInt64 timestamp_us)
{
	HRESULT hr = S_OK;
	hr = ::CoInitialize(NULL);
	assert(hr == S_OK);
	hr = cpVoice.CoCreateInstance(CLSID_SpVoice);
	assert(hr == S_OK);
	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxMSWText2Speech::ic_thread_timer_expired(jvxInt64 timestamp_us)
{
	assert(0);
	return JVX_ERROR_INTERNAL;
}

jvxErrorType 
CjvxMSWText2Speech::ic_thread_wokeup(jvxInt64 timestamp_us)
{
	HRESULT hr = S_OK;
	CComPtr <IStream> cpBaseStream;
	GUID guidFormat; WAVEFORMATEX* pWavFormatEx = nullptr;

	while (theTasks.size())
	{
		std::vector<oneEntryQueue>::iterator elm;

		JVX_LOCK_MUTEX(safeAccess);
		elm = theTasks.begin();
		assert(elm != theTasks.end());

		IjvxText2Speech_report* rep = elm->reportHandle;
		jvxSize uId = elm->selector;
		std::string mess = elm->text;
		format = elm->format;
		theTasks.erase(elm);

		JVX_UNLOCK_MUTEX(safeAccess);

		hr = cpStream.CoCreateInstance(CLSID_SpStream);
		assert(hr == S_OK);

		hr = CreateStreamOnHGlobal(NULL, TRUE, &cpBaseStream);
		assert(hr == S_OK);

		hr = SpConvertStreamFormatEnum(theStreamFormats[format], &guidFormat, &pWavFormatEx);
		assert(hr == S_OK);

		hr = cpStream->SetBaseStream(cpBaseStream, guidFormat, pWavFormatEx);
		cpBaseStream.Release();
		assert(hr == S_OK);

		hr = cpVoice->SetOutput(cpStream, TRUE);
		assert(hr == S_OK);

		SpeechVoiceSpeakFlags my_Spflag = SpeechVoiceSpeakFlags::SVSFlagsAsync; // declaring and initializing Speech Voice Flags
		wchar_t* ptrStr = jvx_stdString2LPWSTR_alloc(mess);
		hr = cpVoice->Speak(
			/*L"I need to spend more and more time on this"*/
			ptrStr, my_Spflag, NULL);
		jvx_stdString2LPWSTR_delete(ptrStr);
		cpVoice->WaitUntilDone(-1);
		assert(hr == S_OK);

		//After SAPI writes the stream, the stream position is at the end, so we need to set it to the beginning.
		_LARGE_INTEGER a = { 0 };
		hr = cpStream->Seek(a, STREAM_SEEK_SET, NULL);
		assert(hr == S_OK);

		//get the base istream from the ispstream 
		IStream *pIstream;
		cpStream->GetBaseStream(&pIstream);

		//calculate the size that is to be read
		STATSTG stats;
		pIstream->Stat(&stats, STATFLAG_NONAME);

		jvxSize sSize = stats.cbSize.QuadPart;	//size of the data to be read

		ULONG bytesRead = 0;	//	this will tell the number of bytes that have been read
		jvxByte *pBuffer = NULL;
		JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(pBuffer, jvxByte, sSize);

		pIstream->Read(pBuffer, sSize, &bytesRead);

		if (rep)
		{
			rep->task_complete(pBuffer, bytesRead, format, uId);
		}

		JVX_DSP_SAFE_DELETE_FIELD(pBuffer);
		pBuffer = NULL;

		JVX_LOCK_MUTEX(safeAccess);
		rep = NULL;
		uId = JVX_SIZE_UNSELECTED;
		mess = "";
		JVX_UNLOCK_MUTEX(safeAccess);

		//don't forget to release everything
		cpStream.Release();
	}
	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxMSWText2Speech::ic_thread_stopped(jvxInt64 timestamp_us)
{
	cpVoice.Release();
	::CoUninitialize();
	return JVX_NO_ERROR;
}