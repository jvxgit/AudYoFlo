#ifndef _CJVXPACKETCAPTUREENDPOINT_H__
#define _CJVXPACKETCAPTUREENDPOINT_H__

#include "jvx.h"

#define EPSMIN 1e-5


class CjvxPacketCaptureEndpoint
{
private:
	jvxSize packetCnt;
	jvxData smooth;

	typedef struct
	{
		jvxData rate;
		jvxData rateOut;
		jvxSize amount_data_since_last_update;
		jvxUInt64 amount_time_since_last_update;
		jvxSize number_frames_total;
		jvxBool request_restart;
		jvxUInt64 timeStampRef;
	} inorout;

	inorout in;
	inorout out;

public:
	CjvxPacketCaptureEndpoint(jvxData smoothI);

	void reset();

	void update_in(jvxSize szLastPacket, jvxUInt64 newTIckCount);

	void update_out(jvxSize szLastPacket, jvxUInt64 newTIckCount);

	void data_in(jvxData& rateOut, jvxSize& frameCntOut);

	void data_out(jvxData& rateOut, jvxSize& frameCntOut);
};

#endif
