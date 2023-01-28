
#include "jvx-helpers.h"

// ===================================================================
 void jvx_getReferenceComponentType(const jvxComponentIdentification& tp, IjvxObject*& theObj, bool addressesStandardComponent, IjvxHost* hostRef)
{
	jvxErrorType res = JVX_NO_ERROR;
	theObj = NULL;
	IjvxToolsHost* theToolsHost = NULL;
	switch(tp.tp)
	{
#include "codeFragments/components/Hjvx_caseStatement_maincomponents.h"
		res = hostRef->request_object_selected_component(tp, &theObj);
		break;
	case JVX_COMPONENT_DATALOGGER:

		if(!addressesStandardComponent)
		{
			res = hostRef->request_hidden_interface(JVX_INTERFACE_TOOLS_HOST, reinterpret_cast<jvxHandle**>(&theToolsHost));
			if((res == JVX_NO_ERROR) && theToolsHost)
			{
				theToolsHost->reference_tool(JVX_COMPONENT_DATALOGGER, &theObj, 0, NULL);
				hostRef->return_hidden_interface(JVX_INTERFACE_TOOLS_HOST, reinterpret_cast<jvxHandle*>(theToolsHost));
			}
		}
		break;
#include "codeFragments/components/Hjvx_caseStatement_othercomponents.h"
		break;
#include "codeFragments/components/Hjvx_caseStatement_systemcomponents.h"
		break;
	default:
		assert(0);
	}
}

void jvx_returnReferenceComponentType(const jvxComponentIdentification& tp, IjvxObject* theObj, bool addressesStandardComponent, IjvxHost* hostRef)
{
	jvxErrorType res = JVX_NO_ERROR;
	IjvxToolsHost* theToolsHost = NULL;

	switch(tp.tp)
	{
#include "codeFragments/components/Hjvx_caseStatement_maincomponents.h"
		res = hostRef->return_object_selected_component(tp, theObj);
		break;
	case JVX_COMPONENT_DATALOGGER:

		if(!addressesStandardComponent)
		{
			res = hostRef->request_hidden_interface(JVX_INTERFACE_TOOLS_HOST, reinterpret_cast<jvxHandle**>(&theToolsHost));
			if((res == JVX_NO_ERROR) && theToolsHost)
			{
				theToolsHost->return_reference_tool( JVX_COMPONENT_DATALOGGER, theObj);
				hostRef->return_hidden_interface(JVX_INTERFACE_TOOLS_HOST, reinterpret_cast<jvxHandle*>(theToolsHost));
			}
		}
		break;

#include "codeFragments/components/Hjvx_caseStatement_othercomponents.h"
		break;
	default:
		assert(0);
	}
}

/*
void jvx_requestConnectorFactory(IjvxDataProcessor* procRef, IjvxConnectorFactory** slvRefPtr, jvxComponentIdentification* tpRet)
{
	IjvxObject* myObj = NULL;
	IjvxNode* myNode = NULL;
	IjvxNode* myDev = NULL;
	jvxComponentIdentification tp = JVX_COMPONENT_UNKNOWN;
	procRef->reference_object(&myObj);
	if (myObj)
	{
		myObj->request_specialization(NULL, &tp, NULL, NULL);
		
		switch (tp.tp)
		{
#include "codeFragments/components/Hjvx_caseStatement_nodes.h"
			myObj->request_specialization(reinterpret_cast<jvxHandle**>(&myNode), NULL, NULL, NULL);
			if (myNode)
			{
				myNode->request_hidden_interface(JVX_INTERFACE_CONNECTOR_FACTORY, reinterpret_cast<jvxHandle**>(slvRefPtr));
			}
			break;
#include "codeFragments/components/Hjvx_caseStatement_devices.h"
			myObj->request_specialization(reinterpret_cast<jvxHandle**>(&myDev), NULL, NULL, NULL);
			if (myDev)
			{
				myDev->request_hidden_interface(JVX_INTERFACE_CONNECTOR_FACTORY, reinterpret_cast<jvxHandle**>(slvRefPtr));
			}
			break;
		}
		if (tpRet)
		{
			*tpRet = tp;
		}
	}
};

void jvx_returnConnectorFactory(IjvxDataProcessor* procRef, IjvxConnectorFactory* slvRef)
{
	IjvxObject* myObj = NULL;
	IjvxNode* myNode = NULL;
	IjvxNode* myDev = NULL;
	jvxComponentIdentification tp = JVX_COMPONENT_UNKNOWN;
	procRef->reference_object(&myObj);
	if (myObj)
	{
		myObj->request_specialization(NULL, &tp, NULL, NULL);
		switch (tp.tp)
		{
#include "codeFragments/components/Hjvx_caseStatement_nodes.h"
			myObj->request_specialization(reinterpret_cast<jvxHandle**>(&myNode), NULL, NULL, NULL);
			if (myNode)
			{
				myNode->return_hidden_interface(JVX_INTERFACE_CONNECTOR_FACTORY, reinterpret_cast<jvxHandle*>(slvRef));
			}
			break;
#include "codeFragments/components/Hjvx_caseStatement_devices.h"
			myObj->request_specialization(reinterpret_cast<jvxHandle**>(&myDev), NULL, NULL, NULL);
			if (myDev)
			{
				myDev->return_hidden_interface(JVX_INTERFACE_CONNECTOR_FACTORY, reinterpret_cast<jvxHandle*>(slvRef));
			}
			break;
		}
	}
};
*/