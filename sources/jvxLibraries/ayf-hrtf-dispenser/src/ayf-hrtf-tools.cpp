#include "ayf-hrtf-tools.h"

void
CayfConnectToHrtfDispenser::try_obtain_reference_hrtfdispenser(IjvxToolsHost* tHost, std::function<void(IjvxPropertyExtenderHrtfDispenser* pExt)> onPropObtained)
{
	if (propExtProvide == nullptr)
	{
		ptrNode = reqRefTool<IjvxNode>(tHost, tpNode, 0, nullptr, JVX_STATE_ACTIVE);
		if (ptrNode.objPtr)
		{
			IjvxPropertyExtender* propExt = nullptr;
			jvxApiString astr;
			ptrNode.objPtr->descriptor(nullptr, &astr); // <- get the subtype component descriptor
			if (astr.std_str() == "custom_node/hrtf_dispenser_node")
			{
				IjvxProperties* props = reqInterfaceObj<IjvxProperties>(ptrNode.objPtr);
				if (props)
				{
					jvx::propertyRawPointerType::CjvxRawPointerTypeExternalPointer<IjvxPropertyExtender> rPtr(&propExt);
					jvxErrorType resL = props->get_property(lval<jvxCallManagerProperties>(jvxCallManagerProperties()),
						rPtr, jPAD("/ext/ex_interface"), jPD(true));
					if (propExt)
					{
						// Here we obtain the property extender interface and the extender chain control interface
						propExtProvide = castPropIfExtender< IjvxPropertyExtenderHrtfDispenser>(propExt);

						if (propExtProvide && onPropObtained)
						{
							onPropObtained(propExtProvide);							
						}
					}
					retInterfaceObj<IjvxProperties>(ptrNode.objPtr, props);
					props = nullptr;
				}
			}
			retRefTool<IjvxNode>(tHost, ptrNode, tpNode.tp);
		}
	}
}

void
CayfConnectToHrtfDispenser::try_return_reference_hrtfdispenser(std::function<void()> onPropReturned)
{
	if (propExtProvide)
	{
		propExtProvide = nullptr;
		if (onPropReturned)
		{
			onPropReturned();
		}
	}
}

jvxErrorType 
CayfConnectToHrtfDispenser::supports_prop_extender_type(jvxPropertyExtenderType tp)
{
	switch (tp)
	{
	case jvxPropertyExtenderType::JVX_PROPERTY_EXTENDER_HRTF_DISPENSER:
		return JVX_NO_ERROR;
	default:
		break;
	}
	return JVX_ERROR_UNSUPPORTED;
}

jvxErrorType 
CayfConnectToHrtfDispenser::prop_extender_specialization(jvxHandle** prop_extender, jvxPropertyExtenderType tp)
{
	switch (tp)
	{
	case jvxPropertyExtenderType::JVX_PROPERTY_EXTENDER_HRTF_DISPENSER:
		JVX_PTR_SAFE_ASSIGN(prop_extender, propExtProvide);
		return JVX_NO_ERROR;
	default:
		break;
	}
	return JVX_ERROR_INVALID_SETTING;
}