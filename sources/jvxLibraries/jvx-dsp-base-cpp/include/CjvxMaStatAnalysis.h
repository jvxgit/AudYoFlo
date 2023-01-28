
#ifndef __CJVXMASTATANALYSIS_H__
#define __CJVXMASTATANALYSIS_H__

#include "jvx.h" 
#include "jvx_threads.h"

JVX_INTERFACE CjvxMaStatAnalysis_report
{
public:
	~CjvxMaStatAnalysis_report(){};

	virtual void reportUpdate(jvxSize line) = 0;
};

class CjvxMaStatAnalysis
{
private:

	typedef struct
	{
		jvxSize numberObservations;
		jvxSize numberObservationsSinceLastEvaluation;
		jvxData average;
		jvxData variance;
		jvxData presetValue;
	
		jvxData maxValue;
		jvxData minValue;
	
		jvxData* circBuffer;
		jvxData* circBuffer_copy;
		int idxCircBuffer;
	
		jvxData delta;
		bool requestUpdate;
		JVX_MUTEX_HANDLE safeAccessBuffer;

	} oneLine;
	
	std::vector<oneLine> lines;

	jvx_thread_handler threadCallbacks;
	jvxHandle* theThreadHandle;

	int period_ms;
	jvxState theState;
	CjvxMaStatAnalysis_report* backRef;

public:
	CjvxMaStatAnalysis();
	~CjvxMaStatAnalysis();
	jvxErrorType registerOneLine(int numberObservationsMax, jvxData presetValue, jvxSize* idxLine);
	jvxErrorType clearLines();
	jvxErrorType start(jvxBool resetBuffer, jvxInt32 period_msI, CjvxMaStatAnalysis_report* backRef = NULL);
	jvxErrorType getCurrentMeasurement(jvxData* avrgOut, jvxData* maxOut, jvxData* minOut, jvxData* delta, jvxData* var, jvxSize lineId);
	jvxErrorType addEvent(jvxData value, jvxSize lineId);
	jvxErrorType prepareUpdate(jvxSize lineId);
	jvxErrorType triggerUpdate();
	jvxErrorType stop(jvxSize timeout_ms);
	jvxErrorType loopUpdate();

	static jvxErrorType st_callback_thread_startup(jvxHandle* privateData_thread, jvxInt64 timestamp_s);
	static jvxErrorType st_callback_thread_timer_expired(jvxHandle* privateData_thread, jvxInt64 timestamp_us, jvxSize* delta_ms);
	static jvxErrorType st_callback_thread_wokeup(jvxHandle* privateData_thread, jvxInt64 timestamp_us, jvxSize* delta_ms);
	static jvxErrorType st_callback_thread_stopped(jvxHandle* privateData_thread, jvxInt64 timestamp_us);

	jvxErrorType ic_callback_thread_startup(jvxInt64 timestamp_s);
	jvxErrorType ic_callback_thread_timer_expired(jvxInt64 timestamp_us);
	jvxErrorType ic_callback_thread_wokeup(jvxInt64 timestamp_us);
	jvxErrorType ic_callback_thread_stopped(jvxInt64 timestamp_us);
};

#endif
