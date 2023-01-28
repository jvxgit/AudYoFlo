#include "CjvxConsoleHost_be_drivehost.h"

#include "jvxTrtAudioFileReader.h"
#include "jvxTrtAudioFileWriter.h"
#include "jvxAuTGenericWrapper.h"
#include "jvxTDataConverter.h"
#include "jvxTResampler.h"

void
CjvxConsoleHost_be_drivehost::bootup_sequencer_config()
{
}

jvxErrorType
CjvxConsoleHost_be_drivehost::bootup_specific()
{
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;
	oneAddedStaticComponent comp;

	LOAD_ONE_MODULE_LIB_FULL(jvxTDataConverter_init, 
		jvxTDataConverter_terminate, 
		"Data Converter", 
		involvedComponents.addedStaticObjects, 
		involvedComponents.theHost.hFHost);
	LOAD_ONE_MODULE_LIB_FULL(jvxTResampler_init, 
		jvxTResampler_terminate, 
		"Resampler", 
		involvedComponents.addedStaticObjects, 
		involvedComponents.theHost.hFHost);
	LOAD_ONE_MODULE_LIB_FULL(jvxTrtAudioFileReader_init,
		jvxTrtAudioFileReader_terminate, 
		"RT Audio Reader", 
		involvedComponents.addedStaticObjects, 
		involvedComponents.theHost.hFHost);
	LOAD_ONE_MODULE_LIB_FULL(jvxTrtAudioFileWriter_init, 
		jvxTrtAudioFileWriter_terminate, 
		"RT Audio Writer", 
		involvedComponents.addedStaticObjects, 
		involvedComponents.theHost.hFHost);
	LOAD_ONE_MODULE_LIB_FULL(jvxAuTGenericWrapper_init, 
		jvxAuTGenericWrapper_terminate, 
		"Generic Wrapper", 
		involvedComponents.addedStaticObjects, 
		involvedComponents.theHost.hFHost);
	
	/*
	this->involvedComponents.theHost.hFHost->add_component_load_blacklist(JVX_COMPONENT_DATACONVERTER);
	this->involvedComponents.theHost.hFHost->add_component_load_blacklist(JVX_COMPONENT_DATACONVERTER);
	this->involvedComponents.theHost.hFHost->add_component_load_blacklist(JVX_COMPONENT_RT_AUDIO_FILE_READER);
	this->involvedComponents.theHost.hFHost->add_component_load_blacklist(JVX_COMPONENT_RT_AUDIO_FILE_WRITER);
	this->involvedComponents.theHost.hFHost->add_component_load_blacklist(JVX_COMPONENT_AUDIO_TECHNOLOGY);
	*/
	return(res);
}

jvxErrorType
CjvxConsoleHost_be_drivehost::postbootup_specific()
{	
	return(JVX_NO_ERROR);
}
