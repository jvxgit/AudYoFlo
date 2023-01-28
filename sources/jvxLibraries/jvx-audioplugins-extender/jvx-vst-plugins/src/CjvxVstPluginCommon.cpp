#include "CjvxVstPluginCommon.h"

std::string 
CjvxVstPluginCommon::readConfiguration(Steinberg::IBStreamer& streamer, jvxBool& err)
{
	std::string txtIn;
	std::string txtExpect = "jvxrt-audioplugin";
	char* ptrRead = nullptr;
	err = true;

	JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(ptrRead, char, (txtExpect.size()+1)); // <- do not expect "0" termination
	if (streamer.readString8(ptrRead, txtExpect.size()+1) == txtExpect.size()) // <- no "0" termination
	{
		std::string txtRead;
		txtRead.assign(ptrRead, txtExpect.size()); // No "0" termination !
		if (txtExpect == txtRead)
		{
			Steinberg::int64 szTxt = 0;
			if (streamer.readInt64(szTxt))
			{				
				char* ptrCfg = nullptr;
				JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(ptrCfg, char, szTxt);
				streamer.readRaw(ptrCfg, szTxt);
				txtIn.assign(ptrCfg, szTxt); // <- Text is not "0" terminated
				JVX_DSP_SAFE_DELETE_FIELD(ptrCfg);

				err = false;
			}
		}
	}
	JVX_DSP_SAFE_DELETE_FIELD(ptrRead);
	return txtIn;
};

void
CjvxVstPluginCommon::writeConfiguration(Steinberg::IBStreamer& streamer, const std::string& txtConfig)
{
	Steinberg::int64 sz = txtConfig.size(); // <- Terminating "0" not in stream
	streamer.writeString8("jvxrt-audioplugin", true); // <- with termination!
	streamer.writeInt64(sz);
	const char* ptr = txtConfig.c_str();
	streamer.writeRaw(ptr, sz);
}
