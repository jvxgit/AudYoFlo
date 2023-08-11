#include "jvx.h"
#include "jvxSpNSpeakerEqualizer.h"

extern "C"
{
	jvxErrorType jvx_access_link_object_audionode(jvxInitObject_tp* funcInit, jvxTerminateObject_tp* funcTerm,
		jvxApiString* adescr)
	{
		adescr->assign("SpeakerEqualizer");
		*funcInit = jvxSpNSpeakerEqualizer_init;
		*funcTerm = jvxSpNSpeakerEqualizer_terminate;
		return(JVX_NO_ERROR);
	};
}

