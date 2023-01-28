#ifndef __CJVXGENERICRS232TECHNOLOGY_H__
#define __CJVXGENERICRS232TECHNOLOGY_H__

#include "jvx.h"

#include "common/CjvxProperties.h"
#include "common/CjvxTechnology.h"

class CjvxGenericRS232Device;

class CjvxGenericRS232Technology: public IjvxTechnology, public CjvxTechnology
{
protected:

	struct
	{
		IjvxToolsHost* theToolsHost;
		IjvxObject* theRs232Obj;
		IjvxConnection* theRs232Ref;
	} subcomponents;

public:

	// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = 
	// Component class interface member functions
	// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = 
	JVX_CALLINGCONVENTION CjvxGenericRS232Technology(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);

	virtual JVX_CALLINGCONVENTION ~CjvxGenericRS232Technology();
	
#include "codeFragments/simplify/jvxStateMachineTechnology_simplify.h"

#include "codeFragments/simplify/jvxTechnologies_simplify.h"
#include "codeFragments/simplify/jvxObjects_simplify.h"
#include "codeFragments/simplify/jvxSystemStatus_simplify.h"

#define JVX_INTERFACE_SUPPORT_PROPERTIES
#define JVX_INTERFACE_SUPPORT_SEQUENCER_CONTROL
#include "codeFragments/simplify/jvxHiddenInterfaces_simplify.h"
#undef JVX_INTERFACE_SUPPORT_PROPERTIES
#undef JVX_INTERFACE_SUPPORT_SEQUENCER_CONTROL

// #include "codeFragments/simplify/jvxInterfaceReference_simplify.h"

	virtual CjvxGenericRS232Device* allocate_device(const std::string& pName, jvxSize id) = 0;
};

#endif
