#include "jvx.h"
#include "CayfAuNStarter.h"

CayfAuNStarter::CayfAuNStarter(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE):
	AYF_AUDIO_NODE_BASE_CLASS(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
{
	_common_set.theComponentType.unselected(JVX_COMPONENT_AUDIO_NODE);
	_common_set.theObjectSpecialization = reinterpret_cast<jvxHandle*>(static_cast<IjvxNode*>(this));
	_common_set.thisisme = static_cast<IjvxObject*>(this);	
}

CayfAuNStarter::~CayfAuNStarter()
{
}


