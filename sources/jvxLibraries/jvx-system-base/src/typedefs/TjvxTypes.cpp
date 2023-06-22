#include <string>
#include "typedefs/TjvxTypes.h"
#include "typedefs/TjvxTypes_cpp.h"

jvxComponentClassAssociation theClassAssociation[JVX_COMPONENT_ALL_LIMIT+1] =
{
	/* JVX_COMPONENT_UNKNOWN */
	{jvxComponentTypeClass::JVX_COMPONENT_TYPE_NONE, JVX_COMPONENT_UNKNOWN, NULL, NULL, NULL, jvxComponentTypeClass::JVX_COMPONENT_TYPE_NONE},
	
	/* JVX_COMPONENT_SIGNAL_PROCESSING_TECHNOLOGY */
	{jvxComponentTypeClass::JVX_COMPONENT_TYPE_TECHNOLOGY, JVX_COMPONENT_SIGNAL_PROCESSING_DEVICE, 
		"Signal Processing Technologies", "sigproc", "Signal Processing Devices", jvxComponentTypeClass::JVX_COMPONENT_TYPE_DEVICE}, /*JVX_COMPONENT_SIGNAL_PROCESSING_TECHNOLOGY*/
	
		/* JVX_COMPONENT_SIGNAL_PROCESSING_DEVICE */
	{jvxComponentTypeClass::JVX_COMPONENT_TYPE_NONE, JVX_COMPONENT_UNKNOWN, NULL, NULL, NULL, jvxComponentTypeClass::JVX_COMPONENT_TYPE_NONE }, /*JVX_COMPONENT_SIGNAL_PROCESSING_DEVICE*/
	
	/* JVX_COMPONENT_SIGNAL_PROCESSING_NODE */
	{jvxComponentTypeClass::JVX_COMPONENT_TYPE_NODE, JVX_COMPONENT_UNKNOWN,
		"Signal Processing Nodes", "sigproc_node", NULL, jvxComponentTypeClass::JVX_COMPONENT_TYPE_NONE},/*JVX_COMPONENT_SIGNAL_PROCESSING_NODE*/
	
	/* JVX_COMPONENT_AUDIO_TECHNOLOGY */
	{jvxComponentTypeClass::JVX_COMPONENT_TYPE_TECHNOLOGY, JVX_COMPONENT_AUDIO_DEVICE,
		"Audio Technologies", "audio", "Audio Devices", jvxComponentTypeClass::JVX_COMPONENT_TYPE_DEVICE},/*JVX_COMPONENT_AUDIO_TECHNOLOGY*/
	
	/* JVX_COMPONENT_AUDIO_DEVICE */
	{ jvxComponentTypeClass::JVX_COMPONENT_TYPE_NONE, JVX_COMPONENT_UNKNOWN, NULL, NULL, NULL,
		jvxComponentTypeClass::JVX_COMPONENT_TYPE_NONE },/*JVX_COMPONENT_AUDIO_DEVICE*/
	/* JVX_COMPONENT_AUDIO_NODE */
	{jvxComponentTypeClass::JVX_COMPONENT_TYPE_NODE, JVX_COMPONENT_UNKNOWN,
		"Audio Nodes", "audio_node", NULL, jvxComponentTypeClass::JVX_COMPONENT_TYPE_NONE },/*JVX_COMPONENT_AUDIO_NODE*/
	
	/* JVX_COMPONENT_VIDEO_TECHNOLOGY */
	{ jvxComponentTypeClass::JVX_COMPONENT_TYPE_TECHNOLOGY, JVX_COMPONENT_VIDEO_DEVICE,
		"Video Technologies", "video", "Video Devices", jvxComponentTypeClass::JVX_COMPONENT_TYPE_DEVICE},/*JVX_COMPONENT_VIDEO_TECHNOLOGY*/
	
	/* JVX_COMPONENT_VIDEO_DEVICE */
	{ jvxComponentTypeClass::JVX_COMPONENT_TYPE_NONE, JVX_COMPONENT_UNKNOWN, NULL, NULL, NULL, jvxComponentTypeClass::JVX_COMPONENT_TYPE_NONE },/*JVX_COMPONENT_VIDEO_DEVICE*/
	
	/* JVX_COMPONENT_VIDEO_NODE */
	{ jvxComponentTypeClass::JVX_COMPONENT_TYPE_NODE, JVX_COMPONENT_UNKNOWN, "Video Nodes", "video_node", NULL , jvxComponentTypeClass::JVX_COMPONENT_TYPE_NONE},/*JVX_COMPONENT_VIDEO_NODE*/
	
	/* JVX_COMPONENT_APPLICATION_CONTROLLER_TECHNOLOGY */
	{ jvxComponentTypeClass::JVX_COMPONENT_TYPE_TECHNOLOGY, JVX_COMPONENT_APPLICATION_CONTROLLER_DEVICE,
		"Ex Contr Technologies", "ex_control", "Ex Contr Devices", jvxComponentTypeClass::JVX_COMPONENT_TYPE_DEVICE },/*JVX_COMPONENT_APPLICATION_CONTROLLER_TECHNOLOGY*/
	
	/* JVX_COMPONENT_APPLICATION_CONTROLLER_DEVICE */
	{ jvxComponentTypeClass::JVX_COMPONENT_TYPE_NONE, JVX_COMPONENT_UNKNOWN, NULL, NULL, NULL, jvxComponentTypeClass::JVX_COMPONENT_TYPE_NONE },/*JVX_COMPONENT_APPLICATION_CONTROLLER_DEVICE*/
	
	/* JVX_COMPONENT_APPLICATION_CONTROLLER_NODE */
	{ jvxComponentTypeClass::JVX_COMPONENT_TYPE_NODE, JVX_COMPONENT_UNKNOWN,
		"Ex Contr Nodes", "app_ctrl_node", NULL, jvxComponentTypeClass::JVX_COMPONENT_TYPE_NONE },/*JVX_COMPONENT_APPLICATION_CONTROLLER_NODE*/
	
	/* JVX_COMPONENT_CUSTOM_TECHNOLOGY */
	{ jvxComponentTypeClass::JVX_COMPONENT_TYPE_TECHNOLOGY, JVX_COMPONENT_CUSTOM_DEVICE,
		"Custom Technologies", "custom", "Custom Devices" , jvxComponentTypeClass::JVX_COMPONENT_TYPE_DEVICE},/*JVX_COMPONENT_CUSTOM_TECHNOLOGY*/
	
	/* JVX_COMPONENT_CUSTOM_DEVICE */
	{ jvxComponentTypeClass::JVX_COMPONENT_TYPE_NONE, JVX_COMPONENT_UNKNOWN, NULL, NULL, NULL, jvxComponentTypeClass::JVX_COMPONENT_TYPE_NONE },/*JVX_COMPONENT_CUSTOM_DEVICE*/
	
	/* JVX_COMPONENT_CUSTOM_NODE */
	{ jvxComponentTypeClass::JVX_COMPONENT_TYPE_NODE, JVX_COMPONENT_UNKNOWN, "Custom Nodes", "custom_node", NULL, jvxComponentTypeClass::JVX_COMPONENT_TYPE_NONE },/*JVX_COMPONENT_CUSTOM_NODE*/
	
	/* JVX_COMPONENT_PACKAGE */
	{ jvxComponentTypeClass::JVX_COMPONENT_TYPE_HOST, JVX_COMPONENT_UNKNOWN, NULL, NULL, NULL, jvxComponentTypeClass::JVX_COMPONENT_TYPE_NONE }, /*JVX_COMPONENT_MIN_HOST*/
	
	/* JVX_COMPONENT_MIN_HOST */
	{ jvxComponentTypeClass::JVX_COMPONENT_TYPE_HOST, JVX_COMPONENT_UNKNOWN, NULL, NULL, NULL, jvxComponentTypeClass::JVX_COMPONENT_TYPE_NONE }, /*JVX_COMPONENT_MIN_HOST*/
	
	/* JVX_COMPONENT_FACTORY_HOST */
	{ jvxComponentTypeClass::JVX_COMPONENT_TYPE_HOST, JVX_COMPONENT_UNKNOWN, NULL, NULL, NULL, jvxComponentTypeClass::JVX_COMPONENT_TYPE_NONE }, /*JVX_COMPONENT_FACTORY_HOST*/
	
	/* JVX_COMPONENT_HOST */
	{ jvxComponentTypeClass::JVX_COMPONENT_TYPE_HOST, JVX_COMPONENT_UNKNOWN, NULL, NULL, NULL, jvxComponentTypeClass::JVX_COMPONENT_TYPE_NONE }, /*JVX_COMPONENT_HOST*/
	
	/* JVX_COMPONENT_EVENTLOOP */
	{ jvxComponentTypeClass::JVX_COMPONENT_TYPE_TOOL, JVX_COMPONENT_UNKNOWN, NULL, NULL, NULL, jvxComponentTypeClass::JVX_COMPONENT_TYPE_NONE }, /*JVX_COMPONENT_EVENTLOOP*/
	
	/* JVX_COMPONENT_DATALOGGER */
	{ jvxComponentTypeClass::JVX_COMPONENT_TYPE_TOOL, JVX_COMPONENT_UNKNOWN, NULL, NULL, NULL, jvxComponentTypeClass::JVX_COMPONENT_TYPE_NONE }, /*JVX_COMPONENT_DATALOGGER*/
		
	/* JVX_COMPONENT_DATALOGREADER */
	{ jvxComponentTypeClass::JVX_COMPONENT_TYPE_TOOL, JVX_COMPONENT_UNKNOWN, NULL, NULL, NULL, jvxComponentTypeClass::JVX_COMPONENT_TYPE_NONE }, /*JVX_COMPONENT_DATALOGREADER*/
		
	/* JVX_COMPONENT_CONNECTION */
	{ jvxComponentTypeClass::JVX_COMPONENT_TYPE_TOOL, JVX_COMPONENT_UNKNOWN, NULL, NULL, NULL, jvxComponentTypeClass::JVX_COMPONENT_TYPE_NONE }, /*JVX_COMPONENT_CONNECTION*/
		
	/* JVX_COMPONENT_CONFIG_PROCESSOR */
	{ jvxComponentTypeClass::JVX_COMPONENT_TYPE_TOOL, JVX_COMPONENT_UNKNOWN, NULL, NULL, NULL, jvxComponentTypeClass::JVX_COMPONENT_TYPE_NONE }, /*JVX_COMPONENT_CONFIG_PROCESSOR*/
		
	/* JVX_COMPONENT_EXTERNAL_CALL */
	{ jvxComponentTypeClass::JVX_COMPONENT_TYPE_TOOL, JVX_COMPONENT_UNKNOWN, NULL, NULL, NULL, jvxComponentTypeClass::JVX_COMPONENT_TYPE_NONE }, /*JVX_COMPONENT_EXTERNAL_CALL*/
		
	/* JVX_COMPONENT_THREADCONTROLLER */
	{ jvxComponentTypeClass::JVX_COMPONENT_TYPE_TOOL, JVX_COMPONENT_UNKNOWN, NULL, NULL, NULL, jvxComponentTypeClass::JVX_COMPONENT_TYPE_NONE }, /*JVX_COMPONENT_THREADCONTROLLER*/
		
	/* JVX_COMPONENT_THREADS */
	{ jvxComponentTypeClass::JVX_COMPONENT_TYPE_TOOL, JVX_COMPONENT_UNKNOWN, NULL, NULL, NULL, jvxComponentTypeClass::JVX_COMPONENT_TYPE_NONE }, /*JVX_COMPONENT_THREADS*/
		
	/* JVX_COMPONENT_SHARED_MEMORY */
	{ jvxComponentTypeClass::JVX_COMPONENT_TYPE_TOOL, JVX_COMPONENT_UNKNOWN, NULL, NULL, NULL, jvxComponentTypeClass::JVX_COMPONENT_TYPE_NONE }, /*JVX_COMPONENT_SHARED_MEMORY*/
		
	/* JVX_COMPONENT_TEXT2SPEECH */
	{ jvxComponentTypeClass::JVX_COMPONENT_TYPE_TOOL, JVX_COMPONENT_UNKNOWN, NULL, NULL, NULL, jvxComponentTypeClass::JVX_COMPONENT_TYPE_NONE }, /*JVX_COMPONENT_TEXT2SPEECH*/
		
	/* JVX_COMPONENT_SYSTEM_TEXT_LOG */
	{ jvxComponentTypeClass::JVX_COMPONENT_TYPE_TOOL, JVX_COMPONENT_UNKNOWN, NULL, NULL, NULL, jvxComponentTypeClass::JVX_COMPONENT_TYPE_NONE }, /*JVX_COMPONENT_SYSTEM_TEXT_LOG*/
		
	/* JVX_COMPONENT_LOCAL_TEXT_LOG */
	{ jvxComponentTypeClass::JVX_COMPONENT_TYPE_TOOL, JVX_COMPONENT_UNKNOWN, NULL, NULL, NULL, jvxComponentTypeClass::JVX_COMPONENT_TYPE_NONE }, /*JVX_COMPONENT_LOCAL_TEXT_LOG*/
		
	/* JVX_COMPONENT_CRYPT */
	{ jvxComponentTypeClass::JVX_COMPONENT_TYPE_TOOL, JVX_COMPONENT_UNKNOWN, NULL, NULL, NULL, jvxComponentTypeClass::JVX_COMPONENT_TYPE_NONE }, /*JVX_COMPONENT_CRYPT*/
		
	/* JVX_COMPONENT_WEBSERVER */
	{ jvxComponentTypeClass::JVX_COMPONENT_TYPE_TOOL, JVX_COMPONENT_UNKNOWN, NULL, NULL, NULL, jvxComponentTypeClass::JVX_COMPONENT_TYPE_NONE }, /*JVX_COMPONENT_WEBSERVER*/
		
	/* JVX_COMPONENT_REMOTE_CALL */
	{ jvxComponentTypeClass::JVX_COMPONENT_TYPE_TOOL, JVX_COMPONENT_UNKNOWN, NULL, NULL, NULL, jvxComponentTypeClass::JVX_COMPONENT_TYPE_NONE }, /*JVX_COMPONENT_REMOTE_CALL*/
		
	/* JVX_COMPONENT_PACKETFILTER_RULE */
	{ jvxComponentTypeClass::JVX_COMPONENT_TYPE_TOOL, JVX_COMPONENT_UNKNOWN, NULL, NULL, NULL, jvxComponentTypeClass::JVX_COMPONENT_TYPE_NONE }, /*,JVX_COMPONENT_PACKETFILTER_RULE*/

	/* JVX_COMPONENT_LOGREMOTEHANDLER */
	{ jvxComponentTypeClass::JVX_COMPONENT_TYPE_TOOL, JVX_COMPONENT_UNKNOWN, NULL, NULL, NULL, jvxComponentTypeClass::JVX_COMPONENT_TYPE_NONE } /* JVX_COMPONENT_LOGREMOTEHANDLER */
				
#ifndef JVX_NO_SYSTEM_EXTENSIONS
#define JVX_INCLUDE_CLASSES
#include "typedefs/TpjvxTypes_components.h"
#include "typedefs/TpjvxTypes_tools.h"
#undef JVX_INCLUDE_COMPONENTS
#endif

	,{ jvxComponentTypeClass::JVX_COMPONENT_TYPE_PROCESS, JVX_COMPONENT_UNKNOWN, NULL, NULL, NULL, jvxComponentTypeClass::JVX_COMPONENT_TYPE_NONE } /*JVX_COMPONENT_PROCESSING_GROUP*/
	,{ jvxComponentTypeClass::JVX_COMPONENT_TYPE_PROCESS, JVX_COMPONENT_UNKNOWN, NULL, NULL, NULL, jvxComponentTypeClass::JVX_COMPONENT_TYPE_NONE } /*JVX_COMPONENT_PROCESSING_PROCESS*/
	,{ jvxComponentTypeClass::JVX_COMPONENT_TYPE_HOST, JVX_COMPONENT_UNKNOWN, NULL, NULL, NULL, jvxComponentTypeClass::JVX_COMPONENT_TYPE_NONE } /*JVX_COMPONENT_OFF_HOST*/
	,{ jvxComponentTypeClass::JVX_COMPONENT_TYPE_PROCESS, JVX_COMPONENT_UNKNOWN, NULL, NULL, NULL, jvxComponentTypeClass::JVX_COMPONENT_TYPE_NONE } /*JVX_COMPONENT_INTERCEPTOR*/
	,{ jvxComponentTypeClass::JVX_COMPONENT_TYPE_SIMPLE, JVX_COMPONENT_SYSTEM_AUTOMATION,
		"Automation", "automation", NULL, jvxComponentTypeClass::JVX_COMPONENT_TYPE_NONE }, /*JVX_COMPONENT_AUTOMATION*/

	// I added this entry here to verify that new components do not break the system!!
	{ jvxComponentTypeClass::JVX_COMPONENT_TYPE_LIMIT, JVX_COMPONENT_ALL_LIMIT, NULL, NULL, NULL, jvxComponentTypeClass::JVX_COMPONENT_TYPE_NONE }

};

jvxUInt16 jvx_txt2UInt16(const char* in, jvxCBool* err)
{
	char* cc = NULL;
	jvxUInt32 ret32 = 0;

	//ret = strtoul(in, &cc, 0);
	ret32 = strtoul(in, &cc, 0);

	intptr_t cc1 = (intptr_t)cc;
	intptr_t cc2 = (intptr_t)in + strlen(in);
	if (cc1 == cc2)
	{
		if (err)
			*err = false;
		return (ret32 & 0xFFFF);
	}
	if (err)
		*err = true;
	return 0;
}

jvxUInt64 jvx_txt2UInt64(const char* in, jvxCBool* err)
{
	char* cc = NULL;
	jvxUInt64 ret = 0;

	ret = strtoull(in, &cc, 0);
	//ret = _strtoui64(in, &cc, 0);

	intptr_t cc1 = (intptr_t)cc;
	intptr_t cc2 = (intptr_t)in + strlen(in);
	if (cc1 == cc2)
	{
		if (err)
			*err = false;
		return ret;
	}
	if (err)
		*err = true;
	return 0;
}

jvxData jvx_txt2Data(const char* in, jvxCBool* err)
{
	char* cc = NULL;
	jvxData ret = 0.0;

	ret = JVX_C_STR2DATA(in, &cc);

	intptr_t cc1 = (intptr_t)cc;
	intptr_t cc2 = (intptr_t)in + strlen(in);
	if (cc1 == cc2)
	{
		if (err)
			*err = c_false;
		return ret;
	}
	if (err)
		*err = c_true;
	return 0;
}

// =======================================================================================================

jvxBitField& 
jvxSelectionList_cpp::selection(jvxSize id)
{
	assert(id < num);
	return fld[id];
}

const jvxBitField& 
jvxSelectionList_cpp::selection(jvxSize id) const
{
	assert(id < num);
	return fld[id];
}

// =======================================================================================================

jvxBitField& 
jvxSelectionList::bitFieldSelected(jvxSize idx)
{
	assert(idx < num);
	return fld[idx];
}

const jvxBitField&
jvxSelectionList::bitFieldSelected(jvxSize idx) const
{
	assert(idx < num);
	return fld[idx];
}

// =======================================================================================================

jvxData& 
jvxValueInRange::val(jvxSize idx)
{
	assert(idx < num);
	return fld[idx];
}

const jvxData&
jvxValueInRange::val(jvxSize idx) const
{
	assert(idx < num);
	return fld[idx];
}

// std::list<jvxHandle*>* lstMem = nullptr;