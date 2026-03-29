#ifndef __CJVXAUDIOANDROIDTECHNOLOGY_H__
#define __CJVXAUDIOANDROIDTECHNOLOGY_H__

#include "jvx.h"
#include <string>
#include <vector>

#include "jvxTechnologies/CjvxTemplateTechnology.h"
#include "CjvxAudioAndroidDevice.h"
#include "pcg_exports_technology.h"

// Describes one hardware audio endpoint discovered at activate() time
struct JvxAndroidDeviceInfo
{
	int         deviceId   = 0;    // Android AudioDeviceInfo.getId() — passed to Oboe setDeviceId()
	int         deviceType = 0;    // AudioDeviceInfo.getType() constant (e.g. TYPE_BUILTIN_MIC)
	bool        isInput    = false; // true = capture source, false = playback sink
	std::string productName;       // AudioDeviceInfo.getProductName() toString
};

class CjvxAudioAndroidTechnology;
#define LOCAL_TEMPLATE_BASE_CLASS CjvxTemplateTechnologyInit<CjvxAudioAndroidDevice, CjvxAudioAndroidTechnology*>

class CjvxAudioAndroidTechnology :
	public LOCAL_TEMPLATE_BASE_CLASS,
	public IjvxConfiguration, public genAndroid_technology
{
	friend class CjvxAudioAndroidDevice;

protected:
	std::map<IjvxDevice*, CjvxAudioAndroidDevice*> lstDevType;

	// Device list built by enumerateAndroidDevices() during activate();
	// consumed one entry per call to local_allocate_device()
	std::vector<JvxAndroidDeviceInfo> pendingDevices;
	jvxSize pendingDeviceIdx = 0;

	// Enumerate all Android audio endpoints into 'out'; returns number found.
	// Falls back silently to empty list if JNI is unavailable (non-Android builds).
	jvxSize enumerateAndroidDevices(std::vector<JvxAndroidDeviceInfo>& out);

	CjvxAudioAndroidDevice* local_allocate_device(
		JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE,
		jvxSize idx, jvxBool actAsProxy_init, jvxHandle* fwd_arg) override;

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
