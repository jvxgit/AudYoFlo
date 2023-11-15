#include "jvx-helpers.h"

// ==================================================================
// Component Types
// ==================================================================

static jvxTextHelpers jvxComponentType_str_[JVX_COMPONENT_ALL_LIMIT] =
{
	{"unknown", "JVX_COMPONENT_UNKNOWN"},
	{"signal processing technology", "JVX_COMPONENT_SIGNAL_PROCESSING_TECHNOLOGY" },
	{"signal processing device", "JVX_COMPONENT_SIGNAL_PROCESSING_DEVICE" },
	{"signal processing node", "JVX_COMPONENT_SIGNAL_PROCESSING_NODE" },
	{"audio technology", "JVX_COMPONENT_AUDIO_TECHNOLOGY" },
	{"audio device", "JVX_COMPONENT_AUDIO_DEVICE" },
	{"audio node", "JVX_COMPONENT_AUDIO_NODE" },
	{"video technology", "JVX_COMPONENT_VIDEO_TECHNOLOGY" },
	{"video device", "JVX_COMPONENT_VIDEO_DEVICE" },
	{"video node", "JVX_COMPONENT_VIDEO_NODE" },
	{"application controller technology", "JVX_COMPONENT_APPLICATION_CONTROLLER_TECHNOLOGY" },
	{"application controller device", "JVX_COMPONENT_APPLICATION_CONTROLLER_DEVICE" },
	{"application controller node", "JVX_COMPONENT_APPLICATION_CONTROLLER_NODE" },
	{"custom technology", "JVX_COMPONENT_CUSTOM_TECHNOLOGY" },
	{"custom device", "JVX_COMPONENT_CUSTOM_DEVICE" },
	{"custom node", "JVX_COMPONENT_CUSTOM_NODE" },
	{"extern technology", "JVX_COMPONENT_EXTERNAL_TECHNOLOGY" },
	{"extern device", "JVX_COMPONENT_EXTERNAL_DEVICE" },
	{"extern node", "JVX_COMPONENT_EXTERNAL_NODE" },
	{"package", "JVX_COMPONENT_PACKAGE" },
	{"min host", "JVX_COMPONENT_MIN_HOST"},
	{"factory host", "JVX_COMPONENT_FACTORY_HOST" },
	{"host", "JVX_COMPONENT_HOST" },
	{"event loop", "JVX_COMPONENT_EVENTLOOP" },
	{"data logger", "JVX_COMPONENT_DATALOGGER" },
	{"data log reader", "JVX_COMPONENT_DATALOGREADER" },
	{"connection", "JVX_COMPONENT_CONNECTION" },
	{"config processor", "JVX_COMPONENT_CONFIG_PROCESSOR" },
	{"external call", "JVX_COMPONENT_EXTERNAL_CALL" },
	{"thread controller", "JVX_COMPONENT_THREADCONTROLLER" },
	{"threads", "JVX_COMPONENT_THREADS"},
	{"shared mem", "JVX_COMPONENT_SHARED_MEMORY"},
	{ "text2speech", "JVX_COMPONENT_TEXT2SPEECH" },
	{ "textlog", "JVX_COMPONENT_SYSTEM_TEXT_LOG" },
	{ "localtextlog", "JVX_COMPONENT_LOCAL_TEXT_LOG" },
	{"encryption/decryption", "JVX_COMPONENT_CRYPT" },
	{"web server", "JVX_COMPONENT_WEBSERVER" },
	{"remote call", "JVX_COMPONENT_REMOTE_CALL" },
	{"packet filter rule", "JVX_COMPONENT_PACKETFILTER_RULE" },
	{"log remote handler", "JVX_COMPONENT_LOGREMOTEHANDLER" }

#ifndef JVX_NO_SYSTEM_EXTENSIONS
#include "extensions/EpjvxTypes_components.h"
#include "extensions/EpjvxTypes_tools.h"
#endif

	, {"processing group", "JVX_COMPONENT_PROCESSING_GROUP"}
	, {"processing process", "JVX_COMPONENT_PROCESSING_PROCESS"}
	, {"off host", "JVX_COMPONENT_OFF_HOST"}
	, {"interceptor", "JVX_COMPONENT_INTERCEPTOR"}
	, {"automation", "JVX_COMPONENT_SYSTEM_AUTOMATION"}
};

// ===========================================================================

jvxTextHelpers* jvxComponentType_str()
{
	return jvxComponentType_str_;
}

const char* jvxComponentType_txt(jvxSize id)
{
	if(id < JVX_COMPONENT_ALL_LIMIT)
	{
		return(jvxComponentType_str_[id].friendly);
	}
	return("Unknown Component Type");
}

const char* jvxComponentType_txtf(jvxSize id)
{
	if (id < JVX_COMPONENT_ALL_LIMIT)
	{
		return(jvxComponentType_str_[id].full);
	}
	return("Unknown Component Type");
}

jvxErrorType jvxComponentType_decode(jvxComponentType* tp, const std::string & cpName)
{
	jvxSize i;
	if (tp)
		*tp = JVX_COMPONENT_UNKNOWN;

	for (i = 0; i < JVX_COMPONENT_ALL_LIMIT; i++)
	{
		if (
			(jvxComponentType_str_[i].friendly == cpName) ||
			(jvxComponentType_str_[i].full == cpName))
		{
			if (tp)
			{
				*tp = (jvxComponentType)i;
			}
			return JVX_NO_ERROR;
		}
	}
	return JVX_ERROR_ELEMENT_NOT_FOUND;
}

/*
JVX_COMPONENT_TYPE_NONE,
	JVX_COMPONENT_TYPE_TECHNOLOGY,
	JVX_COMPONENT_TYPE_NODE,
	JVX_COMPONENT_TYPE_HOST,
	JVX_COMPONENT_TYPE_CODEC,
	JVX_COMPONENT_TYPE_TOOL,

	JVX_COMPONENT_TYPE_SIMPLE,

	JVX_COMPONENT_TYPE_DEVICE,

	JVX_COMPONENT_TYPE_PROCESS,
*/
static jvxComponentTypeClass jvxComponentClassTypeAssoc[JVX_COMPONENT_ALL_LIMIT + 1] =
{	
	jvxComponentTypeClass::JVX_COMPONENT_TYPE_NONE, // JVX_COMPONENT_UNKNOWN

	jvxComponentTypeClass::JVX_COMPONENT_TYPE_TECHNOLOGY, // JVX_COMPONENT_SIGNAL_PROCESSING_TECHNOLOGY
	jvxComponentTypeClass::JVX_COMPONENT_TYPE_NONE, // JVX_COMPONENT_SIGNAL_PROCESSING_DEVICE
	jvxComponentTypeClass::JVX_COMPONENT_TYPE_NODE, // JVX_COMPONENT_SIGNAL_PROCESSING_NODE

	jvxComponentTypeClass::JVX_COMPONENT_TYPE_TECHNOLOGY, // JVX_COMPONENT_AUDIO_TECHNOLOGY
	jvxComponentTypeClass::JVX_COMPONENT_TYPE_NONE, // JVX_COMPONENT_AUDIO_DEVICE
	jvxComponentTypeClass::JVX_COMPONENT_TYPE_NODE, // JVX_COMPONENT_AUDIO_NODE

	jvxComponentTypeClass::JVX_COMPONENT_TYPE_TECHNOLOGY, // JVX_COMPONENT_VIDEO_TECHNOLOGY
	jvxComponentTypeClass::JVX_COMPONENT_TYPE_NONE, // JVX_COMPONENT_VIDEO_DEVICE
	jvxComponentTypeClass::JVX_COMPONENT_TYPE_NODE, // JVX_COMPONENT_VIDEO_NODE

	jvxComponentTypeClass::JVX_COMPONENT_TYPE_TECHNOLOGY, // JVX_COMPONENT_APPLICATION_CONTROLLER_TECHNOLOGY
	jvxComponentTypeClass::JVX_COMPONENT_TYPE_NONE, // JVX_COMPONENT_APPLICATION_CONTROLLER_DEVICE
	jvxComponentTypeClass::JVX_COMPONENT_TYPE_NODE, // JVX_COMPONENT_APPLICATION_CONTROLLER_NODE

	jvxComponentTypeClass::JVX_COMPONENT_TYPE_TECHNOLOGY, // JVX_COMPONENT_CUSTOM_TECHNOLOGY
	jvxComponentTypeClass::JVX_COMPONENT_TYPE_NONE, // JVX_COMPONENT_CUSTOM_DEVICE
	jvxComponentTypeClass::JVX_COMPONENT_TYPE_NODE, // JVX_COMPONENT_CUSTOM_NODE

	jvxComponentTypeClass::JVX_COMPONENT_TYPE_TECHNOLOGY, // JVX_COMPONENT_EXTERNAL_TECHNOLOGY
	jvxComponentTypeClass::JVX_COMPONENT_TYPE_NONE, // JVX_COMPONENT_EXTERNAL_DEVICE
	jvxComponentTypeClass::JVX_COMPONENT_TYPE_NODE, // JVX_COMPONENT_EXTERNAL_NODE

	jvxComponentTypeClass::JVX_COMPONENT_TYPE_PACKAGE, // JVX_COMPONENT_PACKAGE

	jvxComponentTypeClass::JVX_COMPONENT_TYPE_HOST, // JVX_COMPONENT_MIN_HOST
	jvxComponentTypeClass::JVX_COMPONENT_TYPE_HOST, // JVX_COMPONENT_FACTORY_HOST
	jvxComponentTypeClass::JVX_COMPONENT_TYPE_HOST, // JVX_COMPONENT_HOST
	
	jvxComponentTypeClass::JVX_COMPONENT_TYPE_TOOL, /* JVX_COMPONENT_EVENTLOOP */
	jvxComponentTypeClass::JVX_COMPONENT_TYPE_TOOL, /* JVX_COMPONENT_DATALOGGER */
	jvxComponentTypeClass::JVX_COMPONENT_TYPE_TOOL, /* JVX_COMPONENT_DATALOGREADER */
	jvxComponentTypeClass::JVX_COMPONENT_TYPE_TOOL, /* JVX_COMPONENT_CONNECTION */
	jvxComponentTypeClass::JVX_COMPONENT_TYPE_TOOL, /* JVX_COMPONENT_CONFIG_PROCESSOR */
	jvxComponentTypeClass::JVX_COMPONENT_TYPE_TOOL, /* JVX_COMPONENT_EXTERNAL_CALL */
	jvxComponentTypeClass::JVX_COMPONENT_TYPE_TOOL, /* JVX_COMPONENT_THREADCONTROLLER */
	jvxComponentTypeClass::JVX_COMPONENT_TYPE_TOOL, /* JVX_COMPONENT_THREADS */
	jvxComponentTypeClass::JVX_COMPONENT_TYPE_TOOL, /* JVX_COMPONENT_SHARED_MEMORY */
	jvxComponentTypeClass::JVX_COMPONENT_TYPE_TOOL, /* JVX_COMPONENT_TEXT2SPEECH */
	jvxComponentTypeClass::JVX_COMPONENT_TYPE_TOOL, /* JVX_COMPONENT_SYSTEM_TEXT_LOG */
	jvxComponentTypeClass::JVX_COMPONENT_TYPE_TOOL, /* JVX_COMPONENT_LOCAL_TEXT_LOG */
	jvxComponentTypeClass::JVX_COMPONENT_TYPE_TOOL, /* JVX_COMPONENT_CRYPT */
	jvxComponentTypeClass::JVX_COMPONENT_TYPE_TOOL, /* JVX_COMPONENT_WEBSERVER */
	jvxComponentTypeClass::JVX_COMPONENT_TYPE_TOOL, /* JVX_COMPONENT_REMOTE_CALL */
	jvxComponentTypeClass::JVX_COMPONENT_TYPE_TOOL, /* JVX_COMPONENT_PACKETFILTER_RULE */
	jvxComponentTypeClass::JVX_COMPONENT_TYPE_TOOL /* JVX_COMPONENT_LOGREMOTEHANDLER */

#ifndef JVX_NO_SYSTEM_EXTENSIONS
#define JVX_INCLUDE_CLASSE_ASSOCIATIONS
#include "typedefs/TpjvxTypes_components.h"
#include "typedefs/TpjvxTypes_tools.h"
#undef JVX_INCLUDE_CLASSE_ASSOCIATIONS
#endif

	, jvxComponentTypeClass::JVX_COMPONENT_TYPE_PROCESS, /*JVX_COMPONENT_PROCESSING_GROUP*/
	 jvxComponentTypeClass::JVX_COMPONENT_TYPE_PROCESS, /*JVX_COMPONENT_PROCESSING_PROCESS*/
	 jvxComponentTypeClass::JVX_COMPONENT_TYPE_HOST,  /*JVX_COMPONENT_OFF_HOST*/
	 jvxComponentTypeClass::JVX_COMPONENT_TYPE_PROCESS, /*JVX_COMPONENT_INTERCEPTOR*/
	 jvxComponentTypeClass::JVX_COMPONENT_TYPE_SIMPLE, /*JVX_COMPONENT_AUTOMATION*/
	jvxComponentTypeClass::JVX_COMPONENT_TYPE_LIMIT
};

jvxComponentTypeClass jvxComponentType_class(jvxComponentType tp)
{
	jvxComponentTypeClass retVal = jvxComponentTypeClass::JVX_COMPONENT_TYPE_LIMIT;

	assert(tp < JVX_COMPONENT_ALL_LIMIT+1);
	retVal = jvxComponentClassTypeAssoc[tp];
	return retVal;
}

// ==========================================================

static jvxTextHelpers jvxComponentTypeClass_str_[(int)jvxComponentTypeClass::JVX_COMPONENT_TYPE_LIMIT] =
{
	{ "none", "JVX_COMPONENT_TYPE_NONE" },
	{ "technologies", "JVX_COMPONENT_TYPE_TECHNOLOGY" },
	{ "nodes", "JVX_COMPONENT_TYPE_NODE" },
	{ "hosts", "JVX_COMPONENT_TYPE_HOST" },
	{ "codecs", "JVX_COMPONENT_TYPE_CODEC"},
	{ "tools", "JVX_COMPONENT_TYPE_TOOL" },
	{ "simple", "JVX_COMPONENT_TYPE_SIMPLE"},
	{ "device", "JVX_COMPONENT_TYPE_DEVICE"},
	{ "process", "JVX_COMPONENT_TYPE_PROCESS"},
	{ "process", "JVX_COMPONENT_TYPE_PACKAGE"}
};

jvxTextHelpers* jvxComponentTypeClass_str()
{
	return jvxComponentTypeClass_str_;
}

const char* jvxComponentTypeClass_txt(jvxComponentTypeClass id)
{
	assert((int)id < (int)jvxComponentTypeClass::JVX_COMPONENT_TYPE_LIMIT);
	return(jvxComponentTypeClass_str_[(int)id].friendly);
}

const char* jvxComponentTypeClass_txtf(jvxComponentTypeClass id)
{
	assert((int)id < (int)jvxComponentTypeClass::JVX_COMPONENT_TYPE_LIMIT);
	return(jvxComponentTypeClass_str_[(int)id].full);
}
