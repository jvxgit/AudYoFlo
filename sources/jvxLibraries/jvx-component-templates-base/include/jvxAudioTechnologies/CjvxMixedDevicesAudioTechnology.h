#ifndef __CJVXMIXEDMASTERTECHNOLOGY_H__
#define __CJVXMIXEDMASTERTECHNOLOGY_H__

#include "jvx.h"
#include "jvxTechnologies/CjvxTemplateTechnologyVD.h"

// CjvxAudioPWireDevice

template <class T> class CjvxMixedDevicesAudioTechnology :
	public CjvxTemplateTechnologyVD<T>, 
	public IjvxConfiguration
{
public:
	JVX_CALLINGCONVENTION CjvxMixedDevicesAudioTechnology(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE):
		CjvxTemplateTechnologyVD<T>(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
	{
		this->actAsProxy_init = true;
	};

	JVX_CALLINGCONVENTION ~CjvxMixedDevicesAudioTechnology()
	{
	};

	virtual jvxErrorType JVX_CALLINGCONVENTION activate()override
	{
		// "Jump" over the base class implementation 
		jvxErrorType res = CjvxTemplateTechnologyVD<T>::activate();
		if (res == JVX_NO_ERROR)
		{		
			activate_technology_api();
		}
		return(res);
	};

	virtual jvxErrorType JVX_CALLINGCONVENTION deactivate()override
	{
		jvxErrorType res = CjvxTemplateTechnology<T>::_pre_check_deactivate();
		if (res == JVX_NO_ERROR)
		{
			// deactivate_technology_api(); <- not required since handeld in base class
			CjvxTemplateTechnology<T>::deactivate();
		}
		return res;
	};

	// ====================================================================

	// Technology specific functions
	virtual void activate_technology_api() = 0;
	virtual void deactivate_technology_api()
	{
	};

	T* local_allocate_device(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE, jvxSize idx, jvxBool actAsProxy_init) override
	{
		T* newDev = nullptr;
		JVX_SAFE_ALLOCATE_OBJECT(newDev, T(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL));
		newDev->initializeDevice(actAsProxy_init, idx);
		return newDev;
	};

	jvxErrorType local_deallocate_device(T** elmDev)override
	{
		if (elmDev)
		{
			JVX_SAFE_DELETE_OBJECT(*elmDev);
		}
		return JVX_NO_ERROR;
	}


	// void local_init(CjvxAudioPWireDevice* newDevice, jvxBool actAsProxy_init) override;

	virtual jvxErrorType request_device(jvxSize idx, IjvxDevice** dev, jvxCBitField requestFlags)override
	{
		jvxSize numDevicesBefore = 0;
		this->number_devices(&numDevicesBefore);
		jvxErrorType res = CjvxTemplateTechnologyVD<T>::request_device(idx, dev, requestFlags);
		if (res == JVX_NO_ERROR)
		{
			jvxSize numDevicesAfter = 0;
			this->number_devices(&numDevicesAfter);
			if (numDevicesBefore != numDevicesAfter)
			{
				// Report this to host for update of listed devices
				CjvxReportCommandRequest theRequest(
					jvxReportCommandRequest::JVX_REPORT_COMMAND_REQUEST_UPDATE_AVAILABLE_COMPONENT_LIST,
					this->_common_set.theComponentType);
				// This required here in Linux due to compiler processing standards
				this->_request_command(theRequest);
			}
		}
		return res;
	};

	virtual jvxErrorType return_device(IjvxDevice* dev)override
	{
		jvxSize numDevicesBefore = 0;
		this->number_devices(&numDevicesBefore);
		jvxErrorType res = CjvxTemplateTechnologyVD<T>::return_device(dev);
		if (res == JVX_NO_ERROR)
		{
			jvxSize numDevicesAfter = 0;
			this->number_devices(&numDevicesAfter);
			if (numDevicesBefore != numDevicesAfter)
			{
				// Report this to host for update of listed devices
				CjvxReportCommandRequest theRequest(
					jvxReportCommandRequest::JVX_REPORT_COMMAND_REQUEST_UPDATE_AVAILABLE_COMPONENT_LIST,
					this->_common_set.theComponentType);
				// This required here in Linux due to compiler processing standards
				this->_request_command(theRequest);
			}
		}
		return res;
	};

	// ====================================================================

	jvxErrorType put_configuration(jvxCallManagerConfiguration* callConf,
		IjvxConfigProcessor* processor, jvxHandle* sectionToContainAllSubsectionsForMe,
		const char* filename = "", jvxInt32 lineno = -1)override
	{
		std::vector<std::string> warnings;
		if (this->_common_set_min.theState == JVX_STATE_ACTIVE)
		  {
		this->_put_configuration_active(callConf, processor, sectionToContainAllSubsectionsForMe, filename, lineno);
		}
		return JVX_NO_ERROR;
	};

	jvxErrorType get_configuration(jvxCallManagerConfiguration* callConf,
		IjvxConfigProcessor* processor, jvxHandle* sectionWhereToAddAllSubsections)override
	{
		this->_get_configuration_active(callConf, processor, sectionWhereToAddAllSubsections);
		return JVX_NO_ERROR;
	};

	// ================================================================

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
