#include "CayfAutomationModulesSrc2SnkPreChain.h"
#include "CayfAutomationModulesSyncedIo.h"
#include "CayfAutomationModulesSrc2SnkPreChainPriChain.h"
#include "CayfAutomationModulesPrivate.h"

namespace CayfAutomationModules
{	
	template <typename T>
	T& castEstablishProcess(IayfEstablishedProcessesCommon* realizeChainPtr)
	{
		T* ptr = nullptr;
		ayfEstablishedProcessType tp = ayfEstablishedProcessType::AYF_ESTABLISHED_PROCESS_UNKNOWN;
		if (std::is_same<T, IayfEstablishedProcessesSrc2Snk>::value)
		{
			tp = ayfEstablishedProcessType::AYF_ESTABLISHED_PROCESS_SRC2SNK;
		}
		else if (std::is_same<T, IayfEstablishedProcessesSrc2SnkPreChain>::value)
		{
			tp = ayfEstablishedProcessType::AYF_ESTABLISHED_PROCESS_SRC2SNKPRECHAIN;
		}
		else if (std::is_same<T, IayfEstablishedProcessesSyncio>::value)
		{
			tp = ayfEstablishedProcessType::AYF_ESTABLISHED_PROCESS_SYNCIO;
		}
		else if (std::is_same < T, IayfEstablishedProcessesSrc2SnkPreChainPriChain>::value)
		{
			tp = ayfEstablishedProcessType::AYF_ESTABLISHED_PROCESS_SRC2SNKPRECHAINPRICHAIN;
		}
		else
		{
			assert(0);
		}

		ptr = (reinterpret_cast<T*>(realizeChainPtr->specificType(tp)));
		assert(ptr != nullptr);

		return *ptr;
	}
};