#ifndef __AYF_HRTF_TOOLS_H__
#define __AYF_HRTF_TOOLS_H__

#include "jvx.h"

class CayfConnectToHrtfDispenser: public IjvxPropertyExtender
{
public:
	jvxComponentIdentification tpNode = JVX_COMPONENT_CUSTOM_NODE;
	refComp <IjvxNode> ptrNode;
	IjvxPropertyExtenderHrtfDispenser* propExtProvide = nullptr;
	jvxErrorType supports_prop_extender_type(jvxPropertyExtenderType tp);
	jvxErrorType prop_extender_specialization(jvxHandle** prop_extender, jvxPropertyExtenderType tp);
	void try_obtain_reference_hrtfdispenser(IjvxToolsHost* tHost, std::function<void(IjvxPropertyExtenderHrtfDispenser* pExt)> onPropObtained = nullptr);
	void try_return_reference_hrtfdispenser(std::function<void()> onPropReturned = nullptr);
};

#endif