#include "jvx.h"
#include "jvxSignalProcessingTechnologies/CjvxSignalProcessingDevice.h"

CjvxSignalProcessingDevice::CjvxSignalProcessingDevice(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE): 
	CjvxObject(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL), CjvxProperties(module_name, *this)
{
	_common_set.theComponentType.unselected(JVX_COMPONENT_SIGNAL_PROCESSING_DEVICE);
	_common_set.theObjectSpecialization = reinterpret_cast<jvxHandle*>(static_cast<IjvxDevice*>(this));
	_common_set.thisisme = static_cast<IjvxObject*>(this);
}

CjvxSignalProcessingDevice::~CjvxSignalProcessingDevice()
{
}

jvxErrorType 
CjvxSignalProcessingDevice::activate()
{
	jvxErrorType res = _activate();
	if(res == JVX_NO_ERROR)
	{
		CjvxSignalProcessingDevice_genpcg::allocate__properties_active();
		CjvxSignalProcessingDevice_genpcg::register__properties_active(static_cast<CjvxProperties*>(this));

		CjvxSignalProcessingDevice_genpcg::allocate__properties_active_higher();
		CjvxSignalProcessingDevice_genpcg::register__properties_active_higher(static_cast<CjvxProperties*>(this));
	}
	return(res);
}

jvxErrorType 
CjvxSignalProcessingDevice::deactivate()
{
	jvxErrorType res = _deactivate();
	if(res == JVX_NO_ERROR)
	{
		CjvxSignalProcessingDevice_genpcg::unregister__properties_active_higher(static_cast<CjvxProperties*>(this));
		CjvxSignalProcessingDevice_genpcg::deallocate__properties_active_higher();

		CjvxSignalProcessingDevice_genpcg::unregister__properties_active(static_cast<CjvxProperties*>(this));
		CjvxSignalProcessingDevice_genpcg::deallocate__properties_active();
	}
	return(res);
}


