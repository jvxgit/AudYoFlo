/*
*/

#include "jvx.h"
#include "CjvxThreadController.h"


/**
 * Constructor: Set all into uninitialized.
 *///===================================================
CjvxThreadController::CjvxThreadController(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE): 
	CjvxObject(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
{
	_common_set.theComponentType.unselected( JVX_COMPONENT_THREADCONTROLLER);
	_common_set.theObjectSpecialization = reinterpret_cast<jvxHandle*>(static_cast<IjvxThreadController*>(this));
	_common_set.thisisme = static_cast<IjvxObject*>(this);
	JVX_INITIALIZE_MUTEX(safeAccessLists);

	runtime.thread.cbs.callback_thread_startup = NULL;
	runtime.thread.cbs.callback_thread_stopped = NULL;
	runtime.thread.cbs.callback_thread_timer_expired = static_wokeup_cb;
	runtime.thread.cbs.callback_thread_wokeup = NULL;

	runtime.uniqueCnt = 1;

}

CjvxThreadController::~CjvxThreadController()
{
	terminate();
	JVX_TERMINATE_MUTEX(safeAccessLists);
}

	//! Initialize the module, set size of buffer and strategy
jvxErrorType 
CjvxThreadController::initialize(IjvxHiddenInterface* hostRef)
{
	jvxErrorType res = JVX_NO_ERROR;
	JVX_AFFINITY_MASK procAffMask = 0, systemAffinityMask = 0;
	res = CjvxObject::_initialize(hostRef);
	if(res == JVX_NO_ERROR)
	{
		runtime.myHandle = JVX_INVALID_HANDLE_VALUE;
		JVX_GET_CURRENT_PROCESS_HANDLE(runtime.myHandle); // The current process may be returned as an INVALID_HANDLE_VALUE (-1) in Windows
		JVX_GET_PROCESS_ID(runtime.myId, runtime.myHandle);

		JVX_GET_PROCESS_AFFINITY_RESULT result = JVX_GET_PROCESS_AFFINITY_SUCCESS;
		JVX_GET_PROCESS_AFFINITY_MASK(result, runtime.myHandle, procAffMask, systemAffinityMask);

		int cnt = 0;
		while(1)
		{
			if(systemAffinityMask & 0x1)
			{
				cnt++;
				systemAffinityMask = systemAffinityMask >> 1;
			}
			else
			{ 
				break;
			}
		}
		runtime.numCores = cnt;
	}
	return(res);
}

	//! Initialize the module, set size of buffer and strategy
jvxErrorType
CjvxThreadController::number_cores(jvxSize* numCores)
{
	if(_common_set_min.theState >= JVX_STATE_INIT)
	{
		if(numCores)
		{
			*numCores = runtime.numCores;
		}
		return(JVX_NO_ERROR);
	}
	return(JVX_ERROR_WRONG_STATE);
}


jvxErrorType 
CjvxThreadController::configure(jvxInt32 percent_non_time_critical, jvxInt32 timeout_refresh_ms)
{
	if(_common_set_min.theState == JVX_STATE_INIT)
	{
		runtime.config.percent_non_time_critical = percent_non_time_critical;
		runtime.config.timeout_refresh_ms = timeout_refresh_ms;

		runtime.derived.numCoresRetain = (jvxSize)(ceil((jvxData)runtime.config.percent_non_time_critical /100.0 * runtime.numCores));
		runtime.derived.numCoresRetain = JVX_MAX(1, runtime.derived.numCoresRetain);
		runtime.derived.numCoresRetain = JVX_MIN(runtime.numCores, runtime.derived.numCoresRetain);

		return(JVX_NO_ERROR);
	}
	return(JVX_ERROR_WRONG_STATE);
}


jvxErrorType
CjvxThreadController::activate()
{
	jvxErrorType res = JVX_NO_ERROR;
	std::vector<oneProcessObject> newListPs;
	std::vector<oneThreadObject> newListTs;
	std::string errMessage;

	res = CjvxObject::_select();
	if(res == JVX_NO_ERROR)
	{
		res = CjvxObject::_activate();
		if(res == JVX_NO_ERROR)
		{
			HKEY key;
			// Trick from http://stackoverflow.com/questions/560366/detect-if-running-with-administrator-privileges-under-windows-xp
			if(RegOpenKey(HKEY_USERS, "S-1-5-19", &key) == ERROR_SUCCESS)
			{
				// Do something
				this->_report_text_message("Application was started with administrator privileges, process/thread control will work!", JVX_REPORT_PRIORITY_INFO);
				RegCloseKey(key);
			}
			else
			{
				// Do something
				this->_report_text_message("Application was not started with administrator privileges, process/thread control will not work!", JVX_REPORT_PRIORITY_WARNING);
			}


			JVX_LOCK_MUTEX(safeAccessLists);

			res = createProcessList(newListPs, errMessage);
			if(res != JVX_NO_ERROR)
			{
				_common_set.theErrordescr = errMessage;
			}
			this->runtime.lstProcesses = newListPs;

			res = createThreadList(newListTs, errMessage, runtime.myId);
			if(res != JVX_NO_ERROR)
			{
				_common_set.theErrordescr = errMessage;
			}
			this->runtime.lstMyThreads = newListTs;
			JVX_UNLOCK_MUTEX(safeAccessLists);
		}
	}
	return(res);
}

jvxErrorType
CjvxThreadController::createProcessList(std::vector<oneProcessObject>& lst, std::string& err)
{
	lst.clear();

	jvxErrorType res = JVX_NO_ERROR;
	JVX_AFFINITY_MASK procAffMask = 0, systemAffinityMask = 0;
	oneProcessObject oneProcess;
	HANDLE hProcessSnap;
	HANDLE hProcess;
	PROCESSENTRY32 pe32;
	DWORD dwPriorityClass;
	jvxBool errorDetected = false;

	// Take a snapshot of all processes in the system.
	hProcessSnap = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );
	if( hProcessSnap == INVALID_HANDLE_VALUE )
	{
		res = JVX_ERROR_CALL_SUB_COMPONENT_FAILED;
		err = "Failed to obtain process snapshot handle";
	}
	else
	{
		// Set the size of the structure before using it.
		pe32.dwSize = sizeof( PROCESSENTRY32 );

		// Retrieve information about the first process,
		// and exit if unsuccessful
		if( !Process32First( hProcessSnap, &pe32 ) )
		{
			CloseHandle( hProcessSnap );          // clean the snapshot object
			res = JVX_ERROR_CALL_SUB_COMPONENT_FAILED;
			err = "Failed to obtain process handle";
		}
		else
		{

			// Now walk the snapshot of processes, and
			// display information about each process in turn
			do
			{


				// Retrieve the priority class.
				dwPriorityClass = 0;
				if(runtime.myId != pe32.th32ProcessID )
				{
					hProcess = NULL;
					JVX_OPEN_PROCESS(hProcess , pe32.th32ProcessID );
					if( hProcess != NULL )
					{
						JVX_GET_PROCESS_AFFINITY_RESULT result = JVX_GET_PROCESS_AFFINITY_SUCCESS;
						oneProcess.thePrio = JVX_PRIORITY_ABOVE_UNKNOWN;

						JVX_GET_PROCESS_AFFINITY_MASK(result, hProcess, procAffMask, systemAffinityMask);
						//								processAffinityMask = 3;
						//								result = SetProcessAffinityMask(hProcess, processAffinityMask);

						// We will only be able to open non-system threads
						dwPriorityClass = GetPriorityClass( hProcess );
						if(dwPriorityClass )
						{
							switch(dwPriorityClass)
							{
							case ABOVE_NORMAL_PRIORITY_CLASS:
								oneProcess.thePrio = JVX_PRIORITY_ABOVE_NORMAL;
								break;
							case BELOW_NORMAL_PRIORITY_CLASS:
								oneProcess.thePrio = JVX_PRIORITY_BELOW_NORMAL;
								break;
							case HIGH_PRIORITY_CLASS:
								oneProcess.thePrio = JVX_PRIORITY_HIGH;
								break;
							case IDLE_PRIORITY_CLASS:
								oneProcess.thePrio = JVX_PRIORITY_IDLE;
								break;
							case NORMAL_PRIORITY_CLASS:
								oneProcess.thePrio = JVX_PRIORITY_NORMAL;
								break;
							case REALTIME_PRIORITY_CLASS:
								oneProcess.thePrio = JVX_PRIORITY_REALTIME;
								break;
							}						
						}

						oneProcess.theNameProcess = pe32.szExeFile;
						oneProcess.initAffinity = procAffMask;
						oneProcess.affinitySet = false;
						oneProcess.theId = pe32.th32ProcessID;
						lst.push_back(oneProcess);

						JVX_CLOSE_PROCESS( hProcess );


						// List the modules and threads associated with this process
						//ListProcessModules( pe32.th32ProcessID );
						//ListProcessThreads( pe32.th32ProcessID );
						if(errorDetected)
						{
							break;
						}
					}
				} 
			}
			while( Process32Next( hProcessSnap, &pe32 ) );

			CloseHandle( hProcessSnap );
		}
	}
	return(res);
}

jvxErrorType
CjvxThreadController::createThreadList(std::vector<oneThreadObject>& lst, std::string& err, JVX_PROCESS_ID idP)
{ 
	HANDLE hThreadSnap = INVALID_HANDLE_VALUE; 
	THREADENTRY32 te32; 
	jvxErrorType res = JVX_NO_ERROR;

	// Take a snapshot of all running threads  
	hThreadSnap = CreateToolhelp32Snapshot( TH32CS_SNAPTHREAD, 0 ); 
	if( hThreadSnap == INVALID_HANDLE_VALUE ) 
	{
		res = JVX_ERROR_CALL_SUB_COMPONENT_FAILED;
		err = "Creation of snapshot failed";
	}
	
	
	if(res == JVX_NO_ERROR)
	{

		// Fill in the size of the structure before using it. 
		te32.dwSize = sizeof(THREADENTRY32); 

		// Retrieve information about the first thread,
		// and exit if unsuccessful
		if( !Thread32First( hThreadSnap, &te32 ) ) 
		{
			res = JVX_ERROR_CALL_SUB_COMPONENT_FAILED;
			err = "Thread32First"; // show cause of failure
		}
		
		if(res == JVX_NO_ERROR)
		{
			do 
			{ 
				if( te32.th32OwnerProcessID == idP )
				{
					oneThreadObject obj;
					obj.selector[0] = te32.th32ThreadID;
					obj.theHdl = OpenThread(THREAD_ALL_ACCESS, FALSE, obj.selector[0]);
					obj.previousValueSet = -1;
					int prio = GetThreadPriority(obj.theHdl);
					switch(prio)
					{
					case ABOVE_NORMAL_PRIORITY_CLASS:
						obj.thePrio = JVX_PRIORITY_ABOVE_NORMAL;
						break;
					case BELOW_NORMAL_PRIORITY_CLASS:
						obj.thePrio = JVX_PRIORITY_BELOW_NORMAL;
						break;
					case HIGH_PRIORITY_CLASS:
						obj.thePrio = JVX_PRIORITY_HIGH;
						break;
					case IDLE_PRIORITY_CLASS:
						obj.thePrio = JVX_PRIORITY_IDLE;
						break;
					case NORMAL_PRIORITY_CLASS:
						obj.thePrio = JVX_PRIORITY_NORMAL;
						break;
					case REALTIME_PRIORITY_CLASS:
						obj.thePrio = JVX_PRIORITY_REALTIME;
						break;
					}						

					lst.push_back(obj);
					/*
					_tprintf( TEXT("\n\n     THREAD ID      = 0x%08X"), te32.th32ThreadID ); 
					_tprintf( TEXT("\n     Base priority  = %d"), te32.tpBasePri ); 
					_tprintf( TEXT("\n     Delta priority = %d"), te32.tpDeltaPri ); 
					_tprintf( TEXT("\n"));
					*/
				}
			} while( Thread32Next(hThreadSnap, &te32 ) ); 
		}
		CloseHandle( hThreadSnap );          // clean the snapshot object
	}

	return(res);
}


jvxErrorType
CjvxThreadController::register_tc_thread(jvxInt32* id_address)
{
	jvxSize i;
	jvxErrorType res = JVX_ERROR_WRONG_STATE;

	if(_common_set_min.theState >= JVX_STATE_ACTIVE)
	{
		if(id_address)
		{
			JVX_LOCK_MUTEX(safeAccessLists);
			if(runtime.lstRegThreads.size() < runtime.derived.numCoresRetain)
			{

				*id_address = runtime.uniqueCnt;
				oneRegisteredThread thr;
				thr.selector[0] = runtime.uniqueCnt;
				runtime.uniqueCnt++;				
				int cnt = 0;

				for(i = 0; i < runtime.lstRegThreads.size(); i++)
				{
					jvxBool isUsed = false;
					for(i = 0; i < runtime.lstRegThreads.size(); i++)
					{
						if(runtime.lstRegThreads[i].threadAffId == cnt)
						{
							isUsed = true;
							break;
						}
					}
					if(!isUsed)
					{
						break;
					}
					cnt++;
				}

				thr.selector[1] = 0;
				thr.threadAffId = cnt;
				thr.idRegistered = false;
				runtime.lstRegThreads.push_back(thr);

			}
			else
			{
				res = JVX_ERROR_ID_OUT_OF_BOUNDS;
			}
			JVX_UNLOCK_MUTEX(safeAccessLists);
		}
		res = JVX_NO_ERROR;
	}	
	
	return(res);
}

jvxErrorType
CjvxThreadController::unregister_tc_thread(jvxInt32 id_address)
{
	jvxErrorType res = JVX_ERROR_WRONG_STATE;
	if(_common_set_min.theState >= JVX_STATE_ACTIVE)
	{
		JVX_LOCK_MUTEX(safeAccessLists);
		std::vector<oneRegisteredThread>::iterator elm = jvx_findItemSelectorInList_one(runtime.lstRegThreads, id_address, 0);
		if(elm != runtime.lstRegThreads.end())
		{
			runtime.lstRegThreads.erase(elm);
			res = JVX_NO_ERROR;
		}
		else
		{
			res = JVX_ERROR_ELEMENT_NOT_FOUND;
		}
		JVX_UNLOCK_MUTEX(safeAccessLists);
	}
	return(res);
}

jvxErrorType
CjvxThreadController::prepare()
{
	jvxErrorType res = JVX_NO_ERROR;
	std::string errMessage;
	res = CjvxObject::_prepare();
	if(res == JVX_NO_ERROR)
	{
		//BOOL resL = SetPriorityClass(runtime.myHandle, HIGH_PRIORITY_CLASS);
		
		SetProcessAffinityMask(runtime.myHandle, (1 << runtime.numCores) - 1);
		runtime.derived.threadAffMaskEverythingElse = (1 << (runtime.derived.numCoresRetain)) - 1;
		jvx_thread_initialize(&runtime.thread.hdl, &runtime.thread.cbs, reinterpret_cast<jvxHandle*>(this), false, false, false);
		jvx_thread_start(runtime.thread.hdl, runtime.config.timeout_refresh_ms);

	}
	return(res);
}

jvxErrorType 
CjvxThreadController::updateProcessList_nolock(std::vector<oneProcessObject>& lst, jvxBool setMask, jvxInt32 mask)
{
	jvxSize i,j;
	HANDLE hProcess;
	DWORD_PTR aff;
	DWORD_PTR sys_aff;

	hProcess = NULL;
		JVX_OPEN_PROCESS(hProcess , runtime.myId);
	GetProcessAffinityMask(hProcess, &aff, &sys_aff);

	for(i = 0; i < lst.size(); i++)
	{
		JVX_PROCESS_ID hdlId = lst[i].theId;
		jvxBool inPreviousList = false;
		jvxBool activeSet = false;

		for(j = 0; j < runtime.lstProcesses.size(); j++)
		{
			if(runtime.lstProcesses[j].theId == hdlId)
			{
				inPreviousList = true;
				break;
			}
		}

		if(inPreviousList)
		{
			if(!runtime.lstProcesses[j].affinitySet)
			{
				activeSet = true;
			}
		}
		else
		{
			activeSet = true;
			runtime.lstProcesses.push_back(lst[i]);
		}

		if(setMask)
		{
			if(activeSet)
			{
				JVX_GET_PROCESS_AFFINITY_RESULT result = JVX_GET_PROCESS_AFFINITY_SUCCESS;
				hProcess = NULL;
				JVX_OPEN_PROCESS(hProcess , hdlId);
				if( hProcess != NULL )
				{
					JVX_SET_PROCESS_AFFINITY_MASK(result, hProcess, runtime.derived.threadAffMaskEverythingElse);
					JVX_CLOSE_PROCESS(hProcess);
				}
				lst[i].affinitySet = true;
			}
		}
	}

	hProcess = NULL;
		JVX_OPEN_PROCESS(hProcess , runtime.myId);
	GetProcessAffinityMask(hProcess, &aff, &sys_aff);
	/*
	if(setMask)
	{
		JVX_GET_PROCESS_AFFINITY_RESULT result = JVX_GET_PROCESS_AFFINITY_SUCCESS;
		hProcess = NULL;
		JVX_OPEN_PROCESS(hProcess , runtime.myId);
		if( hProcess != NULL )
		{
			JVX_SET_PROCESS_AFFINITY_MASK(result, hProcess, (1 << runtime.numCores)-1);
			JVX_CLOSE_PROCESS(hProcess);
		}
	}*/
	return(JVX_NO_ERROR);
}


jvxErrorType 
CjvxThreadController::updateThreadList_nolock(std::vector<oneThreadObject>& lst)
{
	jvxSize i,j;

	for(i = 0; i < lst.size(); i++)
	{
		JVX_THREAD_ID hdlId = lst[i].selector[0];
		jvxBool inPreviousList = false;

		for(j = 0; j < runtime.lstMyThreads.size(); j++)
		{
			if(runtime.lstMyThreads[j].selector[0] == hdlId)
			{
				inPreviousList = true;
				break;
			}
		}

		if(!inPreviousList)
		{
			oneThreadObject theObj = lst[i];
			theObj.previousValueSet = -1;
			runtime.lstMyThreads.push_back(theObj);
		}
	}

	return(JVX_NO_ERROR);
}

jvxErrorType 
CjvxThreadController::associate_tc_thread(JVX_THREAD_ID theThread, jvxInt32 id_address)
{
	jvxErrorType res = JVX_ERROR_WRONG_STATE;
	if(_common_set_min.theState >= JVX_STATE_PREPARED)
	{
		JVX_LOCK_MUTEX(safeAccessLists);
		std::vector<oneRegisteredThread>::iterator elm = 
			jvx_findItemSelectorInList_one<oneRegisteredThread, jvxInt32>(runtime.lstRegThreads, id_address, 0);
		if(elm != runtime.lstRegThreads.end())
		{
			if(elm->idRegistered == false)
			{
				elm->idThread = theThread;
				elm->idRegistered = true;
				res = JVX_NO_ERROR;
			}
			else
			{
				res = JVX_ERROR_DUPLICATE_ENTRY;
			}
		}
		else
		{
			res = JVX_ERROR_ELEMENT_NOT_FOUND;
		}
		JVX_UNLOCK_MUTEX(safeAccessLists);
	}
	return(res);
}

jvxErrorType 
CjvxThreadController::post_process()
{
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;
	HANDLE hProcess;
	std::vector<oneProcessObject> lstProcessesNew;
	std::vector<oneThreadObject> lstThreadsNew;
	std::string err;

	res = CjvxObject::_postprocess();
	if(res == JVX_NO_ERROR)
	{
		SetPriorityClass(runtime.myHandle, NORMAL_PRIORITY_CLASS);
		jvx_thread_stop(runtime.thread.hdl);
		jvx_thread_terminate(runtime.thread.hdl);
		runtime.thread.hdl = NULL;

		this->createProcessList(lstProcessesNew, err);
		this->createThreadList(lstThreadsNew, err, runtime.myId);
		JVX_LOCK_MUTEX(safeAccessLists);

		updateProcessList_nolock(lstProcessesNew, false, 0);
		updateThreadList_nolock(lstThreadsNew);
		for(i = 0; i < this->runtime.lstProcesses.size(); i++)
		{
			JVX_GET_PROCESS_AFFINITY_RESULT result = JVX_GET_PROCESS_AFFINITY_SUCCESS;
			hProcess = NULL;
			JVX_OPEN_PROCESS(hProcess , this->runtime.lstProcesses[i].theId);
			if( hProcess != NULL )
			{
				JVX_SET_PROCESS_AFFINITY_MASK(result, hProcess, (1 << runtime.numCores) -1);
				JVX_CLOSE_PROCESS(hProcess);
			}
			this->runtime.lstProcesses[i].affinitySet = false;
		}

		for(i = 0; i < this->runtime.lstMyThreads.size(); i++)
		{
			JVX_THREAD_ID tId = this->runtime.lstMyThreads[i].selector[0];
			JVX_AFFINITY_MASK msk = (1 << runtime.numCores) - 1;
			SetThreadAffinityMask(this->runtime.lstMyThreads[i].theHdl, msk);
		}

		for(i = 0; i< runtime.lstRegThreads.size(); i++)
		{
			runtime.lstRegThreads[i].idThread = 0;
			runtime.lstRegThreads[i].idRegistered = false;
		}
		JVX_UNLOCK_MUTEX(safeAccessLists);
	}
	return(res);
}

/*
jvxErrorType 
CjvxThreadController::release_tc_thread(jvxInt32 id_address)
{
	jvxErrorType res = JVX_ERROR_WRONG_STATE;
	this->_lock_state();
	if(_common_set_min.theState == JVX_STATE_ACTIVE)
	{
		std::vector<oneRegisteredThread>::iterator elm = jvx_findItemSelectorInList<oneRegisteredThread, jvxInt32>(runtime.lstRegThreads, id_address, 0);
		if(elm != runtime.lstRegThreads.end())
		{
			runtime.lstRegThreads.erase(elm);
		}
		res = JVX_NO_ERROR;
	}
	this->_unlock_state();
	return(res);
}
*/

jvxErrorType 
CjvxThreadController::deactivate()
{
	jvxSize i;
	jvxErrorType res = JVX_ERROR_WRONG_STATE;
	JVX_PROCESS_HANDLE hdl;
	JVX_GET_PROCESS_AFFINITY_RESULT result = JVX_GET_PROCESS_AFFINITY_SUCCESS;
	res = CjvxObject::_deactivate();
	if(res == JVX_NO_ERROR)
	{
		for(i = 0; i < runtime.lstProcesses.size(); i++)
		{
			JVX_OPEN_PROCESS(hdl, runtime.lstProcesses[i].theId);
			if(hdl)
			{
				JVX_SET_PROCESS_AFFINITY_MASK(result, hdl, runtime.lstProcesses[i].initAffinity);
			}
		}
		runtime.lstProcesses.clear();
	}
	return(res);
}

jvxErrorType 
CjvxThreadController::terminate()
{
	if (_common_set_min.theState == JVX_STATE_ACTIVE)
	{
		this->deactivate();
	}
	if (_common_set_min.theState == JVX_STATE_SELECTED)
	{
		this->_unselect();
	}
	CjvxObject::_terminate();
	return(JVX_NO_ERROR);
}

jvxErrorType
CjvxThreadController::static_wokeup_cb(jvxHandle* privateData_thread, jvxInt64 timestamp_us, jvxSize* delta_ms)
{
	CjvxThreadController* this_pointer = reinterpret_cast<CjvxThreadController*>(privateData_thread);
	if(this_pointer)
	{
		return(this_pointer->wokeup_cb(timestamp_us));
	}
	return(JVX_ERROR_INVALID_ARGUMENT);
}
	
jvxErrorType 
CjvxThreadController::wokeup_cb(jvxInt64 timestamp_us)
{
	jvxSize i,j;
	std::vector<oneProcessObject> lstProcessesNew;
	std::vector<oneThreadObject> lstThreadsNew;
	std::string err;
	BOOL resL = FALSE;
	DWORD errCode;
	JVX_AFFINITY_MASK aff = 0;
	JVX_AFFINITY_MASK sysaff = 0;
	JVX_AFFINITY_MASK affPre = 0;

	// 1) Update process list
	// 2) Update thread list
	// 3) Set all process affinity masks to desired values
	// 4) Set all thread affinity masks to desired values
	this->createProcessList(lstProcessesNew, err);
	this->createThreadList(lstThreadsNew, err, runtime.myId);

	JVX_LOCK_MUTEX(safeAccessLists);
	updateProcessList_nolock(lstProcessesNew, true, runtime.derived.threadAffMaskEverythingElse);
	updateThreadList_nolock(lstThreadsNew);

	for(i = 0; i < this->runtime.lstMyThreads.size(); i++)
	{
		JVX_THREAD_ID tId = this->runtime.lstMyThreads[i].selector[0];
		jvxBool requiresHighPrio = false;
		jvxSize offsetCore = 0;
		for(j = 0; j < runtime.lstRegThreads.size(); j ++)
		{
			if(runtime.lstRegThreads[j].idRegistered)
			{
				if(runtime.lstRegThreads[j].idThread == tId)
				{
					requiresHighPrio = true;
					offsetCore = runtime.lstRegThreads[j].threadAffId;
					break;
				}
			}
		}

		JVX_AFFINITY_MASK msk = (1 << runtime.numCores) - 1;
		if(requiresHighPrio)
		{
			 msk = ((JVX_AFFINITY_MASK)1 << (runtime.derived.numCoresRetain + offsetCore));
		}
		else
		{
			msk = runtime.derived.threadAffMaskEverythingElse;
		}
		GetProcessAffinityMask(runtime.myHandle, &aff, &sysaff);
		//if(msk != this->runtime.lstMyThreads[i].previousValueSet)
		{
			JVX_SET_PROCESS_AFFINITY_MASK(affPre, this->runtime.lstMyThreads[i].theHdl, msk);
			if(affPre == 0)
			{
				resL = JVX_ERROR_INTERNAL;
			}
			this->runtime.lstMyThreads[i].previousValueSet = msk;
		}
		
		if(resL == FALSE)
		{
			errCode = GetLastError();
		}
	}

	JVX_UNLOCK_MUTEX(safeAccessLists);
	return(JVX_NO_ERROR);
}
/*
jvxErrorType
CjvxThreadController::initialize(jvxSize numberBytesBuffer,  jvxSize numBytesWriteAtOnce)
{
	jvxErrorType res = JVX_NO_ERROR;
	res = CjvxObject::_initialize();
	if(res == JVX_NO_ERROR)
	{
		config.bytesBuffer = numberBytesBuffer;
		config.sizeDataFieldWriteAtOnce = JVX_MIN(config.bytesBuffer, numBytesWriteAtOnce);
	}
	return(res);
}
*/

#ifdef SCHWUPPDIWUPPDI




static inline jvxErrorType JVX_SET_PROCESS_AFFINITY_ALL_OTHER(std::string& errDescr, std::vector<processObject>& objs)
{
	HANDLE hProcessSnap;
	HANDLE hProcess;
	PROCESSENTRY32 pe32;
	DWORD dwPriorityClass;
	DWORD processAffinityMask = 0;
	DWORD systemAffinityMask = 0;
	jvxBool errorDetected = false;
	jvxErrorType res = JVX_NO_ERROR;
	// Take a snapshot of all processes in the system.
	hProcessSnap = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );
	if( hProcessSnap == INVALID_HANDLE_VALUE )
	{
		res = JVX_ERROR_CALL_SUB_COMPONENT_FAILED;
		errDescr = "Failed to obtain process snapshot handle";
	}
	else
	{
		// Set the size of the structure before using it.
		pe32.dwSize = sizeof( PROCESSENTRY32 );

		// Retrieve information about the first process,
		// and exit if unsuccessful
		if( !Process32First( hProcessSnap, &pe32 ) )
		{
			CloseHandle( hProcessSnap );          // clean the snapshot object
			res = JVX_ERROR_CALL_SUB_COMPONENT_FAILED;
			errDescr = "Failed to obtain process handle";
		}
		else
		{

			// Now walk the snapshot of processes, and
			// display information about each process in turn
			do
			{
				processObject oneProcess;

				// Retrieve the priority class.
				dwPriorityClass = 0;
				hProcess = OpenProcess( PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID );
				if( hProcess != NULL )
				{
					BOOL result = FALSE;
					oneProcess.thePrio = JVX_PRIORITY_ABOVE_UNKNOWN;

					result = GetProcessAffinityMask(hProcess, &processAffinityMask, &systemAffinityMask);
					processAffinityMask = 3;
					result = SetProcessAffinityMask(hProcess, processAffinityMask);

					// We will only be able to open non-system threads
					dwPriorityClass = GetPriorityClass( hProcess );
					if( !dwPriorityClass )
					{
						switch(dwPriorityClass)
						{
						case ABOVE_NORMAL_PRIORITY_CLASS:
							oneProcess.thePrio = JVX_PRIORITY_ABOVE_NORMAL;
							break;
						case BELOW_NORMAL_PRIORITY_CLASS:
							oneProcess.thePrio = JVX_PRIORITY_BELOW_NORMAL;
							break;
						case HIGH_PRIORITY_CLASS:
							oneProcess.thePrio = JVX_PRIORITY_HIGH;
							break;
						case IDLE_PRIORITY_CLASS:
							oneProcess.thePrio = JVX_PRIORITY_IDLE;
							break;
						case NORMAL_PRIORITY_CLASS:
							oneProcess.thePrio = JVX_PRIORITY_NORMAL;
							break;
						case REALTIME_PRIORITY_CLASS:
							oneProcess.thePrio = JVX_PRIORITY_REALTIME;
							break;
						}						
					}

					oneProcess.theNameProcess = pe32.szExeFile;
					objs.push_back(oneProcess);

					CloseHandle( hProcess );

					/*
					_tprintf( TEXT("\n  Process ID        = 0x%08X"), pe32.th32ProcessID );
					_tprintf( TEXT("\n  Thread count      = %d"),   pe32.cntThreads );
					_tprintf( TEXT("\n  Parent process ID = 0x%08X"), pe32.th32ParentProcessID );
					_tprintf( TEXT("\n  Priority base     = %d"), pe32.pcPriClassBase );
					if( dwPriorityClass )
					_tprintf( TEXT("\n  Priority class    = %d"), dwPriorityClass );
					*/

					// List the modules and threads associated with this process
					//ListProcessModules( pe32.th32ProcessID );
					//ListProcessThreads( pe32.th32ProcessID );
					if(errorDetected)
					{
						break;
					}

				} 
			}
			while( Process32Next( hProcessSnap, &pe32 ) );

			CloseHandle( hProcessSnap );
		}
	}
	return(res);
}

/*
BOOL ListProcessModules( DWORD dwPID )
{
  HANDLE hModuleSnap = INVALID_HANDLE_VALUE;
  MODULEENTRY32 me32;

  // Take a snapshot of all modules in the specified process.
  hModuleSnap = CreateToolhelp32Snapshot( TH32CS_SNAPMODULE, dwPID );
  if( hModuleSnap == INVALID_HANDLE_VALUE )
  {
    printError( TEXT("CreateToolhelp32Snapshot (of modules)") );
    return( FALSE );
  }

  // Set the size of the structure before using it.
  me32.dwSize = sizeof( MODULEENTRY32 );

  // Retrieve information about the first module,
  // and exit if unsuccessful
  if( !Module32First( hModuleSnap, &me32 ) )
  {
    printError( TEXT("Module32First") );  // show cause of failure
    CloseHandle( hModuleSnap );           // clean the snapshot object
    return( FALSE );
  }

  // Now walk the module list of the process,
  // and display information about each module
  do
  {
    _tprintf( TEXT("\n\n     MODULE NAME:     %s"),   me32.szModule );
    _tprintf( TEXT("\n     Executable     = %s"),     me32.szExePath );
    _tprintf( TEXT("\n     Process ID     = 0x%08X"),         me32.th32ProcessID );
    _tprintf( TEXT("\n     Ref count (g)  = 0x%04X"),     me32.GlblcntUsage );
    _tprintf( TEXT("\n     Ref count (p)  = 0x%04X"),     me32.ProccntUsage );
    _tprintf( TEXT("\n     Base address   = 0x%08X"), (DWORD) me32.modBaseAddr );
    _tprintf( TEXT("\n     Base size      = %d"),             me32.modBaseSize );

  } while( Module32Next( hModuleSnap, &me32 ) );

  CloseHandle( hModuleSnap );
  return( TRUE );
}

BOOL ListProcessThreads( DWORD dwOwnerPID ) 
{ 
  HANDLE hThreadSnap = INVALID_HANDLE_VALUE; 
  THREADENTRY32 te32; 
 
  // Take a snapshot of all running threads  
  hThreadSnap = CreateToolhelp32Snapshot( TH32CS_SNAPTHREAD, 0 ); 
  if( hThreadSnap == INVALID_HANDLE_VALUE ) 
    return( FALSE ); 
 
  // Fill in the size of the structure before using it. 
  te32.dwSize = sizeof(THREADENTRY32); 
 
  // Retrieve information about the first thread,
  // and exit if unsuccessful
  if( !Thread32First( hThreadSnap, &te32 ) ) 
  {
    printError( TEXT("Thread32First") ); // show cause of failure
    CloseHandle( hThreadSnap );          // clean the snapshot object
    return( FALSE );
  }

  // Now walk the thread list of the system,
  // and display information about each thread
  // associated with the specified process
  do 
  { 
    if( te32.th32OwnerProcessID == dwOwnerPID )
    {
      _tprintf( TEXT("\n\n     THREAD ID      = 0x%08X"), te32.th32ThreadID ); 
      _tprintf( TEXT("\n     Base priority  = %d"), te32.tpBasePri ); 
      _tprintf( TEXT("\n     Delta priority = %d"), te32.tpDeltaPri ); 
      _tprintf( TEXT("\n"));
    }
  } while( Thread32Next(hThreadSnap, &te32 ) ); 

  CloseHandle( hThreadSnap );
  return( TRUE );
}

void printError( TCHAR* msg )
{
  DWORD eNum;
  TCHAR sysMsg[256];
  TCHAR* p;

  eNum = GetLastError( );
  FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
         NULL, eNum,
         MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
         sysMsg, 256, NULL );

  // Trim the end of the line and terminate it with a null
  p = sysMsg;
  while( ( *p > 31 ) || ( *p == 9 ) )
    ++p;
  do { *p-- = 0; } while( ( p >= sysMsg ) &&
                          ( ( *p == '.' ) || ( *p < 33 ) ) );

  // Display the message
  _tprintf( TEXT("\n  WARNING: %s failed with error %d (%s)"), msg, eNum, sysMsg );
}

BOOL ListProcessModules( DWORD dwPID )
{
  HANDLE hModuleSnap = INVALID_HANDLE_VALUE;
  MODULEENTRY32 me32;

  // Take a snapshot of all modules in the specified process.
  hModuleSnap = CreateToolhelp32Snapshot( TH32CS_SNAPMODULE, dwPID );
  if( hModuleSnap == INVALID_HANDLE_VALUE )
  {
    printError( TEXT("CreateToolhelp32Snapshot (of modules)") );
    return( FALSE );
  }

  // Set the size of the structure before using it.
  me32.dwSize = sizeof( MODULEENTRY32 );

  // Retrieve information about the first module,
  // and exit if unsuccessful
  if( !Module32First( hModuleSnap, &me32 ) )
  {
    printError( TEXT("Module32First") );  // show cause of failure
    CloseHandle( hModuleSnap );           // clean the snapshot object
    return( FALSE );
  }

  // Now walk the module list of the process,
  // and display information about each module
  do
  {
    _tprintf( TEXT("\n\n     MODULE NAME:     %s"),   me32.szModule );
    _tprintf( TEXT("\n     Executable     = %s"),     me32.szExePath );
    _tprintf( TEXT("\n     Process ID     = 0x%08X"),         me32.th32ProcessID );
    _tprintf( TEXT("\n     Ref count (g)  = 0x%04X"),     me32.GlblcntUsage );
    _tprintf( TEXT("\n     Ref count (p)  = 0x%04X"),     me32.ProccntUsage );
    _tprintf( TEXT("\n     Base address   = 0x%08X"), (DWORD) me32.modBaseAddr );
    _tprintf( TEXT("\n     Base size      = %d"),             me32.modBaseSize );

  } while( Module32Next( hModuleSnap, &me32 ) );

  CloseHandle( hModuleSnap );
  return( TRUE );
}

BOOL ListProcessThreads( DWORD dwOwnerPID ) 
{ 
  HANDLE hThreadSnap = INVALID_HANDLE_VALUE; 
  THREADENTRY32 te32; 
 
  // Take a snapshot of all running threads  
  hThreadSnap = CreateToolhelp32Snapshot( TH32CS_SNAPTHREAD, 0 ); 
  if( hThreadSnap == INVALID_HANDLE_VALUE ) 
    return( FALSE ); 
 
  // Fill in the size of the structure before using it. 
  te32.dwSize = sizeof(THREADENTRY32); 
 
  // Retrieve information about the first thread,
  // and exit if unsuccessful
  if( !Thread32First( hThreadSnap, &te32 ) ) 
  {
    printError( TEXT("Thread32First") ); // show cause of failure
    CloseHandle( hThreadSnap );          // clean the snapshot object
    return( FALSE );
  }

  // Now walk the thread list of the system,
  // and display information about each thread
  // associated with the specified process
  do 
  { 
    if( te32.th32OwnerProcessID == dwOwnerPID )
    {
      _tprintf( TEXT("\n\n     THREAD ID      = 0x%08X"), te32.th32ThreadID ); 
      _tprintf( TEXT("\n     Base priority  = %d"), te32.tpBasePri ); 
      _tprintf( TEXT("\n     Delta priority = %d"), te32.tpDeltaPri ); 
      _tprintf( TEXT("\n"));
    }
  } while( Thread32Next(hThreadSnap, &te32 ) ); 

  CloseHandle( hThreadSnap );
  return( TRUE );
}


*/
#endif //#ifdef JVX_OS_WINDOWS
	