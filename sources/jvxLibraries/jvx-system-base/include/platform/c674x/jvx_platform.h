#ifndef __JVX_PLATFORM_H___C674x__
#define __JVX_PLATFORM_H___C674x__

/*
typedef jvxUInt64 jvxTimeStampData;

static inline void JVX_GET_TICKCOUNT_CLK_SETREF(jvxTimeStampData* refData)
{
	*refData = _itoll(TSCH, TSCL);
}

static inline jvxInt64 JVX_GET_TICKCOUNT_CLK_GET(jvxTimeStampData* refData)
{
	jvxUInt64 tsc_now = _itoll(TSCH, TSCL);

    // detect (single) wraparound
    if (*refData > tsc_now)
	{
        return (jvxUInt64)ULLONG_MAX - *refData + tsc_now;
    } 
	else 
	{
        return tsc_now - *refData;
    }
}

// To be used in combination with private (void) function pointers
static inline jvxInt64 JVX_GET_TICKCOUNT_CLK_GET2(jvxTimeStampData* refData, jvxInt64* output)
{
    *output = JVX_GET_TICKCOUNT_CLK_GET(refData);
}
*/

#endif
