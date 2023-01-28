#ifndef __JVX_AUDIO_STACK_SAMPLE_DISPENSER_COMMON_H__
#define __JVX_AUDIO_STACK_SAMPLE_DISPENSER_COMMON_H__

typedef jvxDspBaseErrorType(*jvxAudioStackDispenser_lock)(jvxHandle* prv);
typedef jvxDspBaseErrorType(*jvxAudioStackDispenser_unlock)(jvxHandle* prv);

typedef enum
{
	JVX_AUDIO_DISPENSER_STATE_OFF = 0,
	JVX_AUDIO_DISPENSER_STATE_CHARGING = 1,
	JVX_AUDIO_DISPENSER_STATE_CHARGING_HELD = 2,
	JVX_AUDIO_DISPENSER_STATE_OPERATION = 3,
	JVX_AUDIO_DISPENSER_STATE_UNCHARGING = 4
} jvxAudioDispenser_status;

#endif
