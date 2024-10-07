#include "CjvxPixBufRendering.h"
#include <opencv2/imgproc.hpp>

#define JVX_ALPHA_SMOOTH 0.9

void 
CjvxPixBufRendering::lockf(jvxHandle* priv)
{
	jvxLockWithVariable<jvxOneThreadBufferCombo>* lockHandle = reinterpret_cast<jvxLockWithVariable<jvxOneThreadBufferCombo>*>(priv);
	assert(lockHandle);
	lockHandle->lock();
}

jvxErrorType CjvxPixBufRendering::trylockf(jvxHandle* priv)
{
	jvxLockWithVariable< jvxOneThreadBufferCombo>* lockHandle = reinterpret_cast<jvxLockWithVariable<jvxOneThreadBufferCombo>*>(priv);
	assert(lockHandle);
	if (lockHandle->try_lock())
	{
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_COMPONENT_BUSY;
}

void CjvxPixBufRendering::unlockf(jvxHandle* priv)
{
	jvxLockWithVariable<jvxOneThreadBufferCombo>* lockHandle = reinterpret_cast<jvxLockWithVariable<jvxOneThreadBufferCombo>*>(priv);
	assert(lockHandle);
	lockHandle->unlock();
}

// ==========================================================================================================================

jvxErrorType
CjvxPixBufRendering::startup(jvxInt64 timestamp_us)
{
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxPixBufRendering::expired(jvxInt64 timestamp_us, jvxSize* delta_ms)
{
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxPixBufRendering::wokeup(jvxInt64 timestamp_us, jvxSize* delta_ms)
{
	buffer_proc_core();
	return JVX_NO_ERROR;
}

void
CjvxPixBufRendering::buffer_proc_core()
{
	// This function is in a dedicated thread outside the main thread and the visual data processing thread
	if (buffer)
	{
		if (lockHandle)
		{
			if (lockHandle->v.swb)
			{
				if (lockHandle->v.swb->fill_height > 0)
				{
					jvxTick tickNow = JVX_GET_TICKCOUNT_US_GET_CURRENT(&tStamp);

					// Check buffer match size!!
					// assert(lockHandle->v.swb->specific.the2DFieldBuffer_full.common.szFldOneBuf == szBuf);

					//lockHandle->v.swb->safe_access.lockf(lockHandle->v.swb->safe_access.priv);
					jvxSize idx_read = lockHandle->v.swb->idx_read;
					//lockHandle->v.swb->safe_access.unlockf(lockHandle->v.swb->safe_access.priv);
					jvxByte* ptrRead = lockHandle->v.swb->ptrFld;
					ptrRead += idx_read * lockHandle->v.swb->specific.the2DFieldBuffer_full.common.szFldOneBuf;

					if (lockHandle->v.swb->specific.the2DFieldBuffer_full.common.szFldOneBuf == szBuf)
					{
						memcpy(*buffer, ptrRead, lockHandle->v.swb->specific.the2DFieldBuffer_full.common.szFldOneBuf);
					}
					else
					{
						// Convert from lockHandle->v.swb to szBuf
						cv::Mat imageFrom(lockHandle->v.swb->specific.the2DFieldBuffer_full.common.seg_y,
							lockHandle->v.swb->specific.the2DFieldBuffer_full.common.seg_x, CV_8UC4, ptrRead);
						cv::Mat resize_down;
						cv::resize(imageFrom, resize_down, cv::Size(width, height), cv::INTER_LINEAR);
						memcpy(*buffer, resize_down.data, szBuf);

						// https://learnopencv.com/image-resizing-with-opencv/#resize-by-wdith-height							
					}

					/*
					jvxUInt8* bb = *buffer;
					for (uint32_t k = 0; k < height; k++)
					{
						int rp = k % 255;
						for (uint32_t j = 0; j < width * 4; j += 4)
						{
							*bb++ = rp;//(uint8_t)(127.f * (1 + sin(0.1f * i)));
							*bb++ = 128;// (uint8_t)(127.f * (1.f + sin(0.2f * i)));
							*bb++ = 64;// (uint8_t)(127.f * (1.f + sin(0.3f * i)));
							*bb++ = 255;
						}
					}
					*/
					lockHandle->v.swb->safe_access.lockf(lockHandle->v.swb->safe_access.priv);
					lockHandle->v.swb->idx_read = (lockHandle->v.swb->idx_read + 1) % lockHandle->v.swb->specific.the2DFieldBuffer_full.common.number_buffers;
					lockHandle->v.swb->fill_height--;
					lockHandle->v.swb->safe_access.unlockf(lockHandle->v.swb->safe_access.priv);
					//lockHandle->v.swb->safe_access.unlockf(lockHandle->v.swb->safe_access.priv);

					jvxTick tickNowConvert = JVX_GET_TICKCOUNT_US_GET_CURRENT(&tStamp);

					jvxData deltaT = tickNowConvert - tickNow;
					deltaT *= 1e-6;
					deltaTAverageConvert = JVX_ALPHA_SMOOTH * deltaTAverageConvert + (1 - JVX_ALPHA_SMOOTH) * deltaT;

					if (JVX_CHECK_SIZE_SELECTED(tCountLast))
					{
						deltaT = tickNow - tCountLast;
						deltaT *= 1e-6;
						deltaTAverageBuf2Buf = JVX_ALPHA_SMOOTH * deltaTAverageBuf2Buf + (1 - JVX_ALPHA_SMOOTH) * deltaT;
					}
					tCountLast = tickNow;
					frame_available_cb(texture_id, true);

					jvxTick tickNowWait = JVX_GET_TICKCOUNT_US_GET_CURRENT(&tStamp);
					deltaT = tickNowWait - tickNowConvert;
					deltaT *= 1e-6;
					deltaTAverageWait = JVX_ALPHA_SMOOTH * deltaTAverageWait + (1 - JVX_ALPHA_SMOOTH) * deltaT;

				}
			}
		}
	}
}

jvxErrorType CjvxPixBufRendering::stopped(jvxInt64 timestamp_us)
{
	return JVX_NO_ERROR;
}

// ==========================================================================================================================
jvxErrorType CjvxPixBufRendering::static_report_bufferswitch_trigger(jvxHandle* priv)
{
	jvxErrorType res = JVX_ERROR_INVALID_ARGUMENT;
	CjvxPixBufRendering* tdg = (CjvxPixBufRendering*)priv;
	if (tdg)
	{
		res = tdg->report_bufferswitch_trigger();
	}
	return res;
}

jvxErrorType
CjvxPixBufRendering::report_bufferswitch_trigger()
{
	// Hint: we hold the property lock here. We need to hold it to not remove the callback while calling.
	// In the callback, we will render a buffer and finally report to the UI. The UI protects itself with a log.
	// If UI requests a property and the lock is in the video thread, the UI lock will wait for the propert lock
	// and the property lock will wait for the UI lock -> DEADLOCK
	// Therefore, the buffer callback needs to render the video via another thread!

	// This would be a direct call. However, this is kind of dangerous and may be subject to a runtime race condition
	// buffer_proc_core(); <- this option may yield a deadlock!!!

	// Relaxed version: postpone towards a bgrd thread 
	ref.cpPtr->trigger_wakeup();
	return JVX_NO_ERROR;
}
