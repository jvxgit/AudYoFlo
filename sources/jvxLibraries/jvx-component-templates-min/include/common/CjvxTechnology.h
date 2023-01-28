#ifndef __CJVXTECHNOLOGY_H__
#define __CJVXTECHNOLOGY_H__

#include "jvx.h"

#include "common/CjvxObject.h"
#include "common/CjvxDevice.h"

class CjvxTechnology: public CjvxObject, public IjvxDevice_report
{
protected:

	class oneDeviceWrapper
	{
	public:
		IjvxDevice* hdlDev = nullptr;
		jvxSize refCnt = 0;
	};
	
	struct
	{
		std::list<oneDeviceWrapper> lstDevices;
	} _common_tech_set;
	
	std::map<jvxSize, std::string> previousConfigs;

public:

	// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = 
	// Component class interface member functions
	// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = 
	JVX_CALLINGCONVENTION CjvxTechnology(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);

	virtual JVX_CALLINGCONVENTION ~CjvxTechnology();
	
	// virtual jvxErrorType JVX_CALLINGCONVENTION _activate();

	jvxErrorType _number_devices(jvxSize* num);

	jvxErrorType _name_device(jvxSize idx, jvxApiString* fldStr);

	jvxErrorType _description_device(jvxSize idx, jvxApiString* fldStr);

	jvxErrorType _descriptor_device(jvxSize idx, jvxApiString* fldStr, jvxApiString* substr);

	jvxErrorType _module_name_device(jvxSize idx, jvxApiString* fldStr);

	jvxErrorType _request_device(jvxSize idx, IjvxDevice** hdl_onreturn);

	jvxErrorType _return_device(IjvxDevice* dev);

	jvxErrorType _ident_device(jvxSize* idx, IjvxDevice* dev);

	jvxErrorType _deactivate_cbxt();

	 jvxErrorType _capabilities_device(jvxSize, jvxDeviceCapabilities& caps);

	jvxErrorType _location_info_device(jvxSize, jvxComponentIdentification& tpId);

	jvxErrorType _status_device(jvxSize idx, jvxState* stat);

	// ============================================================================

	virtual jvxErrorType JVX_CALLINGCONVENTION on_device_caps_changed(IjvxDevice* dev) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION config_device_set(jvxSize subslotid, const char* txt = nullptr) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION config_device_get(jvxSize subslotid, jvxApiString* astr) override;

};

#endif