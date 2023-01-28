#ifndef __JVX_MULTICHANNEL_CIRCBUFFER_PLOT_RESAMPLER_H__
#define __JVX_MULTICHANNEL_CIRCBUFFER_PLOT_RESAMPLER_H__

#include "jvx.h"
#include "jvx_resampler/jvx_resampler_plotter.h"
#include "jvx_threads.h"
#include "typedefs/misc/TjvxCircBufferDataCollector.h"

struct privateVisualResampleStruct
{
	resampler_plotter_private** resamplersPlotter;
	jvxData resamplingFactor_spec;
	jvxData resamplingFactor_use;
	jvxData resamplingFactor_fac;
};

struct privateTriggeredBuffer_inband
{
	jvxState theStatus;
	jvxData triggerValue;
	jvxSize triggerLine;
	jvxBool onRisingEdge;
};


class CjvxMultiChannelCircBufferPlotResampler
{
public:
	typedef enum
	{
		JVX_CIRCBUFFER_MULTITHREAD = 0,
		JVX_CIRCBUFFER_SINGLETHRED = 1
	} jvxCircBufferThreadingMode;

protected:


	struct
	{

		privateVisualResampleStruct str_visualResampler;
		privateTriggeredBuffer_inband str_triggerInband;

		jvxSize num_channels;
		jvxSize buffersize;
		struct
		{
			jvxData** fld;
			jvxSize bLength;
			jvxSize fHeight;
			jvxSize idxRead;
			JVX_MUTEX_HANDLE safeAccess;
		} sec_buffers;

		jvxExternalBuffer* theEndpoint;

		struct
		{
			jvxSize numberLostSamples;
		} performance;

		jvxHandle* theResamplerThread;
		jvx_thread_handler my_callbacks;
		jvxState theState;

		jvxCircBufferDataCollectorMode theMode;
		jvxBitField featuresModes;
	} processing;

public:
	CjvxMultiChannelCircBufferPlotResampler();


	jvxErrorType init_circbuffer(jvxSize numChannels, jvxSize buffersize, jvxSize numBuffersSecBuffer, jvxBitField features);

	jvxErrorType set_endpoint_circbuffer(jvxExternalBuffer* endpoint);

	jvxErrorType start_circbuffer(jvxCircBufferThreadingMode threadMode, jvxBool boostPrio);

	jvxErrorType set_mode_circbuffer(jvxCircBufferDataCollectorMode theMode);

	jvxErrorType process_circbuffer(jvxData** samples, bool strictlyNoBlock = false);

	jvxErrorType stop_circbuffer();

	jvxErrorType terminate_circbuffer();

	jvxErrorType set_properties_circbuffer(jvxCircBufferDataCollectorMode collectorMode, jvxSize idxSet, jvxHandle* prop_val);

	jvxErrorType get_properties_circbuffer(jvxCircBufferDataCollectorMode collectorMode, jvxSize idxSet, jvxHandle* prop_val);

private:
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
