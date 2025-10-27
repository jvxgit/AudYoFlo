/*
 *****************************************************
 * Filename: fileWriter.cpp
 *****************************************************
 * Project: RTProc-ESP (Echtzeit-Software-Plattform) *
 *****************************************************
 * Description: Class to write data to a file as efficient as
 *			possible. Therefore data is buffered. the format
 *			for data to be written is still left open.
 *
 *****************************************************
 * Developed by JAVOX SOLUTIONS GMBH, 2012           *
 *****************************************************
 * COPYRIGHT BY JAVOX SOLUTION GMBH                  *
 *****************************************************
 * Contact: rtproc@javox-solutions.com               *
 *****************************************************
*/

#include "jvx.h"
#include "CjvxDataLogger.h"

#define MIN(a, b) (a<b?a:b)

#include <iostream>

#if 0
/**
 * Global entry for the waiting thread for incoming new data
 *///==================================================================
JVX_THREAD_ENTRY_FUNCTION(waitForWriteThread, pParam)
{
	_myJvxTools::CjvxDataLogger* fWrite = NULL;
	if(pParam)
	{
		fWrite = (_myJvxTools::CjvxDataLogger*)pParam;

		// Return into class environment
		return(fWrite->awaitIncomingData());
	}
	return(0);
}
#endif

namespace _myJvxTools {

/**
 * Constructor: Set all into uninitialized.
 *///===================================================
CjvxDataLogger::CjvxDataLogger(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE): 
	CjvxObject(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
{
	_common_set.theComponentType.unselected(JVX_COMPONENT_DATALOGGER);
	_common_set.theObjectSpecialization = reinterpret_cast<jvxHandle*>(static_cast<IjvxDataLogger*>(this));
	_common_set.thisisme = static_cast<IjvxObject*>(this);

	config.sizeDataFieldWriteAtOnce = 0;
	config.bytesBuffer = 0;
	//config.listDataSets;

	fileHandle.fPtr = NULL;
	//fileHandle.fName = "";

	//runtime.loopInThread = false;
	//runtime.idThread = JVX_NULLTHREAD;

	memory.buffer = NULL;
	memory.idxRead = 0;
	memory.fillHeight = 0;
	memory.bytesBuffer = 0;

#ifdef JVX_OS_WINDOWS
	//runtime.handleThread = INVALID_HANDLE_VALUE;
#endif

	runtime.numBytesWrittenToLogger = 0;
	runtime.numBytesWrittenToFile = 0;

	//! Header to be written for each session
	memset(&runtime.headerStruct, 0, sizeof(jvxSessionHeader));

	JVX_THREADS_FORWARD_C_CALLBACK_RESET(runtime.thread_handler);
	runtime.thread_handler.callback_thread_wokeup = callback_wokeup;
}

/**
 * Initialize the file writer module, that is set the size for the buffer.
 *///=======================================================================
jvxErrorType
CjvxDataLogger::initialize(jvxSize numberBytesBuffer,  jvxSize numBytesWriteAtOnce, IjvxHiddenInterface* hostRef)
{
	jvxErrorType res = JVX_NO_ERROR;
	res = CjvxObject::_initialize(hostRef);
	if(res == JVX_NO_ERROR)
	{
		config.bytesBuffer = numberBytesBuffer;
		config.sizeDataFieldWriteAtOnce = JVX_MIN(config.bytesBuffer, numBytesWriteAtOnce);
	}
	return(res);
}

/**
 * Opposite of the initialize function. From wherever you come, always return
 * into uninitialized state.
 *///========================================================================
jvxErrorType
CjvxDataLogger::terminate()
{
	if(_common_set_min.theState == JVX_STATE_PROCESSING)
	{
		this->stop();
	}
	if(_common_set_min.theState == JVX_STATE_ACTIVE)
	{
		this->deactivate();
	}
	CjvxObject::_terminate();

	// Deassociate all handles!
	while(config.listDataSets.size())
	{
		config.listDataSets.erase(config.listDataSets.begin());
	}

	config.bytesBuffer = 0;
	return(JVX_NO_ERROR);
}

/**
 * Open the file to write data to when logging. Must be open before recording can be started.
 *///=============================================================================
jvxErrorType
CjvxDataLogger::activate(const char* fileNameOutput, jvxBool append)
{
	jvxErrorType res = JVX_NO_ERROR;

	res = CjvxObject::_select();
	if(res == JVX_NO_ERROR)
	{
		res = CjvxObject::_activate();
		if(res == JVX_NO_ERROR)
		{
			fileHandle.fName = fileNameOutput;
			if(append)
			{
				JVX_FOPEN_UNBUFFERED(fileHandle.fPtr, fileHandle.fName.c_str(), "ab");
			}
			else
			{
				JVX_FOPEN_UNBUFFERED(fileHandle.fPtr, fileHandle.fName.c_str(), "wb");
			}
			if(!fileHandle.fPtr)
			{
//				int res = GetLastError();
				res = JVX_ERROR_OPEN_FILE_FAILED;

				// Reset states
				res = CjvxObject::_deactivate();
				res = CjvxObject::_unselect();

			}
		}
		else
		{
			CjvxObject::_unselect();
		}
	}
	return(res);
}

jvxErrorType
CjvxDataLogger::filename(jvxApiString* fName)
{
	if(_common_set_min.theState >= JVX_STATE_ACTIVE)
	{
		if (fName)
		{
			fName->assign(this->fileHandle.fName);
		}
		return(JVX_NO_ERROR);
	}
	return(JVX_ERROR_WRONG_STATE);
}

/**
 * Close the file again. If module is currently recording, recording will be stopped first.
 *///===============================================================================
jvxErrorType
CjvxDataLogger::deactivate()
{
	jvxErrorType res = CjvxObject::_deactivate();
	if(res == JVX_NO_ERROR)
	{
		res = CjvxObject::_unselect();
		if(res == JVX_NO_ERROR)
		{

			JVX_FCLOSE(fileHandle.fPtr, fileHandle.fName.c_str());
			fileHandle.fPtr = NULL;
			fileHandle.fName = "";
		}
	}
	return(res);
}

/**
 * Start recording, that is: Start the parallel threads for obtaining data,
 * write header information to file and set to state for recording.
 *///=======================================================================
jvxErrorType
CjvxDataLogger::start(bool applyPrioBoost)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize i;
	jvxUInt16 valI16 = 0;
	jvxUInt16* numTags = NULL;
	res = CjvxObject::_prepare();
	if(res == JVX_NO_ERROR)
	{
		res = CjvxObject::_start();
		if(res == JVX_NO_ERROR)
		{
			memset(&runtime.headerStruct, 0, sizeof(runtime.headerStruct));
			runtime.headerStruct.numberSets = 0;

			// START PROCESSING, write Header first!!
			memcpy(&runtime.headerStruct.fldReserved, JVX_HEADER_F_IO_SESSION_BEGIN, sizeof(char)*JVX_HEADER_F_IO_SIZE_HEADER);
			runtime.headerStruct.fldReserved[0] = JVX_FLOAT_DATAFORMAT_ID;
			
			numTags = reinterpret_cast<jvxUInt16*>(&runtime.headerStruct.reserved[0]);
			*numTags = JVX_SIZE_INT16(theTags.size());

			for(i = 0; i < config.listDataSets.size(); i++)
			{
				if(config.listDataSets[i] != NULL)
				{
					runtime.headerStruct.numberSets ++;
				}
			}

			// Write the header, the number of sets and timing info
			fwrite(&this->runtime.headerStruct, sizeof(runtime.headerStruct), 1, fileHandle.fPtr);

			for(i = 0; i < theTags.size(); i++)
			{
				valI16 = JVX_SIZE_INT16(theTags[i].tagName.size());
				fwrite(JVX_HEADER_F_IO_ONETAG, sizeof(char), JVX_HEADER_F_IO_SIZE_HEADER, fileHandle.fPtr);
				fwrite(&valI16, sizeof(jvxUInt16), 1, fileHandle.fPtr);
				fwrite(theTags[i].tagName.c_str(), sizeof(char), valI16, fileHandle.fPtr);
				valI16 = JVX_SIZE_INT16(theTags[i].tagValue.size());
				fwrite(&valI16, sizeof(jvxUInt16), 1, fileHandle.fPtr);
				fwrite(theTags[i].tagValue.c_str(), sizeof(char), valI16, fileHandle.fPtr);
			}

			// Now write header
			for(size_t i = 0; i < config.listDataSets.size(); i++)
			{
				if(config.listDataSets[i] != NULL)
				{
					fwrite(config.listDataSets[i],
					       sizeof(jvxByte),
					       (int)config.listDataSets[i]->szBytes,
					       fileHandle.fPtr);
				}
			}

			memory.bytesBuffer = config.bytesBuffer;
			JVX_SAFE_NEW_FLD(memory.buffer, jvxByte, memory.bytesBuffer);
			memset(memory.buffer, 0, sizeof(jvxByte) * memory.bytesBuffer);
			memory.idxRead = 0;
			memory.fillHeight = 0;

			//memory.allocationBuffer = new jvxByte[config.bytesBuffer];
			//runtime.spaceAvailableBytes = config.bytesBuffer;
			//memory.ptrWriteApplication = memory.allocationBuffer;
			//memory.ptrWriteFile = memory.allocationBuffer;

			// runtime.loopInThread = true;

			jvx_thread_initialize(&runtime.thread_hdl, &runtime.thread_handler, reinterpret_cast<jvxHandle*>(this), false);
			jvx_thread_start(runtime.thread_hdl, JVX_INFINITE_MS);
			if (applyPrioBoost)
			{
				jvx_thread_set_priority(runtime.thread_hdl, JVX_THREAD_PRIORITY_REALTIME);
			}
			JVX_INITIALIZE_MUTEX(runtime.updateFillHeight);

			/*
			// Start the handles for online processing
			JVX_INITIALIZE_NOTIFICATION(runtime.eventsNotify);
			JVX_INITIALIZE_NOTIFICATION(runtime.started);


			JVX_WAIT_FOR_NOTIFICATION_I(runtime.started);
			// Start thread
			JVX_CREATE_THREAD(runtime.handleThread, waitForWriteThread, this, runtime.idThread);
			
			JVX_WAIT_FOR_NOTIFICATION_II_INF(runtime.started);
			*/

			//if(applyPrioBoost)
			//{
			//	JVX_SET_THREAD_PRIORITY(runtime.handleThread, JVX_THREAD_PRIORITY_REALTIME);
			//}

			// Init recording clock
			JVX_GET_TICKCOUNT_US_SETREF(&runtime.tStampRef);

			runtime.numBytesWrittenToLogger = 0;
			runtime.numBytesWrittenToFile = 0;
		}
		else
		{
			CjvxObject::_postprocess();
		}
	}
	return(res);
}

/**
 * Stop recording: Set the state to not active forst and wait. This is due to the possibility that
 * processing is just in the function to add new data..
 * After that send the event that processing is over and wait for parallel thread to complete.
 * The datat is flushed to the file if necessary.
 * All runtime data for the parallel thread is released and removed.
 *///=====================================================================
jvxErrorType
CjvxDataLogger::stop(jvxSize* byteCounterFromAppl, jvxSize* byteCounterToFile, int timeoutStop)
{
	jvxErrorType res = JVX_NO_ERROR;
	res = CjvxObject::_stop();
	if(res == JVX_NO_ERROR)
	{
		res = CjvxObject::_postprocess();
		if(res == JVX_NO_ERROR)
		{
			/* 
			// Right place to check for consistent state at end of processing
			JVX_LOCK_MUTEX(runtime.updateFillHeight);
			jvxSize statFile = runtime.numBytesWrittenToFile + memory.fillHeight;
			if (runtime.numBytesWrittenToLogger != statFile)
			{
				std::cout << "Mismatch" << std::endl;
			}
			JVX_UNLOCK_MUTEX(runtime.updateFillHeight);
			*/

			if(byteCounterFromAppl)
			{
				*byteCounterFromAppl = runtime.numBytesWrittenToLogger;
			}

			res = jvx_thread_stop(runtime.thread_hdl);
			assert(res == JVX_NO_ERROR);

			/*
			// Let us terminate the loop in write thread
			runtime.loopInThread = false;
			// Indicate to file writing process that recording is over now!
			JVX_SET_NOTIFICATION(runtime.eventsNotify);

			// Wait for thread to terminate
			if(JVX_WAIT_FOR_THREAD_TERMINATE_MS(runtime.handleThread, timeoutStop) != JVX_WAIT_SUCCESS)
			{
				// We should never reach here..
				assert(0);

			}
			*/

			// Write all remaining bytes to file
			int ll1 = (int)JVX_MIN(memory.fillHeight, memory.bytesBuffer - memory.idxRead);
			int ll2 = (int)memory.fillHeight - ll1;

			jvxByte* byteStart = NULL;
			if(ll1 > 0)
			{
				byteStart = memory.buffer + memory.idxRead;
				fwrite(byteStart, ll1, 1, this->fileHandle.fPtr);
				runtime.numBytesWrittenToFile += ll1;
			}
			if(ll2 > 0)
			{
				byteStart = memory.buffer;
				fwrite(byteStart, ll2, 1, this->fileHandle.fPtr);
				runtime.numBytesWrittenToFile += ll2;
			}

			// Complete file stream
			fwrite(JVX_HEADER_F_IO_SESSION_END, sizeof(char), JVX_HEADER_F_IO_SIZE_HEADER, fileHandle.fPtr);

			// Unallocate all the fields etc used
			JVX_TERMINATE_MUTEX(runtime.updateFillHeight);
			//JVX_TERMINATE_NOTIFICATION(runtime.eventsNotify);
			//JVX_TERMINATE_NOTIFICATION(runtime.started);

			//DeleteCriticalSection(&updateAdressing);

			// Now count the number of samples written to file..

			if(byteCounterToFile)
			{
				*byteCounterToFile = runtime.numBytesWrittenToFile;
			}

			// Terminate used buffer
			JVX_SAFE_DELETE_FLD(memory.buffer, jvxByte);
			memory.idxRead = 0;
			memory.bytesBuffer = 0;
			memory.fillHeight = 0;

//			delete[](memory.allocationBuffer);
	//		memory.ptrWriteApplication = NULL;
		//	memory.ptrWriteFile = NULL;
		}
	}
	return(res);
}

/**
 * Get current timer information for ticktime storage.
 * This value must be used in combination with timercounts per second.
 *///====================================================================
jvxErrorType
CjvxDataLogger::timestamp_us(jvxTick* timing_us)
{
	if(_common_set_min.theState >= JVX_STATE_PROCESSING)
	{
		if(timing_us)
		{
			*timing_us = JVX_GET_TICKCOUNT_US_GET_CURRENT(&runtime.tStampRef);
		}
		return(JVX_NO_ERROR);
	}
	return(JVX_ERROR_WRONG_STATE);
}

/**
 * Pass new data for file writing. This function can be called from realtime thread.
 *///==================================================================================
jvxErrorType
CjvxDataLogger::pass_new(jvxLogFileDataChunkHeader* addData, jvxBool noBlocking)
{
	jvxErrorType res = JVX_NO_ERROR;
	if(_common_set_min.theState == JVX_STATE_PROCESSING)
	{
		if(noBlocking)
		{
			memcpy(addData->fldReserved, JVX_HEADER_F_IO_DATACHUNK, sizeof(char)*JVX_HEADER_F_IO_SIZE_HEADER);

			jvxByte* ptrFld = (jvxByte*)addData;
			jvxUInt32 lNewData = addData->szBytes;
			jvxSize sz = memory.bytesBuffer - memory.fillHeight;
			jvxInt64 idxWrite = 0;

			JVX_TRY_LOCK_MUTEX_RESULT_TYPE resBlock = JVX_TRY_LOCK_MUTEX_SUCCESS;
			JVX_TRY_LOCK_MUTEX(resBlock, runtime.updateFillHeight);
			if(resBlock == JVX_TRY_LOCK_MUTEX_SUCCESS)
			{
				idxWrite = (memory.idxRead + memory.fillHeight) % memory.bytesBuffer;
				if((jvxSize)lNewData <= sz)
				{
					jvxSize ll1 = (jvxSize) JVX_MIN(lNewData, memory.bytesBuffer - idxWrite);
					jvxSize ll2 = (jvxSize)lNewData - ll1;

					if(ll1)
					{
						memcpy(memory.buffer + idxWrite, ptrFld, ll1);
					}
					if(ll2)
					{
						memcpy(memory.buffer, ptrFld + ll1, ll2);
					}

					memory.fillHeight += lNewData;
					JVX_UNLOCK_MUTEX(runtime.updateFillHeight);

					runtime.numBytesWrittenToLogger += lNewData;

					jvx_thread_wakeup(runtime.thread_hdl);
					//JVX_SET_NOTIFICATION(runtime.eventsNotify);

					res = JVX_NO_ERROR;
				}
				else
				{
					JVX_UNLOCK_MUTEX(runtime.updateFillHeight);
					res = JVX_ERROR_BUFFER_OVERFLOW;
				}
			}
			else
			{
				res = JVX_ERROR_COMPONENT_BUSY;
			}
			return(res);
		}
		else
		{
			memcpy(addData->fldReserved, JVX_HEADER_F_IO_DATACHUNK, sizeof(char)*JVX_HEADER_F_IO_SIZE_HEADER);

			jvxByte* ptrFld = (jvxByte*)addData;
			jvxUInt32 lNewData = addData->szBytes;
			jvxSize sz = memory.bytesBuffer - memory.fillHeight;
			jvxInt64 idxWrite = 0;
			
			JVX_LOCK_MUTEX(runtime.updateFillHeight); 
			idxWrite = (memory.idxRead + memory.fillHeight) % memory.bytesBuffer;
			JVX_UNLOCK_MUTEX(runtime.updateFillHeight);

			if((jvxSize)lNewData <= sz)
			{
				jvxSize ll1 = (jvxSize) JVX_MIN(lNewData, memory.bytesBuffer - idxWrite);
				jvxSize ll2 = (jvxSize)lNewData - ll1;

				if(ll1)
				{
					memcpy(memory.buffer + idxWrite, ptrFld, ll1);
				}
				if(ll2)
				{
					memcpy(memory.buffer, ptrFld + ll1, ll2);
				}

				JVX_LOCK_MUTEX(runtime.updateFillHeight);
				memory.fillHeight += lNewData;
				JVX_UNLOCK_MUTEX(runtime.updateFillHeight);

				runtime.numBytesWrittenToLogger += lNewData;

				/*
				// Activate these lines if you see that data is lost. It should always just "MATCH"
				JVX_LOCK_MUTEX(runtime.updateFillHeight);
				jvxSize statFile = runtime.numBytesWrittenToFile + memory.fillHeight;
				if (runtime.numBytesWrittenToLogger != statFile)
				{
					std::cout << "Mismatch" << std::endl;
				}
				JVX_UNLOCK_MUTEX(runtime.updateFillHeight);
				*/
				jvx_thread_wakeup(runtime.thread_hdl);
				//JVX_SET_NOTIFICATION(runtime.eventsNotify);

				return(JVX_NO_ERROR);
			}
		}
		return(JVX_ERROR_BUFFER_OVERFLOW);
	}
	return(JVX_ERROR_WRONG_STATE);
}

CjvxDataLogger::~CjvxDataLogger(void)
{
	terminate();
}

/**
 * For association:
 * ptrOutside: Allocated outside, must have lifetime of association. Fields are set as follows:
 * =========================================================================================
 * - long long idDataChunk: output, set by module
 * - long long szBytes: input
 * - int dataTypeGlobal: input
 * - int dataTypeUser: input
 * - char noteOnType: input
 * - freeFieldStart: input, can be left
 * =========================================================================================
 *///=======================================================================
jvxErrorType
CjvxDataLogger::associate_datahandle(jvxLogFileDataSetHeader* ptrOutside)
{
	if(_common_set_min.theState == JVX_STATE_INIT)
	{
		ptrOutside->idDataChunk = (jvxInt32)config.listDataSets.size();
		config.listDataSets.push_back(ptrOutside);
		return(JVX_NO_ERROR);
	}
	return(JVX_ERROR_WRONG_STATE);
}

/**
 * Deassociate a dataset. This will remove the field from list but the list entry is not removed, instead a NULL pointer
 * will be stored.
 *///==============================================================================================
jvxErrorType
CjvxDataLogger::deassociate_datahandle(jvxLogFileDataSetHeader* ptrOutside)
{
	if(_common_set_min.theState == JVX_STATE_INIT)
	{
		if(ptrOutside->idDataChunk < (jvxInt32)config.listDataSets.size())
		{
			if((jvxLogFileDataSetHeader*)ptrOutside == (jvxLogFileDataSetHeader*)(config.listDataSets[(int)ptrOutside->idDataChunk]))
			{
				// Only set the pointer to NULL because we might get into ID trouble otherwhise!!
				config.listDataSets[(int)ptrOutside->idDataChunk] = NULL;
				ptrOutside->idDataChunk = -1;
				return(JVX_NO_ERROR);
			}
		}
		return(JVX_ERROR_ELEMENT_NOT_FOUND);
	}
	return(JVX_ERROR_WRONG_STATE);
}

/**
 * Slow thread function callback. Wait for a new data that has arrived or
 * until the application has been terminated.
 *///======================================================================
#if 0
JVX_THREAD_RETURN_TYPE
CjvxDataLogger::awaitIncomingData()
{
	int dwWait;

	JVX_WAIT_FOR_NOTIFICATION_I(runtime.eventsNotify);
	JVX_SET_NOTIFICATION(runtime.started);

	while(runtime.loopInThread)
	{
		// Wait for the buffers completion (elm 1... and stop indication 0)
		dwWait = JVX_WAIT_FOR_NOTIFICATION_II_INF(runtime.eventsNotify);
		bool retriggered = false;

		do
		{
			// Case buffer: A buffer has been completely read
			if (dwWait == JVX_WAIT_SUCCESS)
			{
				jvxSize numValuesCopy = memory.fillHeight;
				if (numValuesCopy >= config.sizeDataFieldWriteAtOnce)
				{
					jvxSize ll1 = (jvxSize)JVX_MIN(numValuesCopy, memory.bytesBuffer - memory.idxRead);
					jvxSize ll2 = numValuesCopy - ll1;

					if (ll1)
					{
						fwrite(memory.buffer + memory.idxRead, ll1, 1, this->fileHandle.fPtr);
					}
					if (ll2)
					{
						fwrite(memory.buffer, ll2, 1, this->fileHandle.fPtr);
					}

					JVX_LOCK_MUTEX(runtime.updateFillHeight);
					memory.fillHeight -= numValuesCopy;
					memory.idxRead = (memory.idxRead + numValuesCopy) % memory.bytesBuffer;
					JVX_UNLOCK_MUTEX(runtime.updateFillHeight);

					runtime.numBytesWrittenToFile += numValuesCopy;
				}
			}
			else
			{
				assert(0);
			}
			if (runtime.loopInThread)
			{
				retriggered = JVX_WAIT_FOR_CHECK_RETRIGGER_NOTIFICATION_I(runtime.eventsNotify);
				if (retriggered)
				{
					// Change this from a timeout event to a signalled event if necessary
					dwWait = JVX_WAIT_SUCCESS;
				}
			}
			else
			{
				retriggered = false;
			}
		} while (retriggered);// This is a shortcut to not fall into the wait if signal was retriggered in the mean time - linux only		    
	}

	return(0);
}
#endif

JVX_THREADS_FORWARD_C_CALLBACK_DEFINE_FUNC_IP(CjvxDataLogger, wokeup)
{
	jvxSize numValuesCopy = memory.fillHeight;
	if (numValuesCopy >= config.sizeDataFieldWriteAtOnce)
	{
		jvxSize ll1 = (jvxSize)JVX_MIN(numValuesCopy, memory.bytesBuffer - memory.idxRead);
		jvxSize ll2 = numValuesCopy - ll1;

		if (ll1)
		{
			fwrite(memory.buffer + memory.idxRead, ll1, 1, this->fileHandle.fPtr);
		}
		if (ll2)
		{
			fwrite(memory.buffer, ll2, 1, this->fileHandle.fPtr);
		}

		JVX_LOCK_MUTEX(runtime.updateFillHeight);
		memory.fillHeight -= numValuesCopy;
		memory.idxRead = (memory.idxRead + numValuesCopy) % memory.bytesBuffer;
		JVX_UNLOCK_MUTEX(runtime.updateFillHeight);

		runtime.numBytesWrittenToFile += numValuesCopy;
	}
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxDataLogger::buffer_fillheight(jvxData* bFillHeight)
{
	if(_common_set_min.theState == JVX_STATE_PROCESSING)
	{
		if(bFillHeight)
		{
			*bFillHeight = (jvxData)memory.fillHeight/(jvxData)memory.bytesBuffer;
		}
	}
	return(JVX_NO_ERROR);
}

jvxErrorType 
CjvxDataLogger::addSessionTag(const char* tagName, const char* tagValue)
{
	oneTagEntry newTag;
	newTag.tagName = tagName;
	newTag.tagValue = tagValue;
	theTags.push_back(newTag);
	return(JVX_NO_ERROR);
}

jvxErrorType 
CjvxDataLogger::removeAllSessionTags()
{
	theTags.clear();
	return(JVX_NO_ERROR);
}

} // namespace
