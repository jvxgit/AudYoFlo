#include "CjvxCuTGpsDevice.h"
#include <sstream>
#include "jvxGenericRS232Technologies/CjvxGenericRS232Device.h"

CjvxCuTGpsDevice::CjvxCuTGpsDevice(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE) :
	CjvxFlexibleRs232TextDevice(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
{
	CjvxSequencerControl::oneCondition elmCo;

	_common_set.theComponentType.unselected(JVX_COMPONENT_CUSTOM_DEVICE);
	fname_config = "instructions_gps_nmea.jif";
	elmCo.description = "GPS with NMEA infos";
	elmCo.fulfilled = NULL;
	elmCo.stateValid = JVX_STATE_ACTIVE | JVX_STATE_PREPARED | JVX_STATE_PROCESSING;
	elmCo.uniqueId = JVX_FLOW_CONDITION_FLOW_BOUNDS_ID;
	elmCo.priv = reinterpret_cast<jvxHandle*>(this);
	elmCo.theStaticFunc = check_condition_device;
	CjvxSequencerControl::addCondition(elmCo);
}

CjvxCuTGpsDevice::~CjvxCuTGpsDevice()
{
}

jvxErrorType
CjvxCuTGpsDevice::activate_specific_rs232()
{
	jvxErrorType res = JVX_NO_ERROR;
	genGpsRs232_device::init_all();
	genGpsRs232_device::allocate_all();
	genGpsRs232_device::register_all(static_cast<CjvxProperties*>(this));
	
	return CjvxFlexibleRs232TextDevice::activate_specific_rs232();
}

jvxErrorType
CjvxCuTGpsDevice::deactivate_specific_rs232()
{
	jvxErrorType res = JVX_NO_ERROR;
	res = CjvxFlexibleRs232TextDevice::deactivate_specific_rs232();

	genGpsRs232_device::unregister_all(static_cast<CjvxProperties*>(this));
	genGpsRs232_device::deallocate_all();
	genGpsRs232_device::init_all();

	return JVX_NO_ERROR;
}

void 
CjvxCuTGpsDevice::activate_init_messages()
{
	// Do nothing
}

void
CjvxCuTGpsDevice::report_observer_timeout()
{
	CjvxGenericRS232TextDevice::report_observer_timeout();
	// Do nothing
}

jvxErrorType 
CjvxCuTGpsDevice::put_configuration(jvxCallManagerConfiguration* callConf,
	IjvxConfigProcessor* processor,
	jvxHandle* sectionToContainAllSubsectionsForMe, 
	const char* filename, jvxInt32 lineno)
{
	jvxSize i;
	std::vector<std::string> warns;
	jvxErrorType res = CjvxFlexibleRs232TextDevice::put_configuration(callConf, processor, sectionToContainAllSubsectionsForMe);
	if (res == JVX_NO_ERROR)
	{
		if (_common_set_min.theState == JVX_STATE_ACTIVE)
		{
			genGpsRs232_device::put_configuration_all(callConf, processor, sectionToContainAllSubsectionsForMe, &warns);
			for (i = 0; i < warns.size(); i++)
			{
				std::cout << "Warning: " << warns[i] << std::endl;
			}
		}
	}
	return res;
}

jvxErrorType 
CjvxCuTGpsDevice::get_configuration(jvxCallManagerConfiguration* callConf, 
	IjvxConfigProcessor* processor,
	jvxHandle* sectionWhereToAddAllSubsections)
{
	
	jvxErrorType res = CjvxFlexibleRs232TextDevice::get_configuration(callConf, processor, sectionWhereToAddAllSubsections);
	if (res == JVX_NO_ERROR)
	{
		genGpsRs232_device::get_configuration_all(callConf, processor, sectionWhereToAddAllSubsections);
	}
	return res;
}


JVX_SEQCONTROL_CONDITION_C_CALLBACK_EXECUTE_FULL(CjvxCuTGpsDevice, check_condition_device)
{
	jvxBool condAll = true;
	jvxErrorType res = JVX_NO_ERROR;

	/*
	switch (uniqueId)
	{
	case JVX_FLOW_CONDITION_FLOW_BOUNDS_ID:

		if (genFlowRs232_device::remote.device_properties.flow1.value > genFlowRs232_device::remote.operation_limits.flow1_max.value)
		{
			condAll = false;
		}
		if (genFlowRs232_device::remote.device_properties.flow1.value < genFlowRs232_device::remote.operation_limits.flow1_min.value)
		{
			condAll = false;
		}
		if (genFlowRs232_device::remote.device_properties.flow2.value > genFlowRs232_device::remote.operation_limits.flow2_max.value)
		{
			condAll = false;
		}
		if (genFlowRs232_device::remote.device_properties.flow2.value < genFlowRs232_device::remote.operation_limits.flow2_min.value)
		{
			condAll = false;
		}
		if (condAll)
		{
			if (fulfilled)
			{
				*fulfilled = true;
			}
		}
		break;
	default: assert(0);
	}
	*/
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxCuTGpsDevice::set_property(
	jvxCallManagerProperties& callGate,
	const jvx::propertyRawPointerType::IjvxRawPointerType& rawPtr,
	const jvx::propertyAddress::IjvxPropertyAddress& ident,
	const jvx::propertyDetail::CjvxTranferDetail& trans)
{
	jvxErrorType res = CjvxFlexibleRs232TextDevice::set_property(
		callGate, rawPtr,ident, trans);
	if(res == JVX_NO_ERROR)
	{
		JVX_TRANSLATE_PROP_ADDRESS_IDX_CAT(ident, propId, category);
		if (callGate.call_purpose == JVX_PROPERTY_CALL_PURPOSE_INTERNAL_PASS)
		{
			_lock_properties_local();
			if (propId == genGpsRs232_device::remote.device_properties.gpvtg.globalIdx)
			{
				convert_nmea_gpvtg(genGpsRs232_device::remote.device_properties.gpvtg.value);
			}
			else if (propId == genGpsRs232_device::remote.device_properties.gpgga.globalIdx)
			{
				convert_nmea_gpgga(genGpsRs232_device::remote.device_properties.gpgga.value);
			}
			else if (propId == genGpsRs232_device::remote.device_properties.gpgsa.globalIdx)
			{
				convert_nmea_gpgsa(genGpsRs232_device::remote.device_properties.gpgsa.value);
			}
			else if (propId == genGpsRs232_device::remote.device_properties.gpgll.globalIdx)
			{
				convert_nmea_gpgll(genGpsRs232_device::remote.device_properties.gpgll.value);
			}
			else if (propId == genGpsRs232_device::remote.device_properties.gprmc.globalIdx)
			{
				convert_nmea_gprmc(genGpsRs232_device::remote.device_properties.gprmc.value);
			}
			else if (propId == genGpsRs232_device::remote.device_properties.gpgsv.globalIdx)
			{
				convert_nmea_gpgsv(genGpsRs232_device::remote.device_properties.gpgsv.value);
			}
			_unlock_properties_local();

		}
	}
	return res;
}

