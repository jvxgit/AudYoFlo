#include "jvx.h"

jvxErrorType jvx_getReferencePropertiesObject(IjvxHost* theHost, jvx_propertyReferenceTriple* theTriple, const jvxComponentIdentification& tp)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxErrorType resL = JVX_NO_ERROR;
	jvx_initPropertyReferenceTriple(theTriple);

	theTriple->cpId = tp;
	res = theHost->request_object_selected_component(tp, &theTriple->theObj);
	if((res == JVX_NO_ERROR) && (theTriple->theObj))
	{
		theTriple->theObj->request_specialization(&theTriple->theHdl, NULL, NULL);
		if(theTriple->theHdl)
		{
			switch(tp.tp)
			{
			case JVX_COMPONENT_HOST:
				resL = ((IjvxHost*)theTriple->theHdl)->request_hidden_interface(JVX_INTERFACE_PROPERTIES, reinterpret_cast<jvxHandle**>(&theTriple->theProps));
				break;

#include "codeFragments/components/Hjvx_caseStatement_technologies.h"
				resL = ((IjvxTechnology*)theTriple->theHdl)->request_hidden_interface(JVX_INTERFACE_PROPERTIES, reinterpret_cast<jvxHandle**>(&theTriple->theProps));
				break;
#include "codeFragments/components/Hjvx_caseStatement_devices.h"
				resL = ((IjvxDevice*)theTriple->theHdl)->request_hidden_interface(JVX_INTERFACE_PROPERTIES, reinterpret_cast<jvxHandle**>(&theTriple->theProps));
				break;
#include "codeFragments/components/Hjvx_caseStatement_nodes.h"
				resL = ((IjvxNode*)theTriple->theHdl)->request_hidden_interface(JVX_INTERFACE_PROPERTIES, reinterpret_cast<jvxHandle**>(&theTriple->theProps));
				break;
			case JVX_COMPONENT_PROCESSING_PROCESS:
				resL = ((IjvxDataConnectionProcess*)theTriple->theHdl)->request_hidden_interface(JVX_INTERFACE_PROPERTIES, reinterpret_cast<jvxHandle**>(&theTriple->theProps));
				break;
			case JVX_COMPONENT_PROCESSING_GROUP:
				resL = ((IjvxDataConnectionGroup*)theTriple->theHdl)->request_hidden_interface(JVX_INTERFACE_PROPERTIES, reinterpret_cast<jvxHandle**>(&theTriple->theProps));
				break;
			case JVX_COMPONENT_SYSTEM_AUTOMATION:
				resL = ((IjvxSimpleComponent*)theTriple->theHdl)->request_hidden_interface(JVX_INTERFACE_PROPERTIES, reinterpret_cast<jvxHandle**>(&theTriple->theProps));
				break;
			default:
				resL = JVX_ERROR_ELEMENT_NOT_FOUND;
				break;
			}
		}
	}
	
	/*
	 * No, objets without property references should also be valid!
	 *

	if(res != JVX_NO_ERROR)
	{
		if(theTriple->theObj)
		{
			theHost->return_object_component(tp, theTriple->theObj);
			theTriple->theHdl = NULL;
			theTriple->theObj = NULL;
		}
	}
	*/

	return(res);
}

jvxErrorType jvx_returnReferencePropertiesObject(IjvxHost* theHost, jvx_propertyReferenceTriple* theTriple, const jvxComponentIdentification& tp)
{
	jvxErrorType res = JVX_ERROR_INVALID_ARGUMENT;
	if(theTriple->theObj)
	{
		if(theTriple->theHdl)
		{
			if(theTriple->theProps)
			{
				switch(tp.tp)
				{
				case JVX_COMPONENT_HOST:
					((IjvxHost*)theTriple->theHdl)->return_hidden_interface(JVX_INTERFACE_PROPERTIES, reinterpret_cast<jvxHandle*>(theTriple->theProps));
					res = JVX_NO_ERROR;
					break;
#include "codeFragments/components/Hjvx_caseStatement_technologies.h"
					((IjvxTechnology*)theTriple->theHdl)->return_hidden_interface(JVX_INTERFACE_PROPERTIES, reinterpret_cast<jvxHandle*>(theTriple->theProps));
					res = JVX_NO_ERROR;
					break;
#include "codeFragments/components/Hjvx_caseStatement_devices.h"
					((IjvxDevice*)theTriple->theHdl)->return_hidden_interface(JVX_INTERFACE_PROPERTIES, reinterpret_cast<jvxHandle*>(theTriple->theProps));
					res = JVX_NO_ERROR;
					break;
#include "codeFragments/components/Hjvx_caseStatement_nodes.h"
					((IjvxNode*)theTriple->theHdl)->return_hidden_interface(JVX_INTERFACE_PROPERTIES, reinterpret_cast<jvxHandle*>(theTriple->theProps));
					res = JVX_NO_ERROR;
					break;
				case JVX_COMPONENT_PROCESSING_PROCESS:
					res = ((IjvxDataConnectionProcess*)theTriple->theHdl)->return_hidden_interface(JVX_INTERFACE_PROPERTIES, reinterpret_cast<jvxHandle*>(theTriple->theProps));
					break;
				case JVX_COMPONENT_PROCESSING_GROUP:
					res = ((IjvxDataConnectionGroup*)theTriple->theHdl)->return_hidden_interface(JVX_INTERFACE_PROPERTIES, reinterpret_cast<jvxHandle*>(theTriple->theProps));
				break;				
				case JVX_COMPONENT_SYSTEM_AUTOMATION:
					res = ((IjvxSimpleComponent*)theTriple->theHdl)->return_hidden_interface(JVX_INTERFACE_PROPERTIES, reinterpret_cast<jvxHandle*>(theTriple->theProps));
					break;

				default:
					res = JVX_ERROR_ELEMENT_NOT_FOUND;
				}
			}
			theTriple->theProps = NULL;
		}
		theTriple->theHdl = NULL;
		theHost->return_object_selected_component(tp, theTriple->theObj);
	}
	theTriple->theObj = NULL;
	theTriple->cpId.reset();
	return(res);
}

void
jvx_findPropertyCategories_filter(std::vector<std::string>& propsCategory, IjvxAccessProperties* theProps, std::string filter)
{
	jvxSize i, j;
	jvxSize numP = 0;
	jvxCallManagerProperties callGate;
	theProps->get_number_properties(callGate, &numP);
	for (i = 0; i < numP; i++)
	{
		//jvxApiString fldStr;
		jvx::propertyDescriptor::CjvxPropertyDescriptorFull theDescr;
		jvx::propertyAddress::CjvxPropertyAddressLinear ident(i);
		theProps->get_descriptor_property(callGate, theDescr, ident);
		std::string descr = theDescr.descriptor.std_str();

		descr = jvx_pathExprFromFilePath(descr);
		if (!descr.empty())
		{
			jvxBool found = false;
			for (j = 0; j < propsCategory.size(); j++)
			{
				if (descr == propsCategory[j])
				{
					found = true;
					break;
				}
			}
			if (!found)
			{
				std::string filtertag = descr;
				size_t lastSlashPos = descr.rfind("/");
				if (lastSlashPos != std::string::npos)
				{
					filtertag = descr.substr(lastSlashPos, std::string::npos);
				}

				if (filtertag.find(filter) != std::string::npos)
				{
					propsCategory.push_back(descr);
				}
			}
		}

	}
}

void
jvx_findPropertyCategories(std::vector<std::string>& propsCategory, jvx_propertyReferenceTriple& theTriple, std::string filter)
{
	jvxSize i, j;
	jvxSize numP = 0;
	jvxCallManagerProperties callGate;
	jvx::propertyDescriptor::CjvxPropertyDescriptorFull theDescr;
	jvx::propertyAddress::CjvxPropertyAddressLinear ident(0);
	theTriple.theProps->number_properties(callGate, &numP);
	for (i = 0; i < numP; i++)
	{
		ident.idx = i;
		theTriple.theProps->description_property(callGate, theDescr, ident);

		std::string descr = theDescr.descriptor.std_str();
		descr = jvx_pathExprFromFilePath(descr);
		if (!descr.empty())
		{
			jvxBool found = false;
			for (j = 0; j < propsCategory.size(); j++)
			{
				if (descr == propsCategory[j])
				{
					found = true;
					break;
				}
			}
			if (!found)
			{
				propsCategory.push_back(descr);
			}
		}
	}
}
