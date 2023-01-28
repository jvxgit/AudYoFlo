#ifndef __IJVXEXTERNALAUDIOCHANNELS_H__
#define __IJVXEXTERNALAUDIOCHANNELS_H__

JVX_INTERFACE IjvxExternalAudioChannels_data
{
public:
	virtual JVX_CALLINGCONVENTION ~IjvxExternalAudioChannels_data(){};

	virtual jvxErrorType JVX_CALLINGCONVENTION get_one_frame(jvxSize register_id, jvxHandle** fld) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION put_one_frame(jvxSize register_id, jvxHandle** fld) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION prepare(jvxBool is_input, jvxSize register_id, jvxSize framesize, jvxDataFormat format) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION start(jvxBool is_input, jvxSize register_id) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION stop(jvxBool is_input, jvxSize register_id) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION postprocess(jvxBool is_input, jvxSize register_id) = 0;
};


JVX_INTERFACE IjvxExternalAudioChannels
{
public:

	virtual JVX_CALLINGCONVENTION ~IjvxExternalAudioChannels(){};

	virtual jvxErrorType JVX_CALLINGCONVENTION register_one_set(jvxBool is_input, jvxSize num_channels, jvxInt32 sRate, jvxSize* register_id, IjvxExternalAudioChannels_data* ref, const char* description) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION unregister_one_set(jvxSize register_id) = 0;
};

#endif

