#ifndef __CJVXAUDIOPWIRETECHNOLOGY_H__
#define __CJVXAUDIOPWIRETECHNOLOGY_H__

#include "jvxAudioTechnologies/CjvxMixedDevicesAudioTechnology.h"
#include "CjvxAudioPWireDevice.h" 
#include "pcg_exports_technology.h"

class CjvxAudioPWireTechnology: public CjvxMixedDevicesAudioTechnology<CjvxAudioPWireDevice>,
	public genPWire_technology
{
public:
	CjvxAudioPWireTechnology(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);

	jvxErrorType activate()override;
	jvxErrorType deactivate()override;

	virtual void activate_technology_api() override;

	jvxErrorType put_configuration(jvxCallManagerConfiguration* callMan, IjvxConfigProcessor* processor,
		jvxHandle* sectionToContainAllSubsectionsForMe, const char* filename = "", jvxInt32 lineno = -1) override;
	jvxErrorType get_configuration(jvxCallManagerConfiguration* callMan,
		IjvxConfigProcessor* processor, jvxHandle* sectionWhereToAddAllSubsections) override;

};

#endif
