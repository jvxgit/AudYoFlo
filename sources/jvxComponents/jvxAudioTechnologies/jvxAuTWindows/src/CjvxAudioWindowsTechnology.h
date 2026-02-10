#ifndef __CJVXAUDIOWINDOWSTECHNOLOGY_H__
#define __CJVXAUDIOWINDOWSTECHNOLOGY_H__

#include "jvx.h"
#include "jvxTechnologies/CjvxTemplateTechnology.h"
#include "CjvxAudioWindowsDevice.h"
#include "pcg_exports_technology.h"

class CjvxAudioWindowsTechnology : public CjvxTemplateTechnology<CjvxAudioWindowsDevice>, 
	public IjvxConfiguration, public genWindows_technology
{
private:
	JVX_MUTEX_HANDLE safeAccessConnections;
	jvxBool comNeedsUninitialize = false;

public:
	JVX_CALLINGCONVENTION CjvxAudioWindowsTechnology(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);
	JVX_CALLINGCONVENTION ~CjvxAudioWindowsTechnology();

	virtual jvxErrorType JVX_CALLINGCONVENTION activate()override;
	virtual jvxErrorType JVX_CALLINGCONVENTION deactivate()override;

	// ====================================================================

	virtual jvxErrorType JVX_CALLINGCONVENTION put_configuration(jvxCallManagerConfiguration* callConf,
		IjvxConfigProcessor* processor, jvxHandle* sectionToContainAllSubsectionsForMe,
		const char* filename = "", jvxInt32 lineno = -1)override;
	virtual jvxErrorType JVX_CALLINGCONVENTION get_configuration(jvxCallManagerConfiguration* callConf,
		IjvxConfigProcessor* processor, jvxHandle* sectionWhereToAddAllSubsections)override;

	// ================================================================

	void activate_windows_audio_technology();
	void deactivate_windows_audio_device(IMMDevice* device);
	void deactivate_windows_audio_technology();

#define JVX_INTERFACE_SUPPORT_CONFIGURATION
#define JVX_INTERFACE_SUPPORT_PROPERTIES
#include "codeFragments/simplify/jvxHiddenInterface_simplify.h"
#undef JVX_INTERFACE_SUPPORT_PROPERTIES
#undef JVX_INTERFACE_SUPPORT_CONFIGURATION

	// =============================================
	// 
	// ================================================================

#include "codeFragments/simplify/jvxInterfaceReference_simplify.h"
};

#endif
