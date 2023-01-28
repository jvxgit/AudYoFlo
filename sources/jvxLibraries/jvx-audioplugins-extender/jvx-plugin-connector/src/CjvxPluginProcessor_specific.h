// Single include entry to pull in product specific modules

#include "jvxTDataConverter.h"
#include "jvxTResampler.h"
#include "jvxTrtAudioFileReader.h"
#include "jvxTrtAudioFileWriter.h"

#define JVX_PLUGIN_CONNECTOR_LOAD_MODULES \
	LOAD_ONE_MODULE_LIB_FULL(jvxTDataConverter_init, \
		jvxTDataConverter_terminate, "Data Converter", \
		addedStaticObjects, \
		facHost); \
	LOAD_ONE_MODULE_LIB_FULL(jvxTResampler_init, \
		jvxTResampler_terminate, "Resampler", \
		addedStaticObjects, \
		facHost); \
	LOAD_ONE_MODULE_LIB_FULL(jvxTrtAudioFileReader_init, \
		jvxTrtAudioFileReader_terminate, "RT Audio Reader", \
		addedStaticObjects, \
		facHost); \
	LOAD_ONE_MODULE_LIB_FULL(jvxTrtAudioFileWriter_init, \
		jvxTrtAudioFileWriter_terminate, "RT Audio Writer", \
		addedStaticObjects, \
		facHost);

