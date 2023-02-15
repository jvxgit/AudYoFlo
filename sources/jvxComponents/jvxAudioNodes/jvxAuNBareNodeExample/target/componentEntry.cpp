#include <vector>

#include "jvxNodes/CjvxBareNode1io.h"
#include "jvxNodes/CjvxCompNode1io.h"

#ifdef JVX_BARE_NODE
#define JVX_LOCAL_BASE_CLASS CjvxBareNode1io
#define COMPONENT_DESCRIPTION "JVX Bare Node 1io (skeleton - link template)" 
#else
#define JVX_LOCAL_BASE_CLASS CjvxCompNode1io
#define COMPONENT_DESCRIPTION "JVX Comp Node 1io (skeleton - link template)" 
#endif

// Make this mode an audio node
class CjvxAunDefaultT : public JVX_LOCAL_BASE_CLASS
{
public:
	JVX_CALLINGCONVENTION CjvxAunDefaultT(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE) :
		JVX_LOCAL_BASE_CLASS(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
	{
		_common_set.theComponentType.unselected(JVX_COMPONENT_AUDIO_NODE);
		_common_set.theComponentSubTypeDescriptor = "signal_processing_node/audio_node";
	}

	virtual JVX_CALLINGCONVENTION ~CjvxAunDefaultT()
	{
		
	}
};

// ===========================================================================

#define COMPONENT_TYPE CjvxAunDefaultT
/*#define _JVX_ALLOW_MULTIPLE_INSTANCES*/

#include "templates/targets/factory_IjvxObject.cpp"

