#ifndef __CJVXGENERICCONNECTIONTECHNOLOGY_H__
#define __CJVXGENERICCONNECTIONTECHNOLOGY_H__

#include "jvx.h"

#include "common/CjvxProperties.h"
#include "common/CjvxTechnology.h"

class CjvxGenericConnectionDevice;

class CjvxGenericConnectionTechnology: public IjvxTechnology, public CjvxTechnology,
	public IjvxProperties, public CjvxProperties
{
protected:

	// Default case: RS 232
	std::string connectionIdenitificationToken = "jvxTRs232*";
	jvxSize numPortsPolled = JVX_SIZE_UNSELECTED;

	struct
	{
		IjvxToolsHost* theToolsHost;
		IjvxObject* theConnectionObj;
		IjvxConnection* theConnectionRef;
	} subcomponents;

public:

	// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = 
	// Component class interface member functions
	// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = 
	JVX_CALLINGCONVENTION CjvxGenericConnectionTechnology(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);

	virtual JVX_CALLINGCONVENTION ~CjvxGenericConnectionTechnology();
	
#include "codeFragments/simplify/jvxStateMachineTechnology_simplify.h"

#include "codeFragments/simplify/jvxTechnologies_simplify.h"
#include "codeFragments/simplify/jvxObjects_simplify.h"
#include "codeFragments/simplify/jvxSystemStatus_simplify.h"

#define JVX_INTERFACE_SUPPORT_PROPERTIES
#include "codeFragments/simplify/jvxHiddenInterface_simplify.h"
#undef JVX_INTERFACE_SUPPORT_PROPERTIES

#include "codeFragments/simplify/jvxProperties_simplify.h"
#include "codeFragments/simplify/jvxInterfaceReference_simplify.h"

// #include "codeFragments/simplify/jvxInterfaceReference_simplify.h"

	virtual CjvxGenericConnectionDevice* allocate_device(const std::string& pName, jvxSize id) = 0;

};

#endif
