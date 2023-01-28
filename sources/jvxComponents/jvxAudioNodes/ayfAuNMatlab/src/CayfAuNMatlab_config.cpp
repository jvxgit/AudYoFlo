#include "CayfAuNMatlab.h"

#ifdef JVX_PROJECT_NAMESPACE
namespace JVX_PROJECT_NAMESPACE {
#endif

jvxErrorType
CayfAuNMatlab::put_configuration(jvxCallManagerConfiguration* callConf,
	IjvxConfigProcessor* processor,
	jvxHandle* sectionToContainAllSubsectionsForMe, 
	const char* filename,
	jvxInt32 lineno)
{
	jvxSize i;
	jvxBool err = false;
	std::vector<std::string> warns;
	jvxCBitField prio;
	IjvxReport* theReport = NULL;
	jvxErrorType res = JVX_LOCAL_BASE_CLASS::put_configuration(callConf,
		processor,
		sectionToContainAllSubsectionsForMe,
		filename, lineno);

	if (res == JVX_NO_ERROR)
	{
#ifdef JVX_EXTERNAL_CALL_ENABLED
		res = CjvxMexCalls::put_configuration(callConf, processor, this->_common_set_min.theState, sectionToContainAllSubsectionsForMe, filename, lineno);
#endif

		if (_common_set_min.theState == JVX_STATE_ACTIVE)
		{
			
		}
	}
	return res;
}

jvxErrorType
CayfAuNMatlab::get_configuration(jvxCallManagerConfiguration* callConf, IjvxConfigProcessor* processor,
	jvxHandle* sectionWhereToAddAllSubsections)
{
	jvxSize i;
	std::vector<std::string> warns;
	jvxErrorType res = JVX_LOCAL_BASE_CLASS::get_configuration(callConf, processor,
		sectionWhereToAddAllSubsections);

	if (res == JVX_NO_ERROR)
	{
#ifdef JVX_EXTERNAL_CALL_ENABLED
		res = CjvxMexCalls::get_configuration(callConf, processor, sectionWhereToAddAllSubsections);
#endif		
	}
	return res;
}

#ifdef JVX_PROJECT_NAMESPACE
}
#endif
