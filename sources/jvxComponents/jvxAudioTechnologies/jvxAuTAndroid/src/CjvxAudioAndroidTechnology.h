#ifndef __CJVXAUDIOANDROIDTECHNOLOGY_H__
#define __CJVXAUDIOANDROIDTECHNOLOGY_H__

#include "jvx.h"

#ifndef JVX_USE_PART_ANDROIDAUDIO_NO_GLES
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
#include <android/log.h>
#include <jni.h>
#endif

#include "jvxTechnologies/CjvxTemplateTechnology.h"
#include "CjvxAudioAndroidDevice.h"
#include "pcg_exports_technology.h"

class CjvxAudioAndroidTechnology;
#define LOCAL_TEMPLATE_BASE_CLASS CjvxTemplateTechnologyInit<CjvxAudioAndroidDevice, CjvxAudioAndroidTechnology*>

class CjvxAudioAndroidTechnology :
	public LOCAL_TEMPLATE_BASE_CLASS,
	public IjvxConfiguration, public genAndroid_technology
{
	friend class CjvxAudioAndroidDevice;

protected:
	std::map<IjvxDevice*, CjvxAudioAndroidDevice*> lstDevType;

public:

	JVX_CALLINGCONVENTION CjvxAudioAndroidTechnology(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);
	JVX_CALLINGCONVENTION ~CjvxAudioAndroidTechnology();

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
