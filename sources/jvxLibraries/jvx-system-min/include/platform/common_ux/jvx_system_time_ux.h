#ifndef _JVX_SYSTEM_TIME_H__
#define _JVX_SYSTEM_TIME_H__

typedef struct
{
  struct timespec start;
} jvxTimeStampData;

// Timer value is with sign to be able to detect wrap arounds!!
typedef jvxInt64 jvxTick;

JVX_STATIC_INLINE void JVX_GET_TICKCOUNT_US_SETREF(jvxTimeStampData* refData)
{
  int res = clock_gettime(JVX_GLOBAL_CLOCK_SOURCE, &refData->start);
  assert(res == 0);
}

JVX_STATIC_INLINE jvxTick JVX_GET_TICKCOUNT_US_GET_CURRENT(jvxTimeStampData* refData)
{
	jvxTick time_us;
	//timeval tp;
	long deltaSec = 0;
	long deltaNSec = 0;

	struct timespec tpNew;

	tpNew.tv_sec = 0;
	tpNew.tv_nsec = 0;

	int res = clock_gettime(JVX_GLOBAL_CLOCK_SOURCE, &tpNew);
	assert(res == 0);

	deltaSec = tpNew.tv_sec - refData->start.tv_sec;
	deltaNSec = tpNew.tv_nsec - refData->start.tv_nsec;

	time_us  = (jvxTick) deltaSec * 1000000 + deltaNSec/1000;
	return(time_us);
}

#endif
