#ifndef __JVX_AUDIO_STACK_LIBRARY_H__
#define __JVX_AUDIO_STACK_LIBRARY_H__

#include "jvx_dsp_base.h"
#include "jvx_dataformats.h"
#include "typedefs/network/TjvxProtocol.h"


typedef jvxDspBaseErrorType (*jvxAudioStack_send)(jvxHandle* prv, jvxByte* fldTransfer, jvxSize fldSize);
typedef jvxDspBaseErrorType(*jvxAudioStack_allocate)(jvxHandle* prv, jvxByte** bufStart, jvxSize* size_buf, jvxSize* offset_buf, jvxSize* minsize_buf, jvxSize minSizeDesired);
typedef jvxDspBaseErrorType(*jvxAudioStack_deallocate)(jvxHandle* prv, jvxByte* bufStart);
typedef jvxDspBaseErrorType(*jvxAudioStack_mode)(jvxHandle* prv, jvxSize* useMode);
typedef jvxDspBaseErrorType(*jvxAudioStack_simple)(jvxHandle* prv);
typedef jvxDspBaseErrorType(*jvxAudioStack_data)(jvxHandle* prv, jvxByte* fldTransfer, jvxSize fldSize, jvxUInt64 channel_mask, jvxUInt32 seq_id);

typedef enum
{
	JVX_AUDIO_STACK_SLAVE = 0,
	JVX_AUDIO_STACK_MASTER = 1
} jvxAudioStackOperation;

typedef enum
{
	JVX_AUDIOSTACK_STATE_UNKNOWN = 0,
	JVX_AUDIOSTACK_STATE_NONE = 2,
	JVX_AUDIOSTACK_STATE_INIT = 3,
	JVX_AUDIOSTACK_STATE_STATUS = 4,
	
	JVX_AUDIOSTACK_STATE_CONNECTION_INIT = 5,
	JVX_AUDIOSTACK_STATE_CONNECTION_DESCR = 6,
	JVX_AUDIOSTACK_STATE_CONNECTION_ROLE = 7,
	JVX_AUDIOSTACK_STATE_CONNECTION_MODE = 8,
	
	JVX_AUDIOSTACK_STATE_ACTIVE_INIT = 9,
	JVX_AUDIOSTACK_STATE_ACTIVE_SRATE = 10,
	JVX_AUDIOSTACK_STATE_ACTIVE_BSIZE = 11,
	JVX_AUDIOSTACK_STATE_ACTIVE_FORMAT = 12,
	JVX_AUDIOSTACK_STATE_ACTIVE_INCHANS = 13,
	JVX_AUDIOSTACK_STATE_ACTIVE_OUTCHANS = 14,
	JVX_AUDIOSTACK_STATE_ACTIVE_FRAMETYPE = 15,
	
	JVX_AUDIOSTACK_STATE_AWAIT_START = 16,
	
	JVX_AUDIOSTACK_STATE_PROCESSING_WAIT_RESPONSE = 17,
	JVX_AUDIOSTACK_STATE_PROCESSING = 18

} jvxAudioStack_state;

typedef struct
{
	char description[JVX_ADV_DECRIPTION_LENGTH];
	jvxAdvLinkProtocolClockTriggerRole trigRole;

	jvxSize mode;
	jvxSize srate;
	jvxSize bsize;
	jvxDataFormat format;
	jvxSize num_channels_to_external;
	jvxSize num_channels_from_external;
	jvxAdvLinkProtocolChannelFrameType frametype;

} jvxAudioDataParams_slave;

typedef struct
{
    char description[JVX_ADV_DECRIPTION_LENGTH];
    jvxAdvLinkProtocolClockTriggerRole trigRole;

    jvxSize mode;
    jvxSize srate;
    jvxSize bsize;
    jvxDataFormat format;
    jvxSize num_channels_slave_to_master;
    jvxSize num_channels_master_to_slave;
    jvxAdvLinkProtocolChannelFrameType frametype;

} jvxAudioDataParams_master;
// ======================================================================================

typedef struct 
{
	jvxAudioStackOperation operation;
	jvxCBool delayed_send;

	struct 
	{
		jvxHandle* prv_callbacks;

		jvxAudioStack_send send_callback;
		jvxAudioStack_allocate allocate_callback;
		jvxAudioStack_deallocate deallocate_callback;
		jvxAudioStack_simple reset_callback;
		jvxAudioStack_simple error_callback;
		jvxAudioStack_simple sendfail_callback;
		jvxAudioStack_simple started_callback;
		jvxAudioStack_simple stopped_callback;
		jvxAudioStack_data data_callback;
	} common;

	struct 
	{
		jvxUInt32 hw_mode;
	} master;

	struct 
	{
		jvxAudioDataParams_slave params;
		jvxAudioStack_mode mode_callback;
	} slave;
	jvxHandle* prv;
} jvxAudioStack;

// =========================================================================================

jvxDspBaseErrorType jvx_audio_stack_initCfg(jvxAudioStack* hdl_stack);

jvxDspBaseErrorType jvx_audio_stack_init(jvxAudioStack* hdl_stack);
jvxDspBaseErrorType jvx_audio_stack_terminate(jvxAudioStack* hdl_stack);

jvxDspBaseErrorType jvx_audio_stack_connect(jvxAudioStack* hdl_stack, jvxByte** send_ptr, jvxSize* send_sz);
jvxDspBaseErrorType jvx_audio_stack_disconnect(jvxAudioStack* hdl_stack);

jvxDspBaseErrorType jvx_audio_stack_packet_input(jvxAudioStack* hdl_stack, jvxByte* fld, jvxSize offset, jvxSize fldSize, jvxCBool* need_more_input, jvxByte** send_ptr, jvxSize* send_sz);
jvxDspBaseErrorType jvx_audio_stack_state(jvxAudioStack* hdl_stack, jvxAudioStack_state* theStat);
jvxDspBaseErrorType jvx_audio_stack_connected_params(jvxAudioStack* hdl_stack, jvxAudioDataParams_master* params);

jvxDspBaseErrorType jvx_audio_stack_trigger_start(jvxAudioStack* hdl_stack, jvxByte** send_ptr, jvxSize* send_sz);
jvxDspBaseErrorType jvx_audio_stack_trigger_stop(jvxAudioStack* hdl_stack, jvxByte** send_ptr, jvxSize* send_sz);

#endif

