#include "CjvxTextLog.h"
#include <time.h>

#define TIMEOUT 5000


/**
 * Constructor: Set all variables to zero
 *///==============================================
CjvxTextLog::CjvxTextLog(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE): CjvxObject(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
{
	_common_set.theComponentType.unselected(JVX_COMPONENT_SYSTEM_TEXT_LOG);
	_common_set.theObjectSpecialization = reinterpret_cast<jvxHandle*>(static_cast<IjvxTextLog*>(this));
	_common_set.thisisme = static_cast<IjvxObject*>(this);

	buffer.characterBuffer = NULL; 
	buffer.lBuffer = 0;
	buffer.numCharactersInBuffer = 0;
	buffer.idxWrite = 0;

	file.outFile = NULL;
	file.toBeWrittenAtOnce = INT_MAX;

	endlessLoopOn = false;
	logFileState = RTP_LOGFILE_NOTINIT;

	threadHdl = NULL;
	JVX_THREADS_FORWARD_C_CALLBACK_ASSIGN(myThreadHandler, CjvxTextLog, started, stopped, expired, wokeup);

	// Start some mutexes
	JVX_INITIALIZE_MUTEX(this->mutexFBuffer);
	JVX_INITIALIZE_MUTEX(this->mutexCBuffer);
	JVX_INITIALIZE_MUTEX(this->mutexModules);
	JVX_INITIALIZE_MUTEX(this->mutexFile);
	JVX_INITIALIZE_MUTEX(this->mutexConsole);
}

/**
 * Destructor: Set object to initial state
 *///===============================================
CjvxTextLog::~CjvxTextLog()
{
	if(logFileState != RTP_LOGFILE_NOTINIT)
	{
		this->terminate();
	}
	JVX_TERMINATE_MUTEX(this->mutexFile);
	JVX_TERMINATE_MUTEX(this->mutexConsole);
	JVX_TERMINATE_MUTEX(this->mutexFBuffer);
	JVX_TERMINATE_MUTEX(this->mutexCBuffer);
	JVX_TERMINATE_MUTEX(this->mutexModules);
}

/**
 * Init the logfile: Open the outputfile, set the length of the buffers, set how many bytes to write to file at once
 *///===================================================
jvxErrorType
CjvxTextLog::initialize(IjvxHiddenInterface* hostRef, const char* strFileName, jvxSize bytesFilelBuffer, jvxSize bytesFileWriteAtOnce, jvxSize bytesCircularBuffer, 
	jvxSize dbgL)
{
	jvxErrorType resL = JVX_NO_ERROR;
	if(logFileState == RTP_LOGFILE_NOTINIT)
	{
		// Open the file
		file.outFile = fopen(strFileName, "w");
		if(file.outFile)
		{
			// Set output buffer to be non buffered - we do the buffering ourself
			setvbuf(file.outFile, NULL, _IONBF, 0);

			std::string strPrefix = "[0-D]";

			// Write some initialization lines
			fprintf(file.outFile, "%s::=============================\n", strPrefix.c_str());
			fprintf(file.outFile, "%s::JVXRT Text Log :\n", strPrefix.c_str());
			fprintf(file.outFile, "%s::=============================\n", strPrefix.c_str());
			
			std::string tmp = strPrefix + "Start Date: " + JVX_DATE() + "\n";
			fprintf(file.outFile, "%s", tmp.c_str());

			tmp = strPrefix + "Start Time:" + JVX_TIME() + "\n";
			fprintf(file.outFile, "%s", tmp.c_str());
			
			tmp = strPrefix + "Format: Time since start [usec]::Message \n";
			fprintf(file.outFile, "%s", tmp.c_str());

			fprintf(file.outFile, "%s::=============================\n", strPrefix.c_str());
			fprintf(file.outFile, "%s::=============================\n", strPrefix.c_str());

			// Allocate buffer for non-blocking out
			buffer.lBuffer = JVX_SIZE_INT32(bytesFilelBuffer);
			buffer.idxWrite = 0;
			buffer.numCharactersInBuffer = 0;
			buffer.characterBuffer = new char[buffer.lBuffer];
			memset(buffer.characterBuffer, 0, sizeof(char)*buffer.lBuffer);

			// Set the number of bytes to be written at once
			file.toBeWrittenAtOnce = JVX_SIZE_INT32(bytesFileWriteAtOnce);
	
			// Allocate buffer for circular out
			cbuffer.lBuffer = JVX_SIZE_INT32(bytesCircularBuffer);
			cbuffer.idxWrite = 0;
			cbuffer.numIterations = 0;
			cbuffer.characterBuffer = new char[cbuffer.lBuffer];
			memset(cbuffer.characterBuffer, 0, sizeof(char)*cbuffer.lBuffer);

			dbgLevel = dbgL;
			
			logFileState = RTP_LOGFILE_INIT;
			return JVX_NO_ERROR;
		}
		return JVX_ERROR_OPEN_FILE_FAILED;
	}

	return JVX_ERROR_WRONG_STATE;
}

jvxErrorType
CjvxTextLog::addTextLogExpression(const char* moduleFilterE)
{
	jvxSize i;
	jvxErrorType resL = JVX_NO_ERROR;
	if (logFileState == RTP_LOGFILE_INIT)
	{
		std::string strPrefix = "[0-D]";
		std::vector<std::string> moduleFilterExpressionLoc;
		resL = jvx_parsePropertyStringToKeylist(moduleFilterE, moduleFilterExpressionLoc);
		if (resL != JVX_NO_ERROR)
		{
			std::string tmp;
			tmp = strPrefix + __FUNCTION__;
			tmp += ": Invalid text log mudule filter expression <";
			tmp += moduleFilterE;
			tmp += ">\n";
			fprintf(file.outFile, "%s", tmp.c_str());
		}
		for (i = 0; i < moduleFilterExpressionLoc.size(); i++)
		{
			moduleFilterExpression.push_back(moduleFilterExpressionLoc[i]);
		}
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_WRONG_STATE;
}

/**
 * Terminate the log file: Deallocate buffer, terminate mutexes
 *///==========================================================
jvxErrorType
CjvxTextLog::terminate()
{
	if(logFileState == RTP_LOGFILE_STARTED)
	{
		this->stop();
	}
	if(logFileState == RTP_LOGFILE_INIT)
	{
		delete[](buffer.characterBuffer);
		buffer.characterBuffer = NULL;
		buffer.idxWrite = 0;
		buffer.lBuffer = 0;
		buffer.numCharactersInBuffer = 0;

		delete[](cbuffer.characterBuffer);
		cbuffer.characterBuffer = NULL;
		cbuffer.idxWrite = 0;
		cbuffer.lBuffer = 0;
		cbuffer.numIterations = 0;

		fclose(file.outFile);
		file.outFile = NULL;
		file.toBeWrittenAtOnce = INT_MAX;
		logFileState = RTP_LOGFILE_NOTINIT;
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_WRONG_STATE;
}

/**
 * Function to be called to start the logging buffering
 *///=========================================================
jvxErrorType
CjvxTextLog::start()
{
	// Start thread for logging
	if(logFileState == RTP_LOGFILE_INIT)
	{
		JVX_GET_TICKCOUNT_US_SETREF(&tStampRef);
		startTimer = JVX_GET_TICKCOUNT_US_GET_CURRENT(&tStampRef);

		jvx_thread_initialize(&threadHdl, &myThreadHandler, reinterpret_cast<jvxHandle*>(this), false, false, true);
		jvx_thread_start(threadHdl, JVX_SIZE_UNSELECTED);

		/*
		JVX_CREATE_THREAD(threadHdl, thread_entry_c,this,this->threadId);
		*/

		logFileState = RTP_LOGFILE_STARTED;
		return JVX_NO_ERROR;
	}
	return JVX_NO_ERROR;
}

/**
 * Stop the loging.
 *///===========================================================0
jvxErrorType
CjvxTextLog::stop()
{
	if(logFileState == RTP_LOGFILE_STARTED)
	{
		this->endlessLoopOn = false;

		jvx_thread_stop(threadHdl);
		jvx_thread_terminate(threadHdl);
		threadHdl = NULL;

		logFileState = RTP_LOGFILE_INIT;
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_WRONG_STATE;
}

void
CjvxTextLog::prefilterEntries(const std::string& strTime, std::string& out, std::string& txtStr)
{
	while (1)
	{
		std::string token;
		size_t sz = std::string::npos;
		sz = txtStr.find('\n');
		if (sz != std::string::npos)
		{
			token = txtStr.substr(0, sz + 1);// Including \n
			txtStr = txtStr.substr(sz + 1);
			out += strTime + "::" + token;
		}
		else
		{
			break;
		}
	}

	if (txtStr.size())
	{
		out += strTime + "::" + txtStr + "\n";
	}
}

/** 
 * Add a line to the logfile. This function is blocking, the text will be
 * written instantaneously and thus may block the caller.
 *///===========================================================
jvxErrorType
CjvxTextLog::addEntry_direct(const char* txt, const char* moduleName, jvxCBitField outEnum)
{
	jvxTick timing;
	std::string txtStr = txt;
	std::string out;
	if(logFileState == RTP_LOGFILE_STARTED)
	{
		std::string strPrefix = "[";
	
		timing = JVX_GET_TICKCOUNT_US_GET_CURRENT(&tStampRef);
		jvxSize usFromStart = (timing - startTimer);
		strPrefix += jvx_size2String(usFromStart);

		if (moduleName)
		{
			strPrefix += ":<";
			strPrefix += moduleName;
			strPrefix += ">";
		}

		strPrefix += "-D]";

		prefilterEntries(strPrefix, out, txtStr);

		if (out.size())
		{
			JVX_LOCK_MUTEX(this->mutexModules);
			if (jvx_cbitTest(outEnum, JVX_TEXT_LOG_OUTPUT_FILE_SHIFT))
			{
				JVX_LOCK_MUTEX(this->mutexFile);
				fwrite(out.c_str(), sizeof(char), out.size(), file.outFile);
				JVX_UNLOCK_MUTEX(this->mutexFile);
			}
			if (jvx_cbitTest(outEnum, JVX_TEXT_LOG_OUTPUT_CONSOLE_SHIFT))
			{
				JVX_LOCK_MUTEX(this->mutexConsole);
				std::cout << out << std::flush;
				JVX_UNLOCK_MUTEX(this->mutexConsole);
			}
			JVX_UNLOCK_MUTEX(this->mutexModules);
			// Output to circ buffer is not allowed in direct output mode
		}
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_WRONG_STATE;
}

/** 
 * Add a line to the logfile. This function is non-blocking, the text will be
 * written to a large buffer first and to file later in a second low priority thread.
 *///===========================================================
jvxErrorType
CjvxTextLog::addEntry_buffered(const char* txt, const char* moduleName, jvxCBitField outEnum)
{
	jvxTick timing;
	std::string txtStr = txt;
	std::string out;
	jvxErrorType res = JVX_NO_ERROR;
	if(logFileState == RTP_LOGFILE_STARTED)
	{
		std::string strPrefix = "[";

		timing = JVX_GET_TICKCOUNT_US_GET_CURRENT(&tStampRef);
		jvxSize usFromStart = (timing - startTimer);

		strPrefix += jvx_size2String(usFromStart);

		if (moduleName)
		{
			strPrefix += ":<";
			strPrefix += moduleName;
			strPrefix += ">";
		}

		strPrefix += "-B]";

		prefilterEntries(strPrefix, out, txtStr);

		if (out.size())
		{
			JVX_LOCK_MUTEX(this->mutexModules); // Protect different write processes

			if (jvx_cbitTest(outEnum, JVX_TEXT_LOG_OUTPUT_FILE_SHIFT))
			{
				JVX_LOCK_MUTEX(this->mutexFBuffer);
				int idxWriteL = buffer.idxWrite;
				int numSpaceAvailable = buffer.lBuffer - buffer.numCharactersInBuffer;
				JVX_UNLOCK_MUTEX(this->mutexFBuffer);

				if ((int)out.size() <= numSpaceAvailable)
				{
					// Write to buffer
					int t1 = JVX_MIN(buffer.lBuffer - idxWriteL, (int)out.size());
					int t2 = (int)(out.size() - t1);

					char* startBuf1 = buffer.characterBuffer + idxWriteL;
					char* startBuf2 = buffer.characterBuffer;
					const char* strBuf = out.c_str();
					if (t1)
					{
						memcpy(startBuf1, strBuf, t1);
						strBuf += t1;
					}
					if (t2)
					{
						memcpy(startBuf2, strBuf, t2);
					}

					JVX_LOCK_MUTEX(this->mutexFBuffer);
					buffer.idxWrite += (t1 + t2);
					buffer.idxWrite = buffer.idxWrite%buffer.lBuffer;
					buffer.numCharactersInBuffer += (t1 + t2);
					JVX_UNLOCK_MUTEX(this->mutexFBuffer);

					// Inform background thread
					jvx_thread_wakeup(threadHdl);

					res = JVX_NO_ERROR;
				}
				else
				{
					res = JVX_ERROR_BUFFER_OVERFLOW;
				}
			}// if (jvx_cbitTest(outEnum, JVX_TEXT_LOG_OUTPUT_FILE_SHIFT))

			if (res != JVX_NO_ERROR)
			{
				JVX_UNLOCK_MUTEX(this->mutexModules); // Protect different write processes
				return res;
			}

			if (jvx_cbitTest(outEnum, JVX_TEXT_LOG_OUTPUT_CBUFFER_SHIFT))
			{
				int idxRead = 0;
				JVX_LOCK_MUTEX(this->mutexCBuffer);
				jvxSize idxWriteL = cbuffer.idxWrite;

				const char* ptrFrom = out.c_str();
				ptrFrom += idxRead;
				jvxSize writeNow = out.size();

				while (writeNow)
				{

					// Write to buffer
					jvxSize t1 = JVX_MIN(cbuffer.lBuffer - idxWriteL, writeNow);
					char* ptrTo = cbuffer.characterBuffer + idxWriteL;

					if (t1)
					{
						memcpy(ptrTo, ptrFrom, t1);
					}
					ptrFrom += t1;
					writeNow -= t1;

					cbuffer.idxWrite = JVX_SIZE_INT32(cbuffer.idxWrite + t1);
					if (cbuffer.idxWrite == cbuffer.lBuffer)
					{
						cbuffer.idxWrite = 0;
						cbuffer.numIterations++;
					}
				}

				JVX_UNLOCK_MUTEX(this->mutexCBuffer);
				res = JVX_NO_ERROR;

			}

			JVX_UNLOCK_MUTEX(this->mutexModules); // Protect different write processes
			return res;
		}
		
		return JVX_ERROR_INVALID_ARGUMENT;
	}
	return JVX_ERROR_WRONG_STATE;
}

jvxErrorType
CjvxTextLog::addEntry_buffered_nb(const char* txt, const char* moduleName, jvxCBitField outEnum)
{
	jvxTick timing;
	std::string txtStr = txt;
	std::string out;
	jvxErrorType res = JVX_NO_ERROR;
	jvxErrorType res1 = JVX_NO_ERROR;
	jvxErrorType res2 = JVX_NO_ERROR;
	JVX_TRY_LOCK_MUTEX_RESULT_TYPE resM = JVX_TRY_LOCK_MUTEX_NO_SUCCESS;
	JVX_TRY_LOCK_MUTEX_RESULT_TYPE resT1 = JVX_TRY_LOCK_MUTEX_NO_SUCCESS;
	JVX_TRY_LOCK_MUTEX_RESULT_TYPE resT2 = JVX_TRY_LOCK_MUTEX_NO_SUCCESS;
	if(logFileState == RTP_LOGFILE_STARTED)
	{
		std::string strPrefix = "[";

		timing = JVX_GET_TICKCOUNT_US_GET_CURRENT(&tStampRef);
		jvxSize usFromStart = (timing - startTimer);
		strPrefix += jvx_size2String(usFromStart);

		if (moduleName)
		{
			strPrefix += ":<";
			strPrefix += moduleName;
			strPrefix += ">";
		}

		strPrefix += "-BNB]";

		prefilterEntries(strPrefix, out, txtStr);

		JVX_TRY_LOCK_MUTEX(resM, this->mutexModules); // Protect different write processes
		if (JVX_TRY_LOCK_MUTEX_TEST_SUCCESS(resM))
		{
			if (out.size())
			{
				if (jvx_cbitTest(outEnum, JVX_TEXT_LOG_OUTPUT_FILE_SHIFT))
				{
					JVX_TRY_LOCK_MUTEX(resT1, this->mutexFBuffer);
				}
				else
				{
					resT1 = JVX_TRY_LOCK_MUTEX_SUCCESS;
				}

				if (jvx_cbitTest(outEnum, JVX_TEXT_LOG_OUTPUT_CBUFFER_SHIFT))
				{
					JVX_TRY_LOCK_MUTEX(resT2, this->mutexFBuffer);
				}
				else
				{
					resT2 = JVX_TRY_LOCK_MUTEX_SUCCESS;
				}

				if (JVX_TRY_LOCK_MUTEX_TEST_SUCCESS(resT1) && JVX_TRY_LOCK_MUTEX_TEST_SUCCESS(resT2))
				{
					if (jvx_cbitTest(outEnum, JVX_TEXT_LOG_OUTPUT_FILE_SHIFT))
					{
						int idxWriteL = buffer.idxWrite;
						int numSpaceAvailable = buffer.lBuffer - buffer.numCharactersInBuffer;

						if ((int)out.size() <= numSpaceAvailable)
						{
							// Write to buffer
							int t1 = JVX_MIN(buffer.lBuffer - idxWriteL, (int)out.size());
							int t2 = (int)(out.size() - t1);

							char* startBuf1 = buffer.characterBuffer + idxWriteL;
							char* startBuf2 = buffer.characterBuffer;
							const char* strBuf = out.c_str();
							if (t1)
							{
								memcpy(startBuf1, strBuf, t1);
								strBuf += t1;
							}
							if (t2)
							{
								memcpy(startBuf2, strBuf, t2);
							}

							buffer.idxWrite += (t1 + t2);
							buffer.idxWrite = buffer.idxWrite%buffer.lBuffer;
							buffer.numCharactersInBuffer += (t1 + t2);
							JVX_UNLOCK_MUTEX(this->mutexFBuffer);

							// Inform background thread
							jvx_thread_wakeup(threadHdl);

							res1 = JVX_NO_ERROR;
						}
						res1 = JVX_ERROR_BUFFER_OVERFLOW;
					}

					if (jvx_cbitTest(outEnum, JVX_TEXT_LOG_OUTPUT_CBUFFER_SHIFT))
					{
						int idxRead = 0;
						jvxSize idxWriteL = cbuffer.idxWrite;

						const char* ptrFrom = out.c_str();
						ptrFrom += idxRead;
						jvxSize writeNow = out.size();

						while (writeNow)
						{

							// Write to buffer
							jvxSize t1 = JVX_MIN(cbuffer.lBuffer - idxWriteL, writeNow);
							char* ptrTo = cbuffer.characterBuffer + idxWriteL;

							if (t1)
							{
								memcpy(ptrTo, ptrFrom, t1);
							}
							ptrFrom += t1;
							writeNow -= t1;

							cbuffer.idxWrite = JVX_SIZE_INT32(cbuffer.idxWrite + t1);
							if (cbuffer.idxWrite == cbuffer.lBuffer)
							{
								cbuffer.idxWrite = 0;
								cbuffer.numIterations++;
							}
						}
						JVX_UNLOCK_MUTEX(this->mutexCBuffer);
						res2 = JVX_NO_ERROR;
					}

					// Overall result
					res = res1;
					if (res == JVX_NO_ERROR)
					{
						res = res2;
					}
				}
				else
				{
					if (jvx_cbitTest(outEnum, JVX_TEXT_LOG_OUTPUT_FILE_SHIFT))
					{
						if (JVX_TRY_LOCK_MUTEX_TEST_SUCCESS(resT1))
						{
							JVX_UNLOCK_MUTEX(this->mutexFBuffer);
						}
					}
					if (jvx_cbitTest(outEnum, JVX_TEXT_LOG_OUTPUT_CBUFFER_SHIFT))
					{
						if (JVX_TRY_LOCK_MUTEX_TEST_SUCCESS(resT2))
						{
							JVX_UNLOCK_MUTEX(this->mutexCBuffer);
						}
					}
					res = JVX_ERROR_COMPONENT_BUSY;
				}
				JVX_UNLOCK_MUTEX(this->mutexModules);
				return res;
			}
			JVX_UNLOCK_MUTEX(this->mutexModules);
			return JVX_ERROR_INVALID_ARGUMENT;
		}
		return JVX_ERROR_COMPONENT_BUSY;
	}
	return JVX_ERROR_WRONG_STATE;
}

// ========================================================================

jvxErrorType
CjvxTextLog::debug_config(jvxSize* level, const char* moduleName, jvxBool* moduleTextLog)
{
	jvxSize i;
	if (level)
		*level = dbgLevel;
	if (moduleTextLog)
	{
		*moduleTextLog = false;
		if (logFileState == RTP_LOGFILE_STARTED)
		{
			if (moduleFilterExpression.size() == 0)
			{
				std::string entry = "Switching log active for module <";
				entry += moduleName;
				entry += ">.";

				addEntry_buffered(entry.c_str(), "CjvxTextLog");
				*moduleTextLog = true;
			}
			else
			{
				std::string entry;
				bool foundit = false;
				for (i = 0; i < moduleFilterExpression.size(); i++)
				{
					if (jvx_compareStringsWildcard(moduleFilterExpression[i], moduleName))
					{
						*moduleTextLog = true;
						foundit = true;
						break;
					}
				}
				if (foundit)
				{
					entry = "Switching log active for module <";
				}
				else
				{
					entry = "NOT switching log active for module <";
				}
				entry += moduleName;
				entry += ">.";

				addEntry_buffered(entry.c_str(), "CjvxTextLog");
			}
		}
	}
	return JVX_NO_ERROR;
}

#ifdef JVX_OS_WINDOWS
class StackWalkerToString : public StackWalker
{
private:
	std::string* txt;
public:
	StackWalkerToString(std::string* out): StackWalker(RetrieveSymbol|RetrieveSymbol)
	{
		txt = out;
	};

protected:
	virtual void OnOutput(LPCSTR szText) override
	{
		if (txt)
		{
			*txt += szText;
		}
		else
		{
			printf("%s", szText);
		}
	};
};
#endif

jvxErrorType
CjvxTextLog::produceStackframe_direct(const char* description)
{
	std::string out;

	out = "Requested to output stack frame: ";
	out += description;
	out += "\n";

#ifdef JVX_OS_WINDOWS
	StackWalkerToString sw(&out);
	sw.ShowCallstack();
#endif

	addEntry_direct(out.c_str(), "stackframe", JVX_TEXT_LOG_OUTPUT_FILE_SHIFT);

	// To be done later
	return JVX_NO_ERROR;
}


jvxErrorType
CjvxTextLog::current_size_cbuffer_return(jvxSize* sz, jvxTextLogOutputPositionCircBuffer* pos, jvxSize* missed)
{
	if (logFileState == RTP_LOGFILE_STARTED)
	{
	  int diff = cbuffer.idxWrite - JVX_SIZE_INT32(pos->idx); // May be negative due to wrap around
	  int diffIt = cbuffer.numIterations - JVX_SIZE_INT32(pos->iter);

		diff += diffIt*cbuffer.lBuffer;

		if (diff < 0)
		{
			return JVX_ERROR_INVALID_SETTING;
		}

		if (sz)
		{
			*sz = JVX_MIN(diff, cbuffer.lBuffer);
		}
		if (missed)
		{
			diff -= cbuffer.lBuffer;
			*missed = JVX_MAX(diff, 0);
		}
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_WRONG_STATE;
}

jvxErrorType
CjvxTextLog::update_cbuffer_return(jvxByte* buf, jvxSize* sz, jvxTextLogOutputPositionCircBuffer* pos, jvxSize* numLostBytes)
{
	if (logFileState == RTP_LOGFILE_STARTED)
	{
		if (numLostBytes)
		{
			*numLostBytes = 0;
		}
		JVX_LOCK_MUTEX(mutexCBuffer);
		int diff = cbuffer.idxWrite - JVX_SIZE_INT32(pos->idx); // May be negative due to wrap around
		int diffIt = cbuffer.numIterations - JVX_SIZE_INT32(pos->iter);
		diff += diffIt*cbuffer.lBuffer;
		if (diff < 0)
		{
			JVX_UNLOCK_MUTEX(mutexCBuffer);
			return JVX_ERROR_INVALID_SETTING;
		}

		if (!sz)
		{
			JVX_UNLOCK_MUTEX(mutexCBuffer);
			return JVX_ERROR_INVALID_ARGUMENT;
		}

		// Check if we lose bytes due to limited output buffer size
		if (diff > *sz)
		{
			if (numLostBytes)
			{
				*numLostBytes = diff - *sz;
			}
			diff = JVX_SIZE_INT32(*sz);
		}

		// If buffer is too large, reduce output size
		if (*sz > diff)
		{
			*sz = diff;
		}

		// Find output copy position
		jvxSize idxRead = (cbuffer.idxWrite + cbuffer.lBuffer - diff )% cbuffer.lBuffer;

		// Copy in max two segments
		jvxSize t1 = cbuffer.lBuffer - idxRead;
		t1 = JVX_MIN(t1, diff);
		jvxSize t2 = diff - t1;

		if (t1)
		{
			memcpy(buf, cbuffer.characterBuffer + idxRead, t1);
		}
		if (t2)
		{
			memcpy(buf + t1, cbuffer.characterBuffer, t2);
		}
		JVX_UNLOCK_MUTEX(mutexCBuffer);
		pos->idx += diff;
		if (pos->idx >= cbuffer.lBuffer)
		{
			pos->idx -= cbuffer.lBuffer;
			pos->iter++;
		}
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_WRONG_STATE;
}

/*
rtpBool
CjvxTextLog::flushTextBuffer(const char* txt)
{
	// To be done later
	return(true);
}
*/	

JVX_THREADS_FORWARD_C_CALLBACK_DEFINE_FUNC(CjvxTextLog, started)
{
	return JVX_NO_ERROR;
}

JVX_THREADS_FORWARD_C_CALLBACK_DEFINE_FUNC(CjvxTextLog, stopped)
{
	return JVX_NO_ERROR;
}

JVX_THREADS_FORWARD_C_CALLBACK_DEFINE_FUNC_IP(CjvxTextLog, expired)
{
	return JVX_NO_ERROR;
}

JVX_THREADS_FORWARD_C_CALLBACK_DEFINE_FUNC_IP(CjvxTextLog, wokeup)
{
	JVX_LOCK_MUTEX(this->mutexFBuffer);
	int copyLocal = buffer.numCharactersInBuffer;
	int idxRead = buffer.idxWrite - buffer.numCharactersInBuffer;
	JVX_UNLOCK_MUTEX(this->mutexFBuffer);

	while (copyLocal)
	{
		int t1 = 0;
		int t2 = 0;
		char* bufT1S = NULL;
		char* bufT2S = NULL;
		if (idxRead < 0)
		{
			t1 = -idxRead;
			bufT1S = buffer.characterBuffer + (buffer.lBuffer + idxRead);
			bufT2S = buffer.characterBuffer;
		}
		else
		{
			t1 = JVX_MIN(copyLocal, buffer.lBuffer - idxRead);
			bufT1S = buffer.characterBuffer + idxRead;
			bufT2S = buffer.characterBuffer;
		}
		t2 = copyLocal - t1;

		while (t1 > 0)
		{
			int numWrite = JVX_MIN(t1, file.toBeWrittenAtOnce);
			JVX_LOCK_MUTEX(this->mutexFile);
			fwrite(bufT1S, sizeof(char), numWrite, this->file.outFile);
			JVX_UNLOCK_MUTEX(this->mutexFile);
			t1 -= numWrite;
			bufT1S += numWrite;
			copyLocal -= numWrite;
			JVX_LOCK_MUTEX(this->mutexFBuffer);
			buffer.numCharactersInBuffer -= numWrite;
			JVX_UNLOCK_MUTEX(this->mutexFBuffer);
		}
		while (t2 > 0)
		{
			int numWrite = JVX_MIN(t2, file.toBeWrittenAtOnce);
			JVX_LOCK_MUTEX(this->mutexFile);
			fwrite(bufT2S, sizeof(char), numWrite, file.outFile);
			JVX_UNLOCK_MUTEX(this->mutexFile);
			t2 -= numWrite;
			bufT2S += numWrite;
			copyLocal -= numWrite;
			JVX_LOCK_MUTEX(this->mutexFBuffer);
			buffer.numCharactersInBuffer -= numWrite;
			JVX_UNLOCK_MUTEX(this->mutexFBuffer);
		}
	}
	return JVX_NO_ERROR;
}
