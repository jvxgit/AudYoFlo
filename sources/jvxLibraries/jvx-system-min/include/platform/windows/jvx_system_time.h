#ifndef _JVX_SYSTEM_TIME_H__
#define _JVX_SYSTEM_TIME_H__

typedef struct
{
	LARGE_INTEGER referenceTime;
} jvxTimeStampData;

// Timer value is with sign to be able to detect wrap arounds!!
typedef jvxInt64 jvxTick;

JVX_STATIC_INLINE void JVX_GET_TICKCOUNT_US_SETREF(jvxTimeStampData* refData)
{
  QueryPerformanceCounter(&refData->referenceTime);
}

JVX_STATIC_INLINE jvxTick JVX_GET_TICKCOUNT_US_GET_CURRENT(jvxTimeStampData* refData)
{
	LARGE_INTEGER freq, time;
	QueryPerformanceFrequency(&freq);
	QueryPerformanceCounter(&time);
	double freqDbl = (double)freq.QuadPart;
	double cntDbl = (double)(time.QuadPart - refData->referenceTime.QuadPart);
	double time_us = (cntDbl/freqDbl)*1000000.0;
	return((jvxTick)time_us);
}

JVX_STATIC_INLINE jvxTick JVX_GET_TICKCOUNT_US_ABS_CURRENT()
{
	LARGE_INTEGER freq, time;
	QueryPerformanceFrequency(&freq);
	QueryPerformanceCounter(&time);
	double freqDbl = (double)freq.QuadPart;
	double cntDbl = (double)(time.QuadPart);
	double time_us = (cntDbl/freqDbl)*1000000.0;
	return((jvxTick)time_us);
}

#endif
