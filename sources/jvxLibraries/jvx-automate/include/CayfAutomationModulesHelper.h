#ifndef __CAYFAUTOMATIONMODULESHELPER_H__
#define __CAYFAUTOMATIONMODULESHELPER_H__

#include "jvx.h"

namespace CayfAutomationModules
{
	class CayfAutomationModulesHelper
	{
	public:
		static std::string chainPrefixToChain(const std::string& nmChain, jvxComponentIdentification cpId);
	};
};

#endif
