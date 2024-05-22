#include "CjvxSpNMixChainEnterLeave.h"

jvxErrorType
CjvxSpNMixChainEnterLeave::put_configuration(jvxCallManagerConfiguration* callMan,
	IjvxConfigProcessor* processor,
	jvxHandle* sectionToContainAllSubsectionsForMe,
	const char* filename,
	jvxInt32 lineno)
{
	jvxErrorType res = CjvxBareNode1ioRearrange::put_configuration(callMan,
		processor, sectionToContainAllSubsectionsForMe,
		filename, lineno);
	if (res == JVX_NO_ERROR)
	{
		if (_common_set_min.theState == JVX_STATE_ACTIVE)
		{
			jvxErrorType resL = HjvxConfigProcessor_readEntry_1dList<jvxSize>(processor, sectionToContainAllSubsectionsForMe, "JVX_CHANNEL_ROUTING", oldRouting);
		}
	}
	return res;
}

jvxErrorType 
CjvxSpNMixChainEnterLeave::get_configuration(jvxCallManagerConfiguration* callMan,
	IjvxConfigProcessor* processor,
	jvxHandle* sectionWhereToAddAllSubsections)
{
	jvxErrorType res = CjvxBareNode1ioRearrange::get_configuration(callMan,
		processor,sectionWhereToAddAllSubsections );
	if (res == JVX_NO_ERROR)
	{
		jvxConfigData* datSubSection = nullptr;
		// processor->getReferenceEntryCurrentSection_name(sectionWhereToAddAllSubsections, &datSubSection, "JVX_CHANNEL_ROUTING");
		HjvxConfigProcessor_writeEntry_1dList<jvxSize>(processor, sectionWhereToAddAllSubsections, "JVX_CHANNEL_ROUTING", oldRouting);
	}
	return res;
}
