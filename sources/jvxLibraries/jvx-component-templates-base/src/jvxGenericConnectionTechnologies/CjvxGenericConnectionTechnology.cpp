#include "jvx.h"

#include "jvxGenericConnectionTechnologies/CjvxGenericConnectionTechnology.h"
#include "jvxGenericRS232Technologies/CjvxGenericRS232Device.h"

CjvxGenericConnectionTechnology::CjvxGenericConnectionTechnology(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE):
	CjvxTechnology(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL), CjvxProperties(module_name, *this)
{
	JVX_DECLARE_OBJECT_REFERENCES(tpComp, IjvxTechnology);
	/*
	_common_set.theComponentType.unselected(JVX_COMPONENT_UNKNOWN);
	_common_set.theObjectSpecialization = reinterpret_cast<jvxHandle*>(static_cast<IjvxTechnology*>(this));
	JVX_OBJECT_ASSIGN_BASIC_REFERENCES
	*/
}

CjvxGenericConnectionTechnology::~CjvxGenericConnectionTechnology()
{
	terminate();
}

jvxErrorType
CjvxGenericConnectionTechnology::activate()
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
					res = subcomponents.theToolsHost->reference_tool(JVX_COMPONENT_CONNECTION, &subcomponents.theConnectionObj, 0, connectionIdenitificationToken.c_str());
					if ((res == JVX_NO_ERROR) && subcomponents.theConnectionObj)
					{
						res = subcomponents.theConnectionObj->request_specialization(reinterpret_cast<jvxHandle**>(&subcomponents.theConnectionRef), NULL, NULL);

						if ((res == JVX_NO_ERROR) && subcomponents.theConnectionRef)
						{
							JVX_START_LOCK_LOG(jvxLogLevel::JVX_LOGLEVEL_3_DEBUG_OPERATION_WITH_LOW_DEGREE_OUTPUT)
							log << "Initializing custom device family matching connection module <" << connectionIdenitificationToken << ">." << std::endl;

							if (JVX_CHECK_SIZE_SELECTED(numPortsPolled))
							{
								res = subcomponents.theConnectionRef->initialize(_common_set_min.theHostRef, &numPortsPolled, JVX_CONNECT_PRIVATE_ARG_TYPE_CONNECTION_NUM_PORT);
							}
							else
							{
								res = subcomponents.theConnectionRef->initialize(_common_set_min.theHostRef, NULL, JVX_CONNECT_PRIVATE_ARG_TYPE_NONE);
							}
							log << " -- done." << std::endl;

							JVX_STOP_LOCK_LOG;

							res = subcomponents.theConnectionRef->availablePorts(&thePorts);
							if (res == JVX_NO_ERROR)
							{
								for (i = 0; i < thePorts.ll(); i++)
								{
									CjvxGenericConnectionDevice* newDevice = NULL;
									newDevice = this->allocate_device(thePorts.std_str_at(i), i);

									// Set the backward technology reference to allow status update messages
									newDevice->setParent(this);

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
CjvxGenericConnectionTechnology::deactivate()
{
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

	if (subcomponents.theConnectionRef)
	{
		subcomponents.theConnectionRef->terminate();
		subcomponents.theToolsHost->return_reference_tool(JVX_COMPONENT_CONNECTION, subcomponents.theConnectionObj);
		subcomponents.theConnectionRef = NULL;
		subcomponents.theConnectionObj = NULL;
	}

	if (subcomponents.theToolsHost)
	{
		_common_set_min.theHostRef->return_hidden_interface(JVX_INTERFACE_TOOLS_HOST, reinterpret_cast<jvxHandle*>(subcomponents.theToolsHost));
		subcomponents.theToolsHost = NULL;
	}

	return(res);
}

