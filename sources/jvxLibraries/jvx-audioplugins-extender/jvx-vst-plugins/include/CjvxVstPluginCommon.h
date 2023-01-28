#ifndef __CJVXVSTPLUGINCOMMON_H__
#define __CJVXVSTPLUGINCOMMON_H__

#include "jvx.h"
#include "pluginterfaces/base/ibstream.h"
#include "base/source/fstreamer.h"

class CjvxVstPluginCommon
{
public:
	static std::string readConfiguration(Steinberg::IBStreamer& stream, jvxBool& err);
	static void writeConfiguration(Steinberg::IBStreamer& streamer, const std::string& txtConfig);
};

#endif