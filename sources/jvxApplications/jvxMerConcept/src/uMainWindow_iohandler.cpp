#include "uMainWindow.h"
#include <cassert>

jvxErrorType 
uMainWindow::startExternalChannels()
{
	jvxErrorType res = JVX_NO_ERROR;

	res = this->involvedComponents.theHost.hHost->request_object_component(JVX_COMPONENT_AUDIO_TECHNOLOGY, &externalChannels.theObject);
	if((res == JVX_NO_ERROR) && externalChannels.theObject)
	{
		externalChannels.theObject->request_specialization(reinterpret_cast<jvxHandle**>(&externalChannels.theTech), NULL, NULL);
		if((res == JVX_NO_ERROR) && externalChannels.theTech)
		{
			res = externalChannels.theTech->request_hidden_interface(JVX_INTERFACE_EXTERNAL_AUDIO_CHANNELS, reinterpret_cast<jvxHandle**>(&externalChannels.theChannels));
			if((res == JVX_NO_ERROR) && externalChannels.theChannels)
			{
				res = externalChannels.theChannels->register_one_set(true, JVX_STANDALONE_HOST_NUMBER_INPUT_CHANNELS, JVX_STANDALONE_HOST_SAMPLERATE, &externalChannels.inId, static_cast<IjvxExternalAudioChannels_data*>(this), "Input channels");
				res = externalChannels.theChannels->register_one_set(false, JVX_STANDALONE_HOST_NUMBER_OUTPUT_CHANNELS, JVX_STANDALONE_HOST_SAMPLERATE, &externalChannels.outId, static_cast<IjvxExternalAudioChannels_data*>(this), "Output channels");
			}			
		}
	}
	if(res == JVX_NO_ERROR)
	{
		myState = JVX_STANDALONE_HOST_STATE_WAITING;
	}
	else
	{
		res = JVX_ERROR_CALL_SUB_COMPONENT_FAILED;
	}
	return(res);
}

jvxErrorType 
uMainWindow::get_one_frame(jvxSize register_id, jvxHandle** fld)
{
	return(readwrite.theBuffers.read_frame(reinterpret_cast<jvxData**>(fld), readwrite.framesize_read));
}

jvxErrorType 
uMainWindow::put_one_frame(jvxSize register_id, jvxHandle** fld)
{
	return(readwrite.theBuffers.write_frame(reinterpret_cast<jvxData**>(fld), readwrite.framesize_write));
}

jvxErrorType
uMainWindow::prepare(jvxBool is_input, jvxSize register_id, jvxSize framesize, jvxDataFormat format)
{
	jvxErrorType res = JVX_NO_ERROR;
	assert(format == JVX_DATAFORMAT_DATA);

	newValue_level();
	if(is_input)
	{
		res = readwrite.theBuffers.prepare_read();
		readwrite.framesize_read = framesize;
	}
	else
	{
		res = readwrite.theBuffers.prepare_write(JVX_STANDALONE_HOST_NUMBER_INPUT_CHANNELS, JVX_STANDALONE_HOST_SAMPLERATE, JVX_STANDALONE_HOST_SEGMENT_LENGTH_SECONDS,"Recording");
		readwrite.framesize_write = framesize;
	}
	return(res);
}

jvxErrorType 
uMainWindow::start(jvxBool is_input, jvxSize register_id)
{
	return(JVX_NO_ERROR);
}

jvxErrorType 
uMainWindow::stop(jvxBool is_input, jvxSize register_id)
{
	return(JVX_NO_ERROR);
}

jvxErrorType 
uMainWindow::postprocess(jvxBool is_input, jvxSize register_id)
{
	jvxErrorType res = JVX_NO_ERROR;
	if(is_input)
	{
		res = readwrite.theBuffers.postprocess_read();
	}
	else
	{
		res = readwrite.theBuffers.postprocess_write();
	}
	return(res);
}
