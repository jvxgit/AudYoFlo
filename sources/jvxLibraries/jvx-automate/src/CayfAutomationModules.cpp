#include "CayfAutomationModules.h"

namespace CayfAutomationModules
{
	CayfAutomationModulesCommon::CayfAutomationModulesCommon()
	{
	};

	jvxErrorType
		CayfAutomationModulesCommon::activate(IjvxReport* report,
			IjvxHost* host,
			jvxSize purpId,
			CjvxObjectLog* ptrLog)
	{
		reportRefPtr = report;
		refHostRefPtr = host;
		purposeId = purpId;
		objLogRefPtr = ptrLog;
		return JVX_NO_ERROR;
	}

	jvxErrorType CayfAutomationModulesCommon::deactivate()
	{
		reportRefPtr = nullptr;
		refHostRefPtr = nullptr;
		purposeId = JVX_SIZE_UNSELECTED;
		objLogRefPtr = nullptr;
		return JVX_NO_ERROR;
	}
};
