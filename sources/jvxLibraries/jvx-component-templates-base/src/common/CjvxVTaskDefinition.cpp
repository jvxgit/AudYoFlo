#include "common/CjvxVTaskDefinition.h"
#include "jvxNodes/CjvxInputOutputConnector_Vtask.h"

CjvxVTaskDefinition::CjvxVTaskDefinition()
{
	name = "no_name_set";
	reportSelect = NULL;
	numMaxTasks = JVX_SIZE_UNSELECTED;
	numCurrentTasks = 0;
	idOffThisTasks = 0;
	withMaster = false;
};

jvxSize 
CjvxVTaskDefinition::number_connected_icon(jvxConnectorSelectType sel, jvxHandle* ctxt)
{
	jvxSize nn = 0;
	jvxSize subId = (int)reinterpret_cast<intptr_t>(ctxt);
	switch (sel)
	{
	case jvxConnectorSelectType::JVX_CONNECTOR_SELECT_CONNECTABLE:
		if (subId < icon.size())
		{
			auto elm = icon.begin();
			std::advance(elm, subId);
			auto pp = elm->con;
			jvxHandle* ctxtLoc = nullptr;
			auto ifPtr = pp->request_references_icon(&ctxtLoc);
			nn = ifPtr->number_connected_icon(sel, ctxtLoc);
			pp->return_references_icon(ifPtr, ctxtLoc);
		}
		break;

	case jvxConnectorSelectType::JVX_CONNECTOR_SELECT_CONNECTED:
	{
		auto elm = activeRuntimeTasks.begin();
		for (; elm != activeRuntimeTasks.end(); elm++)
		{
			if (subId < elm->second.icons.size())
			{
				auto elmI = elm->second.icons.begin();
				std::advance(elmI, subId);
				auto pp = elmI->con;
				jvxHandle* ctxtLoc = nullptr;
				auto ifPtr = pp->request_references_icon(&ctxtLoc);
				nn += ifPtr->number_connected_icon(sel, ctxtLoc);
				pp->return_references_icon(ifPtr, ctxtLoc);
			}
		}
	}
	break;

	default:
		assert(0);
	}
	return(nn);
}

IjvxInputConnector* 
CjvxVTaskDefinition::reference_connected_icon(jvxSize idx, jvxConnectorSelectType sel, jvxHandle* ctxt)
{
	IjvxInputConnector* retPtr = nullptr;
	jvxSize nn = 0;
	jvxSize subId = (int)reinterpret_cast<intptr_t>(ctxt);
	switch (sel)
	{
	case jvxConnectorSelectType::JVX_CONNECTOR_SELECT_CONNECTABLE:
		if (subId < icon.size())
		{
			auto elm = icon.begin();
			std::advance(elm, subId);
			auto pp = elm->con;
			jvxHandle* ctxtLoc = nullptr;
			auto ifPtr = pp->request_references_icon(&ctxtLoc);
			retPtr = ifPtr->reference_connected_icon(idx, sel, ctxtLoc);
			pp->return_references_icon(ifPtr, ctxtLoc);
		}
		break;

	case jvxConnectorSelectType::JVX_CONNECTOR_SELECT_CONNECTED:
	{
		auto elm = activeRuntimeTasks.begin();
		for (; elm != activeRuntimeTasks.end(); elm++)
		{
			if (subId < elm->second.icons.size())
			{
				auto elmI = elm->second.icons.begin();
				std::advance(elmI, subId);

				if (nn == idx)
				{
					auto pp = elmI->con;
					jvxHandle* ctxtLoc = nullptr;
					auto ifPtr = pp->request_references_icon(&ctxtLoc);
					retPtr = ifPtr->reference_connected_icon(0, sel, ctxtLoc);
					pp->return_references_icon(ifPtr, ctxtLoc);
					break;
				}
				nn++;
			}
		}
	}
	break;

	default:
		assert(0);
	}
	return(retPtr);
}

jvxErrorType
CjvxVTaskDefinition::return_connected_icon(IjvxInputConnector* iconArg, jvxConnectorSelectType sel, jvxHandle* ctxt)
{
	jvxErrorType res = JVX_ERROR_ELEMENT_NOT_FOUND;

	jvxSize nn = 0;
	jvxSize subId = (int)reinterpret_cast<intptr_t>(ctxt);
	switch (sel)
	{
	case jvxConnectorSelectType::JVX_CONNECTOR_SELECT_CONNECTABLE:
		if (subId < icon.size())
		{
			auto elm = icon.begin();
			std::advance(elm, subId);
			auto pp = elm->con;
			jvxHandle* ctxtLoc = nullptr;
			auto ifPtr = pp->request_references_icon(&ctxtLoc);
			res = ifPtr->return_connected_icon(iconArg, sel, ctxtLoc);
			pp->return_references_icon(ifPtr, ctxtLoc);
		}
		break;
	case jvxConnectorSelectType::JVX_CONNECTOR_SELECT_CONNECTED:
	{
		// Loop over active elements to find the USED connectors
		auto elm = activeRuntimeTasks.begin();
		for (; elm != activeRuntimeTasks.end(); elm++)
		{
			if (subId < elm->second.icons.size())
			{
				auto elmI = elm->second.icons.begin();
				std::advance(elmI, subId);

				auto pp = elmI->con;
				jvxHandle* ctxtLoc = nullptr;
				auto ifPtr = pp->request_references_icon(&ctxtLoc);
				res = ifPtr->return_connected_icon(iconArg, sel, ctxtLoc);
				pp->return_references_icon(ifPtr, ctxtLoc);
				if (res == JVX_NO_ERROR)
				{
					break;
				}
			}
		}
	}
	break;
	}
	return res;
}

