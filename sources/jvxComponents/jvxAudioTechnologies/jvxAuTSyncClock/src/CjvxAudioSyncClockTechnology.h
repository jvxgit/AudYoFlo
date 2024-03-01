#ifndef __CJVXAUDIOSYNCCLOCKTECHNOLOGY_H__
#define __CJVXAUDIOSYNCCLOCKTECHNOLOGY_H__

#include "jvx.h"

#include "jvxTechnologies/CjvxTemplateTechnology.h"
#include "CjvxAudioSyncClockDevice.h"
#include "pcg_exports_technology.h"

class CjvxAudioSyncClockTechnology;
#define LOCAL_TEMPLATE_BASE_CLASS CjvxTemplateTechnologyInit<CjvxAudioSyncClockDevice, CjvxAudioSyncClockTechnology*>

class CjvxAudioSyncClockTechnology :
	public LOCAL_TEMPLATE_BASE_CLASS,
	public IjvxConfiguration, public genSyncClock_technology
{
	friend class CjvxAudioSyncClockDevice;

protected:
	std::map<IjvxDevice*, CjvxAudioSyncClockDevice*> lstDevType;

public:

	JVX_CALLINGCONVENTION CjvxAudioSyncClockTechnology(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);
	JVX_CALLINGCONVENTION ~CjvxAudioSyncClockTechnology();	

	virtual jvxErrorType JVX_CALLINGCONVENTION select(IjvxObject* owner)override;
	virtual jvxErrorType JVX_CALLINGCONVENTION unselect()override;

	virtual jvxErrorType JVX_CALLINGCONVENTION activate()override;
	virtual jvxErrorType JVX_CALLINGCONVENTION deactivate()override;

	// ===============================================================
	// ===============================================================

	virtual jvxErrorType JVX_CALLINGCONVENTION put_configuration(jvxCallManagerConfiguration* callMan, IjvxConfigProcessor* processor,
		jvxHandle* sectionToContainAllSubsectionsForMe, const char* filename = "", jvxInt32 lineno = -1) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION get_configuration(jvxCallManagerConfiguration* callMan,
		IjvxConfigProcessor* processor, jvxHandle* sectionWhereToAddAllSubsections) override;

#define JVX_INTERFACE_SUPPORT_CONFIGURATION
#define JVX_INTERFACE_SUPPORT_PROPERTIES
#define JVX_INTERFACE_SUPPORT_BASE_CLASS LOCAL_TEMPLATE_BASE_CLASS
#include "codeFragments/simplify/jvxHiddenInterface_simplify.h"
#undef JVX_INTERFACE_SUPPORT_BASE_CLASS
#undef JVX_INTERFACE_SUPPORT_CONFIGURATION
#undef JVX_INTERFACE_SUPPORT_PROPERTIES

	// =====================================================================
	// For all non-object interfaces, return object reference
	// =====================================================================
	#include "codeFragments/simplify/jvxInterfaceReference_simplify.h"
	// =====================================================================
	// =====================================================================
};

#endif
