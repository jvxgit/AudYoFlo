#include "CjvxAuNBinauralRender.h"

jvxErrorType
CjvxAuNBinauralRender::put_configuration(jvxCallManagerConfiguration* callMan,
	IjvxConfigProcessor* processor,
	jvxHandle* sectionToContainAllSubsectionsForMe,
	const char* filename,
	jvxInt32 lineno)
{
	jvxSize i;
	std::vector<std::string> warns;
	jvxErrorType res = CjvxBareNode1ioRearrange::put_configuration(callMan,
		processor, sectionToContainAllSubsectionsForMe,
		filename, lineno);
	if (res == JVX_NO_ERROR)
	{

	}
	return res;
}

jvxErrorType
CjvxAuNBinauralRender::get_configuration(jvxCallManagerConfiguration* callMan,
	IjvxConfigProcessor* processor,
	jvxHandle* sectionWhereToAddAllSubsections)
{
	std::vector<std::string> warns;
	jvxErrorType res = CjvxBareNode1ioRearrange::get_configuration(callMan,
		processor, sectionWhereToAddAllSubsections);
	if (res == JVX_NO_ERROR)
	{

	}
	return res;
}