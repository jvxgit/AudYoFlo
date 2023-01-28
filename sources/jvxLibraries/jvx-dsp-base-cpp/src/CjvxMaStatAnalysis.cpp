
#include <cfloat>
#include <cmath>
#include "CjvxMaStatAnalysis.h"

/*
RTP_THREAD_ENTRY_FUNCTION(c_entry_update, pParam)
{
	if(pParam)
	{
		CjvxMaStatAnalysis* this_p = (CjvxMaStatAnalysis*) pParam;
		this_p->loopUpdate();
	}
	return(0);
}
*/

CjvxMaStatAnalysis::CjvxMaStatAnalysis()
{
	theState = JVX_STATE_INIT;
	backRef = NULL;
	threadCallbacks.callback_thread_startup = st_callback_thread_startup;
	threadCallbacks.callback_thread_timer_expired = st_callback_thread_timer_expired;
	threadCallbacks.callback_thread_wokeup = st_callback_thread_wokeup;
	threadCallbacks.callback_thread_stopped = st_callback_thread_stopped;
}

CjvxMaStatAnalysis::~CjvxMaStatAnalysis()
{
	if(theState == JVX_STATE_PROCESSING)
	{
		this->stop(JVX_INFINITE_MS);
		this->clearLines();
		theState = JVX_STATE_INIT;
	}
}

jvxErrorType 
CjvxMaStatAnalysis::registerOneLine(int numberObservationsMax, jvxData presetValue, jvxSize* idxLine)
{
	jvxSize j;
	if(theState == JVX_STATE_INIT)
	{
		oneLine ln;
		ln.numberObservations = numberObservationsMax;
		ln.average = 0;
		ln.variance = 0;
		ln.maxValue = 0;
		ln.minValue = 0; 
		ln.presetValue = presetValue;
		ln.circBuffer = new jvxData[ln.numberObservations];
		for(j = 0; j < ln.numberObservations; j++)
		{
			ln.circBuffer[j] = presetValue;
		}
		ln.circBuffer_copy = new jvxData[ln.numberObservations];
		ln.idxCircBuffer = 0;
		ln.delta = 0;
		ln.requestUpdate = false;
		ln.numberObservationsSinceLastEvaluation = 0;
		JVX_INITIALIZE_MUTEX(ln.safeAccessBuffer);
		if(idxLine)
		{
			*idxLine = (jvxSize)lines.size();
		}
		lines.push_back(ln);
	}
	return(JVX_NO_ERROR);
}

jvxErrorType 
CjvxMaStatAnalysis::clearLines()
{
	int i;

	if(theState == JVX_STATE_INIT)
	{
		for(i = 0; i < (int)lines.size(); i++)
		{
			delete[](lines[i].circBuffer);
			delete[](lines[i].circBuffer_copy);
			JVX_TERMINATE_MUTEX(lines[i].safeAccessBuffer);
		}
		lines.clear();
	}
	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxMaStatAnalysis::start(jvxBool resetBuffer, jvxInt32 period_msI, CjvxMaStatAnalysis_report* ref)
{
	jvxSize i,j;
	jvxErrorType res = JVX_NO_ERROR;
	if(theState == JVX_STATE_INIT)
	{
		theState = JVX_STATE_PROCESSING;
		period_ms = period_msI;
		backRef = ref;

		for(i = 0; i < (int)lines.size(); i++)
		{
			lines[i].average = 0;
			lines[i].variance = 0;

			lines[i].maxValue = 0;
			lines[i].minValue = 0; 

			if(resetBuffer)
			{
				for(j = 0; j < lines[i].numberObservations; j++)
				{
					lines[i].circBuffer[j] = lines[i].presetValue;
				}
			}
			lines[i].idxCircBuffer = 0;
			lines[i].delta = 0;
		}

		res = jvx_thread_initialize(&theThreadHandle, &threadCallbacks, reinterpret_cast<jvxHandle*>(this), false);
		assert(res == JVX_NO_ERROR);
		res = jvx_thread_start(theThreadHandle, period_ms);
		assert(res == JVX_NO_ERROR);

		return JVX_NO_ERROR;
	}
	return(JVX_ERROR_WRONG_STATE);
}

jvxErrorType 
CjvxMaStatAnalysis::getCurrentMeasurement(jvxData* avrgOut, jvxData* maxOut, jvxData* minOut, jvxData* deltaOut, jvxData* var, jvxSize lineId)
{
	if(theState == JVX_STATE_PROCESSING)
	{
		if(lineId < lines.size())
		{
			if(avrgOut)
			{
				*avrgOut = lines[lineId].average;
			}
			if(maxOut)
			{
				*maxOut = lines[lineId].maxValue;
			}
			if(minOut)
			{
				*minOut = lines[lineId].minValue;
			}
			if(deltaOut)
			{
				*deltaOut = lines[lineId].delta;
			}
			if(var)
			{
				*var = lines[lineId].variance;
			}

			return JVX_NO_ERROR;
		}
	}
	return(JVX_ERROR_WRONG_STATE);
}

jvxErrorType 
CjvxMaStatAnalysis::addEvent(jvxData value, jvxSize lineId)
{
	if(theState == JVX_STATE_PROCESSING)
	{
		if(lineId < lines.size())
		{
			lines[lineId].circBuffer[lines[lineId].idxCircBuffer] = value;
			lines[lineId].idxCircBuffer = (lines[lineId].idxCircBuffer + 1)%lines[lineId].numberObservations;
			JVX_LOCK_MUTEX(lines[lineId].safeAccessBuffer);
			lines[lineId].numberObservationsSinceLastEvaluation ++;
			lines[lineId].requestUpdate = true;
			JVX_UNLOCK_MUTEX(lines[lineId].safeAccessBuffer);

			return JVX_NO_ERROR;
		}
	}
	return(JVX_ERROR_WRONG_STATE);
}

jvxErrorType 
CjvxMaStatAnalysis::prepareUpdate(jvxSize lineId)
{
	if(theState == JVX_STATE_PROCESSING)
	{
		if(lineId < lines.size())
		{
			JVX_LOCK_MUTEX(lines[lineId].safeAccessBuffer);
			lines[lineId].requestUpdate = true;
			JVX_UNLOCK_MUTEX(lines[lineId].safeAccessBuffer);
			return JVX_NO_ERROR;
		}
	}
	return(JVX_ERROR_WRONG_STATE);
}

jvxErrorType 
CjvxMaStatAnalysis::triggerUpdate()
{
	if(theState == JVX_STATE_PROCESSING)
	{
		jvx_thread_wakeup(theThreadHandle);
		return JVX_NO_ERROR;
	}
	return(JVX_ERROR_WRONG_STATE);
}

jvxErrorType 
CjvxMaStatAnalysis::stop(jvxSize timeout_ms)
{
	jvxErrorType res = JVX_NO_ERROR;
	if(theState == JVX_STATE_PROCESSING)
	{
		jvx_thread_stop(theThreadHandle, timeout_ms);
		if(res != JVX_NO_ERROR)
		{
			assert(0);
		}
		theState = JVX_STATE_INIT;
		return JVX_NO_ERROR;
	}
	return(JVX_ERROR_WRONG_STATE);

}

jvxErrorType 
CjvxMaStatAnalysis::st_callback_thread_startup(jvxHandle* privateData_thread, jvxInt64 timestamp_s)
{
	if(privateData_thread)
	{
		CjvxMaStatAnalysis* this_pointer = reinterpret_cast<CjvxMaStatAnalysis*>(privateData_thread);
		return this_pointer->ic_callback_thread_startup( timestamp_s);
	}
	return JVX_ERROR_INVALID_ARGUMENT;
}

jvxErrorType 
CjvxMaStatAnalysis::st_callback_thread_timer_expired(jvxHandle* privateData_thread, jvxInt64 timestamp_us, jvxSize* delta_ms)
{
	if(privateData_thread)
	{
		CjvxMaStatAnalysis* this_pointer = reinterpret_cast<CjvxMaStatAnalysis*>(privateData_thread);
		return this_pointer->ic_callback_thread_timer_expired(timestamp_us);
	}
	return JVX_ERROR_INVALID_ARGUMENT;
}

jvxErrorType 
CjvxMaStatAnalysis::st_callback_thread_wokeup(jvxHandle* privateData_thread, jvxInt64 timestamp_us, jvxSize* delta_ms)
{
	if(privateData_thread)
	{
		CjvxMaStatAnalysis* this_pointer = reinterpret_cast<CjvxMaStatAnalysis*>(privateData_thread);
		return this_pointer->ic_callback_thread_wokeup(timestamp_us);
	}
	return JVX_ERROR_INVALID_ARGUMENT;
}

jvxErrorType 
CjvxMaStatAnalysis::st_callback_thread_stopped(jvxHandle* privateData_thread, jvxInt64 timestamp_us)
{
	if(privateData_thread)
	{
		CjvxMaStatAnalysis* this_pointer = reinterpret_cast<CjvxMaStatAnalysis*>(privateData_thread);
		return this_pointer->ic_callback_thread_stopped(timestamp_us);
	}
	return JVX_ERROR_INVALID_ARGUMENT;
}

jvxErrorType 
CjvxMaStatAnalysis::ic_callback_thread_startup(jvxInt64 timestamp_s)
{
	return(JVX_NO_ERROR);
}

jvxErrorType 
CjvxMaStatAnalysis::ic_callback_thread_timer_expired(jvxInt64 timestamp_us)
{
	return(loopUpdate());
}

jvxErrorType 
CjvxMaStatAnalysis::ic_callback_thread_wokeup(jvxInt64 timestamp_us)
{
	return(loopUpdate());
}

jvxErrorType 
CjvxMaStatAnalysis::ic_callback_thread_stopped(jvxInt64 timestamp_us)
{
	return(JVX_NO_ERROR);
}


jvxErrorType 
CjvxMaStatAnalysis::loopUpdate()
{
	jvxSize i,j;
	jvxBool requestUpdate = false;

	// Do the computation
	for(i = 0; i < (int) lines.size(); i++)
	{

		if(lines[i].requestUpdate)
		{
			JVX_LOCK_MUTEX(lines[i].safeAccessBuffer);
			int circIdxLoc = lines[i].idxCircBuffer;
			int numberObsLast = JVX_SIZE_INT(lines[i].numberObservationsSinceLastEvaluation);
			lines[i].numberObservationsSinceLastEvaluation = 0;
			memcpy(lines[i].circBuffer_copy, lines[i].circBuffer, sizeof(jvxData)*lines[i].numberObservations);
			lines[i].requestUpdate = false;
			JVX_UNLOCK_MUTEX(lines[i].safeAccessBuffer);

			jvxData accu = 0;
			jvxData accuTmp = 0;
			jvxData maxValue = -FLT_MAX;
			jvxData minValue = +FLT_MAX;
			int cnt = 0;
			int cntHalf = (int)floor((jvxData)lines[i].numberObservations/2);

			jvxSize lh1 = lines[i].numberObservations - circIdxLoc;
			jvxSize lh2 = circIdxLoc;

			for(j = 0; j < lh1; j ++, cnt++)
			{
				jvxData oneValue = lines[i].circBuffer_copy[j];
				if(cnt == cntHalf)
				{
					accuTmp = accu;
				}
				accu += oneValue;
				if(oneValue > maxValue)
				{
					maxValue = oneValue;
				}
				if(oneValue < minValue)
				{
					minValue = oneValue;
				}
			}
			for(j = 0; j < lh2; j ++, cnt++)
			{
				jvxData oneValue = lines[i].circBuffer_copy[j];
				if(cnt == cntHalf)
				{
					accuTmp = accu;
				}
				accu += oneValue;
				if(oneValue > maxValue)
				{
					maxValue = oneValue;
				}
				if(oneValue < minValue)
				{
					minValue = oneValue;
				}
			}

			jvxData avrgH1 = accuTmp / (jvxData)cntHalf;
			jvxData avrgH2 = (accu - accuTmp) /(jvxData)(lines[i].numberObservations - cntHalf);

			lines[i].delta = avrgH1 - avrgH2;
			lines[i].average = accu/(jvxData)(lines[i].numberObservations);
			lines[i].maxValue = maxValue;
			lines[i].minValue = minValue;

			accu = 0;
			accuTmp = 0.0;
			for(j = 0; j < 	lines[i].numberObservations; j++)
			{
				accuTmp = (lines[i].circBuffer_copy[j] -  lines[i].average);
				accuTmp *= accuTmp;
				accu += accuTmp;
			}

			lines[i].variance = accu / (jvxData)lines[i].numberObservations;
			if(backRef)
			{
				backRef->reportUpdate(i);
			}
		}
	}
	return(JVX_NO_ERROR);
}
