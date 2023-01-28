#include "CjvxSpNSpeaker2Binaural.h"

jvxErrorType 
CjvxSpNSpeaker2Binaural::read_hrtf_settings(const std::string& token,
	const std::string& txt, const std::string& fName)
{
	jvxErrorType res = JVX_NO_ERROR;
	IjvxConfigProcessor* confProc = NULL;
	IjvxObject* cfgObj = NULL;

	if ((std::string)token == "binaural_render")
	{
		std::cout << __FUNCTION__ << ": Accepting configuration passed via chain." << std::endl;
		_common_set.theToolsHost->reference_tool(JVX_COMPONENT_CONFIG_PROCESSOR, &cfgObj, 0, NULL);
		if (cfgObj)
		{
			cfgObj->request_specialization(reinterpret_cast<jvxHandle**>(&confProc), NULL, NULL);
			if (confProc)
			{
				filtLeft.clear_config();
				filtRight.clear_config();

				// Inform about invalid functioin, do bypass
				genSpeaker2Binaural_node::config.bypass.value = c_true;
				genSpeaker2Binaural_node::mode.involved.value = c_false;
				update_bypass_mode(false);
				update_properties(false);

				genSpeaker2Binaural_node::monitor.last_error.value.clear();

				res = confProc->parseTextField(txt.c_str(), fName.c_str(), 0);

				if (res == JVX_NO_ERROR)
				{
					jvxConfigData* cfgData = NULL;
					confProc->getConfigurationHandle(&cfgData);
					if (cfgData)
					{
						jvxApiString astr;
						confProc->getNameCurrentEntry(cfgData, &astr);

						if (astr.std_str() == "binaural_render")
						{
							std::string err;

							res = filtLeft.allocate_config(confProc, cfgData,
								"hrtfs_left", err);
							if (res == JVX_NO_ERROR)
							{
								res = filtRight.allocate_config(confProc, cfgData,
									"hrtfs_right", err);

								if (res == JVX_NO_ERROR)
								{
									std::cout << __FUNCTION__ << ": Configuration successfully processed." << std::endl;
									genSpeaker2Binaural_node::monitor.last_error.value.clear();

									genSpeaker2Binaural_node::config.bypass.value = c_false;
									genSpeaker2Binaural_node::mode.involved.value = c_true;
									update_bypass_mode(false);
									update_properties(true);
								}
								else
								{
									std::cout << __FUNCTION__ << ": Configuration <hrtfs_right> unsuccessfully, reason: " << err << "." << std::endl;
									genSpeaker2Binaural_node::monitor.last_error.value = err;
								}
							}
							else
							{
								std::cout << __FUNCTION__ << ": Configuration <hrtfs_left> unsuccessfully, reason: " << err << "." << std::endl;
								genSpeaker2Binaural_node::monitor.last_error.value = err;
							}
						}
						confProc->removeHandle(cfgData);
					}
				}
				else
				{
					jvxApiError err;
					confProc->getParseError(&err);
					std::cout << __FUNCTION__ << ": In file " << fName << ": parse error: " << err.errorDescription.std_str() << std::endl;
					genSpeaker2Binaural_node::monitor.last_error.value = err.errorDescription.std_str();
					res = JVX_ERROR_INVALID_ARGUMENT;
				}


				_common_set.theToolsHost->return_reference_tool(JVX_COMPONENT_CONFIG_PROCESSOR, cfgObj);
			}
		}
	}
	return res;
}