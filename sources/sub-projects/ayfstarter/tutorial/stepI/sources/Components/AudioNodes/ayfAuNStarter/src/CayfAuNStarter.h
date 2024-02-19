#ifndef __CAYFSTARTER_H__
#define __CAYFSTARTER_H__

#include "jvx.h"

#ifdef AYF_INVOLVE_CHANNEL_REARRANGE
#include "jvxNodes/CjvxBareNode1ioRearrange.h"
#define AYF_AUDIO_NODE_BASE_CLASS CjvxBareNode1ioRearrange
#else
#include "jvxNodes/CjvxBareNode1io.h"
#define AYF_AUDIO_NODE_BASE_CLASS CjvxBareNode1io
#endif

class CayfAuNStarter: public AYF_AUDIO_NODE_BASE_CLASS
{	
public:
	JVX_CALLINGCONVENTION CayfAuNStarter(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);
	virtual JVX_CALLINGCONVENTION ~CayfAuNStarter();	
};

#endif
