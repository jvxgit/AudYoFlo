#ifndef __CJVXTEMPLATETECHNOLOGY_H__
#define __CJVXTEMPLATETECHNOLOGY_H__

#include "jvx.h"

#include "common/CjvxProperties.h"
#include "common/CjvxTechnology.h"

/**
 * This template class can be configured for the following use-cases:
 * 
 * 1) Expose all HW components in the system (typical: audio HW devices)
 * 2) Expose a variable number of devices: Devices can be added and removed while
 *    in operation
 * 3) Expose only one device but multiple instances can be used by activating the same 
 *    device multiple times
 * 
 * The functionality for 1) is realized in the base class CjvxTechnologyDevCaps/CjvxTechnology.
 * 
 * The extensions for 2) are handled externally in the class which uses this class by derivation.
 * A example is the class CjvxSignalProcessingTechnologySocket which is used in the project
 * jvxUnrealRender which connects jvxrt to the unreal engine. Note that the extension to 
 * allow a variable number of devices is handled by the modified API to deal with device pointer
 * instances rather that simple ids in request_device and return_device.
 * 
 * For 3), the member variable allowMultipleInstances must be true. In that case on "request_device",
 * always a new device will be allocated and stored in the field lookupDevice. Also, a reference counter will
 * keep track of all requested and returned devices.
 * 
 * All requests to member functions must be thread safe - all from within main thread OR within a global lock.
 * 
 */
template <class T> class CjvxTemplateTechnology: 
	public IjvxTechnology, public CjvxTechnology,
	public IjvxProperties, public CjvxProperties
{
protected:

	class oneRequestedDevice
	{
	public:
		T* new_dev = nullptr;
		IjvxDevice* parent = nullptr;
	};

	/** 
	 * Keep track of the requested devices in case one exposed device 
	 * grants access to an infinite number of instances. 
	 * Note that all devices must be returned on program termination.
	 */
	std::map<IjvxDevice*,oneRequestedDevice> lookupDevice;
	
	//! Use this variable to generate devices with token <deviceNamePrefix> # i
	std::string deviceNamePrefix = "Single Device";

	//! Number of devices to be allocated on init by default
	jvxSize numberDevicesInit = 1;

	//! Unique id counter to 
	jvxSize uId = 1;

	/**
	 * Enable to allow exposure of multiple devices when requesting a single device.
	 * This must be false in all cases other than 2). 
	 */
	jvxBool allowMultipleInstances = false;

public:

	// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = 
	// Component class interface member functions
	// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = 
	JVX_CALLINGCONVENTION CjvxTemplateTechnology(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE):
		CjvxTechnology(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL), CjvxProperties(module_name, *this)
	{
		JVX_OBJECT_ASSIGN_INTERFACE_FACTORY(this);
		_common_set.theComponentType.tp = tpComp;
		_common_set.theObjectSpecialization = reinterpret_cast<jvxHandle*>(static_cast<IjvxTechnology*>(this));
		_common_set.thisisme = static_cast<IjvxObject*>(this);
		_common_set.theInterfaceFactory = static_cast<IjvxInterfaceFactory*>(this);
		
		/*
		deviceNamePrefix = "Single Device";
		numberDevicesInit = 1;
		uId = 1;
		allowMultipleInstances = true;
		*/
	};

	virtual JVX_CALLINGCONVENTION ~CjvxTemplateTechnology()
	{
		terminate();
	};
	
	// ======================================================

	virtual jvxErrorType JVX_CALLINGCONVENTION activate()override
	{
		jvxSize i;
		jvxErrorType res = res = _activate();
		if(res == JVX_NO_ERROR)
		{
			// Allocate the default number of devices on init
			for (i = 0; i < numberDevicesInit; i++)
			{
				// Do whatever is required
				T* newDevice = new T(deviceNamePrefix.c_str(), false,
					_common_set.theDescriptor.c_str(),
					_common_set.theFeatureClass,
					_common_set.theModuleName.c_str(),
					JVX_COMPONENT_ACCESS_SUB_COMPONENT,
					(jvxComponentType)(_common_set.theComponentType.tp + 1),
					"", NULL);
				
				// Run init function for device
				local_init(newDevice);

				// Whatever to be done for initialization
				oneDeviceWrapper elm;
				elm.hdlDev = static_cast<IjvxDevice*>(newDevice);

				_common_tech_set.lstDevices.push_back(elm);
			}
		}
		return(res);
	};

	virtual jvxErrorType JVX_CALLINGCONVENTION deactivate()override
	{
		jvxErrorType res = _pre_check_deactivate();
		if (res == JVX_NO_ERROR)
		{
			auto elm = _common_tech_set.lstDevices.begin();
			for (; elm != _common_tech_set.lstDevices.end(); elm++)
			{
				assert(elm->refCnt == 0);
				delete elm->hdlDev;
			}
			_common_tech_set.lstDevices.clear();
			assert(lookupDevice.size() == 0);

			res = _deactivate();
		}
		return res;
	}

	// ======================================================

	virtual jvxErrorType JVX_CALLINGCONVENTION request_device(jvxSize idx, IjvxDevice** dev)override
	{
		jvxErrorType res = JVX_NO_ERROR;
		std::string deviceName = deviceNamePrefix + "_" + jvx_size2String(uId);

		if (idx < _common_tech_set.lstDevices.size())
		{
			auto elm = _common_tech_set.lstDevices.begin();
			std::advance(elm, idx);

			// If multiple devices are allowed, one exposed device will allow to create an infinite number of devices
			if (allowMultipleInstances)
			{
				oneRequestedDevice newElm;
				newElm.parent = elm->hdlDev;
				elm->refCnt++;

				T* newDevice = new T(deviceName.c_str(), false, _common_set.theDescriptor.c_str(), _common_set.theFeatureClass,
					_common_set.theModuleName.c_str(), JVX_COMPONENT_ACCESS_SUB_COMPONENT,
					(jvxComponentType)(_common_set.theComponentType.tp + 1), "", NULL);

				// Run init function for device
				local_init(newDevice);

				// Store in list for requested devices
				newElm.new_dev = newDevice;
				lookupDevice[static_cast<IjvxDevice*>(newElm.new_dev)] = newElm;
				uId++;

				// Assign device
				if (dev)
				{
					*dev = newElm.new_dev;
				}
			}
			else
			{
				if (elm->refCnt == 0)
				{
					elm->refCnt++;
					if (dev)
					{
						*dev = elm->hdlDev;
					}
				}
				else
				{
					res = JVX_ERROR_ALREADY_IN_USE;
				}
			}
		}
		else
		{
			res = JVX_ERROR_ID_OUT_OF_BOUNDS;
		}

		return res;
	};

	virtual jvxErrorType JVX_CALLINGCONVENTION return_device(IjvxDevice* dev)override
	{
		jvxErrorType res = JVX_NO_ERROR;
		jvxBool foundLookup = false;
		IjvxDevice* parentRef = NULL;

		// Check if device is in lookup list
		auto elm = lookupDevice.find(dev);
		if (elm != lookupDevice.end())
		{
			parentRef = static_cast<IjvxDevice*>(elm->second.parent);
			foundLookup = true;
			lookupDevice.erase(elm);		
		}

		// If not, continue checking actual list
		if (!foundLookup)
		{
			parentRef = dev;
		}
		
		// Check all list elements
		auto elmD = _common_tech_set.lstDevices.begin();
		for (; elmD != _common_tech_set.lstDevices.end(); elmD++)
		{
			if (parentRef == static_cast<IjvxDevice*>(elmD->hdlDev))
			{
				assert(elmD->refCnt > 0);
				elmD->refCnt--;
				break;
			}
		}

		if (elmD == _common_tech_set.lstDevices.end())
		{
			res = JVX_ERROR_ELEMENT_NOT_FOUND;
		}
		return res;
	};

	virtual jvxErrorType JVX_CALLINGCONVENTION ident_device(jvxSize* idx, IjvxDevice* dev)override
	{
		jvxErrorType res = JVX_NO_ERROR;
		jvxSize id = JVX_SIZE_UNSELECTED;
		jvxSize cnt = 0;
		jvxBool foundLookup = false;

		// Check lookup first
		IjvxDevice* parentRef = NULL;
		auto elm = lookupDevice.find(dev);
		if (elm != lookupDevice.end())
		{
			parentRef = static_cast<IjvxDevice*>(elm->second.parent);
			foundLookup = true;
		}
		if (!foundLookup)
		{
			parentRef = dev;
		}

		auto elmD = _common_tech_set.lstDevices.begin();
		for (; elmD != _common_tech_set.lstDevices.end(); elmD++, cnt++)
		{
			if (parentRef == static_cast<IjvxDevice*>(elmD->hdlDev))
			{				
				break;
			}
		}

		if (elmD != _common_tech_set.lstDevices.end())
		{
			id = cnt;
		}
		else
		{
			res = JVX_ERROR_ELEMENT_NOT_FOUND;
		}

		if (idx)
		{
			*idx = id;
		}

		return res;
	};	

	virtual void local_init(T* newDevice) 
	{
	};

#include "codeFragments/simplify/jvxProperties_simplify.h"

#define JVX_STATE_MACHINE_DEFINE_PREPAREPOSTPROCESS_UNSUPPORTED
#define JVX_STATE_MACHINE_DEFINE_STARTSTOP_UNSUPPORTED
#define	JVX_STATE_MACHINE_DEFINE_READY_UNSUPPORTED
	#include "codeFragments/simplify/jvxStateMachine_simplify.h"
#undef JVX_STATE_MACHINE_DEFINE_READY_UNSUPPORTED
#undef JVX_STATE_MACHINE_DEFINE_STARTSTOP_UNSUPPORTED
#undef JVX_STATE_MACHINE_DEFINE_PREPAREPOSTPROCESS_UNSUPPORTED

	#include "codeFragments/simplify/jvxTechnologies_simplify.h"

	#include "codeFragments/simplify/jvxObjects_simplify.h"

#define JVX_INTERFACE_SUPPORT_PROPERTIES
#include "codeFragments/simplify/jvxHiddenInterface_simplify.h"
#undef JVX_INTERFACE_SUPPORT_PROPERTIES

#include "codeFragments/simplify/jvxInterfaceReference_simplify.h"
#include "codeFragments/simplify/jvxSystemStatus_simplify.h"

	// ===========================================================================================
	// ===========================================================================================
};

#ifdef JVX_AUDIO_TECHNOLOGY_GENERIC_INIT_PTR 
JVX_AUDIO_TECHNOLOGY_GENERIC_INIT_PTR parentPointerDevice = nullptr;
#endif

template <class T, class S> class CjvxTemplateTechnologyInit : public CjvxTemplateTechnology<T>
{
protected:
	S passThisInit = nullptr;

public:

	JVX_CALLINGCONVENTION CjvxTemplateTechnologyInit(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE) :
		CjvxTemplateTechnology<T>(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
	{};

	virtual void local_init(T* newDevice) override
	{
		newDevice->init(passThisInit);
	};
};

#endif
