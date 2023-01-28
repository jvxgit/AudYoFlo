#include "jvx.h"

#include "jvxGenericRS232Technologies/CjvxGenericRS232Technology.h"
#include "jvxGenericRS232Technologies/CjvxGenericRS232Device.h"

CjvxGenericRS232Technology::CjvxGenericRS232Technology(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE):
	CjvxTechnology(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
{
	_common_set.theComponentType.unselected(JVX_COMPONENT_UNKNOWN);
	_common_set.theObjectSpecialization = reinterpret_cast<jvxHandle*>(static_cast<IjvxTechnology*>(this));
	_common_set.thisisme = static_cast<IjvxObject*>(this);
}

CjvxGenericRS232Technology::~CjvxGenericRS232Technology()
{
	terminate();
}

jvxErrorType
CjvxGenericRS232Technology::activate()
{
	jvxErrorType res = _activate();
	if(res == JVX_NO_ERROR)
	{
#ifdef USE_STANDALONE_SKELETON
		// Do whatever is required
		CjvxCameraDevice* newDevice = new CjvxCameraDevice("Single Device", false, _common_set.theDescriptor.c_str(), _common_set.theFeatureClass, _common_set.theModuleName.c_str(), JVX_COMPONENT_ACCESS_SUB_COMPONENT);

		// Whatever to be done for initialization
		oneDeviceWrapper elm;
		elm.hdlDev= static_cast<IjvxDevice*>(newDevice);

		_common_tech_set.lstDevices.push_back(elm);
#else
		int numDevices = 0;
		jvxSize i;
		jvxSize num = 0;
		jvxApiStringList thePorts;

		if (_common_set_min.theHostRef)
		{
			res = _common_set_min.theHostRef->request_hidden_interface(JVX_INTERFACE_TOOLS_HOST, reinterpret_cast<jvxHandle**>(&subcomponents.theToolsHost));
			if ((res == JVX_NO_ERROR) && subcomponents.theToolsHost)
			{
				subcomponents.theToolsHost->number_tools(JVX_COMPONENT_CONNECTION, &num);
				if (num > 0)
				{
					/* Check name of connection type */
					res = subcomponents.theToolsHost->reference_tool(JVX_COMPONENT_CONNECTION, &subcomponents.theRs232Obj, 0, "jvxTRs232*");
					if ((res == JVX_NO_ERROR) && subcomponents.theRs232Obj)
					{
						res = subcomponents.theRs232Obj->request_specialization(reinterpret_cast<jvxHandle**>(&subcomponents.theRs232Ref), NULL, NULL);

						if ((res == JVX_NO_ERROR) && subcomponents.theRs232Ref)
						{
							res = subcomponents.theRs232Ref->initialize(_common_set_min.theHostRef, NULL, JVX_CONNECT_PRIVATE_ARG_TYPE_NONE);
							res = subcomponents.theRs232Ref->availablePorts(&thePorts);
							if (res == JVX_NO_ERROR)
							{
								for (i = 0; i < thePorts.ll(); i++)
								{
									CjvxGenericRS232Device* newDevice = NULL;
									newDevice = this->allocate_device(thePorts.std_str_at(i), i);

									oneDeviceWrapper elm;
									elm.hdlDev = static_cast<IjvxDevice*>(newDevice);
									_common_tech_set.lstDevices.push_back(elm);
								}
							}
						}
					}
				}
			}
		}
#endif
	}
	return(res);
}


jvxErrorType
CjvxGenericRS232Technology::deactivate()
{
	jvxSize i;
	jvxErrorType res = _deactivate();
	if(res == JVX_NO_ERROR)
	{
#ifdef USE_STANDALONE_SKELETON
		for(i = 0; i < _common_tech_set.lstDevices.size(); i++)
		{
			delete(_common_tech_set.lstDevices[i].hdlDev);
		}
		_common_tech_set.lstDevices.clear();
#else
		auto elm = _common_tech_set.lstDevices.begin();
		for (; elm != _common_tech_set.lstDevices.end(); elm++)
		{
			IjvxDevice* theDevice = elm->hdlDev;
			theDevice->terminate();
			delete(theDevice);
		}
		_common_tech_set.lstDevices.clear();
#endif
	}

	if (subcomponents.theRs232Ref)
	{
		subcomponents.theRs232Ref->terminate();
		subcomponents.theToolsHost->return_reference_tool(JVX_COMPONENT_CONNECTION, subcomponents.theRs232Obj);
		subcomponents.theRs232Ref = NULL;
		subcomponents.theRs232Obj = NULL;
	}

	if (subcomponents.theToolsHost)
	{
		_common_set_min.theHostRef->return_hidden_interface(JVX_INTERFACE_TOOLS_HOST, reinterpret_cast<jvxHandle*>(subcomponents.theToolsHost));
		subcomponents.theToolsHost = NULL;
	}

	return(res);
}

