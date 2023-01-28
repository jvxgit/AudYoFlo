#ifndef _CJVXOBJECTMIN_H__
#define _CJVXOBJECTMIN_H__

#include "jvx.h"

class CjvxObjectMin : public IjvxProperties_report
{
protected:
	struct commonSetTypeMin
	{
		std::string theDescription = "no description set";
		jvxState theState = JVX_STATE_NONE;
		IjvxObject* theOwner = nullptr;
		IjvxHiddenInterface* theHostRef = nullptr;
	};
	commonSetTypeMin _common_set_min;
public:

	CjvxObjectMin(const std::string& theDescription = "no description");
	jvxErrorType _check_access(jvxCallManager& callGate);

	jvxErrorType _state(jvxState* stat);
	jvxErrorType _owner(IjvxObject** dependsOn);

	virtual jvxErrorType _initialize(IjvxHiddenInterface* hostRef = NULL);
	virtual jvxErrorType _terminate();

	virtual jvxErrorType _select(IjvxObject* dependsOn = NULL);
	virtual jvxErrorType _unselect();

	virtual jvxErrorType _activate();
	virtual jvxErrorType _deactivate();

	jvxErrorType _pre_check_activate();
	jvxErrorType _pre_check_deactivate();
	jvxErrorType _pre_check_unselect();

	virtual jvxErrorType JVX_CALLINGCONVENTION report_properties_modified(const char* props_set) override;
	// Interface for reports from the property reference
};

#endif
