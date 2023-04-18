#include "jvxHosts/CjvxDataConnectionsGroup.h"
#include "common/CjvxConnectorMasterFactory.h"

// =====================================================================================================
// =====================================================================================================

CjvxDataConnectionsGroup_external::CjvxDataConnectionsGroup_external()
{
	refObject = NULL;
}

CjvxDataConnectionsGroup_external::~CjvxDataConnectionsGroup_external()
{
}

jvxErrorType 
CjvxDataConnectionsGroup_external::set_object_ref(IjvxObject* objRefPass)
{
	refObject = objRefPass;
	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxDataConnectionsGroup_external::add_outside_input_connector(IjvxInputConnector* icon)
{
	auto elm = input_connectors.begin();
	elm = input_connectors.find(icon);
	if (elm == input_connectors.end())
	{
		oneIOConnector<IjvxInputConnector> elmI;
		elmI.iocon = icon;
		input_connectors[icon] = elmI;
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_DUPLICATE_ENTRY;
}

jvxErrorType 
CjvxDataConnectionsGroup_external::add_outside_output_connector(IjvxOutputConnector* ocon)
{
	auto elm = output_connectors.begin();
	elm = output_connectors.find(ocon);
	if (elm == output_connectors.end())
	{
		oneIOConnector<IjvxOutputConnector> elmO;
		elmO.iocon = ocon;
		output_connectors[ocon] = elmO;
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_DUPLICATE_ENTRY;
}

jvxErrorType 
CjvxDataConnectionsGroup_external::rem_outside_input_connector(IjvxInputConnector* icon)
{
	auto elm = input_connectors.begin();
	elm = input_connectors.find(icon);
	if (elm != input_connectors.end())
	{
		assert(elm->second.refCnt == 0);
		input_connectors.erase(icon);
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_ELEMENT_NOT_FOUND;
}

jvxErrorType 
CjvxDataConnectionsGroup_external::rem_outside_output_connector(IjvxOutputConnector* ocon)
{
	auto elm = output_connectors.begin();
	elm = output_connectors.find(ocon);
	if (elm != output_connectors.end())
	{
		assert(elm->second.refCnt == 0);
		output_connectors.erase(ocon);
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_ELEMENT_NOT_FOUND;
}

jvxErrorType 
CjvxDataConnectionsGroup_external::request_reference_object(IjvxObject** obj)
{
	if (obj)
	{
		*obj = refObject;
	}
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxDataConnectionsGroup_external::return_reference_object(IjvxObject* obj)
{
	return JVX_NO_ERROR;
}

// ===================================================================================================
jvxErrorType 
CjvxDataConnectionsGroup_external::number_input_connectors(jvxSize* num_in_connectors)
{
	if (num_in_connectors)
	{
		*num_in_connectors = input_connectors.size();
	}
	return JVX_NO_ERROR;

}

jvxErrorType 
CjvxDataConnectionsGroup_external::reference_input_connector(jvxSize idx, IjvxInputConnectorSelect** ref)
{
	if (idx < input_connectors.size())
	{
		auto elm = input_connectors.begin();
		std::advance(elm, idx);
		if (ref)
		{
			*ref = elm->second.iocon;
		}
		elm->second.refCnt++;
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_ID_OUT_OF_BOUNDS;
}

jvxErrorType 
CjvxDataConnectionsGroup_external::return_reference_input_connector(IjvxInputConnectorSelect* ref)
{
	auto elm = input_connectors.begin();
	elm = input_connectors.find(ref);
	if(elm != input_connectors.end())
	{
		elm->second.refCnt--;
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_ELEMENT_NOT_FOUND;
}

jvxErrorType CjvxDataConnectionsGroup_external::number_output_connectors(jvxSize* num_out_connectors)
{
	if (num_out_connectors)
	{
		*num_out_connectors = output_connectors.size();
	}
	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxDataConnectionsGroup_external::reference_output_connector(jvxSize idx, IjvxOutputConnectorSelect** ref)
{
	if (idx < output_connectors.size())
	{
		auto elm = output_connectors.begin();
		std::advance(elm, idx);
		if (ref)
		{
			*ref = elm->second.iocon;
		}
		elm->second.refCnt++;
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_ID_OUT_OF_BOUNDS;
}

jvxErrorType 
CjvxDataConnectionsGroup_external::return_reference_output_connector(IjvxOutputConnectorSelect* ref)
{
	auto elm = output_connectors.begin();
	elm = output_connectors.find(ref);
	if (elm != output_connectors.end())
	{
		elm->second.refCnt--;
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_ELEMENT_NOT_FOUND;
}

