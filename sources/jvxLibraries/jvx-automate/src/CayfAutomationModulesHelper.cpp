#include "CayfAutomationModulesHelper.h"

namespace CayfAutomationModules
{
	std::string 
	 CayfAutomationModulesHelper::chainPrefixToChain(const std::string& nmChain, jvxComponentIdentification cpId)
	 {
		 return nmChain + "[" + jvxComponentIdentification_txt(cpId) + "]";
 	 };
};
