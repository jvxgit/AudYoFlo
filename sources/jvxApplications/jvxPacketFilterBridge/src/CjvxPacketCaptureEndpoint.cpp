#include "CjvxPacketCaptureEndpoint.h"

CjvxPacketCaptureEndpoint::CjvxPacketCaptureEndpoint(jvxData smoothI) : smooth(smoothI)
{
	reset();
}

void
CjvxPacketCaptureEndpoint::reset()
{
	in.amount_data_since_last_update = 0;
	in.amount_time_since_last_update = 0;
	in.timeStampRef = JVX_SIZE_UNSELECTED; 
	in.number_frames_total = 0;
	in.rate = 0;
	in.rateOut = 0;

	out.amount_data_since_last_update = 0;
	out.amount_time_since_last_update = 0;
	out.timeStampRef = JVX_SIZE_UNSELECTED;
	out.number_frames_total = 0;
	out.rate = 0;
	out.rateOut = 0;
}


void
CjvxPacketCaptureEndpoint::update_in(jvxSize szLastPacket, jvxUInt64 newTIckCount)
{
	in.number_frames_total++;

	if (in.request_restart)
	{
		in.amount_data_since_last_update = 0;
		in.timeStampRef = JVX_SIZE_UNSELECTED;
		in.rate = 0;
		in.request_restart = false;
	}

	if (JVX_CHECK_SIZE_SELECTED(in.timeStampRef))
	{
		jvxUInt64 delta = newTIckCount - in.timeStampRef;
		jvxData div = ((jvxData)delta * 0.001);// in kBytes - bytes / usec = MBytes /sec
		div += EPSMIN;
		in.amount_data_since_last_update += szLastPacket;
		in.rate = (jvxData)in.amount_data_since_last_update / div;
	}
	else
	{
		in.timeStampRef = newTIckCount;
	}
}

void
CjvxPacketCaptureEndpoint::update_out(jvxSize szLastPacket, jvxUInt64 newTIckCount)
{
	out.number_frames_total++;

	if (out.request_restart)
	{
		out.amount_data_since_last_update = 0;
		out.timeStampRef = JVX_SIZE_UNSELECTED;
		out.rate = 0;
		out.request_restart = false;
	}

	if (JVX_CHECK_SIZE_SELECTED(out.timeStampRef))
	{
		jvxUInt64 delta = newTIckCount - out.timeStampRef;
		jvxData div = ((jvxData)delta * 0.001);// in kBytes - bytes / usec = MBytes /sec
		div += EPSMIN;
		out.amount_data_since_last_update += szLastPacket;
		out.rate = (jvxData)out.amount_data_since_last_update / div;
	}
	else
	{
		out.timeStampRef = newTIckCount;
	}
}

void
CjvxPacketCaptureEndpoint::data_in(jvxData& rateOut, jvxSize& frameCntOut)
{
	jvxBool newP = in.request_restart;
	rateOut = in.rate;
	frameCntOut = in.number_frames_total;

	if (newP)
	{
		rateOut = 0;
	}
	in.request_restart = true;
	
	// Smooth
	in.rateOut = this->smooth* in.rateOut + (1 - this->smooth) * rateOut;
	rateOut = in.rateOut;
}

void
CjvxPacketCaptureEndpoint::data_out(jvxData& rateOut, jvxSize& frameCntOut)
{
	jvxBool newP = out.request_restart;
	rateOut = out.rate;
	frameCntOut = out.number_frames_total;

	if (newP)
	{
		rateOut = 0;

	}
	out.request_restart = true;

	// Smooth
	out.rateOut = this->smooth * out.rateOut + (1 - this->smooth) * rateOut;
	rateOut = out.rateOut;
}

